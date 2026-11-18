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

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariantList>

#include "deviceInterface.h"

#include <trikControl/trikControlDeclSpec.h>
#include <trikControl/videoSensorStopFlags.h>

namespace trikControl {

/// Continuous single-label image classification on a camera.
///
/// The sensor loads a classification model (.rknn) together with its labels,
/// runs it on every camera frame and exposes the latest result: the most
/// confident label, its confidence and the full top-k list. The model and labels
/// are runtime values, so call load() before init() and call load() again to
/// swap the model while running.
class TRIKCONTROL_EXPORT ClassifierSensorInterface : public QObject, public DeviceInterface
{
	Q_OBJECT

Q_SIGNALS:
	/// Emitted when the sensor is stopped.
	void stopped();

public Q_SLOTS:
	/// Loads or replaces the classification model and its labels.
	///
	/// Call before init(), or while running to swap the model (the sensor is
	/// restarted if it was running). When @p labelsPath is empty, a labels.txt
	/// file next to the model is used. Returns false if the model cannot be
	/// loaded.
	virtual bool load(const QString &modelPath, const QString &labelsPath) = 0;

	/// Starts the camera and continuous classification.
	/// @param showOnDisplay - true if we want an image from a camera to be drawn on robot display.
	virtual void init(bool showOnDisplay) = 0;

	/// Stops classification until init() is called again.
	/// @param flags - how far to tear the camera down (StopAll by default), OR-ed
	///                from VideoSensorStopFlag.
	virtual void stop(int flags = trikControl::StopAll) = 0; // NOLINT(google-default-arguments)

	/// Enables the top-k label overlay on the video preview.
	///
	/// When @p k > 0 and the preview is shown on the display, the sensor draws
	/// the k most confident classes (label and confidence percent) on top of the
	/// video, like an on-screen diagnostic counter. The list is kept inside the
	/// frame bounds and uses a plain readable font. @p k <= 0 disables it.
	virtual void showLabels(int k) = 0;

	/// Hides the label overlay if it is currently shown. Equivalent to
	/// showLabels(0).
	virtual void hideLabels() = 0;

public:
	/// Label of the most confident class, or an empty string before the first
	/// result.
	Q_INVOKABLE virtual QString label() const = 0;

	/// Confidence of the most confident class in percent (0..100).
	Q_INVOKABLE virtual int confidence() const = 0;

	/// Index of the most confident class, or -1 before the first result.
	Q_INVOKABLE virtual int classIndex() const = 0;

	/// The @p k most confident classes as a list of maps with keys "label",
	/// "confidence" (percent) and "index", ordered from the highest score down.
	/// Returns an empty list for @p k <= 0 or before any result.
	Q_INVOKABLE virtual QVariantList top(int k) const = 0;
};

}

Q_DECLARE_METATYPE(trikControl::ClassifierSensorInterface *) // NOLINT(misc-no-recursion)
