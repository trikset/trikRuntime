/* Copyright 2013 Matvey Bryksin, Yurii Litvinov
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

#define I2C_SLAVE	0x0703	/* Use this slave address */
#define I2C_SLAVE_FORCE	0x0706	/* Use this slave address, even if it */
#define I2C_SMBUS	0x0720	/* SMBus transfer */
#define ADDRESS 0x48

#include <QtCore/QObject>

#include "declSpec.h"

namespace trikControl {

class I2cCommunicator;

class TRIKCONTROL_EXPORT Battery : public QObject
{
	Q_OBJECT

public:
	Battery(I2cCommunicator &communicator);

public slots:
	float readVoltage();

private slots:

private:
	I2cCommunicator &mCommunicator;
	int mData;
};

}
