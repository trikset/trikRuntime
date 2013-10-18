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

#include "encoder.h"

#include "i2cCommunicator.h"

#define PAR_TO_RAD 0.03272492

using namespace trikControl;

Encoder::Encoder(I2cCommunicator &communicator, int jbx)
	: mCommunicator(communicator),
	  mJbx(jbx),
	  mData(0.0)
{
}

void Encoder::reset()
{
	QByteArray command(2, '\0');
	switch (mJbx)
	{
		case 2:
			command[0] = static_cast<char>(0x31);
			break;
		case 3:
			command[0] = static_cast<char>(0x32);
			break;
		case 4:
			command[0] = static_cast<char>(0x33);
			break;
	}
	command[1] = static_cast<char>(0x00);

	mCommunicator.send(command);
}

float Encoder::get()
{
	int data = 0;
	QByteArray command(2, '\0');

	switch (mJbx)
	{
		case 2:
			command[0] = static_cast<char>(0x31);
			break;
		case 3:
			command[0] = static_cast<char>(0x32);
			break;
		case 4:
			command[0] = static_cast<char>(0x33);
			break;
	}

	data = mCommunicator.read(command);
	mData = PAR_TO_RAD * data;

	return mData;
}

