#include "PyTrikKernel0.h"
#include <PythonQtConversion.h>
#include <PythonQtMethodInfo.h>
#include <PythonQtSignalReceiver.h>
#include <QVariant>
#include <timeVal.h>

int  PythonQtWrapper_trikKernel::static_trikKernel_operator-(const trikKernel::TimeVal&  left, const trikKernel::TimeVal&  right)
{
  return (trikKernel::operator-(left, right));
}



trikKernel::TimeVal* PythonQtWrapper_trikKernel__TimeVal::new_trikKernel__TimeVal(int  sec, int  mcsec)
{ 
return new trikKernel::TimeVal(sec, mcsec); }

trikKernel::TimeVal  PythonQtWrapper_trikKernel__TimeVal::static_trikKernel__TimeVal_fromPackedUInt32(int  packedTime)
{
  return (trikKernel::TimeVal::fromPackedUInt32(packedTime));
}

trikKernel::TimeVal*  PythonQtWrapper_trikKernel__TimeVal::operator_assign(trikKernel::TimeVal* theWrappedObject, const trikKernel::TimeVal&  timeVal)
{
  return &( (*theWrappedObject)= timeVal);
}

int  PythonQtWrapper_trikKernel__TimeVal::packedUInt32(trikKernel::TimeVal* theWrappedObject) const
{
  return ( theWrappedObject->packedUInt32());
}

int  PythonQtWrapper_trikKernel__TimeVal::static_trikKernel__TimeVal_timeInterval(int  packedTimeLeft, int  packedTimeRight)
{
  return (trikKernel::TimeVal::timeInterval(packedTimeLeft, packedTimeRight));
}


