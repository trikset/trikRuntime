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
TRIKSCRIPTRUNNER_DIR = ../trikScriptRunner/

TEMPLATE = lib

QT += network

DEFINES += TRIKCOMMUNICATOR_LIBRARY

CONFIG(debug, debug | release) {
	CONFIGURATION = debug
	CONFIGURATION_SUFFIX = d
} else {
	CONFIGURATION = release
	CONFIGURATION_SUFFIX =
}

TARGET = trikCommunicator$$CONFIGURATION_SUFFIX

DESTDIR = bin/$$CONFIGURATION/

OBJECTS_DIR = .obj/$$CONFIGURATION
MOC_DIR = .moc/$$CONFIGURATION
RCC_DIR = .rcc/$$CONFIGURATION
UI_DIR = .ui/$$CONFIGURATION

INCLUDEPATH = \
	$$PWD \
	$$PWD/include/trikCommunicator \
	$$TRIKSCRIPTRUNNER_DIR/include \
	$$TRIKCONTROL_DIR/include \

LIBS += -L$$TRIKSCRIPTRUNNER_DIR/bin/$$CONFIGURATION -ltrikScriptRunner$$CONFIGURATION_SUFFIX

HEADERS += \
	$$PWD/include/trikCommunicator/trikCommunicator.h \

SOURCES += \
	$$PWD/src/trikCommunicator.cpp \

win32 {
	QMAKE_POST_LINK = "xcopy $$replace(TRIKCONTROL_DIR, /, \\)\\bin\\$$CONFIGURATION $$replace(DESTDIR, /, \\) /s /e /q /y /i \
			&& xcopy $$replace(TRIKSCRIPTRUNNER_DIR, /, \\)\\bin\\$$CONFIGURATION $$replace(DESTDIR, /, \\) /s /e /q /y /i \
			"
} else {
	QMAKE_POST_LINK = "cp -r $$TRIKCONTROL_DIR/bin/$$CONFIGURATION/* $$DESTDIR \
			&& cp -r $$TRIKSCRIPTRUNNER_DIR/bin/$$CONFIGURATION/* $$DESTDIR \
			"
}
