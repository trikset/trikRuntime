/* Copyright 2014 CyberTech Labs Ltd.
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

#include "informationWidget.h"
#include <trikKernel/version.h>

#include <QtGui/QKeyEvent>
#include <QtNetwork/QNetworkInterface>

using namespace trikGui;

InformationWidget::InformationWidget(QWidget *parent)
	: TrikGuiDialog(parent)
{
	auto osVersionLabel = new QLabel(tr("OS version") + ": \n" + osVersion(), this);
	osVersionLabel->setAlignment(Qt::AlignTop);
	mLayout.addWidget(osVersionLabel);

	auto mac = tr("Not found");

	for (auto &&interface : QNetworkInterface::allInterfaces()) {
		if (interface.name() == "wlan0") {
			mac = interface.hardwareAddress();
			break;
		}
	}

	auto macLabel = new QLabel(tr("MAC address") + ": \n" + mac, this);
	mLayout.addWidget(macLabel);

	auto visit = new QLabel(this);
	visit->setText("<a href=\"https://trikset.com/\">trikset.com</a>");
	visit->setAlignment(Qt::AlignCenter);
	visit->setTextFormat(Qt::RichText);
	visit->setTextInteractionFlags(Qt::TextBrowserInteraction);
	visit->setOpenExternalLinks(true);
	mLayout.addWidget(visit);


	mLayout.addStretch();

	setLayout(&mLayout);
	setFocusPolicy(Qt::StrongFocus);
}

QString InformationWidget::menuEntry()
{
	return QString(tr("Version"));
}

QString InformationWidget::osVersion() const
{
	QFile version("/etc/version");
	if (!version.exists()) {
		return tr("Unknown");
	}

	version.open(QIODevice::ReadOnly);

	return QString::fromUtf8(version.readAll()).trimmed();
}
