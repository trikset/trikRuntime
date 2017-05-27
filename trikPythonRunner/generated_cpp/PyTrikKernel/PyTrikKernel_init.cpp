#include <PythonQt.h>
#include <PythonQtConversion.h>
#include "PyTrikKernel0.h"



void PythonQt_init_PyTrikKernel(PyObject* module) {
PythonQt::priv()->registerCPPClass("trikKernel", "", "PyTrikKernel", PythonQtCreateObject<PythonQtWrapper_trikKernel>, NULL, module, PythonQt::Type_Subtract);
PythonQt::priv()->registerCPPClass("trikKernel::TimeVal", "", "PyTrikKernel", PythonQtCreateObject<PythonQtWrapper_trikKernel__TimeVal>, NULL, module, 0);


}
