TRIKCONTROL_DIR = ../trikControl/

TEMPLATE = app
QT += script network
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
	$$TRIKCONTROL_DIR/include \

LIBS += -L$$TRIKCONTROL_DIR/bin/$$CONFIGURATION -ltrikControl$$CONFIGURATION_SUFFIX

OBJECTS_DIR = .obj/$$CONFIGURATION
MOC_DIR = .moc/$$CONFIGURATION
RCC_DIR = .rcc/$$CONFIGURATION
UI_DIR = .ui/$$CONFIGURATION

!macx {
	QMAKE_LFLAGS += -Wl,-O1,-rpath,.
}

HEADERS += \
	$$PWD/qRealCommunicator.h \
	$$PWD/runner.h \
	$$PWD/scriptableParts.h \
	$$PWD/scriptEngineWorker.h \

SOURCES += \
	$$PWD/main.cpp \
	$$PWD/qRealCommunicator.cpp \
	$$PWD/runner.cpp \
	$$PWD/scriptableParts.cpp \
	$$PWD/scriptEngineWorker.cpp \

win32 {
	QMAKE_POST_LINK = "xcopy ..\\trikControl\\bin\\$$CONFIGURATION $$replace(DESTDIR, /, \\) /s /e /q /y /i \
	&& xcopy ..\\media $$replace(DESTDIR, /, \\)media /s /e /q /y /i \
	&& xcopy test.qts $$replace(DESTDIR, /, \\) /q /y \
	"
} else {
	QMAKE_POST_LINK = "cp -r ../trikControl/bin/$$CONFIGURATION/* $$DESTDIR"
}
