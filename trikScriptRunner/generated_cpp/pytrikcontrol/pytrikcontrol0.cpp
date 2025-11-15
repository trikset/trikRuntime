#include "pytrikcontrol0.h"
#include <../trikControl/include/trikControl/batteryInterface.h>
#include <../trikControl/include/trikControl/brickFactory.h>
#include <../trikControl/include/trikControl/brickInterface.h>
#include <../trikControl/include/trikControl/cameraDeviceInterface.h>
#include <../trikControl/include/trikControl/colorSensorInterface.h>
#include <../trikControl/include/trikControl/deviceInterface.h>
#include <../trikControl/include/trikControl/displayInterface.h>
#include <../trikControl/include/trikControl/displayWidgetInterface.h>
#include <../trikControl/include/trikControl/encoderInterface.h>
#include <../trikControl/include/trikControl/eventCodeInterface.h>
#include <../trikControl/include/trikControl/eventDeviceInterface.h>
#include <../trikControl/include/trikControl/eventInterface.h>
#include <../trikControl/include/trikControl/fifoInterface.h>
#include <../trikControl/include/trikControl/gamepadInterface.h>
#include <../trikControl/include/trikControl/gyroSensorInterface.h>
#include <../trikControl/include/trikControl/i2cDeviceInterface.h>
#include <../trikControl/include/trikControl/irCameraInterface.h>
#include <../trikControl/include/trikControl/keysInterface.h>
#include <../trikControl/include/trikControl/ledInterface.h>
#include <../trikControl/include/trikControl/lidarInterface.h>
#include <../trikControl/include/trikControl/lineSensorInterface.h>
#include <../trikControl/include/trikControl/markerInterface.h>
#include <../trikControl/include/trikControl/motorInterface.h>
#include <../trikControl/include/trikControl/objectSensorInterface.h>
#include <../trikControl/include/trikControl/pwmCaptureInterface.h>
#include <../trikControl/include/trikControl/sensorInterface.h>
#include <../trikControl/include/trikControl/soundSensorInterface.h>
#include <../trikControl/include/trikControl/vectorSensorInterface.h>
#include <../trikKernel/include/trikKernel/timeVal.h>
#include <PythonQtConversion.h>
#include <PythonQtMethodInfo.h>
#include <PythonQtSignalReceiver.h>
#include <QVariant>
#include <QtWidgets/qsizepolicy.h>
#include <QtWidgets/qwidget.h>
#include <qbytearray.h>
#include <qcursor.h>
#include <qfont.h>
#include <qfontinfo.h>
#include <qfontmetrics.h>
#include <qkeysequence.h>
#include <qlist.h>
#include <qmargins.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qpaintdevice.h>
#include <qpalette.h>
#include <qpixmap.h>
#include <qpoint.h>
#include <qrect.h>
#include <qregion.h>
#include <qsize.h>
#include <qstringlist.h>
#include <qvector.h>



const QMetaObject*  PythonQtWrapper_trikControl__BatteryInterface::metaObject(trikControl::BatteryInterface* theWrappedObject) const
{
  return ( theWrappedObject->metaObject());
}

void*  PythonQtWrapper_trikControl__BatteryInterface::qt_metacast(trikControl::BatteryInterface* theWrappedObject, const char*  arg__1)
{
  return ( theWrappedObject->qt_metacast(arg__1));
}

float  PythonQtWrapper_trikControl__BatteryInterface::readRawDataVoltage(trikControl::BatteryInterface* theWrappedObject)
{
  return ( theWrappedObject->readRawDataVoltage());
}

float  PythonQtWrapper_trikControl__BatteryInterface::readVoltage(trikControl::BatteryInterface* theWrappedObject)
{
  return ( theWrappedObject->readVoltage());
}

QString  PythonQtWrapper_trikControl__BatteryInterface::static_trikControl__BatteryInterface_tr(const char*  s, const char*  c, int  n)
{
  return (trikControl::BatteryInterface::tr(s, c, n));
}

QString  PythonQtWrapper_trikControl__BatteryInterface::static_trikControl__BatteryInterface_trUtf8(const char*  s, const char*  c, int  n)
{
  return (trikControl::BatteryInterface::trUtf8(s, c, n));
}



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

void PythonQtWrapper_trikControl__BrickInterface::configure(trikControl::BrickInterface* theWrappedObject, const QString&  portName, const QString&  deviceName)
{
  ( theWrappedObject->configure(portName, deviceName));
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

const QMetaObject*  PythonQtWrapper_trikControl__BrickInterface::metaObject(trikControl::BrickInterface* theWrappedObject) const
{
  return ( theWrappedObject->metaObject());
}

trikControl::MotorInterface*  PythonQtWrapper_trikControl__BrickInterface::motor(trikControl::BrickInterface* theWrappedObject, const QString&  port)
{
  return ( theWrappedObject->motor(port));
}

QStringList  PythonQtWrapper_trikControl__BrickInterface::motorPorts(trikControl::BrickInterface* theWrappedObject, trikControl::MotorInterface::Type  type) const
{
  return ( theWrappedObject->motorPorts(type));
}

trikControl::ObjectSensorInterface*  PythonQtWrapper_trikControl__BrickInterface::objectSensor(trikControl::BrickInterface* theWrappedObject, const QString&  port)
{
  return ( theWrappedObject->objectSensor(port));
}

void PythonQtWrapper_trikControl__BrickInterface::playSound(trikControl::BrickInterface* theWrappedObject, const QString&  soundFileName)
{
  ( theWrappedObject->playSound(soundFileName));
}

void PythonQtWrapper_trikControl__BrickInterface::playTone(trikControl::BrickInterface* theWrappedObject, int  hzFreq, int  msDuration)
{
  ( theWrappedObject->playTone(hzFreq, msDuration));
}

trikControl::PwmCaptureInterface*  PythonQtWrapper_trikControl__BrickInterface::pwmCapture(trikControl::BrickInterface* theWrappedObject, const QString&  port)
{
  return ( theWrappedObject->pwmCapture(port));
}

QStringList  PythonQtWrapper_trikControl__BrickInterface::pwmCapturePorts(trikControl::BrickInterface* theWrappedObject) const
{
  return ( theWrappedObject->pwmCapturePorts());
}

void*  PythonQtWrapper_trikControl__BrickInterface::qt_metacast(trikControl::BrickInterface* theWrappedObject, const char*  arg__1)
{
  return ( theWrappedObject->qt_metacast(arg__1));
}

void PythonQtWrapper_trikControl__BrickInterface::reset(trikControl::BrickInterface* theWrappedObject)
{
  ( theWrappedObject->reset());
}

void PythonQtWrapper_trikControl__BrickInterface::resetCompleted(trikControl::BrickInterface* theWrappedObject)
{
  ( theWrappedObject->resetCompleted());
}

void PythonQtWrapper_trikControl__BrickInterface::say(trikControl::BrickInterface* theWrappedObject, const QString&  text)
{
  ( theWrappedObject->say(text));
}

trikControl::SensorInterface*  PythonQtWrapper_trikControl__BrickInterface::sensor(trikControl::BrickInterface* theWrappedObject, const QString&  port)
{
  return ( theWrappedObject->sensor(port));
}

QStringList  PythonQtWrapper_trikControl__BrickInterface::sensorPorts(trikControl::BrickInterface* theWrappedObject, trikControl::SensorInterface::Type  type) const
{
  return ( theWrappedObject->sensorPorts(type));
}

trikControl::I2cDeviceInterface*  PythonQtWrapper_trikControl__BrickInterface::smBusI2c(trikControl::BrickInterface* theWrappedObject, int  bus, int  address)
{
  return ( theWrappedObject->smBusI2c(bus, address));
}

trikControl::SoundSensorInterface*  PythonQtWrapper_trikControl__BrickInterface::soundSensor(trikControl::BrickInterface* theWrappedObject, const QString&  port)
{
  return ( theWrappedObject->soundSensor(port));
}

void PythonQtWrapper_trikControl__BrickInterface::stop(trikControl::BrickInterface* theWrappedObject)
{
  ( theWrappedObject->stop());
}

void PythonQtWrapper_trikControl__BrickInterface::stopEventDevice(trikControl::BrickInterface* theWrappedObject, const QString&  deviceFile)
{
  ( theWrappedObject->stopEventDevice(deviceFile));
}

void PythonQtWrapper_trikControl__BrickInterface::stopped(trikControl::BrickInterface* theWrappedObject)
{
  ( theWrappedObject->stopped());
}

QString  PythonQtWrapper_trikControl__BrickInterface::static_trikControl__BrickInterface_tr(const char*  s, const char*  c, int  n)
{
  return (trikControl::BrickInterface::tr(s, c, n));
}

QString  PythonQtWrapper_trikControl__BrickInterface::static_trikControl__BrickInterface_trUtf8(const char*  s, const char*  c, int  n)
{
  return (trikControl::BrickInterface::trUtf8(s, c, n));
}





void PythonQtWrapper_trikControl__ColorSensorInterface::init(trikControl::ColorSensorInterface* theWrappedObject, bool  showOnDisplay)
{
  ( theWrappedObject->init(showOnDisplay));
}

const QMetaObject*  PythonQtWrapper_trikControl__ColorSensorInterface::metaObject(trikControl::ColorSensorInterface* theWrappedObject) const
{
  return ( theWrappedObject->metaObject());
}

void*  PythonQtWrapper_trikControl__ColorSensorInterface::qt_metacast(trikControl::ColorSensorInterface* theWrappedObject, const char*  arg__1)
{
  return ( theWrappedObject->qt_metacast(arg__1));
}

QVector<int >  PythonQtWrapper_trikControl__ColorSensorInterface::read(trikControl::ColorSensorInterface* theWrappedObject, int  m, int  n)
{
  return ( theWrappedObject->read(m, n));
}

void PythonQtWrapper_trikControl__ColorSensorInterface::stop(trikControl::ColorSensorInterface* theWrappedObject)
{
  ( theWrappedObject->stop());
}

void PythonQtWrapper_trikControl__ColorSensorInterface::stopped(trikControl::ColorSensorInterface* theWrappedObject)
{
  ( theWrappedObject->stopped());
}

QString  PythonQtWrapper_trikControl__ColorSensorInterface::static_trikControl__ColorSensorInterface_tr(const char*  s, const char*  c, int  n)
{
  return (trikControl::ColorSensorInterface::tr(s, c, n));
}

QString  PythonQtWrapper_trikControl__ColorSensorInterface::static_trikControl__ColorSensorInterface_trUtf8(const char*  s, const char*  c, int  n)
{
  return (trikControl::ColorSensorInterface::trUtf8(s, c, n));
}



trikControl::DeviceInterface::Status  PythonQtWrapper_trikControl__DeviceInterface::static_trikControl__DeviceInterface_combine(const trikControl::DeviceInterface&  underlying, const trikControl::DeviceInterface::Status&  dependent)
{
  return (trikControl::DeviceInterface::combine(underlying, dependent));
}

trikControl::DeviceInterface::Status  PythonQtWrapper_trikControl__DeviceInterface::status(trikControl::DeviceInterface* theWrappedObject) const
{
  return ( theWrappedObject->status());
}



void PythonQtWrapper_trikControl__DisplayInterface::addLabel(trikControl::DisplayInterface* theWrappedObject, const QString&  text, int  x, int  y, int  fontSize)
{
  ( theWrappedObject->addLabel(text, x, y, fontSize));
}

void PythonQtWrapper_trikControl__DisplayInterface::clear(trikControl::DisplayInterface* theWrappedObject)
{
  ( theWrappedObject->clear());
}

void PythonQtWrapper_trikControl__DisplayInterface::drawArc(trikControl::DisplayInterface* theWrappedObject, int  x, int  y, int  width, int  height, int  startAngle, int  spanAngle)
{
  ( theWrappedObject->drawArc(x, y, width, height, startAngle, spanAngle));
}

void PythonQtWrapper_trikControl__DisplayInterface::drawEllipse(trikControl::DisplayInterface* theWrappedObject, int  x, int  y, int  width, int  height, bool  filled)
{
  ( theWrappedObject->drawEllipse(x, y, width, height, filled));
}

void PythonQtWrapper_trikControl__DisplayInterface::drawLine(trikControl::DisplayInterface* theWrappedObject, int  x1, int  y1, int  x2, int  y2)
{
  ( theWrappedObject->drawLine(x1, y1, x2, y2));
}

void PythonQtWrapper_trikControl__DisplayInterface::drawPoint(trikControl::DisplayInterface* theWrappedObject, int  x, int  y)
{
  ( theWrappedObject->drawPoint(x, y));
}

void PythonQtWrapper_trikControl__DisplayInterface::drawRect(trikControl::DisplayInterface* theWrappedObject, int  x, int  y, int  width, int  height, bool  filled)
{
  ( theWrappedObject->drawRect(x, y, width, height, filled));
}

trikControl::DisplayWidgetInterface*  PythonQtWrapper_trikControl__DisplayInterface::graphicsWidget(trikControl::DisplayInterface* theWrappedObject)
{
  return &( theWrappedObject->graphicsWidget());
}

void PythonQtWrapper_trikControl__DisplayInterface::hide(trikControl::DisplayInterface* theWrappedObject)
{
  ( theWrappedObject->hide());
}

const QMetaObject*  PythonQtWrapper_trikControl__DisplayInterface::metaObject(trikControl::DisplayInterface* theWrappedObject) const
{
  return ( theWrappedObject->metaObject());
}

void*  PythonQtWrapper_trikControl__DisplayInterface::qt_metacast(trikControl::DisplayInterface* theWrappedObject, const char*  arg__1)
{
  return ( theWrappedObject->qt_metacast(arg__1));
}

void PythonQtWrapper_trikControl__DisplayInterface::redraw(trikControl::DisplayInterface* theWrappedObject)
{
  ( theWrappedObject->redraw());
}

void PythonQtWrapper_trikControl__DisplayInterface::removeLabels(trikControl::DisplayInterface* theWrappedObject)
{
  ( theWrappedObject->removeLabels());
}

void PythonQtWrapper_trikControl__DisplayInterface::reset(trikControl::DisplayInterface* theWrappedObject)
{
  ( theWrappedObject->reset());
}

void PythonQtWrapper_trikControl__DisplayInterface::setBackground(trikControl::DisplayInterface* theWrappedObject, const QString&  color)
{
  ( theWrappedObject->setBackground(color));
}

void PythonQtWrapper_trikControl__DisplayInterface::setPainterColor(trikControl::DisplayInterface* theWrappedObject, const QString&  color)
{
  ( theWrappedObject->setPainterColor(color));
}

void PythonQtWrapper_trikControl__DisplayInterface::setPainterWidth(trikControl::DisplayInterface* theWrappedObject, int  penWidth)
{
  ( theWrappedObject->setPainterWidth(penWidth));
}

void PythonQtWrapper_trikControl__DisplayInterface::showImage(trikControl::DisplayInterface* theWrappedObject, const QString&  fileName)
{
  ( theWrappedObject->showImage(fileName));
}

QString  PythonQtWrapper_trikControl__DisplayInterface::static_trikControl__DisplayInterface_tr(const char*  s, const char*  c, int  n)
{
  return (trikControl::DisplayInterface::tr(s, c, n));
}

QString  PythonQtWrapper_trikControl__DisplayInterface::static_trikControl__DisplayInterface_trUtf8(const char*  s, const char*  c, int  n)
{
  return (trikControl::DisplayInterface::trUtf8(s, c, n));
}



trikControl::DisplayWidgetInterface* PythonQtWrapper_trikControl__DisplayWidgetInterface::new_trikControl__DisplayWidgetInterface()
{ 
return new trikControl::DisplayWidgetInterface(); }

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



const QMetaObject*  PythonQtWrapper_trikControl__EncoderInterface::metaObject(trikControl::EncoderInterface* theWrappedObject) const
{
  return ( theWrappedObject->metaObject());
}

void*  PythonQtWrapper_trikControl__EncoderInterface::qt_metacast(trikControl::EncoderInterface* theWrappedObject, const char*  arg__1)
{
  return ( theWrappedObject->qt_metacast(arg__1));
}

int  PythonQtWrapper_trikControl__EncoderInterface::read(trikControl::EncoderInterface* theWrappedObject)
{
  return ( theWrappedObject->read());
}

int  PythonQtWrapper_trikControl__EncoderInterface::readRawData(trikControl::EncoderInterface* theWrappedObject)
{
  return ( theWrappedObject->readRawData());
}

void PythonQtWrapper_trikControl__EncoderInterface::reset(trikControl::EncoderInterface* theWrappedObject)
{
  ( theWrappedObject->reset());
}

QString  PythonQtWrapper_trikControl__EncoderInterface::static_trikControl__EncoderInterface_tr(const char*  s, const char*  c, int  n)
{
  return (trikControl::EncoderInterface::tr(s, c, n));
}

QString  PythonQtWrapper_trikControl__EncoderInterface::static_trikControl__EncoderInterface_trUtf8(const char*  s, const char*  c, int  n)
{
  return (trikControl::EncoderInterface::trUtf8(s, c, n));
}



trikControl::EventCodeInterface* PythonQtWrapper_trikControl__EventCodeInterface::new_trikControl__EventCodeInterface()
{ 
return new trikControl::EventCodeInterface(); }

const QMetaObject*  PythonQtWrapper_trikControl__EventCodeInterface::metaObject(trikControl::EventCodeInterface* theWrappedObject) const
{
  return ( theWrappedObject->metaObject());
}

void PythonQtWrapper_trikControl__EventCodeInterface::on(trikControl::EventCodeInterface* theWrappedObject, int  value, int  eventTime)
{
  ( theWrappedObject->on(value, eventTime));
}

void*  PythonQtWrapper_trikControl__EventCodeInterface::qt_metacast(trikControl::EventCodeInterface* theWrappedObject, const char*  arg__1)
{
  return ( theWrappedObject->qt_metacast(arg__1));
}

QString  PythonQtWrapper_trikControl__EventCodeInterface::static_trikControl__EventCodeInterface_tr(const char*  s, const char*  c, int  n)
{
  return (trikControl::EventCodeInterface::tr(s, c, n));
}

QString  PythonQtWrapper_trikControl__EventCodeInterface::static_trikControl__EventCodeInterface_trUtf8(const char*  s, const char*  c, int  n)
{
  return (trikControl::EventCodeInterface::trUtf8(s, c, n));
}



const QMetaObject*  PythonQtWrapper_trikControl__EventDeviceInterface::metaObject(trikControl::EventDeviceInterface* theWrappedObject) const
{
  return ( theWrappedObject->metaObject());
}

void PythonQtWrapper_trikControl__EventDeviceInterface::on(trikControl::EventDeviceInterface* theWrappedObject, int  event, int  code, int  value, int  eventTime)
{
  ( theWrappedObject->on(event, code, value, eventTime));
}

trikControl::EventInterface*  PythonQtWrapper_trikControl__EventDeviceInterface::onEvent(trikControl::EventDeviceInterface* theWrappedObject, int  eventType)
{
  return ( theWrappedObject->onEvent(eventType));
}

void*  PythonQtWrapper_trikControl__EventDeviceInterface::qt_metacast(trikControl::EventDeviceInterface* theWrappedObject, const char*  arg__1)
{
  return ( theWrappedObject->qt_metacast(arg__1));
}

QString  PythonQtWrapper_trikControl__EventDeviceInterface::static_trikControl__EventDeviceInterface_tr(const char*  s, const char*  c, int  n)
{
  return (trikControl::EventDeviceInterface::tr(s, c, n));
}

QString  PythonQtWrapper_trikControl__EventDeviceInterface::static_trikControl__EventDeviceInterface_trUtf8(const char*  s, const char*  c, int  n)
{
  return (trikControl::EventDeviceInterface::trUtf8(s, c, n));
}



trikControl::EventCodeInterface*  PythonQtWrapper_trikControl__EventInterface::code(trikControl::EventInterface* theWrappedObject, int  code)
{
  return ( theWrappedObject->code(code));
}

const QMetaObject*  PythonQtWrapper_trikControl__EventInterface::metaObject(trikControl::EventInterface* theWrappedObject) const
{
  return ( theWrappedObject->metaObject());
}

void PythonQtWrapper_trikControl__EventInterface::on(trikControl::EventInterface* theWrappedObject, int  code, int  value, int  eventTime)
{
  ( theWrappedObject->on(code, value, eventTime));
}

void*  PythonQtWrapper_trikControl__EventInterface::qt_metacast(trikControl::EventInterface* theWrappedObject, const char*  arg__1)
{
  return ( theWrappedObject->qt_metacast(arg__1));
}

QString  PythonQtWrapper_trikControl__EventInterface::static_trikControl__EventInterface_tr(const char*  s, const char*  c, int  n)
{
  return (trikControl::EventInterface::tr(s, c, n));
}

QString  PythonQtWrapper_trikControl__EventInterface::static_trikControl__EventInterface_trUtf8(const char*  s, const char*  c, int  n)
{
  return (trikControl::EventInterface::trUtf8(s, c, n));
}



bool  PythonQtWrapper_trikControl__FifoInterface::hasData(trikControl::FifoInterface* theWrappedObject) const
{
  return ( theWrappedObject->hasData());
}

bool  PythonQtWrapper_trikControl__FifoInterface::hasLine(trikControl::FifoInterface* theWrappedObject) const
{
  return ( theWrappedObject->hasLine());
}

const QMetaObject*  PythonQtWrapper_trikControl__FifoInterface::metaObject(trikControl::FifoInterface* theWrappedObject) const
{
  return ( theWrappedObject->metaObject());
}

void PythonQtWrapper_trikControl__FifoInterface::newLine(trikControl::FifoInterface* theWrappedObject, const QString&  data)
{
  ( theWrappedObject->newLine(data));
}

void*  PythonQtWrapper_trikControl__FifoInterface::qt_metacast(trikControl::FifoInterface* theWrappedObject, const char*  arg__1)
{
  return ( theWrappedObject->qt_metacast(arg__1));
}

QString  PythonQtWrapper_trikControl__FifoInterface::read(trikControl::FifoInterface* theWrappedObject)
{
  return ( theWrappedObject->read());
}

QString  PythonQtWrapper_trikControl__FifoInterface::static_trikControl__FifoInterface_tr(const char*  s, const char*  c, int  n)
{
  return (trikControl::FifoInterface::tr(s, c, n));
}

QString  PythonQtWrapper_trikControl__FifoInterface::static_trikControl__FifoInterface_trUtf8(const char*  s, const char*  c, int  n)
{
  return (trikControl::FifoInterface::trUtf8(s, c, n));
}



void PythonQtWrapper_trikControl__GamepadInterface::button(trikControl::GamepadInterface* theWrappedObject, int  button, int  pressed)
{
  ( theWrappedObject->button(button, pressed));
}

bool  PythonQtWrapper_trikControl__GamepadInterface::buttonIsPressed(trikControl::GamepadInterface* theWrappedObject, int  buttonNumber)
{
  return ( theWrappedObject->buttonIsPressed(buttonNumber));
}

bool  PythonQtWrapper_trikControl__GamepadInterface::buttonWasPressed(trikControl::GamepadInterface* theWrappedObject, int  buttonNumber)
{
  return ( theWrappedObject->buttonWasPressed(buttonNumber));
}

void PythonQtWrapper_trikControl__GamepadInterface::connected(trikControl::GamepadInterface* theWrappedObject)
{
  ( theWrappedObject->connected());
}

void PythonQtWrapper_trikControl__GamepadInterface::custom(trikControl::GamepadInterface* theWrappedObject, const QString&  s)
{
  ( theWrappedObject->custom(s));
}

bool  PythonQtWrapper_trikControl__GamepadInterface::disconnect(trikControl::GamepadInterface* theWrappedObject)
{
  return ( theWrappedObject->disconnect());
}

void PythonQtWrapper_trikControl__GamepadInterface::disconnected(trikControl::GamepadInterface* theWrappedObject)
{
  ( theWrappedObject->disconnected());
}

bool  PythonQtWrapper_trikControl__GamepadInterface::isConnected(trikControl::GamepadInterface* theWrappedObject) const
{
  return ( theWrappedObject->isConnected());
}

bool  PythonQtWrapper_trikControl__GamepadInterface::isPadPressed(trikControl::GamepadInterface* theWrappedObject, int  pad) const
{
  return ( theWrappedObject->isPadPressed(pad));
}

const QMetaObject*  PythonQtWrapper_trikControl__GamepadInterface::metaObject(trikControl::GamepadInterface* theWrappedObject) const
{
  return ( theWrappedObject->metaObject());
}

void PythonQtWrapper_trikControl__GamepadInterface::pad(trikControl::GamepadInterface* theWrappedObject, int  pad, int  x, int  y)
{
  ( theWrappedObject->pad(pad, x, y));
}

void PythonQtWrapper_trikControl__GamepadInterface::padUp(trikControl::GamepadInterface* theWrappedObject, int  pad)
{
  ( theWrappedObject->padUp(pad));
}

int  PythonQtWrapper_trikControl__GamepadInterface::padX(trikControl::GamepadInterface* theWrappedObject, int  pad) const
{
  return ( theWrappedObject->padX(pad));
}

int  PythonQtWrapper_trikControl__GamepadInterface::padY(trikControl::GamepadInterface* theWrappedObject, int  pad) const
{
  return ( theWrappedObject->padY(pad));
}

void*  PythonQtWrapper_trikControl__GamepadInterface::qt_metacast(trikControl::GamepadInterface* theWrappedObject, const char*  arg__1)
{
  return ( theWrappedObject->qt_metacast(arg__1));
}

void PythonQtWrapper_trikControl__GamepadInterface::reset(trikControl::GamepadInterface* theWrappedObject)
{
  ( theWrappedObject->reset());
}

QString  PythonQtWrapper_trikControl__GamepadInterface::static_trikControl__GamepadInterface_tr(const char*  s, const char*  c, int  n)
{
  return (trikControl::GamepadInterface::tr(s, c, n));
}

QString  PythonQtWrapper_trikControl__GamepadInterface::static_trikControl__GamepadInterface_trUtf8(const char*  s, const char*  c, int  n)
{
  return (trikControl::GamepadInterface::trUtf8(s, c, n));
}

int  PythonQtWrapper_trikControl__GamepadInterface::wheel(trikControl::GamepadInterface* theWrappedObject) const
{
  return ( theWrappedObject->wheel());
}

void PythonQtWrapper_trikControl__GamepadInterface::wheelEvent(trikControl::GamepadInterface* theWrappedObject, int  percent)
{
  ( theWrappedObject->wheelEvent(percent));
}



void PythonQtWrapper_trikControl__GyroSensorInterface::calibrate(trikControl::GyroSensorInterface* theWrappedObject, int  msec)
{
  ( theWrappedObject->calibrate(msec));
}

void PythonQtWrapper_trikControl__GyroSensorInterface::calibrationFinished(trikControl::GyroSensorInterface* theWrappedObject)
{
  ( theWrappedObject->calibrationFinished());
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

void PythonQtWrapper_trikControl__GyroSensorInterface::setCalibrationValues(trikControl::GyroSensorInterface* theWrappedObject, const QVector<int >&  values)
{
  ( theWrappedObject->setCalibrationValues(values));
}

QString  PythonQtWrapper_trikControl__GyroSensorInterface::static_trikControl__GyroSensorInterface_tr(const char*  s, const char*  c, int  n)
{
  return (trikControl::GyroSensorInterface::tr(s, c, n));
}

QString  PythonQtWrapper_trikControl__GyroSensorInterface::static_trikControl__GyroSensorInterface_trUtf8(const char*  s, const char*  c, int  n)
{
  return (trikControl::GyroSensorInterface::trUtf8(s, c, n));
}



const QMetaObject*  PythonQtWrapper_trikControl__I2cDeviceInterface::metaObject(trikControl::I2cDeviceInterface* theWrappedObject) const
{
  return ( theWrappedObject->metaObject());
}

void*  PythonQtWrapper_trikControl__I2cDeviceInterface::qt_metacast(trikControl::I2cDeviceInterface* theWrappedObject, const char*  arg__1)
{
  return ( theWrappedObject->qt_metacast(arg__1));
}

int  PythonQtWrapper_trikControl__I2cDeviceInterface::read(trikControl::I2cDeviceInterface* theWrappedObject, int  reg, const QString&  mode)
{
  return ( theWrappedObject->read(reg, mode));
}

int  PythonQtWrapper_trikControl__I2cDeviceInterface::send(trikControl::I2cDeviceInterface* theWrappedObject, int  reg, int  value, const QString&  mode)
{
  return ( theWrappedObject->send(reg, value, mode));
}

QString  PythonQtWrapper_trikControl__I2cDeviceInterface::static_trikControl__I2cDeviceInterface_tr(const char*  s, const char*  c, int  n)
{
  return (trikControl::I2cDeviceInterface::tr(s, c, n));
}

QString  PythonQtWrapper_trikControl__I2cDeviceInterface::static_trikControl__I2cDeviceInterface_trUtf8(const char*  s, const char*  c, int  n)
{
  return (trikControl::I2cDeviceInterface::trUtf8(s, c, n));
}



void PythonQtWrapper_trikControl__IrCameraInterface::imageUpdated(trikControl::IrCameraInterface* theWrappedObject)
{
  ( theWrappedObject->imageUpdated());
}

void PythonQtWrapper_trikControl__IrCameraInterface::init(trikControl::IrCameraInterface* theWrappedObject)
{
  ( theWrappedObject->init());
}

const QMetaObject*  PythonQtWrapper_trikControl__IrCameraInterface::metaObject(trikControl::IrCameraInterface* theWrappedObject) const
{
  return ( theWrappedObject->metaObject());
}

void*  PythonQtWrapper_trikControl__IrCameraInterface::qt_metacast(trikControl::IrCameraInterface* theWrappedObject, const char*  arg__1)
{
  return ( theWrappedObject->qt_metacast(arg__1));
}

int  PythonQtWrapper_trikControl__IrCameraInterface::readSensor(trikControl::IrCameraInterface* theWrappedObject, int  m, int  n) const
{
  return ( theWrappedObject->readSensor(m, n));
}

void PythonQtWrapper_trikControl__IrCameraInterface::sensorUpdated(trikControl::IrCameraInterface* theWrappedObject)
{
  ( theWrappedObject->sensorUpdated());
}

void PythonQtWrapper_trikControl__IrCameraInterface::stop(trikControl::IrCameraInterface* theWrappedObject)
{
  ( theWrappedObject->stop());
}

void PythonQtWrapper_trikControl__IrCameraInterface::stopped(trikControl::IrCameraInterface* theWrappedObject)
{
  ( theWrappedObject->stopped());
}

QString  PythonQtWrapper_trikControl__IrCameraInterface::static_trikControl__IrCameraInterface_tr(const char*  s, const char*  c, int  n)
{
  return (trikControl::IrCameraInterface::tr(s, c, n));
}

QString  PythonQtWrapper_trikControl__IrCameraInterface::static_trikControl__IrCameraInterface_trUtf8(const char*  s, const char*  c, int  n)
{
  return (trikControl::IrCameraInterface::trUtf8(s, c, n));
}



int  PythonQtWrapper_trikControl__KeysInterface::buttonCode(trikControl::KeysInterface* theWrappedObject, bool  wait)
{
  return ( theWrappedObject->buttonCode(wait));
}

void PythonQtWrapper_trikControl__KeysInterface::buttonPressed(trikControl::KeysInterface* theWrappedObject, int  code, int  value)
{
  ( theWrappedObject->buttonPressed(code, value));
}

bool  PythonQtWrapper_trikControl__KeysInterface::isPressed(trikControl::KeysInterface* theWrappedObject, int  code)
{
  return ( theWrappedObject->isPressed(code));
}

const QMetaObject*  PythonQtWrapper_trikControl__KeysInterface::metaObject(trikControl::KeysInterface* theWrappedObject) const
{
  return ( theWrappedObject->metaObject());
}

void*  PythonQtWrapper_trikControl__KeysInterface::qt_metacast(trikControl::KeysInterface* theWrappedObject, const char*  arg__1)
{
  return ( theWrappedObject->qt_metacast(arg__1));
}

void PythonQtWrapper_trikControl__KeysInterface::reset(trikControl::KeysInterface* theWrappedObject)
{
  ( theWrappedObject->reset());
}

QString  PythonQtWrapper_trikControl__KeysInterface::static_trikControl__KeysInterface_tr(const char*  s, const char*  c, int  n)
{
  return (trikControl::KeysInterface::tr(s, c, n));
}

QString  PythonQtWrapper_trikControl__KeysInterface::static_trikControl__KeysInterface_trUtf8(const char*  s, const char*  c, int  n)
{
  return (trikControl::KeysInterface::trUtf8(s, c, n));
}

bool  PythonQtWrapper_trikControl__KeysInterface::wasPressed(trikControl::KeysInterface* theWrappedObject, int  code)
{
  return ( theWrappedObject->wasPressed(code));
}



void PythonQtWrapper_trikControl__LedInterface::green(trikControl::LedInterface* theWrappedObject)
{
  ( theWrappedObject->green());
}

const QMetaObject*  PythonQtWrapper_trikControl__LedInterface::metaObject(trikControl::LedInterface* theWrappedObject) const
{
  return ( theWrappedObject->metaObject());
}

void PythonQtWrapper_trikControl__LedInterface::off(trikControl::LedInterface* theWrappedObject)
{
  ( theWrappedObject->off());
}

void PythonQtWrapper_trikControl__LedInterface::orange(trikControl::LedInterface* theWrappedObject)
{
  ( theWrappedObject->orange());
}

void*  PythonQtWrapper_trikControl__LedInterface::qt_metacast(trikControl::LedInterface* theWrappedObject, const char*  arg__1)
{
  return ( theWrappedObject->qt_metacast(arg__1));
}

void PythonQtWrapper_trikControl__LedInterface::red(trikControl::LedInterface* theWrappedObject)
{
  ( theWrappedObject->red());
}

QString  PythonQtWrapper_trikControl__LedInterface::static_trikControl__LedInterface_tr(const char*  s, const char*  c, int  n)
{
  return (trikControl::LedInterface::tr(s, c, n));
}

QString  PythonQtWrapper_trikControl__LedInterface::static_trikControl__LedInterface_trUtf8(const char*  s, const char*  c, int  n)
{
  return (trikControl::LedInterface::trUtf8(s, c, n));
}



QString  PythonQtWrapper_trikControl__LidarInterface::static_trikControl__LidarInterface_tr(const char*  s, const char*  c, int  n)
{
  return (trikControl::LidarInterface::tr(s, c, n));
}

QString  PythonQtWrapper_trikControl__LidarInterface::static_trikControl__LidarInterface_trUtf8(const char*  s, const char*  c, int  n)
{
  return (trikControl::LidarInterface::trUtf8(s, c, n));
}



void PythonQtWrapper_trikControl__LineSensorInterface::detect(trikControl::LineSensorInterface* theWrappedObject)
{
  ( theWrappedObject->detect());
}

QVector<int >  PythonQtWrapper_trikControl__LineSensorInterface::getDetectParameters(trikControl::LineSensorInterface* theWrappedObject) const
{
  return ( theWrappedObject->getDetectParameters());
}

void PythonQtWrapper_trikControl__LineSensorInterface::init(trikControl::LineSensorInterface* theWrappedObject, bool  showOnDisplay)
{
  ( theWrappedObject->init(showOnDisplay));
}

const QMetaObject*  PythonQtWrapper_trikControl__LineSensorInterface::metaObject(trikControl::LineSensorInterface* theWrappedObject) const
{
  return ( theWrappedObject->metaObject());
}

void*  PythonQtWrapper_trikControl__LineSensorInterface::qt_metacast(trikControl::LineSensorInterface* theWrappedObject, const char*  arg__1)
{
  return ( theWrappedObject->qt_metacast(arg__1));
}

QVector<int >  PythonQtWrapper_trikControl__LineSensorInterface::read(trikControl::LineSensorInterface* theWrappedObject)
{
  return ( theWrappedObject->read());
}

void PythonQtWrapper_trikControl__LineSensorInterface::stop(trikControl::LineSensorInterface* theWrappedObject)
{
  ( theWrappedObject->stop());
}

void PythonQtWrapper_trikControl__LineSensorInterface::stopped(trikControl::LineSensorInterface* theWrappedObject)
{
  ( theWrappedObject->stopped());
}

QString  PythonQtWrapper_trikControl__LineSensorInterface::static_trikControl__LineSensorInterface_tr(const char*  s, const char*  c, int  n)
{
  return (trikControl::LineSensorInterface::tr(s, c, n));
}

QString  PythonQtWrapper_trikControl__LineSensorInterface::static_trikControl__LineSensorInterface_trUtf8(const char*  s, const char*  c, int  n)
{
  return (trikControl::LineSensorInterface::trUtf8(s, c, n));
}



void PythonQtWrapper_trikControl__MarkerInterface::down(trikControl::MarkerInterface* theWrappedObject, const QString&  color)
{
  ( theWrappedObject->down(color));
}

bool  PythonQtWrapper_trikControl__MarkerInterface::isDown(trikControl::MarkerInterface* theWrappedObject) const
{
  return ( theWrappedObject->isDown());
}

const QMetaObject*  PythonQtWrapper_trikControl__MarkerInterface::metaObject(trikControl::MarkerInterface* theWrappedObject) const
{
  return ( theWrappedObject->metaObject());
}

void*  PythonQtWrapper_trikControl__MarkerInterface::qt_metacast(trikControl::MarkerInterface* theWrappedObject, const char*  arg__1)
{
  return ( theWrappedObject->qt_metacast(arg__1));
}

void PythonQtWrapper_trikControl__MarkerInterface::setDown(trikControl::MarkerInterface* theWrappedObject, bool  isDown)
{
  ( theWrappedObject->setDown(isDown));
}

QString  PythonQtWrapper_trikControl__MarkerInterface::static_trikControl__MarkerInterface_tr(const char*  s, const char*  c, int  n)
{
  return (trikControl::MarkerInterface::tr(s, c, n));
}

QString  PythonQtWrapper_trikControl__MarkerInterface::static_trikControl__MarkerInterface_trUtf8(const char*  s, const char*  c, int  n)
{
  return (trikControl::MarkerInterface::trUtf8(s, c, n));
}

void PythonQtWrapper_trikControl__MarkerInterface::up(trikControl::MarkerInterface* theWrappedObject)
{
  ( theWrappedObject->up());
}



void PythonQtWrapper_trikControl__MotorInterface::brake(trikControl::MotorInterface* theWrappedObject, int  durationMs)
{
  ( theWrappedObject->brake(durationMs));
}

int  PythonQtWrapper_trikControl__MotorInterface::maxControl(trikControl::MotorInterface* theWrappedObject) const
{
  return ( theWrappedObject->maxControl());
}

const QMetaObject*  PythonQtWrapper_trikControl__MotorInterface::metaObject(trikControl::MotorInterface* theWrappedObject) const
{
  return ( theWrappedObject->metaObject());
}

int  PythonQtWrapper_trikControl__MotorInterface::minControl(trikControl::MotorInterface* theWrappedObject) const
{
  return ( theWrappedObject->minControl());
}

int  PythonQtWrapper_trikControl__MotorInterface::power(trikControl::MotorInterface* theWrappedObject) const
{
  return ( theWrappedObject->power());
}

void PythonQtWrapper_trikControl__MotorInterface::powerOff(trikControl::MotorInterface* theWrappedObject)
{
  ( theWrappedObject->powerOff());
}

void*  PythonQtWrapper_trikControl__MotorInterface::qt_metacast(trikControl::MotorInterface* theWrappedObject, const char*  arg__1)
{
  return ( theWrappedObject->qt_metacast(arg__1));
}

void PythonQtWrapper_trikControl__MotorInterface::setPeriod(trikControl::MotorInterface* theWrappedObject, int  uSec)
{
  ( theWrappedObject->setPeriod(uSec));
}

void PythonQtWrapper_trikControl__MotorInterface::setPower(trikControl::MotorInterface* theWrappedObject, int  power, bool  constrain)
{
  ( theWrappedObject->setPower(power, constrain));
}

QString  PythonQtWrapper_trikControl__MotorInterface::static_trikControl__MotorInterface_tr(const char*  s, const char*  c, int  n)
{
  return (trikControl::MotorInterface::tr(s, c, n));
}

QString  PythonQtWrapper_trikControl__MotorInterface::static_trikControl__MotorInterface_trUtf8(const char*  s, const char*  c, int  n)
{
  return (trikControl::MotorInterface::trUtf8(s, c, n));
}



void PythonQtWrapper_trikControl__ObjectSensorInterface::detect(trikControl::ObjectSensorInterface* theWrappedObject)
{
  ( theWrappedObject->detect());
}

QVector<int >  PythonQtWrapper_trikControl__ObjectSensorInterface::getDetectParameters(trikControl::ObjectSensorInterface* theWrappedObject) const
{
  return ( theWrappedObject->getDetectParameters());
}

void PythonQtWrapper_trikControl__ObjectSensorInterface::init(trikControl::ObjectSensorInterface* theWrappedObject, bool  showOnDisplay)
{
  ( theWrappedObject->init(showOnDisplay));
}

const QMetaObject*  PythonQtWrapper_trikControl__ObjectSensorInterface::metaObject(trikControl::ObjectSensorInterface* theWrappedObject) const
{
  return ( theWrappedObject->metaObject());
}

void*  PythonQtWrapper_trikControl__ObjectSensorInterface::qt_metacast(trikControl::ObjectSensorInterface* theWrappedObject, const char*  arg__1)
{
  return ( theWrappedObject->qt_metacast(arg__1));
}

QVector<int >  PythonQtWrapper_trikControl__ObjectSensorInterface::read(trikControl::ObjectSensorInterface* theWrappedObject)
{
  return ( theWrappedObject->read());
}

void PythonQtWrapper_trikControl__ObjectSensorInterface::stop(trikControl::ObjectSensorInterface* theWrappedObject)
{
  ( theWrappedObject->stop());
}

void PythonQtWrapper_trikControl__ObjectSensorInterface::stopped(trikControl::ObjectSensorInterface* theWrappedObject)
{
  ( theWrappedObject->stopped());
}

QString  PythonQtWrapper_trikControl__ObjectSensorInterface::static_trikControl__ObjectSensorInterface_tr(const char*  s, const char*  c, int  n)
{
  return (trikControl::ObjectSensorInterface::tr(s, c, n));
}

QString  PythonQtWrapper_trikControl__ObjectSensorInterface::static_trikControl__ObjectSensorInterface_trUtf8(const char*  s, const char*  c, int  n)
{
  return (trikControl::ObjectSensorInterface::trUtf8(s, c, n));
}



long  PythonQtWrapper_trikControl__PwmCaptureInterface::duty(trikControl::PwmCaptureInterface* theWrappedObject)
{
  return ( theWrappedObject->duty());
}

long  PythonQtWrapper_trikControl__PwmCaptureInterface::dutyRaw(trikControl::PwmCaptureInterface* theWrappedObject)
{
  return ( theWrappedObject->dutyRaw());
}

QVector<int >  PythonQtWrapper_trikControl__PwmCaptureInterface::frequency(trikControl::PwmCaptureInterface* theWrappedObject)
{
  return ( theWrappedObject->frequency());
}

long  PythonQtWrapper_trikControl__PwmCaptureInterface::maxValue(trikControl::PwmCaptureInterface* theWrappedObject) const
{
  return ( theWrappedObject->maxValue());
}

const QMetaObject*  PythonQtWrapper_trikControl__PwmCaptureInterface::metaObject(trikControl::PwmCaptureInterface* theWrappedObject) const
{
  return ( theWrappedObject->metaObject());
}

long  PythonQtWrapper_trikControl__PwmCaptureInterface::minValue(trikControl::PwmCaptureInterface* theWrappedObject) const
{
  return ( theWrappedObject->minValue());
}

void*  PythonQtWrapper_trikControl__PwmCaptureInterface::qt_metacast(trikControl::PwmCaptureInterface* theWrappedObject, const char*  arg__1)
{
  return ( theWrappedObject->qt_metacast(arg__1));
}

QString  PythonQtWrapper_trikControl__PwmCaptureInterface::static_trikControl__PwmCaptureInterface_tr(const char*  s, const char*  c, int  n)
{
  return (trikControl::PwmCaptureInterface::tr(s, c, n));
}

QString  PythonQtWrapper_trikControl__PwmCaptureInterface::static_trikControl__PwmCaptureInterface_trUtf8(const char*  s, const char*  c, int  n)
{
  return (trikControl::PwmCaptureInterface::trUtf8(s, c, n));
}



int  PythonQtWrapper_trikControl__SensorInterface::maxValue(trikControl::SensorInterface* theWrappedObject) const
{
  return ( theWrappedObject->maxValue());
}

const QMetaObject*  PythonQtWrapper_trikControl__SensorInterface::metaObject(trikControl::SensorInterface* theWrappedObject) const
{
  return ( theWrappedObject->metaObject());
}

int  PythonQtWrapper_trikControl__SensorInterface::minValue(trikControl::SensorInterface* theWrappedObject) const
{
  return ( theWrappedObject->minValue());
}

void*  PythonQtWrapper_trikControl__SensorInterface::qt_metacast(trikControl::SensorInterface* theWrappedObject, const char*  arg__1)
{
  return ( theWrappedObject->qt_metacast(arg__1));
}

int  PythonQtWrapper_trikControl__SensorInterface::read(trikControl::SensorInterface* theWrappedObject)
{
  return ( theWrappedObject->read());
}

int  PythonQtWrapper_trikControl__SensorInterface::readRawData(trikControl::SensorInterface* theWrappedObject)
{
  return ( theWrappedObject->readRawData());
}

QString  PythonQtWrapper_trikControl__SensorInterface::static_trikControl__SensorInterface_tr(const char*  s, const char*  c, int  n)
{
  return (trikControl::SensorInterface::tr(s, c, n));
}

QString  PythonQtWrapper_trikControl__SensorInterface::static_trikControl__SensorInterface_trUtf8(const char*  s, const char*  c, int  n)
{
  return (trikControl::SensorInterface::trUtf8(s, c, n));
}



void PythonQtWrapper_trikControl__SoundSensorInterface::detect(trikControl::SoundSensorInterface* theWrappedObject)
{
  ( theWrappedObject->detect());
}

void PythonQtWrapper_trikControl__SoundSensorInterface::init(trikControl::SoundSensorInterface* theWrappedObject, bool  showOnDisplay)
{
  ( theWrappedObject->init(showOnDisplay));
}

const QMetaObject*  PythonQtWrapper_trikControl__SoundSensorInterface::metaObject(trikControl::SoundSensorInterface* theWrappedObject) const
{
  return ( theWrappedObject->metaObject());
}

void*  PythonQtWrapper_trikControl__SoundSensorInterface::qt_metacast(trikControl::SoundSensorInterface* theWrappedObject, const char*  arg__1)
{
  return ( theWrappedObject->qt_metacast(arg__1));
}

QVector<int >  PythonQtWrapper_trikControl__SoundSensorInterface::read(trikControl::SoundSensorInterface* theWrappedObject)
{
  return ( theWrappedObject->read());
}

void PythonQtWrapper_trikControl__SoundSensorInterface::stop(trikControl::SoundSensorInterface* theWrappedObject)
{
  ( theWrappedObject->stop());
}

void PythonQtWrapper_trikControl__SoundSensorInterface::stopped(trikControl::SoundSensorInterface* theWrappedObject)
{
  ( theWrappedObject->stopped());
}

QString  PythonQtWrapper_trikControl__SoundSensorInterface::static_trikControl__SoundSensorInterface_tr(const char*  s, const char*  c, int  n)
{
  return (trikControl::SoundSensorInterface::tr(s, c, n));
}

QString  PythonQtWrapper_trikControl__SoundSensorInterface::static_trikControl__SoundSensorInterface_trUtf8(const char*  s, const char*  c, int  n)
{
  return (trikControl::SoundSensorInterface::trUtf8(s, c, n));
}

void PythonQtWrapper_trikControl__SoundSensorInterface::volume(trikControl::SoundSensorInterface* theWrappedObject, int  volCoeff)
{
  ( theWrappedObject->volume(volCoeff));
}



const QMetaObject*  PythonQtWrapper_trikControl__VectorSensorInterface::metaObject(trikControl::VectorSensorInterface* theWrappedObject) const
{
  return ( theWrappedObject->metaObject());
}

void PythonQtWrapper_trikControl__VectorSensorInterface::newData(trikControl::VectorSensorInterface* theWrappedObject, QVector<int >  reading, const trikKernel::TimeVal&  eventTime)
{
  ( theWrappedObject->newData(reading, eventTime));
}

void*  PythonQtWrapper_trikControl__VectorSensorInterface::qt_metacast(trikControl::VectorSensorInterface* theWrappedObject, const char*  arg__1)
{
  return ( theWrappedObject->qt_metacast(arg__1));
}

QVector<int >  PythonQtWrapper_trikControl__VectorSensorInterface::read(trikControl::VectorSensorInterface* theWrappedObject) const
{
  return ( theWrappedObject->read());
}

QString  PythonQtWrapper_trikControl__VectorSensorInterface::static_trikControl__VectorSensorInterface_tr(const char*  s, const char*  c, int  n)
{
  return (trikControl::VectorSensorInterface::tr(s, c, n));
}

QString  PythonQtWrapper_trikControl__VectorSensorInterface::static_trikControl__VectorSensorInterface_trUtf8(const char*  s, const char*  c, int  n)
{
  return (trikControl::VectorSensorInterface::trUtf8(s, c, n));
}


