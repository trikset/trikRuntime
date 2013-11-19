/* Copyright 2013 Roman Kurbatov
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

/* This file was modified by Yurii Litvinov to make it comply with the requirements of trikRuntime
 * project. See git revision history for detailed changes. */

#pragma once

#include <QtCore/qglobal.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QLabel>
	#include <QtGui/QTextEdit>
	#include <QtGui/QVBoxLayout>
	#include <QtGui/QListView>
#else
	#include <QtWidgets/QLabel>
	#include <QtWidgets/QTextEdit>
	#include <QtWidgets/QVBoxLayout>
	#include <QtWidgets/QListView>
#endif

#include <QtCore/QList>
#include <QtGui/QStandardItem>
#include <QtGui/QStandardItemModel>

namespace trikWiFi {
class TrikWiFi;
}

namespace trikGui {

/// Widget that shows current IP address and a list of available WiFi networks.
/// Network is available only when it is listed in networks.cfg file and is physically available.
class NetConfigWidget : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param parent - parent QObject.
	explicit NetConfigWidget(QWidget *parent = 0);

	/// Destructor.
	~NetConfigWidget();

	/// Title for this widget in a main menu.
	static QString menuEntry();

protected:
	void keyPressEvent(QKeyEvent *event);

private slots:
	void scanForAvailableNetworksDone();

private:
	QLabel mTitleLabel;
	QLabel mConnectionIconLabel;
	QLabel mIpLabel;
	QLabel mIpValueLabel;
	QLabel mAvailableNetworksLabel;
	QListView mAvailableNetworksView;
	QList<QStandardItem *> mAvailableNetworksItems;
	QStandardItemModel mAvailableNetworksModel;
	QVBoxLayout mMainLayout;
	QHBoxLayout mIpAddressLayout;

	QScopedPointer<trikWiFi::TrikWiFi> mWiFi;

//	void generateNetConfigList();
};

}
