#include <QObject>
#include <battery.h>
#include <batteryInterface.h>
#include <brick.h>
#include <brickFactory.h>
#include <brickInterface.h>
#include <display.h>
#include <displayInterface.h>
#include <keysInterface.h>
#include "PythonQt.h"

class PyExampleDecorators : public QObject
{
  Q_OBJECT

public Q_SLOTS:

	trikControl::BatteryInterface* battery(trikControl::BrickInterface* theWrappedObject) {return theWrappedObject->battery();}
//  // add a constructor to QSize variant that takes a QPoint
//  QSize* new_QSize(const QPoint& p) { return new QSize(p.x(), p.y()); }

//  // add a constructor for QPushButton that takes a text and a parent widget
//  QPushButton* new_QPushButton(const QString& text, QWidget* parent=NULL) { return new QPushButton(text, parent); }

//  // add a constructor for a CPP object
////  YourCPPObject* new_YourCPPObject(int arg1, float arg2) { return new YourCPPObject(arg1, arg2); }

//  // add a destructor for a CPP object
//  void delete_YourCPPObject(YourCPPObject* obj) { delete obj; }

//  // add a static method to QWidget
//  QWidget* static_QWidget_mouseGrabber() { return QWidget::mouseGrabber(); }

//  // add an additional slot to QWidget (make move() callable, which is not declared as a slot in QWidget)
//  void move(QWidget* w, const QPoint& p) { w->move(p); }

//  // add an additional slot to QWidget, overloading the above move method
//  void move(QWidget* w, int x, int y) { w->move(x,y); }

//  // add a method to your own CPP object
//  int doSomething(YourCPPObject* obj, int arg1) { return obj->doSomething(arg1); }
};
