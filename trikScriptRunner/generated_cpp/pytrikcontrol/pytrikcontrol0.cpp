#include "pytrikcontrol0.h"
#include <PythonQtConversion.h>
#include <PythonQtMethodInfo.h>
#include <PythonQtSignalReceiver.h>
#include <QVariant>
#include <batteryInterface.h>
#include <brickFactory.h>
#include <brickInterface.h>
#include <cameraDeviceInterface.h>
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
#include <irCameraInterface.h>
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



trikControl::VectorSensorInterface*  PythonQtWrapper_trikControl__BrickInterface::accelerometer(trikControl::BrickInterface* theWrappedObject)
{
  return ( theWrappedObject->accelerometer());
}

trikControl::BatteryInterface*  PythonQtWrapper_trikControl__BrickInterface::battery(trikControl::BrickInterface* theWrappedObject)
{
  return ( theWrappedObject->battery());
}

trikControl::ColorSensorInterface*  PythonQtWrapper_trikControl__BrickInterface::colorSensor(trikControl::BrickInterface* theWrappedObject, const QString&  port)
{
  return ( theWrappedObject->colorSensor(port));
}

QString  PythonQtWrapper_trikControl__BrickInterface::configVersion(trikControl::BrickInterface* theWrappedObject) const
{
  return ( theWrappedObject->configVersion());
}

trikControl::DisplayInterface*  PythonQtWrapper_trikControl__BrickInterface::display(trikControl::BrickInterface* theWrappedObject)
{
  return ( theWrappedObject->display());
}

trikControl::EncoderInterface*  PythonQtWrapper_trikControl__BrickInterface::encoder(trikControl::BrickInterface* theWrappedObject, const QString&  port)
{
  return ( theWrappedObject->encoder(port));
}

QStringList  PythonQtWrapper_trikControl__BrickInterface::encoderPorts(trikControl::BrickInterface* theWrappedObject) const
{
  return ( theWrappedObject->encoderPorts());
}

trikControl::EventDeviceInterface*  PythonQtWrapper_trikControl__BrickInterface::eventDevice(trikControl::BrickInterface* theWrappedObject, const QString&  deviceFile)
{
  return ( theWrappedObject->eventDevice(deviceFile));
}

trikControl::FifoInterface*  PythonQtWrapper_trikControl__BrickInterface::fifo(trikControl::BrickInterface* theWrappedObject, const QString&  port)
{
  return ( theWrappedObject->fifo(port));
}

trikControl::GamepadInterface*  PythonQtWrapper_trikControl__BrickInterface::gamepad(trikControl::BrickInterface* theWrappedObject)
{
  return ( theWrappedObject->gamepad());
}

trikControl::DisplayWidgetInterface*  PythonQtWrapper_trikControl__BrickInterface::graphicsWidget(trikControl::BrickInterface* theWrappedObject)
{
  return ( theWrappedObject->graphicsWidget());
}

trikControl::GyroSensorInterface*  PythonQtWrapper_trikControl__BrickInterface::gyroscope(trikControl::BrickInterface* theWrappedObject)
{
  return ( theWrappedObject->gyroscope());
}

trikControl::I2cDeviceInterface*  PythonQtWrapper_trikControl__BrickInterface::i2c(trikControl::BrickInterface* theWrappedObject, int  bus, int  address, int  regSize)
{
  return ( theWrappedObject->i2c(bus, address, regSize));
}

trikControl::IrCameraInterface*  PythonQtWrapper_trikControl__BrickInterface::irCamera(trikControl::BrickInterface* theWrappedObject)
{
  return ( theWrappedObject->irCamera());
}

trikControl::KeysInterface*  PythonQtWrapper_trikControl__BrickInterface::keys(trikControl::BrickInterface* theWrappedObject)
{
  return ( theWrappedObject->keys());
}

trikControl::LedInterface*  PythonQtWrapper_trikControl__BrickInterface::led(trikControl::BrickInterface* theWrappedObject)
{
  return ( theWrappedObject->led());
}

trikControl::LidarInterface*  PythonQtWrapper_trikControl__BrickInterface::lidar(trikControl::BrickInterface* theWrappedObject)
{
  return ( theWrappedObject->lidar());
}

trikControl::LineSensorInterface*  PythonQtWrapper_trikControl__BrickInterface::lineSensor(trikControl::BrickInterface* theWrappedObject, const QString&  port)
{
  return ( theWrappedObject->lineSensor(port));
}

trikControl::MarkerInterface*  PythonQtWrapper_trikControl__BrickInterface::marker(trikControl::BrickInterface* theWrappedObject)
{
  return ( theWrappedObject->marker());
}

QStringList  PythonQtWrapper_trikControl__BrickInterface::motorPorts(trikControl::BrickInterface* theWrappedObject, trikControl::MotorInterface::Type  type) const
{
  return ( theWrappedObject->motorPorts(type));
}

trikControl::ObjectSensorInterface*  PythonQtWrapper_trikControl__BrickInterface::objectSensor(trikControl::BrickInterface* theWrappedObject, const QString&  port)
{
  return ( theWrappedObject->objectSensor(port));
}

trikControl::PwmCaptureInterface*  PythonQtWrapper_trikControl__BrickInterface::pwmCapture(trikControl::BrickInterface* theWrappedObject, const QString&  port)
{
  return ( theWrappedObject->pwmCapture(port));
}

QStringList  PythonQtWrapper_trikControl__BrickInterface::pwmCapturePorts(trikControl::BrickInterface* theWrappedObject) const
{
  return ( theWrappedObject->pwmCapturePorts());
}

void PythonQtWrapper_trikControl__BrickInterface::reset(trikControl::BrickInterface* theWrappedObject)
{
  ( theWrappedObject->reset());
}

trikControl::SensorInterface*  PythonQtWrapper_trikControl__BrickInterface::sensor(trikControl::BrickInterface* theWrappedObject, const QString&  port)
{
  return ( theWrappedObject->sensor(port));
}

QStringList  PythonQtWrapper_trikControl__BrickInterface::sensorPorts(trikControl::BrickInterface* theWrappedObject, trikControl::SensorInterface::Type  type) const
{
  return ( theWrappedObject->sensorPorts(type));
}

trikControl::SoundSensorInterface*  PythonQtWrapper_trikControl__BrickInterface::soundSensor(trikControl::BrickInterface* theWrappedObject, const QString&  port)
{
  return ( theWrappedObject->soundSensor(port));
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



trikControl::DisplayWidgetInterface* PythonQtWrapper_trikControl__DisplayWidgetInterface::new_trikControl__DisplayWidgetInterface()
{ 
return new trikControl::DisplayWidgetInterface(); }





trikControl::EventCodeInterface* PythonQtWrapper_trikControl__EventCodeInterface::new_trikControl__EventCodeInterface()
{ 
return new trikControl::EventCodeInterface(); }







bool  PythonQtWrapper_trikControl__FifoInterface::hasData(trikControl::FifoInterface* theWrappedObject) const
{
  return ( theWrappedObject->hasData());
}

bool  PythonQtWrapper_trikControl__FifoInterface::hasLine(trikControl::FifoInterface* theWrappedObject) const
{
  return ( theWrappedObject->hasLine());
}

QString  PythonQtWrapper_trikControl__FifoInterface::read(trikControl::FifoInterface* theWrappedObject)
{
  return ( theWrappedObject->read());
}



bool  PythonQtWrapper_trikControl__GamepadInterface::buttonIsPressed(trikControl::GamepadInterface* theWrappedObject, int  buttonNumber)
{
  return ( theWrappedObject->buttonIsPressed(buttonNumber));
}

bool  PythonQtWrapper_trikControl__GamepadInterface::buttonWasPressed(trikControl::GamepadInterface* theWrappedObject, int  buttonNumber)
{
  return ( theWrappedObject->buttonWasPressed(buttonNumber));
}

bool  PythonQtWrapper_trikControl__GamepadInterface::disconnect(trikControl::GamepadInterface* theWrappedObject)
{
  return ( theWrappedObject->disconnect());
}

bool  PythonQtWrapper_trikControl__GamepadInterface::isConnected(trikControl::GamepadInterface* theWrappedObject) const
{
  return ( theWrappedObject->isConnected());
}

bool  PythonQtWrapper_trikControl__GamepadInterface::isPadPressed(trikControl::GamepadInterface* theWrappedObject, int  pad) const
{
  return ( theWrappedObject->isPadPressed(pad));
}

int  PythonQtWrapper_trikControl__GamepadInterface::padX(trikControl::GamepadInterface* theWrappedObject, int  pad) const
{
  return ( theWrappedObject->padX(pad));
}

int  PythonQtWrapper_trikControl__GamepadInterface::padY(trikControl::GamepadInterface* theWrappedObject, int  pad) const
{
  return ( theWrappedObject->padY(pad));
}

int  PythonQtWrapper_trikControl__GamepadInterface::wheel(trikControl::GamepadInterface* theWrappedObject) const
{
  return ( theWrappedObject->wheel());
}



QVector<int >  PythonQtWrapper_trikControl__GyroSensorInterface::getCalibrationValues(trikControl::GyroSensorInterface* theWrappedObject)
{
  return ( theWrappedObject->getCalibrationValues());
}

bool  PythonQtWrapper_trikControl__GyroSensorInterface::isCalibrated(trikControl::GyroSensorInterface* theWrappedObject) const
{
  return ( theWrappedObject->isCalibrated());
}

QVector<int >  PythonQtWrapper_trikControl__GyroSensorInterface::readRawData(trikControl::GyroSensorInterface* theWrappedObject) const
{
  return ( theWrappedObject->readRawData());
}



int  PythonQtWrapper_trikControl__I2cDeviceInterface::read(trikControl::I2cDeviceInterface* theWrappedObject, int  reg, const QString&  mode)
{
  return ( theWrappedObject->read(reg, mode));
}

int  PythonQtWrapper_trikControl__I2cDeviceInterface::send(trikControl::I2cDeviceInterface* theWrappedObject, int  reg, int  value, const QString&  mode)
{
  return ( theWrappedObject->send(reg, value, mode));
}



int  PythonQtWrapper_trikControl__IrCameraInterface::readSensor(trikControl::IrCameraInterface* theWrappedObject, int  m, int  n) const
{
  return ( theWrappedObject->readSensor(m, n));
}









QVector<int >  PythonQtWrapper_trikControl__LineSensorInterface::getDetectParameters(trikControl::LineSensorInterface* theWrappedObject) const
{
  return ( theWrappedObject->getDetectParameters());
}

QVector<int >  PythonQtWrapper_trikControl__LineSensorInterface::read(trikControl::LineSensorInterface* theWrappedObject)
{
  return ( theWrappedObject->read());
}





int  PythonQtWrapper_trikControl__MotorInterface::maxControl(trikControl::MotorInterface* theWrappedObject) const
{
  return ( theWrappedObject->maxControl());
}

int  PythonQtWrapper_trikControl__MotorInterface::minControl(trikControl::MotorInterface* theWrappedObject) const
{
  return ( theWrappedObject->minControl());
}





long  PythonQtWrapper_trikControl__PwmCaptureInterface::maxValue(trikControl::PwmCaptureInterface* theWrappedObject) const
{
  return ( theWrappedObject->maxValue());
}

long  PythonQtWrapper_trikControl__PwmCaptureInterface::minValue(trikControl::PwmCaptureInterface* theWrappedObject) const
{
  return ( theWrappedObject->minValue());
}



int  PythonQtWrapper_trikControl__SensorInterface::maxValue(trikControl::SensorInterface* theWrappedObject) const
{
  return ( theWrappedObject->maxValue());
}

int  PythonQtWrapper_trikControl__SensorInterface::minValue(trikControl::SensorInterface* theWrappedObject) const
{
  return ( theWrappedObject->minValue());
}






