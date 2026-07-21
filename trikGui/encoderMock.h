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
#pragma once

#include <trikControl/encoderInterface.h>
using namespace trikControl;
namespace trikGui {
/// Widget that shows mock encoder reading.
class EncoderMock : public EncoderInterface
{
	Q_OBJECT
public:
	/// Constructor
	EncoderMock(QObject *parent = nullptr);

	Status status() const override;

	const int mMaxValue{360};
	const int mMinValue{-360};

	bool reverse{false};

	int mValue{mMinValue};

public Q_SLOTS:
	int read() override;

	int readRawData() override;

	void reset() override;
};
} // namespace trikGui
