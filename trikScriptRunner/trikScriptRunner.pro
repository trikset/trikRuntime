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

TRIKCONTROL_DIR = ../trikControl/

TEMPLATE = lib
QT += script

DEFINES += TRIKSCRIPTRUNNER_LIBRARY

CONFIG(debug, debug | release) {
	CONFIGURATION = debug
	CONFIGURATION_SUFFIX = d
} else {
	CONFIGURATION = release
	CONFIGURATION_SUFFIX =
}

TARGET = trikScriptRunner$$CONFIGURATION_SUFFIX

DESTDIR = ../bin/$$CONFIGURATION

OBJECTS_DIR = .build/$$CONFIGURATION/.obj
MOC_DIR = .build/$$CONFIGURATION/.moc
RCC_DIR = .build/$$CONFIGURATION/.rcc
UI_DIR = .build/$$CONFIGURATION/.ui

INCLUDEPATH = \
	$$PWD \
	$$TRIKCONTROL_DIR/include \

LIBS += -L$$DESTDIR -ltrikControl$$CONFIGURATION_SUFFIX

!macx {
	QMAKE_LFLAGS += -Wl,-O1,-rpath,$$PWD
	QMAKE_LFLAGS += -Wl,-O1,-rpath,$$DESTDIR
}

HEADERS += \
	$$PWD/include/trikScriptRunner/trikScriptRunner.h \
	$$PWD/src/scriptableParts.h \
	$$PWD/src/scriptEngineWorker.h \
	$$PWD/src/fileUtils.h \

SOURCES += \
	$$PWD/src/trikScriptRunner.cpp \
	$$PWD/src/scriptableParts.cpp \
	$$PWD/src/scriptEngineWorker.cpp \
	$$PWD/src/fileUtils.cpp \

unix {
	target.path = $$[INSTALL_ROOT]/
	INSTALLS +=   target
}

win32 {
	QMAKE_POST_LINK = "xcopy system.js $$replace(DESTDIR, /, \\) /q /y \
			"
} else {
	QMAKE_POST_LINK = "cp -f system.js $$DESTDIR/ \
			"
}
