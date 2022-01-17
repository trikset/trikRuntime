#include <PythonQt.h>
#include <QObject>
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



class PythonQtWrapper_trikControl : public QObject
{ Q_OBJECT
public:
public slots:
};





class PythonQtPublicPromoter_trikControl__BatteryInterface : public trikControl::BatteryInterface
{ public:
inline float  py_q_readRawDataVoltage() { return this->readRawDataVoltage(); }
inline float  py_q_readVoltage() { return this->readVoltage(); }
};

class PythonQtWrapper_trikControl__BatteryInterface : public QObject
{ Q_OBJECT
public:
public slots:
void delete_trikControl__BatteryInterface(trikControl::BatteryInterface* obj) { delete obj; } 
   float  py_q_readRawDataVoltage(trikControl::BatteryInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__BatteryInterface*)theWrappedObject)->py_q_readRawDataVoltage());}
   float  py_q_readVoltage(trikControl::BatteryInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__BatteryInterface*)theWrappedObject)->py_q_readVoltage());}
};





class PythonQtWrapper_trikControl__BrickFactory : public QObject
{ Q_OBJECT
public:
public slots:
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
inline trikControl::I2cDeviceInterface*  py_q_i2c(int  bus, int  address) { return this->i2c(bus, address); }
inline trikControl::KeysInterface*  py_q_keys() { return this->keys(); }
inline trikControl::LedInterface*  py_q_led() { return this->led(); }
inline trikControl::LidarInterface*  py_q_lidar() { return this->lidar(); }
inline trikControl::LineSensorInterface*  py_q_lineSensor(const QString&  port) { return this->lineSensor(port); }
inline trikControl::MarkerInterface*  py_q_marker() { return this->marker(); }
inline trikControl::MotorInterface*  py_q_motor(const QString&  port) { return this->motor(port); }
inline QStringList  py_q_motorPorts(trikControl::MotorInterface::Type  type) const { return this->motorPorts(type); }
inline trikControl::ObjectSensorInterface*  py_q_objectSensor(const QString&  port) { return this->objectSensor(port); }
inline void py_q_playSound(const QString&  soundFileName) { this->playSound(soundFileName); }
inline void py_q_playTone(int  hzFreq, int  msDuration) { this->playTone(hzFreq, msDuration); }
inline trikControl::PwmCaptureInterface*  py_q_pwmCapture(const QString&  port) { return this->pwmCapture(port); }
inline QStringList  py_q_pwmCapturePorts() const { return this->pwmCapturePorts(); }
inline void py_q_reset() { this->reset(); }
inline void py_q_say(const QString&  text) { this->say(text); }
inline trikControl::SensorInterface*  py_q_sensor(const QString&  port) { return this->sensor(port); }
inline QStringList  py_q_sensorPorts(trikControl::SensorInterface::Type  type) const { return this->sensorPorts(type); }
inline trikControl::SoundSensorInterface*  py_q_soundSensor(const QString&  port) { return this->soundSensor(port); }
inline void py_q_stop() { this->stop(); }
inline void py_q_stopEventDevice(const QString&  deviceFile) { this->stopEventDevice(deviceFile); }
};

class PythonQtWrapper_trikControl__BrickInterface : public QObject
{ Q_OBJECT
public:
public slots:
void delete_trikControl__BrickInterface(trikControl::BrickInterface* obj) { delete obj; } 
   trikControl::VectorSensorInterface*  py_q_accelerometer(trikControl::BrickInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_accelerometer());}
   trikControl::BatteryInterface*  py_q_battery(trikControl::BrickInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_battery());}
   trikControl::ColorSensorInterface*  py_q_colorSensor(trikControl::BrickInterface* theWrappedObject, const QString&  port){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_colorSensor(port));}
   QString  configVersion(trikControl::BrickInterface* theWrappedObject) const;
   QString  py_q_configVersion(trikControl::BrickInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_configVersion());}
   void py_q_configure(trikControl::BrickInterface* theWrappedObject, const QString&  portName, const QString&  deviceName){  (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_configure(portName, deviceName));}
   trikControl::DisplayInterface*  py_q_display(trikControl::BrickInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_display());}
   trikControl::EncoderInterface*  py_q_encoder(trikControl::BrickInterface* theWrappedObject, const QString&  port){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_encoder(port));}
   QStringList  py_q_encoderPorts(trikControl::BrickInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_encoderPorts());}
   trikControl::EventDeviceInterface*  py_q_eventDevice(trikControl::BrickInterface* theWrappedObject, const QString&  deviceFile){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_eventDevice(deviceFile));}
   trikControl::FifoInterface*  py_q_fifo(trikControl::BrickInterface* theWrappedObject, const QString&  port){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_fifo(port));}
   trikControl::GamepadInterface*  py_q_gamepad(trikControl::BrickInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_gamepad());}
   trikControl::DisplayWidgetInterface*  graphicsWidget(trikControl::BrickInterface* theWrappedObject);
   trikControl::DisplayWidgetInterface*  py_q_graphicsWidget(trikControl::BrickInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_graphicsWidget());}
   trikControl::GyroSensorInterface*  py_q_gyroscope(trikControl::BrickInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_gyroscope());}
   trikControl::I2cDeviceInterface*  py_q_i2c(trikControl::BrickInterface* theWrappedObject, int  bus, int  address){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_i2c(bus, address));}
   trikControl::KeysInterface*  py_q_keys(trikControl::BrickInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_keys());}
   trikControl::LedInterface*  py_q_led(trikControl::BrickInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_led());}
   trikControl::LidarInterface*  py_q_lidar(trikControl::BrickInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_lidar());}
   trikControl::LineSensorInterface*  py_q_lineSensor(trikControl::BrickInterface* theWrappedObject, const QString&  port){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_lineSensor(port));}
   trikControl::MarkerInterface*  py_q_marker(trikControl::BrickInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_marker());}
   trikControl::MotorInterface*  py_q_motor(trikControl::BrickInterface* theWrappedObject, const QString&  port){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_motor(port));}
   QStringList  py_q_motorPorts(trikControl::BrickInterface* theWrappedObject, trikControl::MotorInterface::Type  type) const{  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_motorPorts(type));}
   trikControl::ObjectSensorInterface*  py_q_objectSensor(trikControl::BrickInterface* theWrappedObject, const QString&  port){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_objectSensor(port));}
   void py_q_playSound(trikControl::BrickInterface* theWrappedObject, const QString&  soundFileName){  (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_playSound(soundFileName));}
   void py_q_playTone(trikControl::BrickInterface* theWrappedObject, int  hzFreq, int  msDuration){  (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_playTone(hzFreq, msDuration));}
   trikControl::PwmCaptureInterface*  py_q_pwmCapture(trikControl::BrickInterface* theWrappedObject, const QString&  port){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_pwmCapture(port));}
   QStringList  py_q_pwmCapturePorts(trikControl::BrickInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_pwmCapturePorts());}
   void reset(trikControl::BrickInterface* theWrappedObject);
   void py_q_reset(trikControl::BrickInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_reset());}
   void py_q_say(trikControl::BrickInterface* theWrappedObject, const QString&  text){  (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_say(text));}
   trikControl::SensorInterface*  py_q_sensor(trikControl::BrickInterface* theWrappedObject, const QString&  port){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_sensor(port));}
   QStringList  py_q_sensorPorts(trikControl::BrickInterface* theWrappedObject, trikControl::SensorInterface::Type  type) const{  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_sensorPorts(type));}
   trikControl::SoundSensorInterface*  py_q_soundSensor(trikControl::BrickInterface* theWrappedObject, const QString&  port){  return (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_soundSensor(port));}
   void py_q_stop(trikControl::BrickInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_stop());}
   void py_q_stopEventDevice(trikControl::BrickInterface* theWrappedObject, const QString&  deviceFile){  (((PythonQtPublicPromoter_trikControl__BrickInterface*)theWrappedObject)->py_q_stopEventDevice(deviceFile));}
};





class PythonQtWrapper_trikControl__CameraDeviceInterface : public QObject
{ Q_OBJECT
public:
public slots:
void delete_trikControl__CameraDeviceInterface(trikControl::CameraDeviceInterface* obj) { delete obj; } 
};





class PythonQtPublicPromoter_trikControl__ColorSensorInterface : public trikControl::ColorSensorInterface
{ public:
inline void py_q_init(bool  showOnDisplay) { this->init(showOnDisplay); }
inline QVector<int >  py_q_read(int  m, int  n) { return this->read(m, n); }
inline void py_q_stop() { this->stop(); }
};

class PythonQtWrapper_trikControl__ColorSensorInterface : public QObject
{ Q_OBJECT
public:
public slots:
void delete_trikControl__ColorSensorInterface(trikControl::ColorSensorInterface* obj) { delete obj; } 
   void py_q_init(trikControl::ColorSensorInterface* theWrappedObject, bool  showOnDisplay){  (((PythonQtPublicPromoter_trikControl__ColorSensorInterface*)theWrappedObject)->py_q_init(showOnDisplay));}
   QVector<int >  py_q_read(trikControl::ColorSensorInterface* theWrappedObject, int  m, int  n){  return (((PythonQtPublicPromoter_trikControl__ColorSensorInterface*)theWrappedObject)->py_q_read(m, n));}
   void py_q_stop(trikControl::ColorSensorInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__ColorSensorInterface*)theWrappedObject)->py_q_stop());}
};





class PythonQtPublicPromoter_trikControl__DeviceInterface : public trikControl::DeviceInterface
{ public:
inline trikControl::DeviceInterface::Status  py_q_status() const { return this->status(); }
};

class PythonQtWrapper_trikControl__DeviceInterface : public QObject
{ Q_OBJECT
public:
public slots:
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
public slots:
void delete_trikControl__DisplayInterface(trikControl::DisplayInterface* obj) { delete obj; } 
   void py_q_addLabel(trikControl::DisplayInterface* theWrappedObject, const QString&  text, int  x, int  y, int  fontSize = -1){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_addLabel(text, x, y, fontSize));}
   void py_q_clear(trikControl::DisplayInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_clear());}
   void py_q_drawArc(trikControl::DisplayInterface* theWrappedObject, int  x, int  y, int  width, int  height, int  startAngle, int  spanAngle){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_drawArc(x, y, width, height, startAngle, spanAngle));}
   void py_q_drawEllipse(trikControl::DisplayInterface* theWrappedObject, int  x, int  y, int  width, int  height, bool  filled = false){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_drawEllipse(x, y, width, height, filled));}
   void py_q_drawLine(trikControl::DisplayInterface* theWrappedObject, int  x1, int  y1, int  x2, int  y2){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_drawLine(x1, y1, x2, y2));}
   void py_q_drawPoint(trikControl::DisplayInterface* theWrappedObject, int  x, int  y){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_drawPoint(x, y));}
   void py_q_drawRect(trikControl::DisplayInterface* theWrappedObject, int  x, int  y, int  width, int  height, bool  filled = false){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_drawRect(x, y, width, height, filled));}
   trikControl::DisplayWidgetInterface*  graphicsWidget(trikControl::DisplayInterface* theWrappedObject);
   trikControl::DisplayWidgetInterface*  py_q_graphicsWidget(trikControl::DisplayInterface* theWrappedObject){  return &(((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_graphicsWidget());}
   void py_q_hide(trikControl::DisplayInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_hide());}
   void py_q_redraw(trikControl::DisplayInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_redraw());}
   void py_q_removeLabels(trikControl::DisplayInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_removeLabels());}
   void py_q_reset(trikControl::DisplayInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_reset());}
   void py_q_setBackground(trikControl::DisplayInterface* theWrappedObject, const QString&  color){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_setBackground(color));}
   void py_q_setPainterColor(trikControl::DisplayInterface* theWrappedObject, const QString&  color){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_setPainterColor(color));}
   void py_q_setPainterWidth(trikControl::DisplayInterface* theWrappedObject, int  penWidth){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_setPainterWidth(penWidth));}
   void py_q_showImage(trikControl::DisplayInterface* theWrappedObject, const QString&  fileName){  (((PythonQtPublicPromoter_trikControl__DisplayInterface*)theWrappedObject)->py_q_showImage(fileName));}
};





class PythonQtWrapper_trikControl__DisplayWidgetInterface : public QObject
{ Q_OBJECT
public:
public slots:
void delete_trikControl__DisplayWidgetInterface(trikControl::DisplayWidgetInterface* obj) { delete obj; } 
};





class PythonQtPublicPromoter_trikControl__EncoderInterface : public trikControl::EncoderInterface
{ public:
inline int  py_q_read() { return this->read(); }
inline int  py_q_readRawData() { return this->readRawData(); }
inline void py_q_reset() { this->reset(); }
};

class PythonQtWrapper_trikControl__EncoderInterface : public QObject
{ Q_OBJECT
public:
public slots:
void delete_trikControl__EncoderInterface(trikControl::EncoderInterface* obj) { delete obj; } 
   int  py_q_read(trikControl::EncoderInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__EncoderInterface*)theWrappedObject)->py_q_read());}
   int  py_q_readRawData(trikControl::EncoderInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__EncoderInterface*)theWrappedObject)->py_q_readRawData());}
   void py_q_reset(trikControl::EncoderInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__EncoderInterface*)theWrappedObject)->py_q_reset());}
};





class PythonQtWrapper_trikControl__EventCodeInterface : public QObject
{ Q_OBJECT
public:
public slots:
trikControl::EventCodeInterface* new_trikControl__EventCodeInterface();
void delete_trikControl__EventCodeInterface(trikControl::EventCodeInterface* obj) { delete obj; } 
};





class PythonQtPublicPromoter_trikControl__EventDeviceInterface : public trikControl::EventDeviceInterface
{ public:
inline trikControl::EventInterface*  py_q_onEvent(int  eventType) { return this->onEvent(eventType); }
};

class PythonQtWrapper_trikControl__EventDeviceInterface : public QObject
{ Q_OBJECT
public:
public slots:
void delete_trikControl__EventDeviceInterface(trikControl::EventDeviceInterface* obj) { delete obj; } 
   trikControl::EventInterface*  py_q_onEvent(trikControl::EventDeviceInterface* theWrappedObject, int  eventType){  return (((PythonQtPublicPromoter_trikControl__EventDeviceInterface*)theWrappedObject)->py_q_onEvent(eventType));}
};





class PythonQtPublicPromoter_trikControl__EventInterface : public trikControl::EventInterface
{ public:
inline trikControl::EventCodeInterface*  py_q_code(int  code) { return this->code(code); }
};

class PythonQtWrapper_trikControl__EventInterface : public QObject
{ Q_OBJECT
public:
public slots:
void delete_trikControl__EventInterface(trikControl::EventInterface* obj) { delete obj; } 
   trikControl::EventCodeInterface*  py_q_code(trikControl::EventInterface* theWrappedObject, int  code){  return (((PythonQtPublicPromoter_trikControl__EventInterface*)theWrappedObject)->py_q_code(code));}
};





class PythonQtPublicPromoter_trikControl__FifoInterface : public trikControl::FifoInterface
{ public:
inline bool  py_q_hasData() const { return this->hasData(); }
inline bool  py_q_hasLine() const { return this->hasLine(); }
inline QString  py_q_read() { return this->read(); }
};

class PythonQtWrapper_trikControl__FifoInterface : public QObject
{ Q_OBJECT
public:
public slots:
void delete_trikControl__FifoInterface(trikControl::FifoInterface* obj) { delete obj; } 
   bool  py_q_hasData(trikControl::FifoInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__FifoInterface*)theWrappedObject)->py_q_hasData());}
   bool  py_q_hasLine(trikControl::FifoInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__FifoInterface*)theWrappedObject)->py_q_hasLine());}
   QString  py_q_read(trikControl::FifoInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__FifoInterface*)theWrappedObject)->py_q_read());}
};





class PythonQtPublicPromoter_trikControl__GamepadInterface : public trikControl::GamepadInterface
{ public:
inline bool  py_q_buttonIsPressed(int  buttonNumber) { return this->buttonIsPressed(buttonNumber); }
inline bool  py_q_buttonWasPressed(int  buttonNumber) { return this->buttonWasPressed(buttonNumber); }
inline bool  py_q_disconnect() { return this->disconnect(); }
inline bool  py_q_isConnected() const { return this->isConnected(); }
inline bool  py_q_isPadPressed(int  pad) const { return this->isPadPressed(pad); }
inline int  py_q_padX(int  pad) const { return this->padX(pad); }
inline int  py_q_padY(int  pad) const { return this->padY(pad); }
inline void py_q_reset() { this->reset(); }
inline int  py_q_wheel() const { return this->wheel(); }
};

class PythonQtWrapper_trikControl__GamepadInterface : public QObject
{ Q_OBJECT
public:
public slots:
void delete_trikControl__GamepadInterface(trikControl::GamepadInterface* obj) { delete obj; } 
   bool  py_q_buttonIsPressed(trikControl::GamepadInterface* theWrappedObject, int  buttonNumber){  return (((PythonQtPublicPromoter_trikControl__GamepadInterface*)theWrappedObject)->py_q_buttonIsPressed(buttonNumber));}
   bool  py_q_buttonWasPressed(trikControl::GamepadInterface* theWrappedObject, int  buttonNumber){  return (((PythonQtPublicPromoter_trikControl__GamepadInterface*)theWrappedObject)->py_q_buttonWasPressed(buttonNumber));}
   bool  py_q_disconnect(trikControl::GamepadInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__GamepadInterface*)theWrappedObject)->py_q_disconnect());}
   bool  py_q_isConnected(trikControl::GamepadInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__GamepadInterface*)theWrappedObject)->py_q_isConnected());}
   bool  py_q_isPadPressed(trikControl::GamepadInterface* theWrappedObject, int  pad) const{  return (((PythonQtPublicPromoter_trikControl__GamepadInterface*)theWrappedObject)->py_q_isPadPressed(pad));}
   int  py_q_padX(trikControl::GamepadInterface* theWrappedObject, int  pad) const{  return (((PythonQtPublicPromoter_trikControl__GamepadInterface*)theWrappedObject)->py_q_padX(pad));}
   int  py_q_padY(trikControl::GamepadInterface* theWrappedObject, int  pad) const{  return (((PythonQtPublicPromoter_trikControl__GamepadInterface*)theWrappedObject)->py_q_padY(pad));}
   void py_q_reset(trikControl::GamepadInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__GamepadInterface*)theWrappedObject)->py_q_reset());}
   int  py_q_wheel(trikControl::GamepadInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__GamepadInterface*)theWrappedObject)->py_q_wheel());}
};





class PythonQtPublicPromoter_trikControl__GyroSensorInterface : public trikControl::GyroSensorInterface
{ public:
inline void py_q_calibrate(int  msec) { this->calibrate(msec); }
inline QVector<int >  py_q_getCalibrationValues() { return this->getCalibrationValues(); }
inline bool  py_q_isCalibrated() const { return this->isCalibrated(); }
inline QVector<int >  py_q_readRawData() const { return this->readRawData(); }
inline void py_q_setCalibrationValues(const QVector<int >&  values) { this->setCalibrationValues(values); }
};

class PythonQtWrapper_trikControl__GyroSensorInterface : public QObject
{ Q_OBJECT
public:
public slots:
void delete_trikControl__GyroSensorInterface(trikControl::GyroSensorInterface* obj) { delete obj; } 
   void py_q_calibrate(trikControl::GyroSensorInterface* theWrappedObject, int  msec){  (((PythonQtPublicPromoter_trikControl__GyroSensorInterface*)theWrappedObject)->py_q_calibrate(msec));}
   QVector<int >  py_q_getCalibrationValues(trikControl::GyroSensorInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__GyroSensorInterface*)theWrappedObject)->py_q_getCalibrationValues());}
   bool  py_q_isCalibrated(trikControl::GyroSensorInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__GyroSensorInterface*)theWrappedObject)->py_q_isCalibrated());}
   QVector<int >  py_q_readRawData(trikControl::GyroSensorInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__GyroSensorInterface*)theWrappedObject)->py_q_readRawData());}
   void py_q_setCalibrationValues(trikControl::GyroSensorInterface* theWrappedObject, const QVector<int >&  values){  (((PythonQtPublicPromoter_trikControl__GyroSensorInterface*)theWrappedObject)->py_q_setCalibrationValues(values));}
};





class PythonQtPublicPromoter_trikControl__I2cDeviceInterface : public trikControl::I2cDeviceInterface
{ public:
inline int  py_q_read(int  reg) { return this->read(reg); }
inline void py_q_send(int  reg, int  value) { this->send(reg, value); }
};

class PythonQtWrapper_trikControl__I2cDeviceInterface : public QObject
{ Q_OBJECT
public:
public slots:
void delete_trikControl__I2cDeviceInterface(trikControl::I2cDeviceInterface* obj) { delete obj; } 
   int  read(trikControl::I2cDeviceInterface* theWrappedObject, int  reg);
   int  py_q_read(trikControl::I2cDeviceInterface* theWrappedObject, int  reg){  return (((PythonQtPublicPromoter_trikControl__I2cDeviceInterface*)theWrappedObject)->py_q_read(reg));}
   void send(trikControl::I2cDeviceInterface* theWrappedObject, int  reg, int  value);
   void py_q_send(trikControl::I2cDeviceInterface* theWrappedObject, int  reg, int  value){  (((PythonQtPublicPromoter_trikControl__I2cDeviceInterface*)theWrappedObject)->py_q_send(reg, value));}
};





class PythonQtPublicPromoter_trikControl__KeysInterface : public trikControl::KeysInterface
{ public:
inline int  py_q_buttonCode(bool  wait = true) { return this->buttonCode(wait); }
inline bool  py_q_isPressed(int  code) { return this->isPressed(code); }
inline void py_q_reset() { this->reset(); }
inline bool  py_q_wasPressed(int  code) { return this->wasPressed(code); }
};

class PythonQtWrapper_trikControl__KeysInterface : public QObject
{ Q_OBJECT
public:
public slots:
void delete_trikControl__KeysInterface(trikControl::KeysInterface* obj) { delete obj; } 
   int  py_q_buttonCode(trikControl::KeysInterface* theWrappedObject, bool  wait = true){  return (((PythonQtPublicPromoter_trikControl__KeysInterface*)theWrappedObject)->py_q_buttonCode(wait));}
   bool  py_q_isPressed(trikControl::KeysInterface* theWrappedObject, int  code){  return (((PythonQtPublicPromoter_trikControl__KeysInterface*)theWrappedObject)->py_q_isPressed(code));}
   void py_q_reset(trikControl::KeysInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__KeysInterface*)theWrappedObject)->py_q_reset());}
   bool  py_q_wasPressed(trikControl::KeysInterface* theWrappedObject, int  code){  return (((PythonQtPublicPromoter_trikControl__KeysInterface*)theWrappedObject)->py_q_wasPressed(code));}
};





class PythonQtPublicPromoter_trikControl__LedInterface : public trikControl::LedInterface
{ public:
inline void py_q_green() { this->green(); }
inline void py_q_off() { this->off(); }
inline void py_q_orange() { this->orange(); }
inline void py_q_red() { this->red(); }
};

class PythonQtWrapper_trikControl__LedInterface : public QObject
{ Q_OBJECT
public:
public slots:
void delete_trikControl__LedInterface(trikControl::LedInterface* obj) { delete obj; } 
   void py_q_green(trikControl::LedInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__LedInterface*)theWrappedObject)->py_q_green());}
   void py_q_off(trikControl::LedInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__LedInterface*)theWrappedObject)->py_q_off());}
   void py_q_orange(trikControl::LedInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__LedInterface*)theWrappedObject)->py_q_orange());}
   void py_q_red(trikControl::LedInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__LedInterface*)theWrappedObject)->py_q_red());}
};





class PythonQtWrapper_trikControl__LidarInterface : public QObject
{ Q_OBJECT
public:
public slots:
void delete_trikControl__LidarInterface(trikControl::LidarInterface* obj) { delete obj; } 
};





class PythonQtPublicPromoter_trikControl__LineSensorInterface : public trikControl::LineSensorInterface
{ public:
inline void py_q_detect() { this->detect(); }
inline QVector<int >  py_q_getDetectParameters() const { return this->getDetectParameters(); }
inline void py_q_init(bool  showOnDisplay) { this->init(showOnDisplay); }
inline QVector<int >  py_q_read() { return this->read(); }
inline void py_q_stop() { this->stop(); }
};

class PythonQtWrapper_trikControl__LineSensorInterface : public QObject
{ Q_OBJECT
public:
public slots:
void delete_trikControl__LineSensorInterface(trikControl::LineSensorInterface* obj) { delete obj; } 
   void py_q_detect(trikControl::LineSensorInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__LineSensorInterface*)theWrappedObject)->py_q_detect());}
   QVector<int >  py_q_getDetectParameters(trikControl::LineSensorInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__LineSensorInterface*)theWrappedObject)->py_q_getDetectParameters());}
   void py_q_init(trikControl::LineSensorInterface* theWrappedObject, bool  showOnDisplay){  (((PythonQtPublicPromoter_trikControl__LineSensorInterface*)theWrappedObject)->py_q_init(showOnDisplay));}
   QVector<int >  py_q_read(trikControl::LineSensorInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__LineSensorInterface*)theWrappedObject)->py_q_read());}
   void py_q_stop(trikControl::LineSensorInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__LineSensorInterface*)theWrappedObject)->py_q_stop());}
};





class PythonQtPublicPromoter_trikControl__MarkerInterface : public trikControl::MarkerInterface
{ public:
inline void py_q_down(const QString&  color) { this->down(color); }
inline bool  py_q_isDown() const { return this->isDown(); }
inline void py_q_setDown(bool  isDown) { this->setDown(isDown); }
inline void py_q_up() { this->up(); }
};

class PythonQtWrapper_trikControl__MarkerInterface : public QObject
{ Q_OBJECT
public:
public slots:
void delete_trikControl__MarkerInterface(trikControl::MarkerInterface* obj) { delete obj; } 
   void py_q_down(trikControl::MarkerInterface* theWrappedObject, const QString&  color){  (((PythonQtPublicPromoter_trikControl__MarkerInterface*)theWrappedObject)->py_q_down(color));}
   bool  py_q_isDown(trikControl::MarkerInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__MarkerInterface*)theWrappedObject)->py_q_isDown());}
   void py_q_setDown(trikControl::MarkerInterface* theWrappedObject, bool  isDown){  (((PythonQtPublicPromoter_trikControl__MarkerInterface*)theWrappedObject)->py_q_setDown(isDown));}
   void py_q_up(trikControl::MarkerInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__MarkerInterface*)theWrappedObject)->py_q_up());}
};





class PythonQtPublicPromoter_trikControl__MotorInterface : public trikControl::MotorInterface
{ public:
inline void py_q_brake(int  durationMs = 500) { this->brake(durationMs); }
inline int  py_q_maxControl() const { return this->maxControl(); }
inline int  py_q_minControl() const { return this->minControl(); }
inline int  py_q_power() const { return this->power(); }
inline void py_q_powerOff() { this->powerOff(); }
inline void py_q_setPeriod(int  uSec = 20000) { this->setPeriod(uSec); }
inline void py_q_setPower(int  power, bool  constrain = true) { this->setPower(power, constrain); }
};

class PythonQtWrapper_trikControl__MotorInterface : public QObject
{ Q_OBJECT
public:
public slots:
void delete_trikControl__MotorInterface(trikControl::MotorInterface* obj) { delete obj; } 
   void py_q_brake(trikControl::MotorInterface* theWrappedObject, int  durationMs = 500){  (((PythonQtPublicPromoter_trikControl__MotorInterface*)theWrappedObject)->py_q_brake(durationMs));}
   int  maxControl(trikControl::MotorInterface* theWrappedObject) const;
   int  py_q_maxControl(trikControl::MotorInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__MotorInterface*)theWrappedObject)->py_q_maxControl());}
   int  minControl(trikControl::MotorInterface* theWrappedObject) const;
   int  py_q_minControl(trikControl::MotorInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__MotorInterface*)theWrappedObject)->py_q_minControl());}
   int  py_q_power(trikControl::MotorInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__MotorInterface*)theWrappedObject)->py_q_power());}
   void py_q_powerOff(trikControl::MotorInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__MotorInterface*)theWrappedObject)->py_q_powerOff());}
   void py_q_setPeriod(trikControl::MotorInterface* theWrappedObject, int  uSec = 20000){  (((PythonQtPublicPromoter_trikControl__MotorInterface*)theWrappedObject)->py_q_setPeriod(uSec));}
   void py_q_setPower(trikControl::MotorInterface* theWrappedObject, int  power, bool  constrain = true){  (((PythonQtPublicPromoter_trikControl__MotorInterface*)theWrappedObject)->py_q_setPower(power, constrain));}
};





class PythonQtPublicPromoter_trikControl__ObjectSensorInterface : public trikControl::ObjectSensorInterface
{ public:
inline void py_q_detect() { this->detect(); }
inline QVector<int >  py_q_getDetectParameters() const { return this->getDetectParameters(); }
inline void py_q_init(bool  showOnDisplay) { this->init(showOnDisplay); }
inline QVector<int >  py_q_read() { return this->read(); }
inline void py_q_stop() { this->stop(); }
};

class PythonQtWrapper_trikControl__ObjectSensorInterface : public QObject
{ Q_OBJECT
public:
public slots:
void delete_trikControl__ObjectSensorInterface(trikControl::ObjectSensorInterface* obj) { delete obj; } 
   void py_q_detect(trikControl::ObjectSensorInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__ObjectSensorInterface*)theWrappedObject)->py_q_detect());}
   QVector<int >  py_q_getDetectParameters(trikControl::ObjectSensorInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__ObjectSensorInterface*)theWrappedObject)->py_q_getDetectParameters());}
   void py_q_init(trikControl::ObjectSensorInterface* theWrappedObject, bool  showOnDisplay){  (((PythonQtPublicPromoter_trikControl__ObjectSensorInterface*)theWrappedObject)->py_q_init(showOnDisplay));}
   QVector<int >  py_q_read(trikControl::ObjectSensorInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__ObjectSensorInterface*)theWrappedObject)->py_q_read());}
   void py_q_stop(trikControl::ObjectSensorInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__ObjectSensorInterface*)theWrappedObject)->py_q_stop());}
};





class PythonQtPublicPromoter_trikControl__PwmCaptureInterface : public trikControl::PwmCaptureInterface
{ public:
inline int  py_q_duty() { return this->duty(); }
inline QVector<int >  py_q_frequency() { return this->frequency(); }
};

class PythonQtWrapper_trikControl__PwmCaptureInterface : public QObject
{ Q_OBJECT
public:
public slots:
void delete_trikControl__PwmCaptureInterface(trikControl::PwmCaptureInterface* obj) { delete obj; } 
   int  py_q_duty(trikControl::PwmCaptureInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__PwmCaptureInterface*)theWrappedObject)->py_q_duty());}
   QVector<int >  py_q_frequency(trikControl::PwmCaptureInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__PwmCaptureInterface*)theWrappedObject)->py_q_frequency());}
};





class PythonQtPublicPromoter_trikControl__SensorInterface : public trikControl::SensorInterface
{ public:
inline int  py_q_maxValue() const { return this->maxValue(); }
inline int  py_q_minValue() const { return this->minValue(); }
inline int  py_q_read() { return this->read(); }
inline int  py_q_readRawData() { return this->readRawData(); }
};

class PythonQtWrapper_trikControl__SensorInterface : public QObject
{ Q_OBJECT
public:
public slots:
void delete_trikControl__SensorInterface(trikControl::SensorInterface* obj) { delete obj; } 
   int  maxValue(trikControl::SensorInterface* theWrappedObject) const;
   int  py_q_maxValue(trikControl::SensorInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__SensorInterface*)theWrappedObject)->py_q_maxValue());}
   int  minValue(trikControl::SensorInterface* theWrappedObject) const;
   int  py_q_minValue(trikControl::SensorInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__SensorInterface*)theWrappedObject)->py_q_minValue());}
   int  py_q_read(trikControl::SensorInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__SensorInterface*)theWrappedObject)->py_q_read());}
   int  py_q_readRawData(trikControl::SensorInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__SensorInterface*)theWrappedObject)->py_q_readRawData());}
};





class PythonQtPublicPromoter_trikControl__SoundSensorInterface : public trikControl::SoundSensorInterface
{ public:
inline void py_q_detect() { this->detect(); }
inline void py_q_init(bool  showOnDisplay) { this->init(showOnDisplay); }
inline QVector<int >  py_q_read() { return this->read(); }
inline void py_q_stop() { this->stop(); }
inline void py_q_volume(int  volCoeff) { this->volume(volCoeff); }
};

class PythonQtWrapper_trikControl__SoundSensorInterface : public QObject
{ Q_OBJECT
public:
public slots:
void delete_trikControl__SoundSensorInterface(trikControl::SoundSensorInterface* obj) { delete obj; } 
   void py_q_detect(trikControl::SoundSensorInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__SoundSensorInterface*)theWrappedObject)->py_q_detect());}
   void py_q_init(trikControl::SoundSensorInterface* theWrappedObject, bool  showOnDisplay){  (((PythonQtPublicPromoter_trikControl__SoundSensorInterface*)theWrappedObject)->py_q_init(showOnDisplay));}
   QVector<int >  py_q_read(trikControl::SoundSensorInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__SoundSensorInterface*)theWrappedObject)->py_q_read());}
   void py_q_stop(trikControl::SoundSensorInterface* theWrappedObject){  (((PythonQtPublicPromoter_trikControl__SoundSensorInterface*)theWrappedObject)->py_q_stop());}
   void py_q_volume(trikControl::SoundSensorInterface* theWrappedObject, int  volCoeff){  (((PythonQtPublicPromoter_trikControl__SoundSensorInterface*)theWrappedObject)->py_q_volume(volCoeff));}
};





class PythonQtPublicPromoter_trikControl__VectorSensorInterface : public trikControl::VectorSensorInterface
{ public:
inline QVector<int >  py_q_read() const { return this->read(); }
};

class PythonQtWrapper_trikControl__VectorSensorInterface : public QObject
{ Q_OBJECT
public:
public slots:
void delete_trikControl__VectorSensorInterface(trikControl::VectorSensorInterface* obj) { delete obj; } 
   QVector<int >  py_q_read(trikControl::VectorSensorInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__VectorSensorInterface*)theWrappedObject)->py_q_read());}
};


