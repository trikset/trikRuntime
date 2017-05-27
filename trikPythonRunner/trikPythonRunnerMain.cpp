#include "PythonQt.h"
#include "gui/PythonQtScriptingConsole.h"
#include "./generated_cpp/PyTrikControl/PyTrikControl0.h"
#include "PyExampleDecorators.h"

#include <QApplication>

void PythonQt_init_PyTrikControl(PyObject* module);

int main(int argc, char *argv[])
{
	QApplication qapp(argc, argv);

	PythonQt::init(PythonQt::IgnoreSiteModule | PythonQt::RedirectStdOut);


	PythonQtObjectPtr mainContext = PythonQt::self()->getMainModule();
//	mainContext.addObject("PyTrikControl");
	PythonQtScriptingConsole console(NULL, mainContext);

//	PythonQt::self()->addDecorators(new PyExampleDecorators());
	PythonQt_init_PyTrikControl(mainContext);

	// register the new object as a known classname and add it's wrapper object
//	PythonQt::self()->registerCPPClass("CustomObject", "","example", PythonQtCreateObject<CustomObjectWrapper>);

	trikControl::BrickInterface* brick =
			trikControl::BrickFactory::create("./", "./media/");

	mainContext.addObject("brick", brick);

//	PythonQtObjectPtr py_q = mainContext.evalScript("brick.py_q_battery()\n", Py_eval_input);

	mainContext.evalFile("example.py");

	console.appendCommandPrompt();
	console.show();

	return qapp.exec();
}
