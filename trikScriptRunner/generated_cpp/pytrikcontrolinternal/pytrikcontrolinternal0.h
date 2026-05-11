#include <PythonQt.h>
#include <QObject>
#include <QVariant>
#include <QtQuick/qquickitem.h>
#include <internalBrickInterface.h>
#include <internalDisplayInterface.h>
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



class PythonQtPublicPromoter_trikControl__DisplayWidgetInterface : public trikControl::DisplayWidgetInterface
{ public:
inline const QMetaObject*  py_q_metaObject() const { return trikControl::DisplayWidgetInterface::metaObject(); }
inline void*  py_q_qt_metacast(const char*  arg__1) { return trikControl::DisplayWidgetInterface::qt_metacast(arg__1); }
};

class PythonQtWrapper_trikControl__DisplayWidgetInterface : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
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





class PythonQtPublicPromoter_trikControl__InternalBrickInterface : public trikControl::InternalBrickInterface
{ public:
inline trikControl::DisplayWidgetInterface*  py_q_graphicsWidget() { return this->graphicsWidget(); }
inline const QMetaObject*  py_q_metaObject() const { return trikControl::InternalBrickInterface::metaObject(); }
inline void*  py_q_qt_metacast(const char*  arg__1) { return trikControl::InternalBrickInterface::qt_metacast(arg__1); }
};

class PythonQtWrapper_trikControl__InternalBrickInterface : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
void delete_trikControl__InternalBrickInterface(trikControl::InternalBrickInterface* obj) { delete obj; }
   trikControl::DisplayWidgetInterface*  graphicsWidget(trikControl::InternalBrickInterface* theWrappedObject);
   trikControl::DisplayWidgetInterface*  py_q_graphicsWidget(trikControl::InternalBrickInterface* theWrappedObject){  return (((PythonQtPublicPromoter_trikControl__InternalBrickInterface*)theWrappedObject)->py_q_graphicsWidget());}
   const QMetaObject*  py_q_metaObject(trikControl::InternalBrickInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__InternalBrickInterface*)theWrappedObject)->py_q_metaObject());}
   void*  py_q_qt_metacast(trikControl::InternalBrickInterface* theWrappedObject, const char*  arg__1){  return (((PythonQtPublicPromoter_trikControl__InternalBrickInterface*)theWrappedObject)->py_q_qt_metacast(arg__1));}
   QString  static_trikControl__InternalBrickInterface_tr(const char*  s, const char*  c = nullptr, int  n = -1);
   QString  static_trikControl__InternalBrickInterface_trUtf8(const char*  s, const char*  c = nullptr, int  n = -1);
};





class PythonQtPublicPromoter_trikControl__InternalDisplayInterface : public trikControl::InternalDisplayInterface
{ public:
inline trikControl::DisplayWidgetInterface&  py_q_graphicsWidget() { return this->graphicsWidget(); }
inline const QMetaObject*  py_q_metaObject() const { return trikControl::InternalDisplayInterface::metaObject(); }
inline void*  py_q_qt_metacast(const char*  arg__1) { return trikControl::InternalDisplayInterface::qt_metacast(arg__1); }
};

class PythonQtWrapper_trikControl__InternalDisplayInterface : public QObject
{ Q_OBJECT
public:
public Q_SLOTS:
void delete_trikControl__InternalDisplayInterface(trikControl::InternalDisplayInterface* obj) { delete obj; }
   trikControl::DisplayWidgetInterface*  graphicsWidget(trikControl::InternalDisplayInterface* theWrappedObject);
   trikControl::DisplayWidgetInterface*  py_q_graphicsWidget(trikControl::InternalDisplayInterface* theWrappedObject){  return &(((PythonQtPublicPromoter_trikControl__InternalDisplayInterface*)theWrappedObject)->py_q_graphicsWidget());}
   const QMetaObject*  py_q_metaObject(trikControl::InternalDisplayInterface* theWrappedObject) const{  return (((PythonQtPublicPromoter_trikControl__InternalDisplayInterface*)theWrappedObject)->py_q_metaObject());}
   void*  py_q_qt_metacast(trikControl::InternalDisplayInterface* theWrappedObject, const char*  arg__1){  return (((PythonQtPublicPromoter_trikControl__InternalDisplayInterface*)theWrappedObject)->py_q_qt_metacast(arg__1));}
   QString  static_trikControl__InternalDisplayInterface_tr(const char*  s, const char*  c = nullptr, int  n = -1);
   QString  static_trikControl__InternalDisplayInterface_trUtf8(const char*  s, const char*  c = nullptr, int  n = -1);
};


