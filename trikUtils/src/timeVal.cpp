#include "timeVal.h"

trikUtils::TimeVal::TimeVal() :
	mTime(0)
{
}

trikUtils::TimeVal::TimeVal(int sec, int mcsec)
{
	mTime = sec * mSecConst + (mcsec << mShift);
}

int trikUtils::TimeVal::toMcSec() const
{
	return mTime >> mShift;
}

trikUtils::TimeVal::TimeVal(int bobtailTime) :
	mTime(bobtailTime)
{
}
