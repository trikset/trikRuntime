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

#pragma once

#include <QtCore/qglobal.h>

#include "abstractIndicator.h"

namespace trikControl {
class EncoderInterface;
}

namespace trikGui {

/// Widget that shows current encoder reading.
class EncoderIndicator : public AbstractIndicator
{
	Q_OBJECT
	Q_PROPERTY(QString nameLabel READ nameLabel CONSTANT)
	Q_PROPERTY(int value READ value NOTIFY valueChanged)

public:
	/// Constructor.
	/// @param port - port to which sensor is plugged.
	/// @param encoder - encoder which we will read.
	/// @param parent - parent of this widget in Qt widget parent-child system.
	EncoderIndicator(const QString &port, trikControl::EncoderInterface &encoder, QObject *parent = nullptr);

public Q_SLOTS:
	void renew() override;

private:
	trikControl::EncoderInterface &mEncoder;
	int mValue;
	QString mNameLabel;
	QString nameLabel();
	int value();
Q_SIGNALS:
	/// Emitted when value changed
	void valueChanged();
};

}
