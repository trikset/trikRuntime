#include "pytrikcontrolinternal0.h"
#include <PythonQtConversion.h>
#include <PythonQtMethodInfo.h>
#include <PythonQtSignalReceiver.h>
#include <QVariant>
#include <QtQuick/qquickitem.h>
#include <qbytearray.h>
#include <qcolor.h>
#include <qcoreevent.h>
#include <qevent.h>
#include <qlist.h>
#include <qmetaobject.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qpoint.h>
#include <qrect.h>
#include <qsize.h>
#include <qstringlist.h>
#include <qtransform.h>
#include <qvector.h>
#include <trikControl/batteryInterface.h>
#include <trikControl/colorSensorInterface.h>
#include <trikControl/displayInterface.h>
#include <trikControl/displayWidgetInterface.h>
#include <trikControl/encoderInterface.h>
#include <trikControl/eventDeviceInterface.h>
#include <trikControl/fifoInterface.h>
#include <trikControl/gamepadInterface.h>
#include <trikControl/gyroSensorInterface.h>
#include <trikControl/i2cDeviceInterface.h>
#include <trikControl/irCameraInterface.h>
#include <trikControl/keysInterface.h>
#include <trikControl/ledInterface.h>
#include <trikControl/lidarInterface.h>
#include <trikControl/lineSensorInterface.h>
#include <trikControl/markerInterface.h>
#include <trikControl/motorInterface.h>
#include <trikControl/objectSensorInterface.h>
#include <trikControl/pwmCaptureInterface.h>
#include <trikControl/sensorInterface.h>
#include <trikControl/soundSensorInterface.h>
#include <trikControl/vectorSensorInterface.h>

void PythonQtWrapper_trikControl__DisplayWidgetInterface::hidden(trikControl::DisplayWidgetInterface* theWrappedObject)
{
  ( theWrappedObject->hidden());
}

const QMetaObject*  PythonQtWrapper_trikControl__DisplayWidgetInterface::metaObject(trikControl::DisplayWidgetInterface* theWrappedObject) const
{
  return ( theWrappedObject->metaObject());
}

void*  PythonQtWrapper_trikControl__DisplayWidgetInterface::qt_metacast(trikControl::DisplayWidgetInterface* theWrappedObject, const char*  arg__1)
{
  return ( theWrappedObject->qt_metacast(arg__1));
}

void PythonQtWrapper_trikControl__DisplayWidgetInterface::shown(trikControl::DisplayWidgetInterface* theWrappedObject)
{
  ( theWrappedObject->shown());
}

QString  PythonQtWrapper_trikControl__DisplayWidgetInterface::static_trikControl__DisplayWidgetInterface_tr(const char*  s, const char*  c, int  n)
{
  return (trikControl::DisplayWidgetInterface::tr(s, c, n));
}

QString  PythonQtWrapper_trikControl__DisplayWidgetInterface::static_trikControl__DisplayWidgetInterface_trUtf8(const char*  s, const char*  c, int  n)
{
  return (trikControl::DisplayWidgetInterface::trUtf8(s, c, n));
}



trikControl::DisplayWidgetInterface*  PythonQtWrapper_trikControl__InternalBrickInterface::graphicsWidget(trikControl::InternalBrickInterface* theWrappedObject)
{
  return ( theWrappedObject->graphicsWidget());
}

QString  PythonQtWrapper_trikControl__InternalBrickInterface::static_trikControl__InternalBrickInterface_tr(const char*  s, const char*  c, int  n)
{
  return (trikControl::InternalBrickInterface::tr(s, c, n));
}

QString  PythonQtWrapper_trikControl__InternalBrickInterface::static_trikControl__InternalBrickInterface_trUtf8(const char*  s, const char*  c, int  n)
{
  return (trikControl::InternalBrickInterface::trUtf8(s, c, n));
}



trikControl::DisplayWidgetInterface*  PythonQtWrapper_trikControl__InternalDisplayInterface::graphicsWidget(trikControl::InternalDisplayInterface* theWrappedObject)
{
  return &( theWrappedObject->graphicsWidget());
}

QString  PythonQtWrapper_trikControl__InternalDisplayInterface::static_trikControl__InternalDisplayInterface_tr(const char*  s, const char*  c, int  n)
{
  return (trikControl::InternalDisplayInterface::tr(s, c, n));
}

QString  PythonQtWrapper_trikControl__InternalDisplayInterface::static_trikControl__InternalDisplayInterface_trUtf8(const char*  s, const char*  c, int  n)
{
  return (trikControl::InternalDisplayInterface::trUtf8(s, c, n));
}


