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

namespace trikUtils {

/// Structure of a time value in a convenient format.
class TimeVal
{

public:

	TimeVal();

	/// Constructor. Parameters represent time in the format - (sec * 10^6 + mcsec) msces.
	/// Constructor translates this value to the new format - 1 unit of a new value(mTime) is equal to 16 mcsec.
	/// Note that after these conversions value in microseconds becomes rounded, but we can neglect this fact
	/// in most cases.
	/// Formula for translation : mTime = (sec * 10^6 + mcsec) << 6 = sec * 10^6 << mShift + mcsec << mShift =
	/// = sec * mSecConst + mcsec << mShift
	TimeVal(int sec, int mcsec);

	/// Translates an interior format to mcsec.
	int toMcSec() const;

	friend const TimeVal operator-(const TimeVal &left, const TimeVal &right)
	{
		const int deltaTime = left.mTime - right.mTime;
		return TimeVal(deltaTime);
	}

private:

	TimeVal(int bobtailTime);

	int mTime;

	static const int mSecConst = 15625;
	static const int mShift = 6;

};

}
