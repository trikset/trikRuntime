/* Copyright 2014 - 2015 Roman Kurbatov and CyberTech Labs Ltd.
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

#include "sensorsWidget.h"

#include <QtGui/QKeyEvent>

#include <trikControl/brickInterface.h>

#include "sensorIndicator.h"

using namespace trikGui;

SensorsWidget::SensorsWidget(trikControl::BrickInterface &brick, QStringList const &ports, QWidget *parent)
	: TrikGuiDialog(parent)
	, mBrick(brick)
	, mIndicators(ports.size())
	, mInterval(100)
{
	mTimer.setInterval(mInterval);
	mTimer.setSingleShot(false);

	int i = 0;
	for (QString const &port : ports) {
		SensorIndicator *indicator = new SensorIndicator(port, *mBrick.sensor(port), this);
		mLayout.addWidget(indicator);
		connect(&mTimer, SIGNAL(timeout()), indicator, SLOT(renew()));
		mIndicators[i] = indicator;
		++i;
	}

	setLayout(&mLayout);
}

SensorsWidget::~SensorsWidget()
{
	qDeleteAll(mIndicators);
}

int SensorsWidget::exec()
{
	mTimer.start();
	return TrikGuiDialog::exec();
}

void SensorsWidget::renewFocus()
{
	setFocus();
}

void SensorsWidget::exit()
{
	mTimer.stop();
	TrikGuiDialog::exit();
}

void SensorsWidget::goHome()
{
	mTimer.stop();
	TrikGuiDialog::goHome();
}
