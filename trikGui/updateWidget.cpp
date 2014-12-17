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

void UpdateWidget::showStatus(QString const &text, bool isError)
{
	mStatusLabel.setAlignment(Qt::AlignLeft);
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
	QLOG_INFO() << "Running: " << "opkg update";
	qDebug() << "Running:" << "opkg update";
	bool update = true;//QProcess::startDetached("opkg update");

	QLOG_INFO() << "Running: " << "opkg upgrade";
	qDebug() << "Running:" << "opkg upgrade";
	bool upgrade = true;//QProcess::startDetached("opkg upgrade");

	if (update && upgrade) {
		showStatus(tr("Update is successfully finished"));
	} else {
		showStatus(tr("Update is failed"), true);
	}

	return TrikGuiDialog::exec();
}
