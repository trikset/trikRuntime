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

#include "abstractIndicator.h"
#include "sensorIndicator.h"
#include "encoderIndicator.h"
#include "gyroscopeIndicator.h"
#include "accelerometerWidget.h"
#include "cameraWidget.h"
#include "pwmCaptureIndicator.h"

using namespace trikGui;

SensorsWidget::SensorsWidget(trikControl::BrickInterface &brick, const QStringList &ports
		, SensorType sensorType, QWidget *parent)
	: TrikGuiDialog(parent)
	, mBrick(brick)
	, mInterval(100)
	, mSensorType(sensorType)
{
	mTimer.setInterval(mInterval);
	mTimer.setSingleShot(false);

	int i = 0;

	if (mSensorType == SensorsWidget::SensorType::gyroscope
			|| mSensorType == SensorsWidget::SensorType::accelerometer
			|| mSensorType == SensorsWidget::SensorType::camera) {
		AbstractIndicator *indicator = produceIndicator(QString(""), mSensorType);
		mIndicators.resize(1);

		if (indicator) {
			mLayout.addWidget(indicator);
			connect(&mTimer, &QTimer::timeout, indicator, &AbstractIndicator::renew);
			mIndicators[i] = indicator;
		}

	} else {
		mIndicators.resize(ports.size());

		for (const QString &port : ports) {
			AbstractIndicator *indicator = produceIndicator(port, mSensorType);
			if (indicator) {
				mLayout.addWidget(indicator);
				connect(&mTimer, &QTimer::timeout, indicator, &AbstractIndicator::renew);
				mIndicators[i] = indicator;
				++i;
			}
		}
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

void SensorsWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_PowerOff: {
			goHome();
			break;
		}
		case Qt::Key_Escape: {
			exit();
			break;
		}
		case Qt::Key_Return: {
			mIndicators[0]->setFocus();
			break;
		}
		default: {
			TrikGuiDialog::keyPressEvent(event);
			break;
		}
	}
}

AbstractIndicator *SensorsWidget::produceIndicator(const QString &port, SensorType sensorType)
{
	switch (sensorType) {
	case SensorType::analogOrDigitalSensor: {
		return new SensorIndicator(port, *mBrick.sensor(port), this);
	}
	case SensorType::encoder: {
		return new EncoderIndicator(port, *mBrick.encoder(port), this);
	}
	case SensorType::gyroscope: {
		return new GyroscopeIndicator(mBrick.gyroscope(), this);
	}
	case SensorType::accelerometer: {
		return new AccelerometerWidget(mBrick.accelerometer(), this);
	}
	case SensorType::camera: {
		return new CameraWidget(mBrick, this);
	}
	case SensorType::pwmCapture: {
		return new PwmCaptureIndicator(port, *mBrick.pwmCapture(port), this);
	}
	}

	return nullptr;
}
