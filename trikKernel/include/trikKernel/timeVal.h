/* Copyright 2015 - 2016 Anastasiia Kornilova.
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
#include <QtCore/qmetatype.h>
namespace trikKernel {

/// Structure of a time value in a convenient format.
class TimeVal
{
public:

	/// Constructor. Parameters represent time in the format - (sec * 10^6 + mcsec) mcsec.
	/// Constructor translates this value to the new format - 1 unit of a new value(mTime) is equal to 256 mcsec.
	/// Note that after these conversions value in microseconds becomes rounded, but we can neglect this fact
	/// in most cases.
	/// Formula for translation : mTime = (sec * 10^6 + mcsec) << 8 = sec * 10^6 << mShift + mcsec << mShift =
	/// = sec * mSecConst << (mShift - 6) + mcsec << mShift
	TimeVal(int sec, int mcsec);

	/// Overloaded "is equal" operator.
	/// @param timeVal - a value, which is assigned to a variable.
	TimeVal &operator=(const TimeVal &timeVal);

	/// Returns packed data that shifted to the left on mShift bits.
	int packedUInt32() const;

	/// Creates TimeVal using packed data.
	/// It needs for hiding one argument constructor of TimeVal from packed data.
	static TimeVal fromPackedUInt32(int packedTime);

	/// Counts time interval between two packed data of time
	static int timeInterval(int packedTimeLeft, int packedTimeRight);
#if QT_VERSION < 0x050000
	/// This method is used in qRegisterMetaType() method and needs default constructor.
	/// It is a friend method for hiding default constructor.
	friend void *qMetaTypeConstructHelper<TimeVal>(const TimeVal *t);
#else
	friend struct QtMetaTypePrivate::QMetaTypeFunctionHelper<TimeVal>;
#endif

	/// "Minus" operator is for computing time interval between two timestamps, returns value in microsends.
	/// @param left - a value before sign, usually "time after event".
	/// @param right - a value after sign, usually "time before event".
	friend int operator-(const TimeVal &left, const TimeVal &right);

private:
	TimeVal() = default;

	explicit TimeVal(int packedTime);

	uint32_t mTime = 0;

	static const uint32_t mSecConst = 15625;
	static const uint32_t mShift = 8;
};

inline int operator-(const TimeVal &left, const TimeVal &right)
{
	return (left.mTime - right.mTime) << TimeVal::mShift;
}

}
