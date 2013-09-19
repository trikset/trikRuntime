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
	QMAKE_LFLAGS += -Wl,-O1,-rpath,.
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
	QMAKE_POST_LINK = "xcopy \"$$replace(TRIKCONTROL_DIR, /, \\)bin\\$$CONFIGURATION\" \"$$replace(DESTDIR, /, \\)\" /s /e /q /y /i \
			"
} else {
	QMAKE_POST_LINK = "cp -r $$TRIKCONTROL_DIR/bin/$$CONFIGURATION/* $$DESTDIR"
}
