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

PwmCaptureIndicator::PwmCaptureIndicator(const QString &port
		, trikControl::PwmCaptureInterface &pwmCapture
		, QWidget *parent)
	: AbstractIndicator(parent)
	, mPwmCapture(pwmCapture)
	, mNameLabel(port)
	, mValueLabel("0")
{
	mValueBar.setOrientation(Qt::Horizontal);
	mValueBar.setMaximum(pwmCapture.maxValue());
	mValueBar.setMinimum(pwmCapture.minValue());
	mValueBar.setValue(0);
	mValueBar.setTextVisible(false);
	mValueBar.setAlignment(Qt::AlignRight);

	mNameLabel.setAlignment(Qt::AlignLeft);
	mValueLabel.setAlignment(Qt::AlignRight);
	// mValueLabel can change its width during work. It will cause mValueBar
	// width change. To prevent it, we set fixed width for mValueLabel.
	// It is equal to maximum width of the widget which it achieves
	// when the label text is set to "100".
	mValueLabel.setFixedWidth(fontMetricsHorizontalAdvance(this, "WWWW"));
	mLayout.addWidget(&mNameLabel);
	mLayout.addWidget(&mValueBar);
	mLayout.addWidget(&mValueLabel);
	setLayout(&mLayout);

	setFocusPolicy(Qt::StrongFocus);
}

void PwmCaptureIndicator::renew()
{
	int value = mPwmCapture.duty();
	mValueLabel.setText(QString::number(value));
	value = std::max(value, mValueBar.minimum());
	value = std::min(value, mValueBar.maximum());
	mValueBar.setValue(value);
}
