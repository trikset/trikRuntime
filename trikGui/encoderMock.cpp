/* Copyright 2024 Daniel Chehade.
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

#include "encoderMock.h"
using namespace trikControl;
using namespace trikGui;

EncoderMock::EncoderMock(QObject *parent) { setParent(parent); }

int EncoderMock::read() {
	if (!reverse && mValue < mMaxValue) {
		mValue += 30;
	} else {
		reverse = true;
	}
	if (reverse && mValue > mMinValue) {
		mValue -= 30;
	} else {
		reverse = false;
	}
	return mValue;
}

int EncoderMock::readRawData() { return 0; }

void EncoderMock::reset() {}

trikControl::DeviceInterface::Status EncoderMock::status() const { return trikControl::DeviceInterface::Status::ready; }
