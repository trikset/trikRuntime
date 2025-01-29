/* Copyright 2015 CyberTech Labs Ltd.
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

#include <QtCore/qglobal.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QLabel>
#else
	#include <QtWidgets/QLabel>
#endif

namespace trikGui {

/// A label that shows whether or not some connection is open (for example, Mailbox connection)
class OpenSocketIndicator : public QLabel
{
	Q_OBJECT
public:
	/// @param openFilePic - path to picture for open connection status
	/// @param status - initial connection status
	OpenSocketIndicator(const QString &openFilePic, bool status, QWidget *parent = 0);

public Q_SLOTS:
	/// Changes status of indicator to a given value.
	void changeStatus(bool status);

private:
	QPixmap mOpenPic;
};

}
