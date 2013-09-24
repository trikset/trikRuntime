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
TRIKCOMMUNICATOR_DIR = ../trikCommunicator/
TRIKSCRIPTRUNNER_DIR = ../trikScriptRunner/

HEADERS += \
	$$PWD/fileManagerWidget.h \
	$$PWD/netConfigWidget.h \
	$$PWD/startWidget.h \
	$$PWD/trikGuiApplication.h \
	$$PWD/controller.h \

SOURCES += \
	$$PWD/main.cpp \
	$$PWD/fileManagerWidget.cpp \
	$$PWD/netConfigWidget.cpp \
	$$PWD/startWidget.cpp \
	$$PWD/trikGuiApplication.cpp \
	$$PWD/controller.cpp \

TEMPLATE = app
QT += network

CONFIG(debug, debug | release) {
	CONFIGURATION = debug
	CONFIGURATION_SUFFIX = d
} else {
	CONFIGURATION = release
	CONFIGURATION_SUFFIX =
}

DESTDIR = ../bin/$$CONFIGURATION/

INCLUDEPATH = \
	$$PWD \
	$$TRIKCOMMUNICATOR_DIR/include \
	$$TRIKSCRIPTRUNNER_DIR/include \

LIBS += -L$$TRIKCOMMUNICATOR_DIR/bin/$$CONFIGURATION -ltrikCommunicator$$CONFIGURATION_SUFFIX -L$$TRIKSCRIPTRUNNER_DIR/bin/$$CONFIGURATION -ltrikScriptRunner$$CONFIGURATION_SUFFIX

OBJECTS_DIR = .obj/$$CONFIGURATION
MOC_DIR = .moc/$$CONFIGURATION
RCC_DIR = .rcc/$$CONFIGURATION
UI_DIR = .ui/$$CONFIGURATION

!macx {
	QMAKE_LFLAGS += -Wl,-O1,-rpath,.
	QMAKE_LFLAGS += -Wl,-rpath-link,$$TRIKSCRIPTRUNNER_DIR/bin/$$CONFIGURATION
	QMAKE_LFLAGS += -Wl,-rpath-link,$$TRIKCOMMUNICATOR_DIR/bin/$$CONFIGURATION
}

win32 {
	QMAKE_POST_LINK = "xcopy $$replace(TRIKCONTROL_DIR, /, \\)\\bin\\$$CONFIGURATION $$replace(DESTDIR, /, \\) /s /e /q /y /i \
			&& xcopy ..\\media $$replace(DESTDIR, /, \\)media /s /e /q /y /i \
			&& xcopy $$replace(TRIKCOMMUNICATOR_DIR, /, \\)\\bin\\$$CONFIGURATION $$replace(DESTDIR, /, \\) /s /e /q /y /i \
			&& xcopy $$replace(TRIKSCRIPTRUNNER_DIR, /, \\)\\bin\\$$CONFIGURATION $$replace(DESTDIR, /, \\) /s /e /q /y /i \
			"
} else {
	QMAKE_POST_LINK = "cp -r ../trikControl/bin/$$CONFIGURATION/* $$DESTDIR \
			"
}
