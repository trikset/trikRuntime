TEMPLATE = lib

DEFINES += TRIKCONTROL_LIBRARY

CONFIG(debug, debug | release) {
	CONFIGURATION = debug
	CONFIGURATION_SUFFIX = d
} else {
	CONFIGURATION = release
	CONFIGURATION_SUFFIX =
}

TARGET = trikControl$$CONFIGURATION_SUFFIX

DESTDIR = bin/$$CONFIGURATION/

OBJECTS_DIR = .obj/$$CONFIGURATION
MOC_DIR = .moc/$$CONFIGURATION
RCC_DIR = .rcc/$$CONFIGURATION
UI_DIR = .ui/$$CONFIGURATION

INCLUDEPATH = \
	$$PWD \
	$$PWD/include/trikControl \

HEADERS += \
	$$PWD/include/trikControl/brick.h \
	$$PWD/include/trikControl/servoMotor.h \
	$$PWD/include/trikControl/powerMotor.h \
	$$PWD/include/trikControl/sensor.h \
	$$PWD/include/trikControl/declSpec.h \

SOURCES += \
	$$PWD/src/brick.cpp \
	$$PWD/src/servoMotor.cpp \
	$$PWD/src/powerMotor.cpp \
	$$PWD/src/sensor.cpp \
