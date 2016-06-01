/* Copyright 2016 Mikhail Kita
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

#include "sensorSettingsWidget.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QScrollArea>
#else
	#include <QtWidgets/QScrollArea>
#endif

#include <QtGui/QKeyEvent>

#include <trikKernel/paths.h>
#include <QsLog.h>

#include "sensorLever.h"
#include "connectButton.h"

using namespace trikGui;
using trikControl::MotorInterface;

SensorSettingsWidget::SensorSettingsWidget(const QString &port, bool isEncoder, QWidget *parent)
	: TrikGuiDialog(parent)
	, mPort(port)
	, mIsEncoder(isEncoder)
	, mPowerLabel(isEncoder ? tr("Select tacho limit:\n") : tr("Select distance:\n"))
	, mContinueButton(new ConnectButton())
{
	mPowerLabel.setAlignment(Qt::AlignTop);
	mPowerLabel.setMaximumHeight(20);
	mLayout.addWidget(&mPowerLabel);

	mLever.reset(new SensorLever(mPort, this));
	mLever->setMaximumHeight(50);
	mLayout.addWidget(mLever.data());

	mContinueButton->setText(tr("Continue"));
	mContinueButton->setAutoFillBackground(true);
	mLayout.addWidget(mContinueButton.data());

	connect(mContinueButton.data(), SIGNAL(upPressed()), this, SLOT(focus()));
	connect(mContinueButton.data(), SIGNAL(downPressed()), this, SLOT(focus()));

	setLayout(&mLayout);
}

SensorSettingsWidget::~SensorSettingsWidget()
{
}

QString SensorSettingsWidget::menuEntry()
{
	return QString(tr("Sensor Settings"));
}

void SensorSettingsWidget::renewFocus()
{
}

void SensorSettingsWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_Up: {
		focus();
		break;
	}
	case Qt::Key_Down: {
		focus();
		break;
	}
	case Qt::Key_Return: {
		mContinueButton->animateClick();
		exit();
		break;
	}
	default: {
		TrikGuiDialog::keyPressEvent(event);
		break;
	}
	}
}

void SensorSettingsWidget::focus()
{
	mLever->hasFocus() ? mContinueButton->setFocus() : mLever->setFocus();
}

QString SensorSettingsWidget::createScript()
{
	QString name("sensor");
	if (mIsEncoder) {
		name = "encoder";
	}

	QString sign(">");
	if (!mLever->isGreater()) {
		sign = "<";
	}

	return QString("    while (!(brick.%1(%2).read() %3 %4)) {\n"
		"        script.wait(10);\n    }\n").arg(name).arg(mPort).arg(sign).arg(mLever->distance());
}
