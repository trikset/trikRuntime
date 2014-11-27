# This pro file will build QsLog as a shared library
include(QsLog.pri)

CROSS_COMPILE = $$(CROSS_COMPILE)

!isEmpty(CROSS_COMPILE) {
	ARCHITECTURE = arm
} else {
	ARCHITECTURE = x86
}

CONFIG(debug, debug | release) {
	CONFIGURATION = $$ARCHITECTURE-debug
	CONFIGURATION_SUFFIX = -$$ARCHITECTURE-d
} else {
	CONFIGURATION = $$ARCHITECTURE-release
	equals(ARCHITECTURE, "arm") {
		CONFIGURATION_SUFFIX =
	} else {
		CONFIGURATION_SUFFIX = -$$ARCHITECTURE
	}
}

DESTDIR = $$PWD/../bin/$$CONFIGURATION

equals(TEMPLATE, app) {
	!macx {
		QMAKE_LFLAGS += -Wl,-O1,-rpath,.
		QMAKE_LFLAGS += -Wl,-rpath-link,$$DESTDIR
	}
}

OBJECTS_DIR = .build/$$CONFIGURATION/.obj
MOC_DIR = .build/$$CONFIGURATION/.moc
RCC_DIR = .build/$$CONFIGURATION/.rcc
UI_DIR = .build/$$CONFIGURATION/.ui

TARGET = qslog$$CONFIGURATION_SUFFIX
QT -= gui
CONFIG -= console
CONFIG -= app_bundle
CONFIG += shared
TEMPLATE = lib

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -Wextra -Wcast-qual -Wwrite-strings -Wredundant-decls -Wunreachable-code -Wnon-virtual-dtor -Woverloaded-virtual

win32 {
	DEFINES += QSLOG_IS_SHARED_LIBRARY
}

unix:!macx {
	# make install will install the shared object in the appropriate folders
	headers.files = QsLog.h QsLogDest.h QsLogLevel.h
	headers.path = /usr/include/$(QMAKE_TARGET)

	other_files.files = *.txt
	other_files.path = /usr/local/share/$(QMAKE_TARGET)

	contains(QT_ARCH, x86_64) {
		target.path = /usr/lib64
	} else {
		target.path = /usr/lib
	}

	INSTALLS += headers target other_files
}
