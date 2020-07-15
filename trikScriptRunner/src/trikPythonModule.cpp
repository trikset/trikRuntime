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

#include <Python.h>
#include <PythonQt.h>
#include <PythonQtConversion.h>

#include <trikControl/brickInterface.h>
#include <trikNetwork/mailboxInterface.h>

#include "scriptExecutionControl.h"

static trikControl::BrickInterface* gBrick;
static trikScriptRunner::ScriptExecutionControl* gScript;
static trikNetwork::MailboxInterface* gMailbox;

void trikPythonModuleSetObjects(trikControl::BrickInterface* brick,
	trikScriptRunner::ScriptExecutionControl* script, trikNetwork::MailboxInterface* mailbox)
{
	gBrick = brick;
	gScript = script;
	gMailbox = mailbox;
}

static PyObject* trik_get_native_brick(PyObject *self, PyObject* args)
{
	if (!PyArg_ParseTuple(args, ""))
		return nullptr;

	return PythonQtConv::QVariantToPyObject(QVariant::fromValue(gBrick));
}

static PyObject* trik_get_native_script(PyObject *self, PyObject* args)
{
	if (!PyArg_ParseTuple(args, ""))
		return nullptr;

	return PythonQtConv::QVariantToPyObject(QVariant::fromValue(gScript));
}

static PyObject* trik_get_native_mailbox(PyObject *self, PyObject* args)
{
	if (!PyArg_ParseTuple(args, ""))
		return nullptr;

	return PythonQtConv::QVariantToPyObject(QVariant::fromValue(gMailbox));
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
	-1,       /* size of per-interpreter state of the module,
				 or -1 if the module keeps state in global variables. */
	TrikMethods
};

PyMODINIT_FUNC trikPythonModuleInit()
{
	PythonQtObjectPtr mod = PyModule_Create(&trikmodule);

	// make module a package
	PyModule_AddStringConstant(mod, "__package__", "trik");
	PyModule_AddObject(mod, "__path__", PyList_New(0));

	PythonQtObjectPtr all_ptr = PyList_New((gMailbox != nullptr) ? 3 : 2);
	PyList_SetItem(all_ptr, 0, PyString_FromString("brick"));
	PyList_SetItem(all_ptr, 1, PyString_FromString("script"));
	if (gMailbox != nullptr)
		PyList_SetItem(all_ptr, 2, PyString_FromString("mailbox"));
	PyModule_AddObject(mod, "__all__", all_ptr.takeObject());

	mod.evalScript(	"_modules = __import__('sys').modules\n"
					"brick = _modules['trik.brick'] = _get_native_brick()\n"
					"script = _modules['trik.script'] = _get_native_script()\n"
					"if _get_native_mailbox():\n  mailbox = _modules['trik.mailbox'] = _get_native_mailbox()\n"
					"del _modules");

	return mod;
}



