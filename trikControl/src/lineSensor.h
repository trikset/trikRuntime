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

#include <QtCore/QReadWriteLock>
#include <QtCore/QVector>

#include <trikDsp/dspTypes.h>

#include "lineSensorInterface.h"
#include "dspSensorBase.h"

namespace trikKernel { class Configurer; }

namespace trikControl {

class LineSensor : public LineSensorInterface
{
	Q_OBJECT

public:
	/// Reads the port's toleranceFactor and marks the sensor ready.
	LineSensor(const QString &port, const trikKernel::Configurer &configurer);
	~LineSensor() override;

	Status status() const override;

	/// Returns {x, crossroadsProbability, mass} from the last DSP result.
	Q_INVOKABLE QVector<int> read() override;

	/// Returns {hue, sat, val, hueTol, satTol, valTol} of the last detect().
	Q_INVOKABLE QVector<int> getDetectParameters() const override;

	/// Returns true when the port is configured as an edge line sensor
	/// (port property "lineIsEdge"), which drives the EdgeLine DSP algorithm.
	bool isEdge() const;

	/// Consumes a DSP result: updates the reading and, for a detect() frame,
	/// applies the tolerance factor and re-activates the DSP with the new range.
	void onResult(const trikDsp::OutArgs &result);

Q_SIGNALS:
	/// Requests the VideoSensorManager to (re)activate the DSP channel.
	void activateRequested(const trikDsp::InArgs &args, bool videoOut, bool canOpen);
	/// Requests the VideoSensorManager to tear the camera down by @p flags.
	void stopRequested(int flags);

public Q_SLOTS:
	void init(bool showOnDisplay) override;
	void detect() override;
	void stop(int flags = StopAll) override; // NOLINT(google-default-arguments)

private:
	DspSensorHelper m;
	qreal mToleranceFactor = 1.0;
	bool mIsEdge = false;

	QVector<int> mReading{0, 0, 0};
	QVector<int> mDetectParameters{0, 0, 0, 0, 0, 0};
	mutable QReadWriteLock mReadingLock;
	mutable QReadWriteLock mDetectParametersLock;
};

}

