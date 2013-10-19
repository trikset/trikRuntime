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

TEMPLATE = lib

DEFINES += TRIKCONTROL_LIBRARY

QT += xml gui

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

TARGET = trikControl$$CONFIGURATION_SUFFIX

DESTDIR = ../bin/$$CONFIGURATION

OBJECTS_DIR = .build/$$CONFIGURATION/.obj
MOC_DIR = .build/$$CONFIGURATION/.moc
RCC_DIR = .build/$$CONFIGURATION/.rcc
UI_DIR = .build/$$CONFIGURATION/.ui

INCLUDEPATH = \
	$$PWD \
	$$PWD/include/trikControl \

win32 {
	PLATFORM = windows
} else {
	PLATFORM = linux
}

HEADERS += \
	$$PWD/include/trikControl/brick.h \
	$$PWD/include/trikControl/servoMotor.h \
	$$PWD/include/trikControl/powerMotor.h \
	$$PWD/include/trikControl/sensor.h \
	$$PWD/include/trikControl/analogSensor.h \
	$$PWD/include/trikControl/display.h \
	$$PWD/include/trikControl/declSpec.h \
	$$PWD/src/configurer.h \
	$$PWD/src/i2cCommunicator.h \
	$$PWD/include/trikControl/battery.h \
	$$PWD/include/trikControl/device.h \
	$$PWD/include/trikControl/encoder.h \
	$$PWD/src/guiWorker.h \

SOURCES += \
	$$PWD/src/brick.cpp \
	$$PWD/src/servoMotor.cpp \
	$$PWD/src/powerMotor.cpp \
	$$PWD/src/sensor.cpp \
	$$PWD/src/analogSensor.cpp \
	$$PWD/src/configurer.cpp \
	$$PWD/src/battery.cpp \
	$$PWD/src/device.cpp \
	$$PWD/src/encoder.cpp \
	$$PWD/src/display.cpp \
	$$PWD/src/guiWorker.cpp \
	$$PWD/src/$$PLATFORM/i2cCommunicator.cpp \

win32 {
	QMAKE_POST_LINK = "xcopy config.xml $$replace(DESTDIR, /, \\) /q /y \
			"
} else {
	QMAKE_POST_LINK = "cp -f config.xml $$DESTDIR \
			"
}
