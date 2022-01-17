/* Copyright 2021 CyberTech Labs Ltd.
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

#include <QString>
#include <QObject>
#include <trikHal/trikHalDeclSpec.h>

namespace trikKernel {
class TimeVal;
}

namespace trikHal {

/// IIO file abstraction.
class TRIKHAL_EXPORT IIOFileInterface : public QObject
{
	Q_OBJECT

public:
	/// Opens IIO file and starts listening for events.
	virtual bool open() = 0;

	/// Closes IIO file and stops listening for events.
	virtual bool close() = 0;

	/// Returns name of an event file.
	virtual QString fileName() const = 0;

	/// Returns true if a file is opened.
	virtual bool isOpened() const = 0;

signals:
	void newData(QVector<int>, const trikKernel::TimeVal &eventTime);
};
}
