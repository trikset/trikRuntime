#include "timeVal.h"

trikUtils::TimeVal::TimeVal(int sec, int mcsec)
{
	mTime = sec * mSecConst + (mcsec << mShift);
}

int trikUtils::TimeVal::toMSec() const
{
	return mTime >> mShift;
}
