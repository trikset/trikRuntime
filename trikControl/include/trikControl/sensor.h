/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QFile>

#include "declSpec.h"

namespace trikControl {

/// Generic TRIK sensor.
class TRIKCONTROL_EXPORT Sensor : public QObject
{
	Q_OBJECT

public:
	Sensor();

	/// Initializes sensor.
	/// @param controlFile Device file for this sensor.
	void init(int min, int max, QString const &controlFile);

public slots:
	/// Returns current raw reading of a sensor.
	int read();

private:
	QFile mControlFile;
	int mMin;
	int mMax;
};

}
