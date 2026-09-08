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

#include "colorSensorInterface.h"
#include "dspSensorBase.h"

namespace trikKernel { class Configurer; }

namespace trikControl {

class ColorSensor : public ColorSensorInterface
{
	Q_OBJECT

public:
	/// Reads the port's grid size (m/n) and marks the sensor ready.
	ColorSensor(const QString &port, const trikKernel::Configurer &configurer);
	~ColorSensor() override;

	Status status() const override;

	/// Consumes a DSP result and fills the mxn grid of dominant colors.
	void onResult(const trikDsp::OutArgs &result);

	/// Returns the RGB dominant color of cell (m, n), or {-1,-1,-1} on bad indices.
	QVector<int> read(int m, int n) override;

Q_SIGNALS:
	/// Requests the VideoSensorManager to (re)activate the DSP channel.
	void activateRequested(const trikDsp::InArgs &args, bool videoOut, bool canOpen);
	/// Requests the VideoSensorManager to tear the camera down by @p flags.
	void stopRequested(int flags);

public Q_SLOTS:
	void init(bool showOnDisplay) override;
	void stop(int flags = StopAll) override; // NOLINT(google-default-arguments)

private:
	DspSensorHelper m;

	QVector<QVector<QVector<int>>> mReading;
	int mM = 0;
	int mN = 0;
	mutable QReadWriteLock mReadingLock;
};

}

