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

#include <QtCore/QProcess>
#include <QsLog.h>
#include <QtCore/QTimer>

#include "updateWidget.h"

using namespace trikGui;

UpdateWidget::UpdateWidget(QWidget *parent)
	: TrikGuiDialog(parent)
{
	setWindowState(Qt::WindowFullScreen);

	mStatusLabel.setWordWrap(true);
	mStatusLabel.setAlignment(Qt::AlignCenter);
	mStatusLabel.setText(tr("Update is started..."));

	mLayout.addWidget(&mStatusLabel);

	setLayout(&mLayout);
	setFocusPolicy(Qt::StrongFocus);
}

UpdateWidget::~UpdateWidget()
{
}

void UpdateWidget::renewFocus()
{
}

QString UpdateWidget::menuEntry()
{
	return QString(tr("Update"));
}

void UpdateWidget::showStatus(const QString &text, bool isError)
{
	mStatusLabel.setAlignment(Qt::AlignCenter);
	if (isError) {
		mStatusLabel.setText(QString("<font color='red'>%1</font>").arg(text));
	} else {
		mStatusLabel.setText(QString("<font color='green'>%1</font>").arg(text));
	}

	update();
}

int UpdateWidget::exec()
{
	show();

	QEventLoop loop;
	QTimer::singleShot(1000, &loop, &QEventLoop::quit);
	loop.exec();

	QLOG_INFO() << "Running: " << "opkg update";
	QProcess updateCommand;
	updateCommand.start("opkg", {"update"});
	bool update = updateCommand.waitForFinished();

	if (update) {
		QLOG_INFO() << "Running: " << "opkg upgrade";
		QProcess upgradeCommand;
		upgradeCommand.start("opkg", {"upgrade"});
		bool upgrade = upgradeCommand.waitForFinished(100000);

		if (upgrade) {
			showStatus(tr("Update and Upgrade is successfully finished"));
		} else {
			showStatus(tr("Upgrade is failed"), true);
		}
	} else {
		showStatus(tr("Update is failed"), true);
	}

	return TrikGuiDialog::exec();
}
