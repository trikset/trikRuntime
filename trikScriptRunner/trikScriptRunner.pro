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
include(./PyTrikControl/PyTrikControl.pri)

PUBLIC_HEADERS += \
	$$PWD/include/trikScriptRunner/trikScriptRunner.h \

HEADERS += \
	$$PWD/src/scriptable.h \
	$$PWD/src/scriptExecutionControl.h \
	$$PWD/src/scriptEngineWorker.h \
	$$PWD/src/pythonEngineWorker.h \
	$$PWD/src/threading.h \
	$$PWD/src/utils.h \
	$$PWD/src/scriptThread.h \
	$$PWD/include/trikScriptRunner/trikScriptRunnerInterface.h \
	$$PWD/include/trikScriptRunner/trikPythonRunner.h \
	$$PWD/include/trikScriptRunner/trikJavaScriptRunner.h \
	$$PWD/include/trikScriptRunner/trikVariablesServer.h

SOURCES += \
	$$PWD/src/scriptExecutionControl.cpp \
	$$PWD/src/scriptEngineWorker.cpp \
	$$PWD/src/pythonEngineWorker.cpp \
	$$PWD/src/trikScriptRunner.cpp \
	$$PWD/src/trikPythonRunner.cpp \
	$$PWD/src/trikJavaScriptRunner.cpp \
	$$PWD/src/threading.cpp \
	$$PWD/src/utils.cpp \
	$$PWD/src/scriptThread.cpp \
	$$PWD/src/trikVariablesServer.cpp

OTHER_FILES += \
	$$PWD/system.js \
	$$PWD/system.py \

INCLUDEPATH += \
        $$PWD/PyTrikControl \
        $$PWD/../trikControl/src \
        $$PWD/../trikControl/include/trikControl \

TRANSLATIONS = \
	$$PWD/../translations/ru/trikScriptRunner_ru.ts \
	$$PWD/../translations/fr/trikScriptRunner_fr.ts \

QT += script network

DEFINES += TRIKSCRIPTRUNNER_LIBRARY

copyToDestdir($$PWD/system.js)
copyToDestdir($$PWD/system.py)

links(trikKernel trikControl trikNetwork)
implementationIncludes(trikKernel trikNetwork trikControl)
PythonQtIncludes()

installs()
installAdditionalSharedFiles($$PWD/system.js)
installAdditionalSharedFiles($$PWD/system.py)
