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

#include "classifierSensorWorker.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <utility>

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QMutexLocker>
#include <QtCore/QPointF>
#include <QtCore/QRectF>
#include <QtCore/QTextStream>
#include <QtCore/QVariantMap>

#include <QtGui/QBrush>
#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtGui/QFontMetricsF>
#include <QtGui/QPainterPath>
#include <QtGui/QPen>

#include <QsLog.h>
#include <trikKernel/paths.h>

using namespace trikControl;

namespace {

int argmaxIndex(const QVector<float> &scores)
{
	if (scores.isEmpty()) {
		return -1;
	}

	const auto *data = scores.constData();
	const auto size = scores.size();
	auto bestIndex = 0;
	auto bestValue = data[0];
	for (auto i = 1; i < size; ++i) {
		if (data[i] > bestValue) {
			bestValue = data[i];
			bestIndex = i;
		}
	}
	return bestIndex;
}

int toPercent(float probability)
{
	const auto clamped = std::max(0.0f, std::min(1.0f, probability));
	return static_cast<int>(std::lround(static_cast<double>(clamped) * 100.0));
}

} // namespace

ClassifierSensorWorker::ClassifierSensorWorker(QSharedPointer<CameraManager> camera, QString port,
	trikHal::HardwareAbstractionInterface &hal, std::shared_ptr<VideoPreviewSink> previewSink,
	int previewBufferCount, QObject *parent)
	: NpuAlgorithmWorker(std::move(camera), std::move(port), hal, std::move(previewSink), previewBufferCount,
		  parent)
{
}

QString ClassifierSensorWorker::resolvePath(const QString &path) const
{
	if (path.isEmpty()) {
		return path;
	}

	const QFileInfo info(path);
	if (info.exists()) {
		return info.absoluteFilePath();
	}

	if (info.isAbsolute()) {
		return info.absoluteFilePath();
	}

	const QFileInfo candidate(trikKernel::Paths::mediaPath() + path);
	if (candidate.exists()) {
		return candidate.absoluteFilePath();
	}

	return info.absoluteFilePath();
}

bool ClassifierSensorWorker::loadClassifier(const QString &modelPath, const QString &labelsPath, QString *error)
{
	const auto resolvedModel = resolvePath(modelPath);

	auto localError = QString {};
	if (!loadModel(resolvedModel, &localError)) {
		if (error) {
			*error = localError;
		}
		return false;
	}

	loadLabels(resolvedModel, labelsPath);

	QMutexLocker locker(&mResultLock);
	mScores.clear();
	mBestIndex = -1;
	mLabelMismatchWarned = false;
	return true;
}

void ClassifierSensorWorker::loadLabels(const QString &modelPath, const QString &labelsPath)
{
	mLabels.clear();

	QString path;
	if (labelsPath.isEmpty()) {
		path = resolvePath(QFileInfo(modelPath).dir().filePath(QStringLiteral("labels.txt")));
	} else {
		path = resolvePath(labelsPath);
	}

	QFile file(path);
	if (!file.exists()) {
		QLOG_WARN() << "ClassifierSensorWorker: labels file not found:" << path;
		return;
	}

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QLOG_WARN() << "ClassifierSensorWorker: cannot open labels file:" << path;
		return;
	}

	QTextStream stream(&file);
	while (!stream.atEnd()) {
		const auto line = stream.readLine().trimmed();
		if (!line.isEmpty()) {
			mLabels.append(line);
		}
	}

	QLOG_INFO() << "ClassifierSensorWorker: loaded" << mLabels.size() << "labels from" << path;
}

QString ClassifierSensorWorker::labelFor(int index) const
{
	if (index >= 0 && index < mLabels.size()) {
		return mLabels.at(index);
	}
	return QString::number(index);
}

void ClassifierSensorWorker::processOutputs(const QVector<trikHal::RknnOutputView> &outputs)
{
	if (outputs.isEmpty()) {
		return;
	}

	const auto &view = outputs.first();
	if (!view.data || view.count == 0) {
		return;
	}

	const auto *raw = static_cast<const float *>(view.data);
	const auto count = static_cast<int>(view.count);
	auto scores = QVector<float>(count);
	std::copy(raw, raw + count, scores.begin());

	// Classification models output either probabilities or logits. Detect the
	// former by range/sum and apply a numerically stable softmax otherwise.
	auto sum = 0.0;
	auto looksLikeProbabilities = true;
	for (const auto value : scores) {
		if (value < -0.001f || value > 1.001f) {
			looksLikeProbabilities = false;
		}
		sum += value;
	}

	if (!looksLikeProbabilities || sum < 0.5 || sum > 1.5) {
		const auto maxIt = std::max_element(scores.cbegin(), scores.cend());
		const auto maxValue = maxIt != scores.cend() ? *maxIt : 0.0f;
		auto expSum = 0.0;
		for (auto &value : scores) {
			value = static_cast<float>(std::exp(static_cast<double>(value) - maxValue));
			expSum += value;
		}
		if (expSum > 0.0) {
			for (auto &value : scores) {
				value = static_cast<float>(value / expSum);
			}
		}
	}

	QMutexLocker locker(&mResultLock);
	if (!mLabelMismatchWarned && !mLabels.isEmpty() && mLabels.size() != count) {
		QLOG_WARN() << "ClassifierSensorWorker: labels count" << mLabels.size()
			    << "does not match model classes" << count;
		mLabelMismatchWarned = true;
	}
	mScores = std::move(scores);
	mBestIndex = argmaxIndex(mScores);
}

QString ClassifierSensorWorker::label() const
{
	QMutexLocker locker(&mResultLock);
	return mBestIndex < 0 ? QString() : labelFor(mBestIndex);
}

int ClassifierSensorWorker::confidence() const
{
	QMutexLocker locker(&mResultLock);
	return mBestIndex < 0 ? 0 : toPercent(mScores[mBestIndex]);
}

int ClassifierSensorWorker::classIndex() const
{
	QMutexLocker locker(&mResultLock);
	return mBestIndex;
}

QVariantList ClassifierSensorWorker::top(int k) const
{
	if (k <= 0) {
		return {};
	}

	auto entries = QVector<ScoredClass> {};
	computeTopK(k, entries);

	auto result = QVariantList {};
	result.reserve(entries.size());
	for (const auto &entry : entries) {
		auto item = QVariantMap {};
		item.insert(QStringLiteral("label"), labelFor(entry.second));
		item.insert(QStringLiteral("confidence"), toPercent(entry.first));
		item.insert(QStringLiteral("index"), entry.second);
		result.append(item);
	}
	return result;
}

void ClassifierSensorWorker::showLabels(int k)
{
	mOverlayK.store(k > 0 ? k : 0);
}

void ClassifierSensorWorker::hideLabels()
{
	mOverlayK.store(0);
}

void ClassifierSensorWorker::computeTopK(int k, QVector<ScoredClass> &out) const
{
	out.clear();
	if (k <= 0) {
		return;
	}

	QMutexLocker locker(&mResultLock);
	const auto size = mScores.size();
	const auto count = std::min(k, size);
	if (count <= 0) {
		return;
	}

	// Reserve once; clear() keeps the capacity, so steady-state calls allocate
	// nothing. A min-heap keeps the k largest while scanning every score.
	out.reserve(count);
	const auto *data = mScores.constData();
	const auto byScore = [](const ScoredClass &left, const ScoredClass &right) { return left > right; };
	for (int i = 0; i < size; ++i) {
		const ScoredClass entry {data[i], i};
		if (out.size() < count) {
			out.append(entry);
			std::push_heap(out.begin(), out.end(), byScore);
		} else if (entry > out.first()) {
			std::pop_heap(out.begin(), out.end(), byScore);
			out.last() = entry;
			std::push_heap(out.begin(), out.end(), byScore);
		}
	}

	std::sort(out.begin(), out.end(),
		[](const ScoredClass &left, const ScoredClass &right) { return left.first > right.first; });
}

OverlayResult ClassifierSensorWorker::buildOverlay(const QSize &previewSize)
{
	const auto wanted = mOverlayK.load();
	if (wanted <= 0) {
		if (!mOverlayVisible) {
			return OverlayResult {};
		}
		// Clear the overlay that was drawn while labels were enabled: publish an
		// empty overlay with a fresh generation so the consumer repaints it.
		mOverlayVisible = false;
		mOverlayShown.clear();
		mOverlayLastK = -1;
		++mOverlayGeneration;
		if (mOverlayGeneration == 0) {
			mOverlayGeneration = 1;
		}
		auto cleared = OverlayResult {};
		cleared.generation = mOverlayGeneration;
		mOverlayCache = cleared;
		return cleared;
	}

	if (previewSize.isEmpty()) {
		return OverlayResult {};
	}

	const auto wasVisible = mOverlayVisible;
	mOverlayVisible = true;

	// A human cannot read a label list refreshed at video rate, so the overlay is
	// rebuilt at most once per second. Between refreshes the cached overlay is
	// reused; the top-k is sampled fresh at each refresh.
	using Clock = std::chrono::steady_clock;
	constexpr auto refreshMs = qint64 {1000};
	const auto nowMs =
		std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now().time_since_epoch()).count();
	const auto due = !wasVisible || wanted != mOverlayLastK || (nowMs - mOverlayLastRefreshMs) >= refreshMs;
	if (!due) {
		return mOverlayCache;
	}

	mOverlayLastK = wanted;
	mOverlayLastRefreshMs = nowMs;
	computeTopK(wanted, mOverlayEntries);

	if (mOverlayEntries.isEmpty()) {
		return mOverlayCache;
	}

	// Compare the visible content (class index + integer percent) with the new
	// snapshot. No strings or glyph paths are built while it stays the same.
	auto same = mOverlayShown.size() == mOverlayEntries.size();
	if (same) {
		for (int i = 0; i < mOverlayEntries.size(); ++i) {
			const auto index = mOverlayEntries.at(i).second;
			const auto percent = toPercent(mOverlayEntries.at(i).first);
			if (mOverlayShown.at(i).first != index || mOverlayShown.at(i).second != percent) {
				same = false;
				break;
			}
		}
	}
	if (same && mOverlayGeneration != 0) {
		return mOverlayCache;
	}

	QStringList lines;
	lines.reserve(mOverlayEntries.size());
	for (const auto &entry : mOverlayEntries) {
		lines.append(labelFor(entry.second) + QLatin1Char(' ') + QString::number(toPercent(entry.first))
			     + QLatin1Char('%'));
	}

	const auto width = previewSize.width();
	const auto height = previewSize.height();
	const auto margin = qMax(3, height / 40);
	const auto fontPixels = qBound(10, height / 16, 28);

	auto font = QFont {};
	font.setPixelSize(fontPixels);

	const QFontMetricsF metrics(font);
	const auto lineSpacing = static_cast<int>(std::ceil(metrics.height() * 1.15));
	const auto availableWidth = static_cast<qreal>(qMax(1, width - 2 * margin));
	const auto maxLines = qMax(1, (height - 2 * margin) / qMax(1, lineSpacing));

	auto textPath = QPainterPath {};
	auto baseline = static_cast<qreal>(margin + metrics.ascent());
	for (int i = 0; i < lines.size() && i < maxLines; ++i) {
		const auto elided = metrics.elidedText(lines.at(i), Qt::ElideRight, availableWidth);
		textPath.addText(QPointF(margin, baseline), font, elided);
		baseline += lineSpacing;
	}

	if (textPath.isEmpty()) {
		return OverlayResult {};
	}

	const auto pad = qMax(2, fontPixels / 4);
	const auto frameRect = QRectF(0, 0, width, height);
	const auto backgroundRect = textPath.boundingRect().adjusted(-pad, -pad, pad, pad).intersected(frameRect);

	auto result = OverlayResult {};
	result.overlay.reserve(2);

	auto background = PreviewOverlayPath {};
	background.path.addRoundedRect(backgroundRect, pad, pad);
	background.pen = QPen(Qt::NoPen);
	auto backgroundColor = QColor(0, 0, 0);
	backgroundColor.setAlpha(150);
	background.brush = QBrush(backgroundColor);
	result.overlay.append(background);

	auto text = PreviewOverlayPath {};
	text.path = textPath;
	text.pen = QPen(Qt::NoPen);
	auto textColor = QColor(255, 255, 255);
	textColor.setAlpha(235);
	text.brush = QBrush(textColor);
	result.overlay.append(text);

	++mOverlayGeneration;
	if (mOverlayGeneration == 0) {
		mOverlayGeneration = 1; // 0 is reserved for "no overlay".
	}
	result.generation = mOverlayGeneration;

	// Remember what is now on screen so the next frames can skip rebuilding.
	mOverlayShown.resize(mOverlayEntries.size());
	for (int i = 0; i < mOverlayEntries.size(); ++i) {
		mOverlayShown[i] = {mOverlayEntries.at(i).second, toPercent(mOverlayEntries.at(i).first)};
	}

	mOverlayCache = result;
	return result;
}
