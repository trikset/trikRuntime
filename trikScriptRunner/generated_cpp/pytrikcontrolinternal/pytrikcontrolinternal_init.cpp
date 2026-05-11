#include <PythonQt.h>
#include <PythonQtConversion.h>
#include "pytrikcontrolinternal0.h"



void PythonQt_init_PyTrikControlInternal(PyObject* module) {
PythonQt::priv()->registerClass(&trikControl::DisplayWidgetInterface::staticMetaObject, "PyTrikControlInternal", PythonQtCreateObject<PythonQtWrapper_trikControl__DisplayWidgetInterface>, nullptr, module, 0);
PythonQt::priv()->registerClass(&trikControl::InternalBrickInterface::staticMetaObject, "PyTrikControlInternal", PythonQtCreateObject<PythonQtWrapper_trikControl__InternalBrickInterface>, nullptr, module, 0);
PythonQt::self()->addParentClass("trikControl::InternalBrickInterface", "trikControl::BrickInterface",PythonQtUpcastingOffset<trikControl::InternalBrickInterface,trikControl::BrickInterface>());
PythonQt::priv()->registerClass(&trikControl::InternalDisplayInterface::staticMetaObject, "PyTrikControlInternal", PythonQtCreateObject<PythonQtWrapper_trikControl__InternalDisplayInterface>, nullptr, module, 0);
PythonQt::self()->addParentClass("trikControl::InternalDisplayInterface", "trikControl::DisplayInterface",PythonQtUpcastingOffset<trikControl::InternalDisplayInterface,trikControl::DisplayInterface>());


}
