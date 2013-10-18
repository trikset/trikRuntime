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

SOURCES += \
	$$PWD/main.cpp \

TRIKCONTROL_DIR = ../trikControl/
TRIKSCRIPTRUNNER_DIR = ../trikScriptRunner/

TEMPLATE = app
CONFIG += console

QT += gui

if (equals(QT_MAJOR_VERSION, 5)) {
	QT += widgets
}

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
	$$TRIKSCRIPTRUNNER_DIR/include \

LIBS += -L$$DESTDIR -ltrikScriptRunner$$CONFIGURATION_SUFFIX

OBJECTS_DIR = .build/$$CONFIGURATION/.obj
MOC_DIR = .build/$$CONFIGURATION/.moc
RCC_DIR = .build/$$CONFIGURATION/.rcc
UI_DIR = .build/$$CONFIGURATION/.ui

!macx {
	QMAKE_LFLAGS += -Wl,-O1,-rpath,.
	QMAKE_LFLAGS += -Wl,-rpath-link,$$DESTDIR
}

win32 {
	QMAKE_POST_LINK = "xcopy test.qts $$replace(DESTDIR, /, \\) /q /y \
			"
} else {
	QMAKE_POST_LINK = "cp -f test.qts $$DESTDIR \
			"
}

unix {
        target.path = $$[INSTALL_ROOT]/
        INSTALLS +=   target
}

