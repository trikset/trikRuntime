TEMPLATE = lib

DEFINES += TRIKCONTROL_LIBRARY

CONFIGURATION = debug

CONFIG -= debug release
CONFIG += $$CONFIGURATION

DESTDIR = $$CONFIGURATION/bin

OBJECTS_DIR = $$CONFIGURATION/.obj
MOC_DIR = $$CONFIGURATION/.moc
RCC_DIR = $$CONFIGURATION/.moc

INCLUDEPATH = include/trikControl

HEADERS += \
	include/trikControl/brick.h \
	include/trikControl/servoMotor.h \
	include/trikControl/powerMotor.h \
	include/trikControl/sensor.h \
	include/trikControl/declSpec.h \

SOURCES += \
	src/brick.cpp \
	src/servoMotor.cpp \
	src/powerMotor.cpp \
	src/sensor.cpp \
