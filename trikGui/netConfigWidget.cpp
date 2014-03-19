/* Copyright 2014 Roman Kurbatov
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
 * limitations under the License.
 */

#include "netConfigWidget.h"

#include <QtGui/QKeyEvent>

#include "wiFiClientWidget.h"
#include "wiFiAPWidget.h"
#include "netInitWidget.h"

using namespace trikGui;

NetConfigWidget::NetConfigWidget(QString const &configPath
		, NetConfigWidget::NetworkMode &currentMode
		, QWidget *parent)
	: QWidget(parent)
	, mConfigPath(configPath)
	, mMode(currentMode)
	, mTitle(tr("Choose mode:"))
{
	setWindowState(Qt::WindowFullScreen);

	mModes.addItem(tr("Wi-Fi client"));
	mModes.addItem(tr("Wi-Fi access point"));

	mLayout.addWidget(&mTitle);
	mLayout.addWidget(&mModes);
	setLayout(&mLayout);

	mModes.selectionModel()->select(
			mModes.model()->index(0, 0)
			, QItemSelectionModel::ClearAndSelect
			);
}

void NetConfigWidget::exec()
{
	show();
	mEventLoop.exec();
}

QString NetConfigWidget::menuEntry()
{
	return tr("Network Config");
}

void NetConfigWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_Return:
		case Qt::Key_Right: {
			if (mModes.currentItem()->text() == tr("Wi-Fi client")) {
				setClient();
			} else {
				setAccessPoint();
			}
			break;
		}
		case Qt::Key_Left:
		case Qt::Key_PowerDown:
		case Qt::Key_Meta: {
			mEventLoop.quit();
			break;
		}
		default: {
			QWidget::keyPressEvent(event);
			break;
		}
	}
}

void NetConfigWidget::setClient()
{
	if (mMode != client) {
		mMode = client;

		NetInitWidget netInitWidget;
		netInitWidget.show();
		mEventLoop.processEvents();
		netInitWidget.init(client);
		netInitWidget.close();
	}

	WiFiClientWidget wiFiClientWidget(mConfigPath);

	wiFiClientWidget.exec();

	mEventLoop.quit();
}

void NetConfigWidget::setAccessPoint()
{
	if (mMode != accessPoint) {
		mMode = accessPoint;

		NetInitWidget netInitWidget;
		netInitWidget.show();
		mEventLoop.processEvents();
		netInitWidget.init(accessPoint);
		netInitWidget.close();
	}

	WiFiAPWidget wiFiAPWidget;

	wiFiAPWidget.exec();

	mEventLoop.quit();
}
