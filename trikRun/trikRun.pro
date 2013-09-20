# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/. */

SOURCES += \
	$$PWD/main.cpp \

TRIKCONTROL_DIR = ../trikControl/
TRIKSCRIPTRUNNER_DIR = ../trikScriptRunner/

TEMPLATE = app
CONFIG += console

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

LIBS += -L$$TRIKSCRIPTRUNNER_DIR/bin/$$CONFIGURATION -ltrikScriptRunner$$CONFIGURATION_SUFFIX

OBJECTS_DIR = .obj/$$CONFIGURATION
MOC_DIR = .moc/$$CONFIGURATION
RCC_DIR = .rcc/$$CONFIGURATION
UI_DIR = .ui/$$CONFIGURATION

!macx {
	QMAKE_LFLAGS += -Wl,-O1,-rpath,.
	QMAKE_LFLAGS += -Wl,-rpath-link,$$TRIKSCRIPTRUNNER_DIR/bin/$$CONFIGURATION
}

win32 {
	QMAKE_POST_LINK = "xcopy $$replace(TRIKCONTROL_DIR, /, \\)\\bin\\$$CONFIGURATION $$replace(DESTDIR, /, \\) /s /e /q /y /i \
			&& xcopy $$replace(TRIKSCRIPTRUNNER_DIR, /, \\)\\bin\\$$CONFIGURATION $$replace(DESTDIR, /, \\) /s /e /q /y /i \
			&& xcopy test.qts $$replace(DESTDIR, /, \\) /q /y \
			"
} else {
	QMAKE_POST_LINK = "cp -r $$TRIKCONTROL_DIR/bin/$$CONFIGURATION/* $$DESTDIR \
			&& cp -rf $$TRIKSCRIPTRUNNER_DIR/bin/$$CONFIGURATION/* $$DESTDIR \
			&& cp -f test.qts $$DESTDIR \
			"
}
