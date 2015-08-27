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

PUBLIC_HEADERS += \
	$$PWD/include/trikScriptRunner/trikScriptRunner.h \

HEADERS += \
	$$PWD/src/scriptableParts.h \
	$$PWD/src/scriptExecutionControl.h \
	$$PWD/src/scriptEngineWorker.h \
	$$PWD/src/threading.h \
	$$PWD/src/utils.h \
	$$PWD/src/scriptThread.h \

SOURCES += \
	$$PWD/src/scriptableParts.cpp \
	$$PWD/src/scriptExecutionControl.cpp \
	$$PWD/src/scriptEngineWorker.cpp \
	$$PWD/src/trikScriptRunner.cpp \
	$$PWD/src/threading.cpp \
	$$PWD/src/utils.cpp \
	$$PWD/src/scriptThread.cpp \

OTHER_FILES += \
	$$PWD/system.js \


TRANSLATIONS = \
	$$PWD/../translations/ru/trikScriptRunner_ru.ts \
	$$PWD/../translations/fr/trikScriptRunner_fr.ts \

QT += script

DEFINES += TRIKSCRIPTRUNNER_LIBRARY

copyToDestdir($$PWD/system.js)

links(trikKernel trikControl trikNetwork)
implementationIncludes(trikKernel trikControl trikNetwork)

installs()
installAdditionalSharedFiles($$DESTDIR/system.js)
