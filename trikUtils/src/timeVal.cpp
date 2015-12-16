#include "timeVal.h"

trikUtils::TimeVal::TimeVal() :
	mTime(0)
{
}

trikUtils::TimeVal::TimeVal(const TimeVal &timeVal) :
	mTime(timeVal.mTime)
{
}

trikUtils::TimeVal::TimeVal(int sec, int mcsec)
{
	mTime = ((sec * mSecConst) >> (mShift - 6))  + (mcsec >> mShift);
}

int trikUtils::TimeVal::toMcSec() const
{
	return (mTime << mShift);
}
