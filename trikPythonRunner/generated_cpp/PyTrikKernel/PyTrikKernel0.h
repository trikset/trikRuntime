#include <PythonQt.h>
#include <QObject>
#include <QVariant>
#include <timeVal.h>
#include <version.h>



class PythonQtWrapper_trikKernel : public QObject
{ Q_OBJECT
public:
public slots:
   int  static_trikKernel_operator-(const trikKernel::TimeVal&  left, const trikKernel::TimeVal&  right);
};





class PythonQtWrapper_trikKernel__TimeVal : public QObject
{ Q_OBJECT
public:
public slots:
trikKernel::TimeVal* new_trikKernel__TimeVal(int  sec, int  mcsec);
void delete_trikKernel__TimeVal(trikKernel::TimeVal* obj) { delete obj; } 
   trikKernel::TimeVal  static_trikKernel__TimeVal_fromPackedUInt32(int  packedTime);
   trikKernel::TimeVal*  operator_assign(trikKernel::TimeVal* theWrappedObject, const trikKernel::TimeVal&  timeVal);
   int  packedUInt32(trikKernel::TimeVal* theWrappedObject) const;
   int  static_trikKernel__TimeVal_timeInterval(int  packedTimeLeft, int  packedTimeRight);
};


