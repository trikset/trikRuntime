/* Copyright 2023 Nick Ponomarev
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

#include "pwmCaptureIndicator.h"

#include <QtWidgets/QStylePainter>
#include <QtWidgets/QStyleOptionFocusRect>

#include <QtCore/QString>

#include <trikControl/pwmCaptureInterface.h>

using namespace trikGui;

PwmCaptureIndicator::PwmCaptureIndicator(const QString &port, trikControl::PwmCaptureInterface &pwmCapture,
					 QObject *parent)
	: AbstractIndicator(parent), mPwmCapture(pwmCapture), mMinValue(pwmCapture.minValue()),
	  mMaxValue(pwmCapture.maxValue()), mValue(0), mNameLabel(port) {}

void PwmCaptureIndicator::renew() {
	long value = mPwmCapture.duty();

	value = std::max(value, mMinValue);
	value = std::min(value, mMaxValue);
	mValue = value;
	Q_EMIT valueChanged();
}

long PwmCaptureIndicator::maxValue() { return mMaxValue; }
long PwmCaptureIndicator::minValue() { return mMinValue; }
long PwmCaptureIndicator::value() { return mValue; }
QString PwmCaptureIndicator::nameLabel() { return mNameLabel; }
