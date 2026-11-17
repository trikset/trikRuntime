/* Copyright 2026 CyberTech Labs Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#pragma once

#include <atomic>
#include <utility>

#include <QtCore/QMutex>
#include <QtCore/QSize>
#include <QtCore/QStringList>
#include <QtCore/QVariantList>
#include <QtCore/QVector>

#include "npuAlgorithmWorker.h"

namespace trikControl {

/// Continuous single-label image classification on the NPU.
///
/// The worker consumes camera frames through NpuAlgorithmWorker (RGA -> uint8
/// RGB NHWC -> .rknn session) and treats the model output as a vector of class
/// scores. The latest result is exposed through a small thread-safe API
/// (label/confidence/classIndex/top) that readers may call from any thread.
///
/// The model and its labels are runtime values: call loadClassifier() before
/// start(), or load again to swap the model (the sensor is restarted when it
/// was running). The input geometry is taken from the model, so any
/// single-input classification .rknn works without rebuilding.
///
/// @section output Output handling
///
/// processOutputs() keeps the first output tensor only. It detects whether the
/// values are already probabilities (range/sum based) or raw logits and applies
/// a numerically stable softmax in the latter case, so the same worker works
/// with models that do or do not apply softmax internally.
///
/// @section overlay Preview overlay
///
/// showLabels(k) draws an on-screen top-k list (label + confidence percent) over
/// the preview and hideLabels() removes it. The overlay is refreshed at most once
/// per second (a human cannot read faster), is clamped and elided to the preview
/// bounds, and reuses cached glyph paths until the visible content changes.
///
/// @section threading Threading
///
/// loadClassifier() runs on the control thread while the worker is stopped.
/// processOutputs() runs on the worker thread; readers use the getters, which
/// lock mResultLock.
class ClassifierSensorWorker : public NpuAlgorithmWorker
{
public:
	ClassifierSensorWorker(QSharedPointer<CameraManager> camera, QString port,
		trikHal::HardwareAbstractionInterface &hal, std::shared_ptr<VideoPreviewSink> previewSink,
		int previewBufferCount = 4, QObject *parent = nullptr);

	/// Loads or replaces the classification model and its labels.
	///
	/// Must be called while the worker is not running. @p modelPath and
	/// @p labelsPath are resolved with resolvePath(); when @p labelsPath is
	/// empty, labels.txt next to the (resolved) model is used. The previous
	/// result is dropped and the one-shot label/class mismatch warning is
	/// re-armed. Returns false and fills @p error when the model cannot be
	/// loaded.
	bool loadClassifier(const QString &modelPath, const QString &labelsPath, QString *error);

	/// Label of the most confident class, or an empty string before the first
	/// result. Indices outside the labels file fall back to their decimal form.
	QString label() const;

	/// Confidence of the most confident class in percent (0..100), or 0 before
	/// the first result.
	int confidence() const;

	/// Index of the most confident class, or -1 before the first result.
	int classIndex() const;

	/// The @p k most confident classes, ordered from the highest score down, as
	/// a list of maps with keys "label", "confidence" (percent) and "index".
	/// Returns an empty list for @p k <= 0 or before any result.
	QVariantList top(int k) const;

	/// Enables (k > 0) or disables (k <= 0) the top-k label overlay on the
	/// preview. Safe to call from any thread, including while running.
	void showLabels(int k);

	/// Hides the label overlay if it is shown. Equivalent to showLabels(0).
	void hideLabels();

protected:
	/// Classifier post-processing: keeps the first output tensor, turns it into
	/// class scores (softmax when it looks like logits), updates the cached
	/// argmax and refreshes the shared result under mResultLock.
	void processOutputs(const QVector<trikHal::RknnOutputView> &outputs) override;

	/// Builds the top-k label overlay (label + confidence percent) for the
	/// current preview frame, kept inside @p previewSize bounds. Returns an
	/// empty overlay when showLabels() is off or there is no result yet.
	///
	/// Runs on the worker thread and owns the overlay cache state, hence it is
	/// not const. The list is refreshed at most once per second, which is enough
	/// for a human-readable overlay; the top-k is sampled fresh at each refresh.
	OverlayResult buildOverlay(const QSize &previewSize) override;

private:
	/// One scored class: {score, class index}.
	using ScoredClass = std::pair<float, int>;

	/// One overlay line as rendered: {class index, confidence percent}.
	using ShownLabel = std::pair<int, int>;

	/// Loads labels from @p labelsPath, or from labels.txt next to
	/// @p modelPath when @p labelsPath is empty. Logs and leaves the list empty
	/// on failure.
	void loadLabels(const QString &modelPath, const QString &labelsPath);

	/// Human-readable label for @p index, or its decimal form when the index is
	/// outside mLabels.
	QString labelFor(int index) const;

	/// Computes the @p k highest scores into the caller's reusable buffer, sorted
	/// from the highest score down. Takes mResultLock but performs no heap
	/// allocation once @p out has grown to @p k, so it is cheap enough to run on
	/// every frame.
	void computeTopK(int k, QVector<ScoredClass> &out) const;

	/// Resolves a model/labels path the same way playSound resolves media paths:
	/// use it as given if it exists, otherwise resolve it against Paths::mediaPath().
	QString resolvePath(const QString &path) const;

	/// Class names loaded from the labels file; may be empty.
	QStringList mLabels;

	/// Guards mScores, mBestIndex and the mismatch warning flag between the
	/// worker thread (writer) and the getters (readers).
	mutable QMutex mResultLock;

	/// Latest class scores (one per model class), guarded by mResultLock.
	QVector<float> mScores;

	/// Cached argmax of mScores, or -1 when there is no result yet.
	int mBestIndex = -1;

	/// Set after the labels/model class-count mismatch has been logged once, so
	/// the warning is not repeated for every frame. Reset by loadClassifier().
	bool mLabelMismatchWarned = false;

	/// Number of top classes to draw on the preview overlay; 0 disables it.
	std::atomic<int> mOverlayK {0};

	/// Reused top-k snapshot for the current refresh. Never reallocated once
	/// grown to the requested k.
	QVector<ScoredClass> mOverlayEntries;

	/// What is currently drawn: {class index, confidence percent} per line.
	///
	/// Comparing it with the new snapshot detects a real content change without
	/// building any strings; the glyph paths are rebuilt only when it differs.
	QVector<ShownLabel> mOverlayShown;

	/// Overlay returned between refreshes while @c mOverlayShown does not change.
	OverlayResult mOverlayCache;

	/// Generation handed to the consumer; bumped only when the overlay changes.
	quint32 mOverlayGeneration = 0;

	/// Monotonic timestamp (ms) of the last overlay refresh, and the k used for
	/// that snapshot. The overlay is refreshed at most once per second.
	qint64 mOverlayLastRefreshMs = 0;
	int mOverlayLastK = -1;

	/// True while an overlay is (or just was) visible; used to clear it once
	/// after showLabels(0) instead of leaving stale text on screen.
	bool mOverlayVisible = false;
};

} // namespace trikControl
