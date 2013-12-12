/* Copyright 2013 Roman Kurbatov, Yurii Litvinov
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
#include <QtCore/QFile>
#include <QtCore/QVector>

#include "declSpec.h"

namespace trikControl {

/// Provides current frequency and duty info for given engine.
class TRIKCONTROL_EXPORT PwmCapture : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param frequencyFile - device file with frequency.
	/// @param dutyFile - device file with duty.
	PwmCapture(QString const &frequencyFile, QString const &dutyFile);

	~PwmCapture();

public slots:

	// TODO: Document this. What values it will return?
	QVector<int> frequency();

	// TODO: Document this. What value is this?
	int duty();

private:
	QFile mFrequencyFile;
	QFile mDutyFile;
};

}
