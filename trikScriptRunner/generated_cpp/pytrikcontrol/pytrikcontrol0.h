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
#include <PythonQt.h>
#include <QObject>
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



class PythonQtWrapper_trikControl : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
};





class PythonQtPublicPromoter_trikControl__BatteryInterface : public trikControl::BatteryInterface
{ public:
inline const QMetaObject*  py_q_metaObject() const { return trikControl::BatteryInterface::metaObject(); }
inline void*  py_q_qt_metacast(const char*  arg__1) { return trikControl::BatteryInterface::qt_metacast(arg__1); }
inline float  py_q_readRawDataVoltage() { return this->readRawDataVoltage(); }
inline float  py_q_readVoltage() { return this->readVoltage(); }
};

class PythonQtWrapper_trikControl__BatteryInterface : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
void delete_trikControl__BatteryInterface(trikControl::BatteryInterface* obj) { delete obj; }
   const QMetaObject*  metaObject(trikControl::BatteryInterface* theWrappedObject) const;
   const QMetaObject*  py_q_metaObject(trikControl::BatteryInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__BatteryInterface*)theWrappedObject)->py_q_metaObject());}
   void*  qt_metacast(trikControl::BatteryInterface* theWrappedObject, const char*  arg__1);
   void*  py_q_qt_metacast(trikControl::BatteryInterface* theWrappedObject, const char*  arg__1){  return (((PythonQtPublicPromoter_trikControl__BatteryInterface*)theWrappedObject)->py_q_qt_metacast(arg__1));}
   float  readRawDataVoltage(trikControl::BatteryInterface* theWrappedObject);
   float  py_q_readRawDataVoltage(trikControl::BatteryInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__BatteryInterface*)theWrappedObject)->py_q_readRawDataVoltage());}
   float  readVoltage(trikControl::BatteryInterface* theWrappedObject);
   float  py_q_readVoltage(trikControl::BatteryInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__BatteryInterface*)theWrappedObject)->py_q_readVoltage());}
   QString  static_trikControl__BatteryInterface_tr(const char*  s, const char*  c = nullptr, int  n = -1);
   QString  static_trikControl__BatteryInterface_trUtf8(const char*  s, const char*  c = nullptr, int  n = -1);
};





class PythonQtWrapper_trikControl__BrickFactory : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
trikControl::BrickFactory* new_trikControl__BrickFactory();
void delete_trikControl__BrickFactory(trikControl::BrickFactory* obj) { delete obj; }
   trikControl::BrickInterface*  static_trikControl__BrickFactory_create(const QString&  configFilesPath = ".", const QString&  mediaPath = ".");
   trikControl::BrickInterface*  static_trikControl__BrickFactory_create(const QString&  systemConfig, const QString&  modelConfig, const QString&  mediaPath);
};





class PythonQtPublicPromoter_trikControl__BrickInterface : public trikControl::BrickInterface
{ public:
inline trikControl::VectorSensorInterface*  py_q_accelerometer() { return this->accelerometer(); }
inline trikControl::BatteryInterface*  py_q_battery() { return this->battery(); }
inline trikControl::ColorSensorInterface*  py_q_colorSensor(const QString&  port) { return this->colorSensor(port); }
inline QString  py_q_configVersion() const { return this->configVersion(); }
inline void py_q_configure(const QString&  portName, const QString&  deviceName) { this->configure(portName, deviceName); }
inline trikControl::DisplayInterface*  py_q_display() { return this->display(); }
inline trikControl::EncoderInterface*  py_q_encoder(const QString&  port) { return this->encoder(port); }
inline QStringList  py_q_encoderPorts() const { return this->encoderPorts(); }
inline trikControl::EventDeviceInterface*  py_q_eventDevice(const QString&  deviceFile) { return this->eventDevice(deviceFile); }
inline trikControl::FifoInterface*  py_q_fifo(const QString&  port) { return this->fifo(port); }
inline trikControl::GamepadInterface*  py_q_gamepad() { return this->gamepad(); }
inline trikControl::DisplayWidgetInterface*  py_q_graphicsWidget() { return this->graphicsWidget(); }
inline trikControl::GyroSensorInterface*  py_q_gyroscope() { return this->gyroscope(); }
inline trikControl::I2cDeviceInterface*  py_q_i2c(int  bus, int  address, int  regSize = 1) { return this->i2c(bus, address, regSize); }
inline trikControl::IrCameraInterface*  py_q_irCamera() { return this->irCamera(); }
inline trikControl::KeysInterface*  py_q_keys() { return this->keys(); }
inline trikControl::LedInterface*  py_q_led() { return this->led(); }
inline trikControl::LidarInterface*  py_q_lidar() { return this->lidar(); }
inline trikControl::LineSensorInterface*  py_q_lineSensor(const QString&  port) { return this->lineSensor(port); }
inline trikControl::MarkerInterface*  py_q_marker() { return this->marker(); }
inline const QMetaObject*  py_q_metaObject() const { return trikControl::BrickInterface::metaObject(); }
inline trikControl::MotorInterface*  py_q_motor(const QString&  port) { return this->motor(port); }
inline QStringList  py_q_motorPorts(trikControl::MotorInterface::Type  type) const { return this->motorPorts(type); }
inline trikControl::ObjectSensorInterface*  py_q_objectSensor(const QString&  port) { return this->objectSensor(port); }
inline void py_q_playSound(const QString&  soundFileName) { this->playSound(soundFileName); }
inline void py_q_playTone(int  hzFreq, int  msDuration) { this->playTone(hzFreq, msDuration); }
inline trikControl::PwmCaptureInterface*  py_q_pwmCapture(const QString&  port) { return this->pwmCapture(port); }
inline QStringList  py_q_pwmCapturePorts() const { return this->pwmCapturePorts(); }
inline void*  py_q_qt_metacast(const char*  arg__1) { return trikControl::BrickInterface::qt_metacast(arg__1); }
inline void py_q_reset() { this->reset(); }
inline void py_q_say(const QString&  text) { this->say(text); }
inline trikControl::SensorInterface*  py_q_sensor(const QString&  port) { return this->sensor(port); }
inline QStringList  py_q_sensorPorts(trikControl::SensorInterface::Type  type) const { return this->sensorPorts(type); }
inline trikControl::I2cDeviceInterface*  py_q_smBusI2c(int  bus, int  address) { return this->smBusI2c(bus, address); }
inline trikControl::SoundSensorInterface*  py_q_soundSensor(const QString&  port) { return this->soundSensor(port); }
inline void py_q_stop() { this->stop(); }
inline void py_q_stopEventDevice(const QString&  deviceFile) { this->stopEventDevice(deviceFile); }
};

class PythonQtWrapper_trikControl__BrickInterface : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
void delete_trikControl__BrickInterface(trikControl::BrickInterface* obj) { delete obj; }
   trikControl::VectorSensorInterface*  accelerometer(trikControl::BrickInterface* theWrappedObject);
   trikControl::VectorSensorInterface*  py_q_accelerometer(trikControl::BrickInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_accelerometer());}
   trikControl::BatteryInterface*  battery(trikControl::BrickInterface* theWrappedObject);
   trikControl::BatteryInterface*  py_q_battery(trikControl::BrickInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_battery());}
   trikControl::ColorSensorInterface*  colorSensor(trikControl::BrickInterface* theWrappedObject, const QString&  port);
   trikControl::ColorSensorInterface*  py_q_colorSensor(trikControl::BrickInterface* theWrappedObject, const QString&  port){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_colorSensor(port));}
   QString  configVersion(trikControl::BrickInterface* theWrappedObject) const;
   QString  py_q_configVersion(trikControl::BrickInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_configVersion());}
   void configure(trikControl::BrickInterface* theWrappedObject, const QString&  portName, const QString&  deviceName);
   void py_q_configure(trikControl::BrickInterface* theWrappedObject, const QString&  portName, const QString&  deviceName){  (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_configure(portName, deviceName));}
   trikControl::DisplayInterface*  display(trikControl::BrickInterface* theWrappedObject);
   trikControl::DisplayInterface*  py_q_display(trikControl::BrickInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_display());}
   trikControl::EncoderInterface*  encoder(trikControl::BrickInterface* theWrappedObject, const QString&  port);
   trikControl::EncoderInterface*  py_q_encoder(trikControl::BrickInterface* theWrappedObject, const QString&  port){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_encoder(port));}
   QStringList  encoderPorts(trikControl::BrickInterface* theWrappedObject) const;
   QStringList  py_q_encoderPorts(trikControl::BrickInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_encoderPorts());}
   trikControl::EventDeviceInterface*  eventDevice(trikControl::BrickInterface* theWrappedObject, const QString&  deviceFile);
   trikControl::EventDeviceInterface*  py_q_eventDevice(trikControl::BrickInterface* theWrappedObject, const QString&  deviceFile){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_eventDevice(deviceFile));}
   trikControl::FifoInterface*  fifo(trikControl::BrickInterface* theWrappedObject, const QString&  port);
   trikControl::FifoInterface*  py_q_fifo(trikControl::BrickInterface* theWrappedObject, const QString&  port){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_fifo(port));}
   trikControl::GamepadInterface*  gamepad(trikControl::BrickInterface* theWrappedObject);
   trikControl::GamepadInterface*  py_q_gamepad(trikControl::BrickInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_gamepad());}
   trikControl::DisplayWidgetInterface*  graphicsWidget(trikControl::BrickInterface* theWrappedObject);
   trikControl::DisplayWidgetInterface*  py_q_graphicsWidget(trikControl::BrickInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_graphicsWidget());}
   trikControl::GyroSensorInterface*  gyroscope(trikControl::BrickInterface* theWrappedObject);
   trikControl::GyroSensorInterface*  py_q_gyroscope(trikControl::BrickInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_gyroscope());}
   trikControl::I2cDeviceInterface*  i2c(trikControl::BrickInterface* theWrappedObject, int  bus, int  address, int  regSize = 1);
   trikControl::I2cDeviceInterface*  py_q_i2c(trikControl::BrickInterface* theWrappedObject, int  bus, int  address, int  regSize = 1){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_i2c(bus, address, regSize));}
   trikControl::IrCameraInterface*  irCamera(trikControl::BrickInterface* theWrappedObject);
   trikControl::IrCameraInterface*  py_q_irCamera(trikControl::BrickInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_irCamera());}
   trikControl::KeysInterface*  keys(trikControl::BrickInterface* theWrappedObject);
   trikControl::KeysInterface*  py_q_keys(trikControl::BrickInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_keys());}
   trikControl::LedInterface*  led(trikControl::BrickInterface* theWrappedObject);
   trikControl::LedInterface*  py_q_led(trikControl::BrickInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_led());}
   trikControl::LidarInterface*  lidar(trikControl::BrickInterface* theWrappedObject);
   trikControl::LidarInterface*  py_q_lidar(trikControl::BrickInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_lidar());}
   trikControl::LineSensorInterface*  lineSensor(trikControl::BrickInterface* theWrappedObject, const QString&  port);
   trikControl::LineSensorInterface*  py_q_lineSensor(trikControl::BrickInterface* theWrappedObject, const QString&  port){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_lineSensor(port));}
   trikControl::MarkerInterface*  marker(trikControl::BrickInterface* theWrappedObject);
   trikControl::MarkerInterface*  py_q_marker(trikControl::BrickInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_marker());}
   const QMetaObject*  metaObject(trikControl::BrickInterface* theWrappedObject) const;
   const QMetaObject*  py_q_metaObject(trikControl::BrickInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_metaObject());}
   trikControl::MotorInterface*  motor(trikControl::BrickInterface* theWrappedObject, const QString&  port);
   trikControl::MotorInterface*  py_q_motor(trikControl::BrickInterface* theWrappedObject, const QString&  port){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_motor(port));}
   QStringList  motorPorts(trikControl::BrickInterface* theWrappedObject, trikControl::MotorInterface::Type  type) const;
   QStringList  py_q_motorPorts(trikControl::BrickInterface* theWrappedObject, trikControl::MotorInterface::Type  type) const{  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_motorPorts(type));}
   trikControl::ObjectSensorInterface*  objectSensor(trikControl::BrickInterface* theWrappedObject, const QString&  port);
   trikControl::ObjectSensorInterface*  py_q_objectSensor(trikControl::BrickInterface* theWrappedObject, const QString&  port){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_objectSensor(port));}
   void playSound(trikControl::BrickInterface* theWrappedObject, const QString&  soundFileName);
   void py_q_playSound(trikControl::BrickInterface* theWrappedObject, const QString&  soundFileName){  (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_playSound(soundFileName));}
   void playTone(trikControl::BrickInterface* theWrappedObject, int  hzFreq, int  msDuration);
   void py_q_playTone(trikControl::BrickInterface* theWrappedObject, int  hzFreq, int  msDuration){  (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_playTone(hzFreq, msDuration));}
   trikControl::PwmCaptureInterface*  pwmCapture(trikControl::BrickInterface* theWrappedObject, const QString&  port);
   trikControl::PwmCaptureInterface*  py_q_pwmCapture(trikControl::BrickInterface* theWrappedObject, const QString&  port){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_pwmCapture(port));}
   QStringList  pwmCapturePorts(trikControl::BrickInterface* theWrappedObject) const;
   QStringList  py_q_pwmCapturePorts(trikControl::BrickInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_pwmCapturePorts());}
   void*  qt_metacast(trikControl::BrickInterface* theWrappedObject, const char*  arg__1);
   void*  py_q_qt_metacast(trikControl::BrickInterface* theWrappedObject, const char*  arg__1){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_qt_metacast(arg__1));}
   void reset(trikControl::BrickInterface* theWrappedObject);
   void py_q_reset(trikControl::BrickInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_reset());}
   void resetCompleted(trikControl::BrickInterface* theWrappedObject);
   void say(trikControl::BrickInterface* theWrappedObject, const QString&  text);
   void py_q_say(trikControl::BrickInterface* theWrappedObject, const QString&  text){  (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_say(text));}
   trikControl::SensorInterface*  sensor(trikControl::BrickInterface* theWrappedObject, const QString&  port);
   trikControl::SensorInterface*  py_q_sensor(trikControl::BrickInterface* theWrappedObject, const QString&  port){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_sensor(port));}
   QStringList  sensorPorts(trikControl::BrickInterface* theWrappedObject, trikControl::SensorInterface::Type  type) const;
   QStringList  py_q_sensorPorts(trikControl::BrickInterface* theWrappedObject, trikControl::SensorInterface::Type  type) const{  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_sensorPorts(type));}
   trikControl::I2cDeviceInterface*  smBusI2c(trikControl::BrickInterface* theWrappedObject, int  bus, int  address);
   trikControl::I2cDeviceInterface*  py_q_smBusI2c(trikControl::BrickInterface* theWrappedObject, int  bus, int  address){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_smBusI2c(bus, address));}
   trikControl::SoundSensorInterface*  soundSensor(trikControl::BrickInterface* theWrappedObject, const QString&  port);
   trikControl::SoundSensorInterface*  py_q_soundSensor(trikControl::BrickInterface* theWrappedObject, const QString&  port){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_soundSensor(port));}
   void stop(trikControl::BrickInterface* theWrappedObject);
   void py_q_stop(trikControl::BrickInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_stop());}
   void stopEventDevice(trikControl::BrickInterface* theWrappedObject, const QString&  deviceFile);
   void py_q_stopEventDevice(trikControl::BrickInterface* theWrappedObject, const QString&  deviceFile){  (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_stopEventDevice(deviceFile));}
   void stopped(trikControl::BrickInterface* theWrappedObject);
   QString  static_trikControl__BrickInterface_tr(const char*  s, const char*  c = nullptr, int  n = -1);
   QString  static_trikControl__BrickInterface_trUtf8(const char*  s, const char*  c = nullptr, int  n = -1);
};





class PythonQtWrapper_trikControl__CameraDeviceInterface : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
void delete_trikControl__CameraDeviceInterface(trikControl::CameraDeviceInterface* obj) { delete obj; }
};





class PythonQtPublicPromoter_trikControl__ColorSensorInterface : public trikControl::ColorSensorInterface
{ public:
inline void py_q_init(bool  showOnDisplay) { this->init(showOnDisplay); }
inline const QMetaObject*  py_q_metaObject() const { return trikControl::ColorSensorInterface::metaObject(); }
inline void*  py_q_qt_metacast(const char*  arg__1) { return trikControl::ColorSensorInterface::qt_metacast(arg__1); }
inline QVector<int >  py_q_read(int  m, int  n) { return this->read(m, n); }
inline void py_q_stop() { this->stop(); }
};

class PythonQtWrapper_trikControl__ColorSensorInterface : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
void delete_trikControl__ColorSensorInterface(trikControl::ColorSensorInterface* obj) { delete obj; }
   void init(trikControl::ColorSensorInterface* theWrappedObject, bool  showOnDisplay);
   void py_q_init(trikControl::ColorSensorInterface* theWrappedObject, bool  showOnDisplay){  (((PythonQtPublicPromoter_trikControl__ColorSensorInterface*)theWrappedObject)->py_q_init(showOnDisplay));}
   const QMetaObject*  metaObject(trikControl::ColorSensorInterface* theWrappedObject) const;
   const QMetaObject*  py_q_metaObject(trikControl::ColorSensorInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__ColorSensorInterface*)theWrappedObject)->py_q_metaObject());}
   void*  qt_metacast(trikControl::ColorSensorInterface* theWrappedObject, const char*  arg__1);
   void*  py_q_qt_metacast(trikControl::ColorSensorInterface* theWrappedObject, const char*  arg__1){  return (((PythonQtPublicPromoter_trikControl__ColorSensorInterface*)theWrappedObject)->py_q_qt_metacast(arg__1));}
   QVector<int >  read(trikControl::ColorSensorInterface* theWrappedObject, int  m, int  n);
   QVector<int >  py_q_read(trikControl::ColorSensorInterface* theWrappedObject, int  m, int  n){  return (((PythonQtPublicPromoter_trikControl__ColorSensorInterface*)theWrappedObject)->py_q_read(m, n));}
   void stop(trikControl::ColorSensorInterface* theWrappedObject);
   void py_q_stop(trikControl::ColorSensorInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__ColorSensorInterface*)theWrappedObject)->py_q_stop());}
   void stopped(trikControl::ColorSensorInterface* theWrappedObject);
   QString  static_trikControl__ColorSensorInterface_tr(const char*  s, const char*  c = nullptr, int  n = -1);
   QString  static_trikControl__ColorSensorInterface_trUtf8(const char*  s, const char*  c = nullptr, int  n = -1);
};





class PythonQtPublicPromoter_trikControl__DeviceInterface : public trikControl::DeviceInterface
{ public:
inline trikControl::DeviceInterface::Status  py_q_status() const { return this->status(); }
};

class PythonQtWrapper_trikControl__DeviceInterface : public QObject
{ Q_OBJECT
public:
Q_ENUMS(Status )
enum class Status{
  permanentFailure = static_cast<int>(trikControl::DeviceInterface::Status::permanentFailure),   temporaryFailure = static_cast<int>(trikControl::DeviceInterface::Status::temporaryFailure),   off = static_cast<int>(trikControl::DeviceInterface::Status::off),   starting = static_cast<int>(trikControl::DeviceInterface::Status::starting),   ready = static_cast<int>(trikControl::DeviceInterface::Status::ready),   stopping = static_cast<int>(trikControl::DeviceInterface::Status::stopping)};
public Q_SLOTS:
void delete_trikControl__DeviceInterface(trikControl::DeviceInterface* obj) { delete obj; }
   trikControl::DeviceInterface::Status  static_trikControl__DeviceInterface_combine(const trikControl::DeviceInterface&  underlying, const trikControl::DeviceInterface::Status&  dependent);
   trikControl::DeviceInterface::Status  status(trikControl::DeviceInterface* theWrappedObject) const;
   trikControl::DeviceInterface::Status  py_q_status(trikControl::DeviceInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__DeviceInterface*)theWrappedObject)->py_q_status());}
};





class PythonQtPublicPromoter_trikControl__DisplayInterface : public trikControl::DisplayInterface
{ public:
inline void py_q_addLabel(const QString&  text, int  x, int  y, int  fontSize = -1) { this->addLabel(text, x, y, fontSize); }
inline void py_q_clear() { this->clear(); }
inline void py_q_drawArc(int  x, int  y, int  width, int  height, int  startAngle, int  spanAngle) { this->drawArc(x, y, width, height, startAngle, spanAngle); }
inline void py_q_drawEllipse(int  x, int  y, int  width, int  height, bool  filled = false) { this->drawEllipse(x, y, width, height, filled); }
inline void py_q_drawLine(int  x1, int  y1, int  x2, int  y2) { this->drawLine(x1, y1, x2, y2); }
inline void py_q_drawPoint(int  x, int  y) { this->drawPoint(x, y); }
inline void py_q_drawRect(int  x, int  y, int  width, int  height, bool  filled = false) { this->drawRect(x, y, width, height, filled); }
inline trikControl::DisplayWidgetInterface&  py_q_graphicsWidget() { return this->graphicsWidget(); }
inline void py_q_hide() { this->hide(); }
inline const QMetaObject*  py_q_metaObject() const { return trikControl::DisplayInterface::metaObject(); }
inline void*  py_q_qt_metacast(const char*  arg__1) { return trikControl::DisplayInterface::qt_metacast(arg__1); }
inline void py_q_redraw() { this->redraw(); }
inline void py_q_removeLabels() { this->removeLabels(); }
inline void py_q_reset() { this->reset(); }
inline void py_q_setBackground(const QString&  color) { this->setBackground(color); }
inline void py_q_setPainterColor(const QString&  color) { this->setPainterColor(color); }
inline void py_q_setPainterWidth(int  penWidth) { this->setPainterWidth(penWidth); }
inline void py_q_showImage(const QString&  fileName) { this->showImage(fileName); }
};

class PythonQtWrapper_trikControl__DisplayInterface : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
void delete_trikControl__DisplayInterface(trikControl::DisplayInterface* obj) { delete obj; }
   void addLabel(trikControl::DisplayInterface* theWrappedObject, const QString&  text, int  x, int  y, int  fontSize = -1);
   void py_q_addLabel(trikControl::DisplayInterface* theWrappedObject, const QString&  text, int  x, int  y, int  fontSize = -1){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_addLabel(text, x, y, fontSize));}
   void clear(trikControl::DisplayInterface* theWrappedObject);
   void py_q_clear(trikControl::DisplayInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_clear());}
   void drawArc(trikControl::DisplayInterface* theWrappedObject, int  x, int  y, int  width, int  height, int  startAngle, int  spanAngle);
   void py_q_drawArc(trikControl::DisplayInterface* theWrappedObject, int  x, int  y, int  width, int  height, int  startAngle, int  spanAngle){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_drawArc(x, y, width, height, startAngle, spanAngle));}
   void drawEllipse(trikControl::DisplayInterface* theWrappedObject, int  x, int  y, int  width, int  height, bool  filled = false);
   void py_q_drawEllipse(trikControl::DisplayInterface* theWrappedObject, int  x, int  y, int  width, int  height, bool  filled = false){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_drawEllipse(x, y, width, height, filled));}
   void drawLine(trikControl::DisplayInterface* theWrappedObject, int  x1, int  y1, int  x2, int  y2);
   void py_q_drawLine(trikControl::DisplayInterface* theWrappedObject, int  x1, int  y1, int  x2, int  y2){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_drawLine(x1, y1, x2, y2));}
   void drawPoint(trikControl::DisplayInterface* theWrappedObject, int  x, int  y);
   void py_q_drawPoint(trikControl::DisplayInterface* theWrappedObject, int  x, int  y){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_drawPoint(x, y));}
   void drawRect(trikControl::DisplayInterface* theWrappedObject, int  x, int  y, int  width, int  height, bool  filled = false);
   void py_q_drawRect(trikControl::DisplayInterface* theWrappedObject, int  x, int  y, int  width, int  height, bool  filled = false){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_drawRect(x, y, width, height, filled));}
   trikControl::DisplayWidgetInterface*  graphicsWidget(trikControl::DisplayInterface* theWrappedObject);
   trikControl::DisplayWidgetInterface*  py_q_graphicsWidget(trikControl::DisplayInterface* theWrappedObject){  return &(((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_graphicsWidget());}
   void hide(trikControl::DisplayInterface* theWrappedObject);
   void py_q_hide(trikControl::DisplayInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_hide());}
   const QMetaObject*  metaObject(trikControl::DisplayInterface* theWrappedObject) const;
   const QMetaObject*  py_q_metaObject(trikControl::DisplayInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_metaObject());}
   void*  qt_metacast(trikControl::DisplayInterface* theWrappedObject, const char*  arg__1);
   void*  py_q_qt_metacast(trikControl::DisplayInterface* theWrappedObject, const char*  arg__1){  return (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_qt_metacast(arg__1));}
   void redraw(trikControl::DisplayInterface* theWrappedObject);
   void py_q_redraw(trikControl::DisplayInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_redraw());}
   void removeLabels(trikControl::DisplayInterface* theWrappedObject);
   void py_q_removeLabels(trikControl::DisplayInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_removeLabels());}
   void reset(trikControl::DisplayInterface* theWrappedObject);
   void py_q_reset(trikControl::DisplayInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_reset());}
   void setBackground(trikControl::DisplayInterface* theWrappedObject, const QString&  color);
   void py_q_setBackground(trikControl::DisplayInterface* theWrappedObject, const QString&  color){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_setBackground(color));}
   void setPainterColor(trikControl::DisplayInterface* theWrappedObject, const QString&  color);
   void py_q_setPainterColor(trikControl::DisplayInterface* theWrappedObject, const QString&  color){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_setPainterColor(color));}
   void setPainterWidth(trikControl::DisplayInterface* theWrappedObject, int  penWidth);
   void py_q_setPainterWidth(trikControl::DisplayInterface* theWrappedObject, int  penWidth){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_setPainterWidth(penWidth));}
   void showImage(trikControl::DisplayInterface* theWrappedObject, const QString&  fileName);
   void py_q_showImage(trikControl::DisplayInterface* theWrappedObject, const QString&  fileName){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_showImage(fileName));}
   QString  static_trikControl__DisplayInterface_tr(const char*  s, const char*  c = nullptr, int  n = -1);
   QString  static_trikControl__DisplayInterface_trUtf8(const char*  s, const char*  c = nullptr, int  n = -1);
};





class PythonQtPublicPromoter_trikControl__DisplayWidgetInterface : public trikControl::DisplayWidgetInterface
{ public:
inline const QMetaObject*  py_q_metaObject() const { return trikControl::DisplayWidgetInterface::metaObject(); }
inline void*  py_q_qt_metacast(const char*  arg__1) { return trikControl::DisplayWidgetInterface::qt_metacast(arg__1); }
};

class PythonQtWrapper_trikControl__DisplayWidgetInterface : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
trikControl::DisplayWidgetInterface* new_trikControl__DisplayWidgetInterface();
void delete_trikControl__DisplayWidgetInterface(trikControl::DisplayWidgetInterface* obj) { delete obj; }
   void hidden(trikControl::DisplayWidgetInterface* theWrappedObject);
   const QMetaObject*  metaObject(trikControl::DisplayWidgetInterface* theWrappedObject) const;
   const QMetaObject*  py_q_metaObject(trikControl::DisplayWidgetInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__DisplayWidgetInterface*)theWrappedObject)->py_q_metaObject());}
   void*  qt_metacast(trikControl::DisplayWidgetInterface* theWrappedObject, const char*  arg__1);
   void*  py_q_qt_metacast(trikControl::DisplayWidgetInterface* theWrappedObject, const char*  arg__1){  return (((PythonQtPublicPromoter_trikControl__DisplayWidgetInterface*)theWrappedObject)->py_q_qt_metacast(arg__1));}
   void shown(trikControl::DisplayWidgetInterface* theWrappedObject);
   QString  static_trikControl__DisplayWidgetInterface_tr(const char*  s, const char*  c = nullptr, int  n = -1);
   QString  static_trikControl__DisplayWidgetInterface_trUtf8(const char*  s, const char*  c = nullptr, int  n = -1);
};





class PythonQtPublicPromoter_trikControl__EncoderInterface : public trikControl::EncoderInterface
{ public:
inline const QMetaObject*  py_q_metaObject() const { return trikControl::EncoderInterface::metaObject(); }
inline void*  py_q_qt_metacast(const char*  arg__1) { return trikControl::EncoderInterface::qt_metacast(arg__1); }
inline int  py_q_read() { return this->read(); }
inline int  py_q_readRawData() { return this->readRawData(); }
inline void py_q_reset() { this->reset(); }
};

class PythonQtWrapper_trikControl__EncoderInterface : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
void delete_trikControl__EncoderInterface(trikControl::EncoderInterface* obj) { delete obj; }
   const QMetaObject*  metaObject(trikControl::EncoderInterface* theWrappedObject) const;
   const QMetaObject*  py_q_metaObject(trikControl::EncoderInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__EncoderInterface*)theWrappedObject)->py_q_metaObject());}
   void*  qt_metacast(trikControl::EncoderInterface* theWrappedObject, const char*  arg__1);
   void*  py_q_qt_metacast(trikControl::EncoderInterface* theWrappedObject, const char*  arg__1){  return (((PythonQtPublicPromoter_trikControl__EncoderInterface*)theWrappedObject)->py_q_qt_metacast(arg__1));}
   int  read(trikControl::EncoderInterface* theWrappedObject);
   int  py_q_read(trikControl::EncoderInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__EncoderInterface*)theWrappedObject)->py_q_read());}
   int  readRawData(trikControl::EncoderInterface* theWrappedObject);
   int  py_q_readRawData(trikControl::EncoderInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__EncoderInterface*)theWrappedObject)->py_q_readRawData());}
   void reset(trikControl::EncoderInterface* theWrappedObject);
   void py_q_reset(trikControl::EncoderInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__EncoderInterface*)theWrappedObject)->py_q_reset());}
   QString  static_trikControl__EncoderInterface_tr(const char*  s, const char*  c = nullptr, int  n = -1);
   QString  static_trikControl__EncoderInterface_trUtf8(const char*  s, const char*  c = nullptr, int  n = -1);
};





class PythonQtPublicPromoter_trikControl__EventCodeInterface : public trikControl::EventCodeInterface
{ public:
inline const QMetaObject*  py_q_metaObject() const { return trikControl::EventCodeInterface::metaObject(); }
inline void*  py_q_qt_metacast(const char*  arg__1) { return trikControl::EventCodeInterface::qt_metacast(arg__1); }
};

class PythonQtWrapper_trikControl__EventCodeInterface : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
trikControl::EventCodeInterface* new_trikControl__EventCodeInterface();
void delete_trikControl__EventCodeInterface(trikControl::EventCodeInterface* obj) { delete obj; }
   const QMetaObject*  metaObject(trikControl::EventCodeInterface* theWrappedObject) const;
   const QMetaObject*  py_q_metaObject(trikControl::EventCodeInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__EventCodeInterface*)theWrappedObject)->py_q_metaObject());}
   void on(trikControl::EventCodeInterface* theWrappedObject, int  value, int  eventTime);
   void*  qt_metacast(trikControl::EventCodeInterface* theWrappedObject, const char*  arg__1);
   void*  py_q_qt_metacast(trikControl::EventCodeInterface* theWrappedObject, const char*  arg__1){  return (((PythonQtPublicPromoter_trikControl__EventCodeInterface*)theWrappedObject)->py_q_qt_metacast(arg__1));}
   QString  static_trikControl__EventCodeInterface_tr(const char*  s, const char*  c = nullptr, int  n = -1);
   QString  static_trikControl__EventCodeInterface_trUtf8(const char*  s, const char*  c = nullptr, int  n = -1);
};





class PythonQtPublicPromoter_trikControl__EventDeviceInterface : public trikControl::EventDeviceInterface
{ public:
inline const QMetaObject*  py_q_metaObject() const { return trikControl::EventDeviceInterface::metaObject(); }
inline trikControl::EventInterface*  py_q_onEvent(int  eventType) { return this->onEvent(eventType); }
inline void*  py_q_qt_metacast(const char*  arg__1) { return trikControl::EventDeviceInterface::qt_metacast(arg__1); }
};

class PythonQtWrapper_trikControl__EventDeviceInterface : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
void delete_trikControl__EventDeviceInterface(trikControl::EventDeviceInterface* obj) { delete obj; }
   const QMetaObject*  metaObject(trikControl::EventDeviceInterface* theWrappedObject) const;
   const QMetaObject*  py_q_metaObject(trikControl::EventDeviceInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__EventDeviceInterface*)theWrappedObject)->py_q_metaObject());}
   void on(trikControl::EventDeviceInterface* theWrappedObject, int  event, int  code, int  value, int  eventTime);
   trikControl::EventInterface*  onEvent(trikControl::EventDeviceInterface* theWrappedObject, int  eventType);
   trikControl::EventInterface*  py_q_onEvent(trikControl::EventDeviceInterface* theWrappedObject, int  eventType){  return (((PythonQtPublicPromoter_trikControl__EventDeviceInterface*)theWrappedObject)->py_q_onEvent(eventType));}
   void*  qt_metacast(trikControl::EventDeviceInterface* theWrappedObject, const char*  arg__1);
   void*  py_q_qt_metacast(trikControl::EventDeviceInterface* theWrappedObject, const char*  arg__1){  return (((PythonQtPublicPromoter_trikControl__EventDeviceInterface*)theWrappedObject)->py_q_qt_metacast(arg__1));}
   QString  static_trikControl__EventDeviceInterface_tr(const char*  s, const char*  c = nullptr, int  n = -1);
   QString  static_trikControl__EventDeviceInterface_trUtf8(const char*  s, const char*  c = nullptr, int  n = -1);
};





class PythonQtPublicPromoter_trikControl__EventInterface : public trikControl::EventInterface
{ public:
inline trikControl::EventCodeInterface*  py_q_code(int  code) { return this->code(code); }
inline const QMetaObject*  py_q_metaObject() const { return trikControl::EventInterface::metaObject(); }
inline void*  py_q_qt_metacast(const char*  arg__1) { return trikControl::EventInterface::qt_metacast(arg__1); }
};

class PythonQtWrapper_trikControl__EventInterface : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
void delete_trikControl__EventInterface(trikControl::EventInterface* obj) { delete obj; }
   trikControl::EventCodeInterface*  code(trikControl::EventInterface* theWrappedObject, int  code);
   trikControl::EventCodeInterface*  py_q_code(trikControl::EventInterface* theWrappedObject, int  code){  return (((PythonQtPublicPromoter_trikControl__EventInterface*)theWrappedObject)->py_q_code(code));}
   const QMetaObject*  metaObject(trikControl::EventInterface* theWrappedObject) const;
   const QMetaObject*  py_q_metaObject(trikControl::EventInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__EventInterface*)theWrappedObject)->py_q_metaObject());}
   void on(trikControl::EventInterface* theWrappedObject, int  code, int  value, int  eventTime);
   void*  qt_metacast(trikControl::EventInterface* theWrappedObject, const char*  arg__1);
   void*  py_q_qt_metacast(trikControl::EventInterface* theWrappedObject, const char*  arg__1){  return (((PythonQtPublicPromoter_trikControl__EventInterface*)theWrappedObject)->py_q_qt_metacast(arg__1));}
   QString  static_trikControl__EventInterface_tr(const char*  s, const char*  c = nullptr, int  n = -1);
   QString  static_trikControl__EventInterface_trUtf8(const char*  s, const char*  c = nullptr, int  n = -1);
};





class PythonQtPublicPromoter_trikControl__FifoInterface : public trikControl::FifoInterface
{ public:
inline bool  py_q_hasData() const { return this->hasData(); }
inline bool  py_q_hasLine() const { return this->hasLine(); }
inline const QMetaObject*  py_q_metaObject() const { return trikControl::FifoInterface::metaObject(); }
inline void*  py_q_qt_metacast(const char*  arg__1) { return trikControl::FifoInterface::qt_metacast(arg__1); }
inline QString  py_q_read() { return this->read(); }
};

class PythonQtWrapper_trikControl__FifoInterface : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
void delete_trikControl__FifoInterface(trikControl::FifoInterface* obj) { delete obj; }
   bool  hasData(trikControl::FifoInterface* theWrappedObject) const;
   bool  py_q_hasData(trikControl::FifoInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__FifoInterface*)theWrappedObject)->py_q_hasData());}
   bool  hasLine(trikControl::FifoInterface* theWrappedObject) const;
   bool  py_q_hasLine(trikControl::FifoInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__FifoInterface*)theWrappedObject)->py_q_hasLine());}
   const QMetaObject*  metaObject(trikControl::FifoInterface* theWrappedObject) const;
   const QMetaObject*  py_q_metaObject(trikControl::FifoInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__FifoInterface*)theWrappedObject)->py_q_metaObject());}
   void newLine(trikControl::FifoInterface* theWrappedObject, const QString&  data);
   void*  qt_metacast(trikControl::FifoInterface* theWrappedObject, const char*  arg__1);
   void*  py_q_qt_metacast(trikControl::FifoInterface* theWrappedObject, const char*  arg__1){  return (((PythonQtPublicPromoter_trikControl__FifoInterface*)theWrappedObject)->py_q_qt_metacast(arg__1));}
   QString  read(trikControl::FifoInterface* theWrappedObject);
   QString  py_q_read(trikControl::FifoInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__FifoInterface*)theWrappedObject)->py_q_read());}
   QString  static_trikControl__FifoInterface_tr(const char*  s, const char*  c = nullptr, int  n = -1);
   QString  static_trikControl__FifoInterface_trUtf8(const char*  s, const char*  c = nullptr, int  n = -1);
};





class PythonQtPublicPromoter_trikControl__GamepadInterface : public trikControl::GamepadInterface
{ public:
inline bool  py_q_buttonIsPressed(int  buttonNumber) { return this->buttonIsPressed(buttonNumber); }
inline bool  py_q_buttonWasPressed(int  buttonNumber) { return this->buttonWasPressed(buttonNumber); }
inline bool  py_q_disconnect() { return this->disconnect(); }
inline bool  py_q_isConnected() const { return this->isConnected(); }
inline bool  py_q_isPadPressed(int  pad) const { return this->isPadPressed(pad); }
inline const QMetaObject*  py_q_metaObject() const { return trikControl::GamepadInterface::metaObject(); }
inline int  py_q_padX(int  pad) const { return this->padX(pad); }
inline int  py_q_padY(int  pad) const { return this->padY(pad); }
inline void*  py_q_qt_metacast(const char*  arg__1) { return trikControl::GamepadInterface::qt_metacast(arg__1); }
inline void py_q_reset() { this->reset(); }
inline int  py_q_wheel() const { return this->wheel(); }
};

class PythonQtWrapper_trikControl__GamepadInterface : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
void delete_trikControl__GamepadInterface(trikControl::GamepadInterface* obj) { delete obj; }
   void button(trikControl::GamepadInterface* theWrappedObject, int  button, int  pressed);
   bool  buttonIsPressed(trikControl::GamepadInterface* theWrappedObject, int  buttonNumber);
   bool  py_q_buttonIsPressed(trikControl::GamepadInterface* theWrappedObject, int  buttonNumber){  return (((PythonQtPublicPromoter_trikControl__GamepadInterface*)theWrappedObject)->py_q_buttonIsPressed(buttonNumber));}
   bool  buttonWasPressed(trikControl::GamepadInterface* theWrappedObject, int  buttonNumber);
   bool  py_q_buttonWasPressed(trikControl::GamepadInterface* theWrappedObject, int  buttonNumber){  return (((PythonQtPublicPromoter_trikControl__GamepadInterface*)theWrappedObject)->py_q_buttonWasPressed(buttonNumber));}
   void connected(trikControl::GamepadInterface* theWrappedObject);
   void custom(trikControl::GamepadInterface* theWrappedObject, const QString&  s);
   bool  disconnect(trikControl::GamepadInterface* theWrappedObject);
   bool  py_q_disconnect(trikControl::GamepadInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__GamepadInterface*)theWrappedObject)->py_q_disconnect());}
   void disconnected(trikControl::GamepadInterface* theWrappedObject);
   bool  isConnected(trikControl::GamepadInterface* theWrappedObject) const;
   bool  py_q_isConnected(trikControl::GamepadInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__GamepadInterface*)theWrappedObject)->py_q_isConnected());}
   bool  isPadPressed(trikControl::GamepadInterface* theWrappedObject, int  pad) const;
   bool  py_q_isPadPressed(trikControl::GamepadInterface* theWrappedObject, int  pad) const{  return (((PythonQtPublicPromoter_trikControl__GamepadInterface*)theWrappedObject)->py_q_isPadPressed(pad));}
   const QMetaObject*  metaObject(trikControl::GamepadInterface* theWrappedObject) const;
   const QMetaObject*  py_q_metaObject(trikControl::GamepadInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__GamepadInterface*)theWrappedObject)->py_q_metaObject());}
   void pad(trikControl::GamepadInterface* theWrappedObject, int  pad, int  x, int  y);
   void padUp(trikControl::GamepadInterface* theWrappedObject, int  pad);
   int  padX(trikControl::GamepadInterface* theWrappedObject, int  pad) const;
   int  py_q_padX(trikControl::GamepadInterface* theWrappedObject, int  pad) const{  return (((PythonQtPublicPromoter_trikControl__GamepadInterface*)theWrappedObject)->py_q_padX(pad));}
   int  padY(trikControl::GamepadInterface* theWrappedObject, int  pad) const;
   int  py_q_padY(trikControl::GamepadInterface* theWrappedObject, int  pad) const{  return (((PythonQtPublicPromoter_trikControl__GamepadInterface*)theWrappedObject)->py_q_padY(pad));}
   void*  qt_metacast(trikControl::GamepadInterface* theWrappedObject, const char*  arg__1);
   void*  py_q_qt_metacast(trikControl::GamepadInterface* theWrappedObject, const char*  arg__1){  return (((PythonQtPublicPromoter_trikControl__GamepadInterface*)theWrappedObject)->py_q_qt_metacast(arg__1));}
   void reset(trikControl::GamepadInterface* theWrappedObject);
   void py_q_reset(trikControl::GamepadInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__GamepadInterface*)theWrappedObject)->py_q_reset());}
   QString  static_trikControl__GamepadInterface_tr(const char*  s, const char*  c = nullptr, int  n = -1);
   QString  static_trikControl__GamepadInterface_trUtf8(const char*  s, const char*  c = nullptr, int  n = -1);
   int  wheel(trikControl::GamepadInterface* theWrappedObject) const;
   int  py_q_wheel(trikControl::GamepadInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__GamepadInterface*)theWrappedObject)->py_q_wheel());}
   void wheelEvent(trikControl::GamepadInterface* theWrappedObject, int  percent);
};





class PythonQtPublicPromoter_trikControl__GyroSensorInterface : public trikControl::GyroSensorInterface
{ public:
inline void py_q_calibrate(int  msec) { this->calibrate(msec); }
inline QVector<int >  py_q_getCalibrationValues() { return this->getCalibrationValues(); }
inline bool  py_q_isCalibrated() const { return this->isCalibrated(); }
inline const QMetaObject*  py_q_metaObject() const { return trikControl::GyroSensorInterface::metaObject(); }
inline void*  py_q_qt_metacast(const char*  arg__1) { return trikControl::GyroSensorInterface::qt_metacast(arg__1); }
inline QVector<int >  py_q_readRawData() const { return this->readRawData(); }
inline void py_q_setCalibrationValues(const QVector<int >&  values) { this->setCalibrationValues(values); }
};

class PythonQtWrapper_trikControl__GyroSensorInterface : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
void delete_trikControl__GyroSensorInterface(trikControl::GyroSensorInterface* obj) { delete obj; }
   void calibrate(trikControl::GyroSensorInterface* theWrappedObject, int  msec);
   void py_q_calibrate(trikControl::GyroSensorInterface* theWrappedObject, int  msec){  (((PythonQtPublicPromoter_trikControl__GyroSensorInterface*)theWrappedObject)->py_q_calibrate(msec));}
   void calibrationFinished(trikControl::GyroSensorInterface* theWrappedObject);
   QVector<int >  getCalibrationValues(trikControl::GyroSensorInterface* theWrappedObject);
   QVector<int >  py_q_getCalibrationValues(trikControl::GyroSensorInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__GyroSensorInterface*)theWrappedObject)->py_q_getCalibrationValues());}
   bool  isCalibrated(trikControl::GyroSensorInterface* theWrappedObject) const;
   bool  py_q_isCalibrated(trikControl::GyroSensorInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__GyroSensorInterface*)theWrappedObject)->py_q_isCalibrated());}
   const QMetaObject*  py_q_metaObject(trikControl::GyroSensorInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__GyroSensorInterface*)theWrappedObject)->py_q_metaObject());}
   void*  py_q_qt_metacast(trikControl::GyroSensorInterface* theWrappedObject, const char*  arg__1){  return (((PythonQtPublicPromoter_trikControl__GyroSensorInterface*)theWrappedObject)->py_q_qt_metacast(arg__1));}
   QVector<int >  readRawData(trikControl::GyroSensorInterface* theWrappedObject) const;
   QVector<int >  py_q_readRawData(trikControl::GyroSensorInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__GyroSensorInterface*)theWrappedObject)->py_q_readRawData());}
   void setCalibrationValues(trikControl::GyroSensorInterface* theWrappedObject, const QVector<int >&  values);
   void py_q_setCalibrationValues(trikControl::GyroSensorInterface* theWrappedObject, const QVector<int >&  values){  (((PythonQtPublicPromoter_trikControl__GyroSensorInterface*)theWrappedObject)->py_q_setCalibrationValues(values));}
   QString  static_trikControl__GyroSensorInterface_tr(const char*  s, const char*  c = nullptr, int  n = -1);
   QString  static_trikControl__GyroSensorInterface_trUtf8(const char*  s, const char*  c = nullptr, int  n = -1);
};





class PythonQtPublicPromoter_trikControl__I2cDeviceInterface : public trikControl::I2cDeviceInterface
{ public:
inline const QMetaObject*  py_q_metaObject() const { return trikControl::I2cDeviceInterface::metaObject(); }
inline void*  py_q_qt_metacast(const char*  arg__1) { return trikControl::I2cDeviceInterface::qt_metacast(arg__1); }
inline int  py_q_read(int  reg, const QString&  mode = "b") { return this->read(reg, mode); }
inline int  py_q_send(int  reg, int  value, const QString&  mode = "b") { return this->send(reg, value, mode); }
};

class PythonQtWrapper_trikControl__I2cDeviceInterface : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
void delete_trikControl__I2cDeviceInterface(trikControl::I2cDeviceInterface* obj) { delete obj; }
   const QMetaObject*  metaObject(trikControl::I2cDeviceInterface* theWrappedObject) const;
   const QMetaObject*  py_q_metaObject(trikControl::I2cDeviceInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__I2cDeviceInterface*)theWrappedObject)->py_q_metaObject());}
   void*  qt_metacast(trikControl::I2cDeviceInterface* theWrappedObject, const char*  arg__1);
   void*  py_q_qt_metacast(trikControl::I2cDeviceInterface* theWrappedObject, const char*  arg__1){  return (((PythonQtPublicPromoter_trikControl__I2cDeviceInterface*)theWrappedObject)->py_q_qt_metacast(arg__1));}
   int  read(trikControl::I2cDeviceInterface* theWrappedObject, int  reg, const QString&  mode = "b");
   int  py_q_read(trikControl::I2cDeviceInterface* theWrappedObject, int  reg, const QString&  mode = "b"){  return (((PythonQtPublicPromoter_trikControl__I2cDeviceInterface*)theWrappedObject)->py_q_read(reg, mode));}
   int  send(trikControl::I2cDeviceInterface* theWrappedObject, int  reg, int  value, const QString&  mode = "b");
   int  py_q_send(trikControl::I2cDeviceInterface* theWrappedObject, int  reg, int  value, const QString&  mode = "b"){  return (((PythonQtPublicPromoter_trikControl__I2cDeviceInterface*)theWrappedObject)->py_q_send(reg, value, mode));}
   QString  static_trikControl__I2cDeviceInterface_tr(const char*  s, const char*  c = nullptr, int  n = -1);
   QString  static_trikControl__I2cDeviceInterface_trUtf8(const char*  s, const char*  c = nullptr, int  n = -1);
};





class PythonQtPublicPromoter_trikControl__IrCameraInterface : public trikControl::IrCameraInterface
{ public:
inline void py_q_init() { this->init(); }
inline const QMetaObject*  py_q_metaObject() const { return trikControl::IrCameraInterface::metaObject(); }
inline void*  py_q_qt_metacast(const char*  arg__1) { return trikControl::IrCameraInterface::qt_metacast(arg__1); }
inline int  py_q_readSensor(int  m, int  n) const { return this->readSensor(m, n); }
inline void py_q_stop() { this->stop(); }
};

class PythonQtWrapper_trikControl__IrCameraInterface : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
void delete_trikControl__IrCameraInterface(trikControl::IrCameraInterface* obj) { delete obj; }
   void imageUpdated(trikControl::IrCameraInterface* theWrappedObject);
   void init(trikControl::IrCameraInterface* theWrappedObject);
   void py_q_init(trikControl::IrCameraInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__IrCameraInterface*)theWrappedObject)->py_q_init());}
   const QMetaObject*  metaObject(trikControl::IrCameraInterface* theWrappedObject) const;
   const QMetaObject*  py_q_metaObject(trikControl::IrCameraInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__IrCameraInterface*)theWrappedObject)->py_q_metaObject());}
   void*  qt_metacast(trikControl::IrCameraInterface* theWrappedObject, const char*  arg__1);
   void*  py_q_qt_metacast(trikControl::IrCameraInterface* theWrappedObject, const char*  arg__1){  return (((PythonQtPublicPromoter_trikControl__IrCameraInterface*)theWrappedObject)->py_q_qt_metacast(arg__1));}
   int  readSensor(trikControl::IrCameraInterface* theWrappedObject, int  m, int  n) const;
   int  py_q_readSensor(trikControl::IrCameraInterface* theWrappedObject, int  m, int  n) const{  return (((PythonQtPublicPromoter_trikControl__IrCameraInterface*)theWrappedObject)->py_q_readSensor(m, n));}
   void sensorUpdated(trikControl::IrCameraInterface* theWrappedObject);
   void stop(trikControl::IrCameraInterface* theWrappedObject);
   void py_q_stop(trikControl::IrCameraInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__IrCameraInterface*)theWrappedObject)->py_q_stop());}
   void stopped(trikControl::IrCameraInterface* theWrappedObject);
   QString  static_trikControl__IrCameraInterface_tr(const char*  s, const char*  c = nullptr, int  n = -1);
   QString  static_trikControl__IrCameraInterface_trUtf8(const char*  s, const char*  c = nullptr, int  n = -1);
};





class PythonQtPublicPromoter_trikControl__KeysInterface : public trikControl::KeysInterface
{ public:
inline int  py_q_buttonCode(bool  wait = true) { return this->buttonCode(wait); }
inline bool  py_q_isPressed(int  code) { return this->isPressed(code); }
inline const QMetaObject*  py_q_metaObject() const { return trikControl::KeysInterface::metaObject(); }
inline void*  py_q_qt_metacast(const char*  arg__1) { return trikControl::KeysInterface::qt_metacast(arg__1); }
inline void py_q_reset() { this->reset(); }
inline bool  py_q_wasPressed(int  code) { return this->wasPressed(code); }
};

class PythonQtWrapper_trikControl__KeysInterface : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
void delete_trikControl__KeysInterface(trikControl::KeysInterface* obj) { delete obj; }
   int  buttonCode(trikControl::KeysInterface* theWrappedObject, bool  wait = true);
   int  py_q_buttonCode(trikControl::KeysInterface* theWrappedObject, bool  wait = true){  return (((PythonQtPublicPromoter_trikControl__KeysInterface*)theWrappedObject)->py_q_buttonCode(wait));}
   void buttonPressed(trikControl::KeysInterface* theWrappedObject, int  code, int  value);
   bool  isPressed(trikControl::KeysInterface* theWrappedObject, int  code);
   bool  py_q_isPressed(trikControl::KeysInterface* theWrappedObject, int  code){  return (((PythonQtPublicPromoter_trikControl__KeysInterface*)theWrappedObject)->py_q_isPressed(code));}
   const QMetaObject*  metaObject(trikControl::KeysInterface* theWrappedObject) const;
   const QMetaObject*  py_q_metaObject(trikControl::KeysInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__KeysInterface*)theWrappedObject)->py_q_metaObject());}
   void*  qt_metacast(trikControl::KeysInterface* theWrappedObject, const char*  arg__1);
   void*  py_q_qt_metacast(trikControl::KeysInterface* theWrappedObject, const char*  arg__1){  return (((PythonQtPublicPromoter_trikControl__KeysInterface*)theWrappedObject)->py_q_qt_metacast(arg__1));}
   void reset(trikControl::KeysInterface* theWrappedObject);
   void py_q_reset(trikControl::KeysInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__KeysInterface*)theWrappedObject)->py_q_reset());}
   QString  static_trikControl__KeysInterface_tr(const char*  s, const char*  c = nullptr, int  n = -1);
   QString  static_trikControl__KeysInterface_trUtf8(const char*  s, const char*  c = nullptr, int  n = -1);
   bool  wasPressed(trikControl::KeysInterface* theWrappedObject, int  code);
   bool  py_q_wasPressed(trikControl::KeysInterface* theWrappedObject, int  code){  return (((PythonQtPublicPromoter_trikControl__KeysInterface*)theWrappedObject)->py_q_wasPressed(code));}
};





class PythonQtPublicPromoter_trikControl__LedInterface : public trikControl::LedInterface
{ public:
inline void py_q_green() { this->green(); }
inline const QMetaObject*  py_q_metaObject() const { return trikControl::LedInterface::metaObject(); }
inline void py_q_off() { this->off(); }
inline void py_q_orange() { this->orange(); }
inline void*  py_q_qt_metacast(const char*  arg__1) { return trikControl::LedInterface::qt_metacast(arg__1); }
inline void py_q_red() { this->red(); }
};

class PythonQtWrapper_trikControl__LedInterface : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
void delete_trikControl__LedInterface(trikControl::LedInterface* obj) { delete obj; }
   void green(trikControl::LedInterface* theWrappedObject);
   void py_q_green(trikControl::LedInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__LedInterface*)theWrappedObject)->py_q_green());}
   const QMetaObject*  metaObject(trikControl::LedInterface* theWrappedObject) const;
   const QMetaObject*  py_q_metaObject(trikControl::LedInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__LedInterface*)theWrappedObject)->py_q_metaObject());}
   void off(trikControl::LedInterface* theWrappedObject);
   void py_q_off(trikControl::LedInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__LedInterface*)theWrappedObject)->py_q_off());}
   void orange(trikControl::LedInterface* theWrappedObject);
   void py_q_orange(trikControl::LedInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__LedInterface*)theWrappedObject)->py_q_orange());}
   void*  qt_metacast(trikControl::LedInterface* theWrappedObject, const char*  arg__1);
   void*  py_q_qt_metacast(trikControl::LedInterface* theWrappedObject, const char*  arg__1){  return (((PythonQtPublicPromoter_trikControl__LedInterface*)theWrappedObject)->py_q_qt_metacast(arg__1));}
   void red(trikControl::LedInterface* theWrappedObject);
   void py_q_red(trikControl::LedInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__LedInterface*)theWrappedObject)->py_q_red());}
   QString  static_trikControl__LedInterface_tr(const char*  s, const char*  c = nullptr, int  n = -1);
   QString  static_trikControl__LedInterface_trUtf8(const char*  s, const char*  c = nullptr, int  n = -1);
};





class PythonQtPublicPromoter_trikControl__LidarInterface : public trikControl::LidarInterface
{ public:
inline const QMetaObject*  py_q_metaObject() const { return trikControl::LidarInterface::metaObject(); }
inline void*  py_q_qt_metacast(const char*  arg__1) { return trikControl::LidarInterface::qt_metacast(arg__1); }
};

class PythonQtWrapper_trikControl__LidarInterface : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
void delete_trikControl__LidarInterface(trikControl::LidarInterface* obj) { delete obj; }
   const QMetaObject*  py_q_metaObject(trikControl::LidarInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__LidarInterface*)theWrappedObject)->py_q_metaObject());}
   void*  py_q_qt_metacast(trikControl::LidarInterface* theWrappedObject, const char*  arg__1){  return (((PythonQtPublicPromoter_trikControl__LidarInterface*)theWrappedObject)->py_q_qt_metacast(arg__1));}
   QString  static_trikControl__LidarInterface_tr(const char*  s, const char*  c = nullptr, int  n = -1);
   QString  static_trikControl__LidarInterface_trUtf8(const char*  s, const char*  c = nullptr, int  n = -1);
};





class PythonQtPublicPromoter_trikControl__LineSensorInterface : public trikControl::LineSensorInterface
{ public:
inline void py_q_detect() { this->detect(); }
inline QVector<int >  py_q_getDetectParameters() const { return this->getDetectParameters(); }
inline void py_q_init(bool  showOnDisplay) { this->init(showOnDisplay); }
inline const QMetaObject*  py_q_metaObject() const { return trikControl::LineSensorInterface::metaObject(); }
inline void*  py_q_qt_metacast(const char*  arg__1) { return trikControl::LineSensorInterface::qt_metacast(arg__1); }
inline QVector<int >  py_q_read() { return this->read(); }
inline void py_q_stop() { this->stop(); }
};

class PythonQtWrapper_trikControl__LineSensorInterface : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
void delete_trikControl__LineSensorInterface(trikControl::LineSensorInterface* obj) { delete obj; }
   void detect(trikControl::LineSensorInterface* theWrappedObject);
   void py_q_detect(trikControl::LineSensorInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__LineSensorInterface*)theWrappedObject)->py_q_detect());}
   QVector<int >  getDetectParameters(trikControl::LineSensorInterface* theWrappedObject) const;
   QVector<int >  py_q_getDetectParameters(trikControl::LineSensorInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__LineSensorInterface*)theWrappedObject)->py_q_getDetectParameters());}
   void init(trikControl::LineSensorInterface* theWrappedObject, bool  showOnDisplay);
   void py_q_init(trikControl::LineSensorInterface* theWrappedObject, bool  showOnDisplay){  (((PythonQtPublicPromoter_trikControl__LineSensorInterface*)theWrappedObject)->py_q_init(showOnDisplay));}
   const QMetaObject*  metaObject(trikControl::LineSensorInterface* theWrappedObject) const;
   const QMetaObject*  py_q_metaObject(trikControl::LineSensorInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__LineSensorInterface*)theWrappedObject)->py_q_metaObject());}
   void*  qt_metacast(trikControl::LineSensorInterface* theWrappedObject, const char*  arg__1);
   void*  py_q_qt_metacast(trikControl::LineSensorInterface* theWrappedObject, const char*  arg__1){  return (((PythonQtPublicPromoter_trikControl__LineSensorInterface*)theWrappedObject)->py_q_qt_metacast(arg__1));}
   QVector<int >  read(trikControl::LineSensorInterface* theWrappedObject);
   QVector<int >  py_q_read(trikControl::LineSensorInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__LineSensorInterface*)theWrappedObject)->py_q_read());}
   void stop(trikControl::LineSensorInterface* theWrappedObject);
   void py_q_stop(trikControl::LineSensorInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__LineSensorInterface*)theWrappedObject)->py_q_stop());}
   void stopped(trikControl::LineSensorInterface* theWrappedObject);
   QString  static_trikControl__LineSensorInterface_tr(const char*  s, const char*  c = nullptr, int  n = -1);
   QString  static_trikControl__LineSensorInterface_trUtf8(const char*  s, const char*  c = nullptr, int  n = -1);
};





class PythonQtPublicPromoter_trikControl__MarkerInterface : public trikControl::MarkerInterface
{ public:
inline void py_q_down(const QString&  color) { this->down(color); }
inline bool  py_q_isDown() const { return this->isDown(); }
inline const QMetaObject*  py_q_metaObject() const { return trikControl::MarkerInterface::metaObject(); }
inline void*  py_q_qt_metacast(const char*  arg__1) { return trikControl::MarkerInterface::qt_metacast(arg__1); }
inline void py_q_setDown(bool  isDown) { this->setDown(isDown); }
inline void py_q_up() { this->up(); }
};

class PythonQtWrapper_trikControl__MarkerInterface : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
void delete_trikControl__MarkerInterface(trikControl::MarkerInterface* obj) { delete obj; }
   void down(trikControl::MarkerInterface* theWrappedObject, const QString&  color);
   void py_q_down(trikControl::MarkerInterface* theWrappedObject, const QString&  color){  (((PythonQtPublicPromoter_trikControl__MarkerInterface*)theWrappedObject)->py_q_down(color));}
   bool  isDown(trikControl::MarkerInterface* theWrappedObject) const;
   bool  py_q_isDown(trikControl::MarkerInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__MarkerInterface*)theWrappedObject)->py_q_isDown());}
   const QMetaObject*  metaObject(trikControl::MarkerInterface* theWrappedObject) const;
   const QMetaObject*  py_q_metaObject(trikControl::MarkerInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__MarkerInterface*)theWrappedObject)->py_q_metaObject());}
   void*  qt_metacast(trikControl::MarkerInterface* theWrappedObject, const char*  arg__1);
   void*  py_q_qt_metacast(trikControl::MarkerInterface* theWrappedObject, const char*  arg__1){  return (((PythonQtPublicPromoter_trikControl__MarkerInterface*)theWrappedObject)->py_q_qt_metacast(arg__1));}
   void setDown(trikControl::MarkerInterface* theWrappedObject, bool  isDown);
   void py_q_setDown(trikControl::MarkerInterface* theWrappedObject, bool  isDown){  (((PythonQtPublicPromoter_trikControl__MarkerInterface*)theWrappedObject)->py_q_setDown(isDown));}
   QString  static_trikControl__MarkerInterface_tr(const char*  s, const char*  c = nullptr, int  n = -1);
   QString  static_trikControl__MarkerInterface_trUtf8(const char*  s, const char*  c = nullptr, int  n = -1);
   void up(trikControl::MarkerInterface* theWrappedObject);
   void py_q_up(trikControl::MarkerInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__MarkerInterface*)theWrappedObject)->py_q_up());}
};





class PythonQtPublicPromoter_trikControl__MotorInterface : public trikControl::MotorInterface
{ public:
inline void py_q_brake(int  durationMs = 500) { this->brake(durationMs); }
inline int  py_q_maxControl() const { return this->maxControl(); }
inline const QMetaObject*  py_q_metaObject() const { return trikControl::MotorInterface::metaObject(); }
inline int  py_q_minControl() const { return this->minControl(); }
inline int  py_q_power() const { return this->power(); }
inline void py_q_powerOff() { this->powerOff(); }
inline void*  py_q_qt_metacast(const char*  arg__1) { return trikControl::MotorInterface::qt_metacast(arg__1); }
inline void py_q_setPeriod(int  uSec = 20000) { this->setPeriod(uSec); }
inline void py_q_setPower(int  power, bool  constrain = true) { this->setPower(power, constrain); }
};

class PythonQtWrapper_trikControl__MotorInterface : public QObject
{ Q_OBJECT
public:
Q_ENUMS(Type )
enum class Type{
  powerMotor = static_cast<int>(trikControl::MotorInterface::Type::powerMotor),   servoMotor = static_cast<int>(trikControl::MotorInterface::Type::servoMotor)};
public Q_SLOTS:
void delete_trikControl__MotorInterface(trikControl::MotorInterface* obj) { delete obj; }
   void brake(trikControl::MotorInterface* theWrappedObject, int  durationMs = 500);
   void py_q_brake(trikControl::MotorInterface* theWrappedObject, int  durationMs = 500){  (((PythonQtPublicPromoter_trikControl__MotorInterface*)theWrappedObject)->py_q_brake(durationMs));}
   int  maxControl(trikControl::MotorInterface* theWrappedObject) const;
   int  py_q_maxControl(trikControl::MotorInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__MotorInterface*)theWrappedObject)->py_q_maxControl());}
   const QMetaObject*  metaObject(trikControl::MotorInterface* theWrappedObject) const;
   const QMetaObject*  py_q_metaObject(trikControl::MotorInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__MotorInterface*)theWrappedObject)->py_q_metaObject());}
   int  minControl(trikControl::MotorInterface* theWrappedObject) const;
   int  py_q_minControl(trikControl::MotorInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__MotorInterface*)theWrappedObject)->py_q_minControl());}
   int  power(trikControl::MotorInterface* theWrappedObject) const;
   int  py_q_power(trikControl::MotorInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__MotorInterface*)theWrappedObject)->py_q_power());}
   void powerOff(trikControl::MotorInterface* theWrappedObject);
   void py_q_powerOff(trikControl::MotorInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__MotorInterface*)theWrappedObject)->py_q_powerOff());}
   void*  qt_metacast(trikControl::MotorInterface* theWrappedObject, const char*  arg__1);
   void*  py_q_qt_metacast(trikControl::MotorInterface* theWrappedObject, const char*  arg__1){  return (((PythonQtPublicPromoter_trikControl__MotorInterface*)theWrappedObject)->py_q_qt_metacast(arg__1));}
   void setPeriod(trikControl::MotorInterface* theWrappedObject, int  uSec = 20000);
   void py_q_setPeriod(trikControl::MotorInterface* theWrappedObject, int  uSec = 20000){  (((PythonQtPublicPromoter_trikControl__MotorInterface*)theWrappedObject)->py_q_setPeriod(uSec));}
   void setPower(trikControl::MotorInterface* theWrappedObject, int  power, bool  constrain = true);
   void py_q_setPower(trikControl::MotorInterface* theWrappedObject, int  power, bool  constrain = true){  (((PythonQtPublicPromoter_trikControl__MotorInterface*)theWrappedObject)->py_q_setPower(power, constrain));}
   QString  static_trikControl__MotorInterface_tr(const char*  s, const char*  c = nullptr, int  n = -1);
   QString  static_trikControl__MotorInterface_trUtf8(const char*  s, const char*  c = nullptr, int  n = -1);
};





class PythonQtPublicPromoter_trikControl__ObjectSensorInterface : public trikControl::ObjectSensorInterface
{ public:
inline void py_q_detect() { this->detect(); }
inline QVector<int >  py_q_getDetectParameters() const { return this->getDetectParameters(); }
inline void py_q_init(bool  showOnDisplay) { this->init(showOnDisplay); }
inline const QMetaObject*  py_q_metaObject() const { return trikControl::ObjectSensorInterface::metaObject(); }
inline void*  py_q_qt_metacast(const char*  arg__1) { return trikControl::ObjectSensorInterface::qt_metacast(arg__1); }
inline QVector<int >  py_q_read() { return this->read(); }
inline void py_q_stop() { this->stop(); }
};

class PythonQtWrapper_trikControl__ObjectSensorInterface : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
void delete_trikControl__ObjectSensorInterface(trikControl::ObjectSensorInterface* obj) { delete obj; }
   void detect(trikControl::ObjectSensorInterface* theWrappedObject);
   void py_q_detect(trikControl::ObjectSensorInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__ObjectSensorInterface*)theWrappedObject)->py_q_detect());}
   QVector<int >  getDetectParameters(trikControl::ObjectSensorInterface* theWrappedObject) const;
   QVector<int >  py_q_getDetectParameters(trikControl::ObjectSensorInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__ObjectSensorInterface*)theWrappedObject)->py_q_getDetectParameters());}
   void init(trikControl::ObjectSensorInterface* theWrappedObject, bool  showOnDisplay);
   void py_q_init(trikControl::ObjectSensorInterface* theWrappedObject, bool  showOnDisplay){  (((PythonQtPublicPromoter_trikControl__ObjectSensorInterface*)theWrappedObject)->py_q_init(showOnDisplay));}
   const QMetaObject*  metaObject(trikControl::ObjectSensorInterface* theWrappedObject) const;
   const QMetaObject*  py_q_metaObject(trikControl::ObjectSensorInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__ObjectSensorInterface*)theWrappedObject)->py_q_metaObject());}
   void*  qt_metacast(trikControl::ObjectSensorInterface* theWrappedObject, const char*  arg__1);
   void*  py_q_qt_metacast(trikControl::ObjectSensorInterface* theWrappedObject, const char*  arg__1){  return (((PythonQtPublicPromoter_trikControl__ObjectSensorInterface*)theWrappedObject)->py_q_qt_metacast(arg__1));}
   QVector<int >  read(trikControl::ObjectSensorInterface* theWrappedObject);
   QVector<int >  py_q_read(trikControl::ObjectSensorInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__ObjectSensorInterface*)theWrappedObject)->py_q_read());}
   void stop(trikControl::ObjectSensorInterface* theWrappedObject);
   void py_q_stop(trikControl::ObjectSensorInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__ObjectSensorInterface*)theWrappedObject)->py_q_stop());}
   void stopped(trikControl::ObjectSensorInterface* theWrappedObject);
   QString  static_trikControl__ObjectSensorInterface_tr(const char*  s, const char*  c = nullptr, int  n = -1);
   QString  static_trikControl__ObjectSensorInterface_trUtf8(const char*  s, const char*  c = nullptr, int  n = -1);
};





class PythonQtPublicPromoter_trikControl__PwmCaptureInterface : public trikControl::PwmCaptureInterface
{ public:
inline long  py_q_duty() { return this->duty(); }
inline long  py_q_dutyRaw() { return this->dutyRaw(); }
inline QVector<int >  py_q_frequency() { return this->frequency(); }
inline long  py_q_maxValue() const { return this->maxValue(); }
inline const QMetaObject*  py_q_metaObject() const { return trikControl::PwmCaptureInterface::metaObject(); }
inline long  py_q_minValue() const { return this->minValue(); }
inline void*  py_q_qt_metacast(const char*  arg__1) { return trikControl::PwmCaptureInterface::qt_metacast(arg__1); }
};

class PythonQtWrapper_trikControl__PwmCaptureInterface : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
void delete_trikControl__PwmCaptureInterface(trikControl::PwmCaptureInterface* obj) { delete obj; }
   long  duty(trikControl::PwmCaptureInterface* theWrappedObject);
   long  py_q_duty(trikControl::PwmCaptureInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__PwmCaptureInterface*)theWrappedObject)->py_q_duty());}
   long  dutyRaw(trikControl::PwmCaptureInterface* theWrappedObject);
   long  py_q_dutyRaw(trikControl::PwmCaptureInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__PwmCaptureInterface*)theWrappedObject)->py_q_dutyRaw());}
   QVector<int >  frequency(trikControl::PwmCaptureInterface* theWrappedObject);
   QVector<int >  py_q_frequency(trikControl::PwmCaptureInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__PwmCaptureInterface*)theWrappedObject)->py_q_frequency());}
   long  maxValue(trikControl::PwmCaptureInterface* theWrappedObject) const;
   long  py_q_maxValue(trikControl::PwmCaptureInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__PwmCaptureInterface*)theWrappedObject)->py_q_maxValue());}
   const QMetaObject*  metaObject(trikControl::PwmCaptureInterface* theWrappedObject) const;
   const QMetaObject*  py_q_metaObject(trikControl::PwmCaptureInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__PwmCaptureInterface*)theWrappedObject)->py_q_metaObject());}
   long  minValue(trikControl::PwmCaptureInterface* theWrappedObject) const;
   long  py_q_minValue(trikControl::PwmCaptureInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__PwmCaptureInterface*)theWrappedObject)->py_q_minValue());}
   void*  qt_metacast(trikControl::PwmCaptureInterface* theWrappedObject, const char*  arg__1);
   void*  py_q_qt_metacast(trikControl::PwmCaptureInterface* theWrappedObject, const char*  arg__1){  return (((PythonQtPublicPromoter_trikControl__PwmCaptureInterface*)theWrappedObject)->py_q_qt_metacast(arg__1));}
   QString  static_trikControl__PwmCaptureInterface_tr(const char*  s, const char*  c = nullptr, int  n = -1);
   QString  static_trikControl__PwmCaptureInterface_trUtf8(const char*  s, const char*  c = nullptr, int  n = -1);
};





class PythonQtPublicPromoter_trikControl__SensorInterface : public trikControl::SensorInterface
{ public:
inline int  py_q_maxValue() const { return this->maxValue(); }
inline const QMetaObject*  py_q_metaObject() const { return trikControl::SensorInterface::metaObject(); }
inline int  py_q_minValue() const { return this->minValue(); }
inline void*  py_q_qt_metacast(const char*  arg__1) { return trikControl::SensorInterface::qt_metacast(arg__1); }
inline int  py_q_read() { return this->read(); }
inline int  py_q_readRawData() { return this->readRawData(); }
};

class PythonQtWrapper_trikControl__SensorInterface : public QObject
{ Q_OBJECT
public:
Q_ENUMS(Type )
enum class Type{
  analogSensor = static_cast<int>(trikControl::SensorInterface::Type::analogSensor),   digitalSensor = static_cast<int>(trikControl::SensorInterface::Type::digitalSensor),   specialSensor = static_cast<int>(trikControl::SensorInterface::Type::specialSensor)};
public Q_SLOTS:
void delete_trikControl__SensorInterface(trikControl::SensorInterface* obj) { delete obj; }
   int  maxValue(trikControl::SensorInterface* theWrappedObject) const;
   int  py_q_maxValue(trikControl::SensorInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__SensorInterface*)theWrappedObject)->py_q_maxValue());}
   const QMetaObject*  metaObject(trikControl::SensorInterface* theWrappedObject) const;
   const QMetaObject*  py_q_metaObject(trikControl::SensorInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__SensorInterface*)theWrappedObject)->py_q_metaObject());}
   int  minValue(trikControl::SensorInterface* theWrappedObject) const;
   int  py_q_minValue(trikControl::SensorInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__SensorInterface*)theWrappedObject)->py_q_minValue());}
   void*  qt_metacast(trikControl::SensorInterface* theWrappedObject, const char*  arg__1);
   void*  py_q_qt_metacast(trikControl::SensorInterface* theWrappedObject, const char*  arg__1){  return (((PythonQtPublicPromoter_trikControl__SensorInterface*)theWrappedObject)->py_q_qt_metacast(arg__1));}
   int  read(trikControl::SensorInterface* theWrappedObject);
   int  py_q_read(trikControl::SensorInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__SensorInterface*)theWrappedObject)->py_q_read());}
   int  readRawData(trikControl::SensorInterface* theWrappedObject);
   int  py_q_readRawData(trikControl::SensorInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__SensorInterface*)theWrappedObject)->py_q_readRawData());}
   QString  static_trikControl__SensorInterface_tr(const char*  s, const char*  c = nullptr, int  n = -1);
   QString  static_trikControl__SensorInterface_trUtf8(const char*  s, const char*  c = nullptr, int  n = -1);
};





class PythonQtPublicPromoter_trikControl__SoundSensorInterface : public trikControl::SoundSensorInterface
{ public:
inline void py_q_detect() { this->detect(); }
inline void py_q_init(bool  showOnDisplay) { this->init(showOnDisplay); }
inline const QMetaObject*  py_q_metaObject() const { return trikControl::SoundSensorInterface::metaObject(); }
inline void*  py_q_qt_metacast(const char*  arg__1) { return trikControl::SoundSensorInterface::qt_metacast(arg__1); }
inline QVector<int >  py_q_read() { return this->read(); }
inline void py_q_stop() { this->stop(); }
inline void py_q_volume(int  volCoeff) { this->volume(volCoeff); }
};

class PythonQtWrapper_trikControl__SoundSensorInterface : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
void delete_trikControl__SoundSensorInterface(trikControl::SoundSensorInterface* obj) { delete obj; }
   void detect(trikControl::SoundSensorInterface* theWrappedObject);
   void py_q_detect(trikControl::SoundSensorInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__SoundSensorInterface*)theWrappedObject)->py_q_detect());}
   void init(trikControl::SoundSensorInterface* theWrappedObject, bool  showOnDisplay);
   void py_q_init(trikControl::SoundSensorInterface* theWrappedObject, bool  showOnDisplay){  (((PythonQtPublicPromoter_trikControl__SoundSensorInterface*)theWrappedObject)->py_q_init(showOnDisplay));}
   const QMetaObject*  metaObject(trikControl::SoundSensorInterface* theWrappedObject) const;
   const QMetaObject*  py_q_metaObject(trikControl::SoundSensorInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__SoundSensorInterface*)theWrappedObject)->py_q_metaObject());}
   void*  qt_metacast(trikControl::SoundSensorInterface* theWrappedObject, const char*  arg__1);
   void*  py_q_qt_metacast(trikControl::SoundSensorInterface* theWrappedObject, const char*  arg__1){  return (((PythonQtPublicPromoter_trikControl__SoundSensorInterface*)theWrappedObject)->py_q_qt_metacast(arg__1));}
   QVector<int >  read(trikControl::SoundSensorInterface* theWrappedObject);
   QVector<int >  py_q_read(trikControl::SoundSensorInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__SoundSensorInterface*)theWrappedObject)->py_q_read());}
   void stop(trikControl::SoundSensorInterface* theWrappedObject);
   void py_q_stop(trikControl::SoundSensorInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__SoundSensorInterface*)theWrappedObject)->py_q_stop());}
   void stopped(trikControl::SoundSensorInterface* theWrappedObject);
   QString  static_trikControl__SoundSensorInterface_tr(const char*  s, const char*  c = nullptr, int  n = -1);
   QString  static_trikControl__SoundSensorInterface_trUtf8(const char*  s, const char*  c = nullptr, int  n = -1);
   void volume(trikControl::SoundSensorInterface* theWrappedObject, int  volCoeff);
   void py_q_volume(trikControl::SoundSensorInterface* theWrappedObject, int  volCoeff){  (((PythonQtPublicPromoter_trikControl__SoundSensorInterface*)theWrappedObject)->py_q_volume(volCoeff));}
};





class PythonQtPublicPromoter_trikControl__VectorSensorInterface : public trikControl::VectorSensorInterface
{ public:
inline const QMetaObject*  py_q_metaObject() const { return trikControl::VectorSensorInterface::metaObject(); }
inline void*  py_q_qt_metacast(const char*  arg__1) { return trikControl::VectorSensorInterface::qt_metacast(arg__1); }
inline QVector<int >  py_q_read() const { return this->read(); }
};

class PythonQtWrapper_trikControl__VectorSensorInterface : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
void delete_trikControl__VectorSensorInterface(trikControl::VectorSensorInterface* obj) { delete obj; }
   const QMetaObject*  metaObject(trikControl::VectorSensorInterface* theWrappedObject) const;
   const QMetaObject*  py_q_metaObject(trikControl::VectorSensorInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__VectorSensorInterface*)theWrappedObject)->py_q_metaObject());}
   void newData(trikControl::VectorSensorInterface* theWrappedObject, QVector<int >  reading, const trikKernel::TimeVal&  eventTime);
   void*  qt_metacast(trikControl::VectorSensorInterface* theWrappedObject, const char*  arg__1);
   void*  py_q_qt_metacast(trikControl::VectorSensorInterface* theWrappedObject, const char*  arg__1){  return (((PythonQtPublicPromoter_trikControl__VectorSensorInterface*)theWrappedObject)->py_q_qt_metacast(arg__1));}
   QVector<int >  read(trikControl::VectorSensorInterface* theWrappedObject) const;
   QVector<int >  py_q_read(trikControl::VectorSensorInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__VectorSensorInterface*)theWrappedObject)->py_q_read());}
   QString  static_trikControl__VectorSensorInterface_tr(const char*  s, const char*  c = nullptr, int  n = -1);
   QString  static_trikControl__VectorSensorInterface_trUtf8(const char*  s, const char*  c = nullptr, int  n = -1);
};


