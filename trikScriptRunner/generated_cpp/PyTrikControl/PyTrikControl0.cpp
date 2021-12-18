#include "PyTrikControl0.h"
#include <PythonQtConversion.h>
#include <PythonQtMethodInfo.h>
#include <PythonQtSignalReceiver.h>
#include <QVariant>
#include <batteryInterface.h>
#include <brickInterface.h>
#include <colorSensorInterface.h>
#include <deviceInterface.h>
#include <displayInterface.h>
#include <displayWidgetInterface.h>
#include <encoderInterface.h>
#include <eventCodeInterface.h>
#include <eventDeviceInterface.h>
#include <eventInterface.h>
#include <fifoInterface.h>
#include <gamepadInterface.h>
#include <gyroSensorInterface.h>
#include <i2cDeviceInterface.h>
#include <keysInterface.h>
#include <ledInterface.h>
#include <lidarInterface.h>
#include <lineSensorInterface.h>
#include <markerInterface.h>
#include <motorInterface.h>
#include <objectSensorInterface.h>
#include <pwmCaptureInterface.h>
#include <qbytearray.h>
#include <qcursor.h>
#include <qfont.h>
#include <qfontinfo.h>
#include <qfontmetrics.h>
#include <qkeysequence.h>
#include <qlist.h>
#include <qmargins.h>
#include <qobject.h>
#include <qpaintdevice.h>
#include <qpalette.h>
#include <qpixmap.h>
#include <qpoint.h>
#include <qrect.h>
#include <qregion.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qstringlist.h>
#include <qvector.h>
#include <qwidget.h>
#include <sensorInterface.h>
#include <soundSensorInterface.h>
#include <timeVal.h>
#include <vectorSensorInterface.h>





trikControl::BrickFactory* PythonQtWrapper_trikControl__BrickFactory::new_trikControl__BrickFactory()
{ 
return new trikControl::BrickFactory(); }

trikControl::BrickInterface*  PythonQtWrapper_trikControl__BrickFactory::static_trikControl__BrickFactory_create(const QString&  configFilesPath, const QString&  mediaPath)
{
  return (trikControl::BrickFactory::create(configFilesPath, mediaPath));
}

trikControl::BrickInterface*  PythonQtWrapper_trikControl__BrickFactory::static_trikControl__BrickFactory_create(const QString&  systemConfig, const QString&  modelConfig, const QString&  mediaPath)
{
  return (trikControl::BrickFactory::create(systemConfig, modelConfig, mediaPath));
}



QString  PythonQtWrapper_trikControl__BrickInterface::configVersion(trikControl::BrickInterface* theWrappedObject) const
{
  return ( theWrappedObject->configVersion());
}

trikControl::DisplayWidgetInterface*  PythonQtWrapper_trikControl__BrickInterface::graphicsWidget(trikControl::BrickInterface* theWrappedObject)
{
  return ( theWrappedObject->graphicsWidget());
}

void PythonQtWrapper_trikControl__BrickInterface::reset(trikControl::BrickInterface* theWrappedObject)
{
  ( theWrappedObject->reset());
}







trikControl::DeviceInterface::Status  PythonQtWrapper_trikControl__DeviceInterface::static_trikControl__DeviceInterface_combine(const trikControl::DeviceInterface&  underlying, const trikControl::DeviceInterface::Status&  dependent)
{
  return (trikControl::DeviceInterface::combine(underlying, dependent));
}

trikControl::DeviceInterface::Status  PythonQtWrapper_trikControl__DeviceInterface::status(trikControl::DeviceInterface* theWrappedObject) const
{
  return ( theWrappedObject->status());
}



trikControl::DisplayWidgetInterface*  PythonQtWrapper_trikControl__DisplayInterface::graphicsWidget(trikControl::DisplayInterface* theWrappedObject)
{
  return &( theWrappedObject->graphicsWidget());
}







trikControl::EventCodeInterface* PythonQtWrapper_trikControl__EventCodeInterface::new_trikControl__EventCodeInterface()
{ 
return new trikControl::EventCodeInterface(); }













int  PythonQtWrapper_trikControl__I2cDeviceInterface::read(trikControl::I2cDeviceInterface* theWrappedObject, int  reg)
{
  return ( theWrappedObject->read(reg));
}

void PythonQtWrapper_trikControl__I2cDeviceInterface::send(trikControl::I2cDeviceInterface* theWrappedObject, int  reg, int  value)
{
  ( theWrappedObject->send(reg, value));
}













int  PythonQtWrapper_trikControl__MotorInterface::maxControl(trikControl::MotorInterface* theWrappedObject) const
{
  return ( theWrappedObject->maxControl());
}

int  PythonQtWrapper_trikControl__MotorInterface::minControl(trikControl::MotorInterface* theWrappedObject) const
{
  return ( theWrappedObject->minControl());
}







int  PythonQtWrapper_trikControl__SensorInterface::maxValue(trikControl::SensorInterface* theWrappedObject) const
{
  return ( theWrappedObject->maxValue());
}

int  PythonQtWrapper_trikControl__SensorInterface::minValue(trikControl::SensorInterface* theWrappedObject) const
{
  return ( theWrappedObject->minValue());
}






