# Copyright 2013 Yurii Litvinov
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

TEMPLATE = lib

include(../global.pri)

DEBUG_EXT=$${CONFIGURATION_SUFFIX}

clang:QMAKE_CXXFLAGS *= -Wno-error -Wno-error=sometimes-uninitialized -Wno-error=writable-strings
QMAKE_CXXFLAGS *= -Wno-error=cast-qual -Wno-error=redundant-decls
QMAKE_CXXFLAGS -= -Werror -Werror=pedantic -pedantic-errors -Werror=write-strings
QT += widgets

!trik_nopython {
  #PythonQt generated files have problems
  INCLUDEPATH *= $$PWD/../trikControl/include/trikControl $$PWD/../trikKernel/include/trikKernel
  include($$PWD/../PythonQt/PythonQt/build/PythonQt_QtAll.prf)
  include(./generated_cpp/PyTrikControl/PyTrikControl.pri)
}

!macx:enableFlagIfCan(-Wno-error=cast-function-type)

PUBLIC_HEADERS += \
	$$PWD/include/trikScriptRunner/trikScriptRunner.h \
	$$PWD/include/trikScriptRunner/trikScriptControlInterface.h \

HEADERS += \
	$$PWD/src/scriptable.h \
	$$PWD/src/scriptExecutionControl.h \
	$$PWD/src/scriptEngineWorker.h \
	$$PWD/src/threading.h \
	$$PWD/src/utils.h \
	$$PWD/src/scriptThread.h \
	$$PWD/include/trikScriptRunner/trikScriptRunnerInterface.h \
	$$PWD/include/trikScriptRunner/trikJavaScriptRunner.h \
	$$PWD/include/trikScriptRunner/trikVariablesServer.h \
	$$PWD/include/trikScriptRunner/trikScriptRunnerDeclSpec.h

SOURCES += \
	$$PWD/src/scriptExecutionControl.cpp \
	$$PWD/src/scriptEngineWorker.cpp \
	$$PWD/src/trikScriptRunner.cpp \
	$$PWD/src/trikJavaScriptRunner.cpp \
	$$PWD/src/threading.cpp \
	$$PWD/src/utils.cpp \
	$$PWD/src/scriptThread.cpp \
	$$PWD/src/trikVariablesServer.cpp \
	$$PWD/src/trikScriptControlInterface.cpp

!trik_nopython {
	HEADERS += \
		$$PWD/src/pythonEngineWorker.h \
		$$PWD/include/trikScriptRunner/trikPythonRunner.h

	SOURCES += \
		$$PWD/src/pythonEngineWorker.cpp \
		$$PWD/src/trikPythonRunner.cpp

	INCLUDEPATH += \
		$$PWD/generated_cpp/PyTrikControl \
}

OTHER_FILES += \
	$$PWD/system.js \
	$$PWD/TRIK.py \

INCLUDEPATH += \
	$$PWD/../trikControl/src \

QT += script network

DEFINES += TRIKSCRIPTRUNNER_LIBRARY
trik_nopython:DEFINES += TRIK_NOPYTHON

copyToDestdir($$PWD/system.js, NOW)
copyToDestdir($$PWD/TRIK.py, NOW)

links(trikQsLog trikKernel trikControl trikNetwork)
implementationIncludes(trikKernel trikNetwork trikControl)
PythonQtIncludes()

installs()
installAdditionalSharedFiles($$PWD/system.js)
installAdditionalSharedFiles($$PWD/TRIK.py)
