#include <PythonQt.h>
#include <PythonQtConversion.h>
#include "pytrikcontrol0.h"



void PythonQt_init_QtPyTrikControl(PyObject* module) {
PythonQt::priv()->registerCPPClass("trikControl", "", "QtPyTrikControl", PythonQtCreateObject<PythonQtWrapper_trikControl>, nullptr, module, 0);
PythonQt::priv()->registerClass(&trikControl::BatteryInterface::staticMetaObject, "QtPyTrikControl", PythonQtCreateObject<PythonQtWrapper_trikControl__BatteryInterface>, nullptr, module, 0);
PythonQt::self()->addParentClass("trikControl::BatteryInterface", "trikControl::DeviceInterface",PythonQtUpcastingOffset<trikControl::BatteryInterface,trikControl::DeviceInterface>());
PythonQt::priv()->registerCPPClass("trikControl::BrickFactory", "", "QtPyTrikControl", PythonQtCreateObject<PythonQtWrapper_trikControl__BrickFactory>, nullptr, module, 0);
PythonQt::priv()->registerClass(&trikControl::BrickInterface::staticMetaObject, "QtPyTrikControl", PythonQtCreateObject<PythonQtWrapper_trikControl__BrickInterface>, nullptr, module, 0);
PythonQt::priv()->registerCPPClass("trikControl::CameraDeviceInterface", "", "QtPyTrikControl", PythonQtCreateObject<PythonQtWrapper_trikControl__CameraDeviceInterface>, nullptr, module, 0);
PythonQt::self()->addParentClass("trikControl::CameraDeviceInterface", "trikControl::DeviceInterface",PythonQtUpcastingOffset<trikControl::CameraDeviceInterface,trikControl::DeviceInterface>());
PythonQt::priv()->registerClass(&trikControl::ColorSensorInterface::staticMetaObject, "QtPyTrikControl", PythonQtCreateObject<PythonQtWrapper_trikControl__ColorSensorInterface>, nullptr, module, 0);
PythonQt::self()->addParentClass("trikControl::ColorSensorInterface", "trikControl::DeviceInterface",PythonQtUpcastingOffset<trikControl::ColorSensorInterface,trikControl::DeviceInterface>());
PythonQt::priv()->registerCPPClass("trikControl::DeviceInterface", "", "QtPyTrikControl", PythonQtCreateObject<PythonQtWrapper_trikControl__DeviceInterface>, nullptr, module, 0);
PythonQt::priv()->registerClass(&trikControl::DisplayInterface::staticMetaObject, "QtPyTrikControl", PythonQtCreateObject<PythonQtWrapper_trikControl__DisplayInterface>, nullptr, module, 0);
PythonQt::priv()->registerClass(&trikControl::DisplayWidgetInterface::staticMetaObject, "QtPyTrikControl", PythonQtCreateObject<PythonQtWrapper_trikControl__DisplayWidgetInterface>, nullptr, module, 0);
PythonQt::priv()->registerClass(&trikControl::EncoderInterface::staticMetaObject, "QtPyTrikControl", PythonQtCreateObject<PythonQtWrapper_trikControl__EncoderInterface>, nullptr, module, 0);
PythonQt::self()->addParentClass("trikControl::EncoderInterface", "trikControl::DeviceInterface",PythonQtUpcastingOffset<trikControl::EncoderInterface,trikControl::DeviceInterface>());
PythonQt::priv()->registerClass(&trikControl::EventCodeInterface::staticMetaObject, "QtPyTrikControl", PythonQtCreateObject<PythonQtWrapper_trikControl__EventCodeInterface>, nullptr, module, 0);
PythonQt::priv()->registerClass(&trikControl::EventDeviceInterface::staticMetaObject, "QtPyTrikControl", PythonQtCreateObject<PythonQtWrapper_trikControl__EventDeviceInterface>, nullptr, module, 0);
PythonQt::self()->addParentClass("trikControl::EventDeviceInterface", "trikControl::DeviceInterface",PythonQtUpcastingOffset<trikControl::EventDeviceInterface,trikControl::DeviceInterface>());
PythonQt::priv()->registerClass(&trikControl::EventInterface::staticMetaObject, "QtPyTrikControl", PythonQtCreateObject<PythonQtWrapper_trikControl__EventInterface>, nullptr, module, 0);
PythonQt::priv()->registerClass(&trikControl::FifoInterface::staticMetaObject, "QtPyTrikControl", PythonQtCreateObject<PythonQtWrapper_trikControl__FifoInterface>, nullptr, module, 0);
PythonQt::self()->addParentClass("trikControl::FifoInterface", "trikControl::DeviceInterface",PythonQtUpcastingOffset<trikControl::FifoInterface,trikControl::DeviceInterface>());
PythonQt::priv()->registerClass(&trikControl::GamepadInterface::staticMetaObject, "QtPyTrikControl", PythonQtCreateObject<PythonQtWrapper_trikControl__GamepadInterface>, nullptr, module, 0);
PythonQt::priv()->registerClass(&trikControl::GyroSensorInterface::staticMetaObject, "QtPyTrikControl", PythonQtCreateObject<PythonQtWrapper_trikControl__GyroSensorInterface>, nullptr, module, 0);
PythonQt::self()->addParentClass("trikControl::GyroSensorInterface", "trikControl::VectorSensorInterface",PythonQtUpcastingOffset<trikControl::GyroSensorInterface,trikControl::VectorSensorInterface>());
PythonQt::self()->addParentClass("trikControl::GyroSensorInterface", "trikControl::DeviceInterface",PythonQtUpcastingOffset<trikControl::GyroSensorInterface,trikControl::DeviceInterface>());
PythonQt::priv()->registerClass(&trikControl::I2cDeviceInterface::staticMetaObject, "QtPyTrikControl", PythonQtCreateObject<PythonQtWrapper_trikControl__I2cDeviceInterface>, nullptr, module, 0);
PythonQt::self()->addParentClass("trikControl::I2cDeviceInterface", "trikControl::DeviceInterface",PythonQtUpcastingOffset<trikControl::I2cDeviceInterface,trikControl::DeviceInterface>());
PythonQt::priv()->registerClass(&trikControl::KeysInterface::staticMetaObject, "QtPyTrikControl", PythonQtCreateObject<PythonQtWrapper_trikControl__KeysInterface>, nullptr, module, 0);
PythonQt::self()->addParentClass("trikControl::KeysInterface", "trikControl::DeviceInterface",PythonQtUpcastingOffset<trikControl::KeysInterface,trikControl::DeviceInterface>());
PythonQt::priv()->registerClass(&trikControl::LedInterface::staticMetaObject, "QtPyTrikControl", PythonQtCreateObject<PythonQtWrapper_trikControl__LedInterface>, nullptr, module, 0);
PythonQt::self()->addParentClass("trikControl::LedInterface", "trikControl::DeviceInterface",PythonQtUpcastingOffset<trikControl::LedInterface,trikControl::DeviceInterface>());
PythonQt::priv()->registerClass(&trikControl::LidarInterface::staticMetaObject, "QtPyTrikControl", PythonQtCreateObject<PythonQtWrapper_trikControl__LidarInterface>, nullptr, module, 0);
PythonQt::self()->addParentClass("trikControl::LidarInterface", "trikControl::VectorSensorInterface",PythonQtUpcastingOffset<trikControl::LidarInterface,trikControl::VectorSensorInterface>());
PythonQt::self()->addParentClass("trikControl::LidarInterface", "trikControl::DeviceInterface",PythonQtUpcastingOffset<trikControl::LidarInterface,trikControl::DeviceInterface>());
PythonQt::priv()->registerClass(&trikControl::LineSensorInterface::staticMetaObject, "QtPyTrikControl", PythonQtCreateObject<PythonQtWrapper_trikControl__LineSensorInterface>, nullptr, module, 0);
PythonQt::self()->addParentClass("trikControl::LineSensorInterface", "trikControl::DeviceInterface",PythonQtUpcastingOffset<trikControl::LineSensorInterface,trikControl::DeviceInterface>());
PythonQt::priv()->registerClass(&trikControl::MarkerInterface::staticMetaObject, "QtPyTrikControl", PythonQtCreateObject<PythonQtWrapper_trikControl__MarkerInterface>, nullptr, module, 0);
PythonQt::self()->addParentClass("trikControl::MarkerInterface", "trikControl::DeviceInterface",PythonQtUpcastingOffset<trikControl::MarkerInterface,trikControl::DeviceInterface>());
PythonQt::priv()->registerClass(&trikControl::MotorInterface::staticMetaObject, "QtPyTrikControl", PythonQtCreateObject<PythonQtWrapper_trikControl__MotorInterface>, nullptr, module, 0);
PythonQt::self()->addParentClass("trikControl::MotorInterface", "trikControl::DeviceInterface",PythonQtUpcastingOffset<trikControl::MotorInterface,trikControl::DeviceInterface>());
PythonQt::priv()->registerClass(&trikControl::ObjectSensorInterface::staticMetaObject, "QtPyTrikControl", PythonQtCreateObject<PythonQtWrapper_trikControl__ObjectSensorInterface>, nullptr, module, 0);
PythonQt::self()->addParentClass("trikControl::ObjectSensorInterface", "trikControl::DeviceInterface",PythonQtUpcastingOffset<trikControl::ObjectSensorInterface,trikControl::DeviceInterface>());
PythonQt::priv()->registerClass(&trikControl::PwmCaptureInterface::staticMetaObject, "QtPyTrikControl", PythonQtCreateObject<PythonQtWrapper_trikControl__PwmCaptureInterface>, nullptr, module, 0);
PythonQt::self()->addParentClass("trikControl::PwmCaptureInterface", "trikControl::DeviceInterface",PythonQtUpcastingOffset<trikControl::PwmCaptureInterface,trikControl::DeviceInterface>());
PythonQt::priv()->registerClass(&trikControl::SensorInterface::staticMetaObject, "QtPyTrikControl", PythonQtCreateObject<PythonQtWrapper_trikControl__SensorInterface>, nullptr, module, 0);
PythonQt::self()->addParentClass("trikControl::SensorInterface", "trikControl::DeviceInterface",PythonQtUpcastingOffset<trikControl::SensorInterface,trikControl::DeviceInterface>());
PythonQt::priv()->registerClass(&trikControl::SoundSensorInterface::staticMetaObject, "QtPyTrikControl", PythonQtCreateObject<PythonQtWrapper_trikControl__SoundSensorInterface>, nullptr, module, 0);
PythonQt::self()->addParentClass("trikControl::SoundSensorInterface", "trikControl::DeviceInterface",PythonQtUpcastingOffset<trikControl::SoundSensorInterface,trikControl::DeviceInterface>());
PythonQt::priv()->registerClass(&trikControl::VectorSensorInterface::staticMetaObject, "QtPyTrikControl", PythonQtCreateObject<PythonQtWrapper_trikControl__VectorSensorInterface>, nullptr, module, 0);
PythonQt::self()->addParentClass("trikControl::VectorSensorInterface", "trikControl::DeviceInterface",PythonQtUpcastingOffset<trikControl::VectorSensorInterface,trikControl::DeviceInterface>());


}
