/* Copyright 2020 CyberTech Labs Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#include "trikPythonModule.h"

#include <PythonQt.h>
#include <PythonQtConversion.h>

#include <trikControl/brickInterface.h>
#include <trikNetwork/mailboxInterface.h>

#include "scriptExecutionControl.h"


struct trikPythonModuleState {
	trikControl::BrickInterface* mBrick;
	trikScriptRunner::ScriptExecutionControl* mScript;
	trikNetwork::MailboxInterface* mMailbox;
};

static PyObject* trik_get_native_brick(PyObject *self, PyObject* args)
{
	if (!PyArg_ParseTuple(args, ""))
		return nullptr;

	auto state = reinterpret_cast<trikPythonModuleState*>(PyModule_GetState(self));

	return PythonQtConv::QVariantToPyObject(QVariant::fromValue(static_cast<QObject*>(state->mBrick)));
}

static PyObject* trik_get_native_script(PyObject *self, PyObject* args)
{
	if (!PyArg_ParseTuple(args, ""))
		return nullptr;

	auto state = reinterpret_cast<trikPythonModuleState*>(PyModule_GetState(self));

	return PythonQtConv::QVariantToPyObject(QVariant::fromValue(static_cast<QObject*>(state->mScript)));
}

static PyObject* trik_get_native_mailbox(PyObject *self, PyObject* args)
{
	if (!PyArg_ParseTuple(args, ""))
		return nullptr;

	auto state = reinterpret_cast<trikPythonModuleState*>(PyModule_GetState(self));

	return PythonQtConv::QVariantToPyObject(QVariant::fromValue(static_cast<QObject*>(state->mMailbox)));
}

static PyMethodDef TrikMethods[] =
{
	{"_get_native_brick", trik_get_native_brick, METH_VARARGS, nullptr},
	{"_get_native_script", trik_get_native_script, METH_VARARGS, nullptr},
	{"_get_native_mailbox", trik_get_native_mailbox, METH_VARARGS, nullptr},
	{nullptr, nullptr, 0, nullptr}        /* Sentinel */
};

static struct PyModuleDef trikmodule =
{
	PyModuleDef_HEAD_INIT,
	"trik",   /* name of module */
	nullptr, /* module documentation, may be nullptr */
	sizeof(trikPythonModuleState),       /* size of per-interpreter state of the module,
				 or -1 if the module keeps state in global variables. */
	TrikMethods
};

void trikPythonModuleInit(trikControl::BrickInterface* brick,
						trikScriptRunner::ScriptExecutionControl* script, trikNetwork::MailboxInterface* mailbox)
{
	PythonQtObjectPtr mod = PyModule_Create(&trikmodule);

	auto state = reinterpret_cast<trikPythonModuleState*>(PyModule_GetState(mod));

	state->mBrick = brick;
	state->mScript = script;
	state->mMailbox = mailbox;

	// make module a package
	PyModule_AddStringConstant(mod, "__package__", "trik");
	PyModule_AddObject(mod, "__path__", PyList_New(0));

	PythonQtObjectPtr all_ptr = PyList_New((mailbox != nullptr) ? 3 : 2);
	PyList_SetItem(all_ptr, 0, PyString_FromString("brick"));
	PyList_SetItem(all_ptr, 1, PyString_FromString("script"));
	if (mailbox != nullptr)
		PyList_SetItem(all_ptr, 2, PyString_FromString("mailbox"));
	PyModule_AddObject(mod, "__all__", all_ptr.takeObject());

	mod.evalScript(	"_modules = __import__('sys').modules\n"
					"brick = _modules['trik.brick'] = _get_native_brick()\n"
					"script = _modules['trik.script'] = _get_native_script()\n"
					"if _get_native_mailbox():\n  mailbox = _modules['trik.mailbox'] = _get_native_mailbox()\n"
					"del _modules");

	PythonQtObjectPtr sys;
	sys.setNewRef(PyImport_ImportModule("sys"));


	PyObject *old_module_names = PyObject_GetAttrString(sys.object(),"builtin_module_names");
	if (old_module_names && PyTuple_Check(old_module_names)) {
	  Py_ssize_t old_size = PyTuple_Size(old_module_names);
	  PyObject *module_names = PyTuple_New(old_size + 1);
	  for (Py_ssize_t i = 0; i < old_size; i++) {
		PyObject* val = PyTuple_GetItem(old_module_names, i);
		Py_INCREF(val);
		PyTuple_SetItem(module_names, i, val);
	  }
	  PyTuple_SetItem(module_names, old_size, PyString_FromString("trik"));
	  PyModule_AddObject(sys.object(), "builtin_module_names", module_names);
	}
	Py_XDECREF(old_module_names);

	PyDict_SetItemString(PyObject_GetAttrString(sys.object(), "modules"), "trik", mod);
}




