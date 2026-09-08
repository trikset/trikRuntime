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

#include "objectSensorInterface.h"
#include "dspSensorBase.h"

namespace trikKernel { class Configurer; }

namespace trikControl {

class ObjectSensor : public ObjectSensorInterface
{
	Q_OBJECT

public:
	/// Reads the port's toleranceFactor and marks the sensor ready.
	ObjectSensor(const QString &port, const trikKernel::Configurer &configurer);
	~ObjectSensor() override;

	Status status() const override;

	/// Consumes a DSP result: updates the reading and, for a detect() frame,
	/// applies the tolerance factor and re-activates the DSP with the new range.
	void onResult(const trikDsp::OutArgs &result);

	/// Returns {x, y, size} from the last DSP result.
	QVector<int> read() override;

	/// Returns {hue, sat, val, hueTol, satTol, valTol} of the last detect().
	QVector<int> getDetectParameters() const override;

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

	QVector<int> mReading{0, 0, 0};
	QVector<int> mDetectParameters{0, 0, 0, 0, 0, 0};
	mutable QReadWriteLock mReadingLock;
	mutable QReadWriteLock mDetectParametersLock;
};

}

