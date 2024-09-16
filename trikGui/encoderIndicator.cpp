/* Copyright 2015 CyberTech Labs Ltd.
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

#include "encoderIndicator.h"

#include <QtCore/QString>

#include <trikControl/encoderInterface.h>

using namespace trikGui;

EncoderIndicator::EncoderIndicator(const QString &port
	, trikControl::EncoderInterface &encoder
	, QWidget *parent)
	: AbstractIndicator(parent)
	, mEncoder(encoder)
	, mNameLabel(port)
	, mValueLabel("0")
{
	mNameLabel.setAlignment(Qt::AlignCenter);
	mValueLabel.setAlignment(Qt::AlignCenter);

	mValueDial.setMinimum(0);
	mValueDial.setMaximum(360);
	mValueDial.setWrapping(true);

	mLayout.addWidget(&mNameLabel);
	mLayout.addWidget(&mValueDial);
	mLayout.addWidget(&mValueLabel);
	setLayout(&mLayout);

	setFocusPolicy(Qt::StrongFocus);
}

void EncoderIndicator::renew()
{
	const int value = mEncoder.read();
	mValueLabel.setText(QString::number(value));
	mValueDial.setValue(value);
}
