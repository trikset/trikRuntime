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

DESTDIR = bin/$$CONFIGURATION/

OBJECTS_DIR = .obj/$$CONFIGURATION
MOC_DIR = .moc/$$CONFIGURATION
RCC_DIR = .rcc/$$CONFIGURATION
UI_DIR = .ui/$$CONFIGURATION

INCLUDEPATH = \
	$$PWD \
	$$TRIKCONTROL_DIR/include \

LIBS += -L$$TRIKCONTROL_DIR/bin/$$CONFIGURATION -ltrikControl$$CONFIGURATION_SUFFIX

!macx {
	QMAKE_LFLAGS += -Wl,-O1,-rpath,$$PWD
	QMAKE_LFLAGS += -Wl,-O1,-rpath,$$DESTDIR
}

HEADERS += \
	$$PWD/include/trikScriptRunner/trikScriptRunner.h \
	$$PWD/src/scriptableParts.h \
	$$PWD/src/scriptEngineWorker.h \

SOURCES += \
	$$PWD/src/trikScriptRunner.cpp \
	$$PWD/src/scriptableParts.cpp \
	$$PWD/src/scriptEngineWorker.cpp \

win32 {
	QMAKE_PRE_LINK = "xcopy \"$$replace(TRIKCONTROL_DIR, /, \\)bin\\$$CONFIGURATION\" \"$$replace(DESTDIR, /, \\)\" /s /e /q /y /i \
			"
} else {
	QMAKE_PRE_LINK = "cp -r $$TRIKCONTROL_DIR/bin/$$CONFIGURATION/* $$DESTDIR"
}
