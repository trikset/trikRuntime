# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
