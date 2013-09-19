TRIKCONTROL_DIR = ../trikControl/
TRIKCOMMUNICATOR_DIR = ../trikCommunicator/
TRIKSCRIPTRUNNER_DIR = ../trikScriptRunner/

HEADERS += \

SOURCES += \
	$$PWD/main.cpp \

TEMPLATE = app
QT += widgets

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
