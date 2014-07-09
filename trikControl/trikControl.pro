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

QT += xml gui network

if (equals(QT_MAJOR_VERSION, 5)) {
	QT += widgets
}

FILES_TO_COPY = \
	$$PWD/config.xml  \
	$$PWD/config_capture.xml \
	$$PWD/../media/ \

include(../global.pri)

TARGET = trikControl$$CONFIGURATION_SUFFIX

INCLUDEPATH = \
	$$PWD \
	$$PWD/include/trikControl \

win32 {
	PLATFORM = windows
} else {
	PLATFORM = linux
}

HEADERS += \
	$$PWD/include/trikControl/analogSensor.h \
	$$PWD/include/trikControl/battery.h \
	$$PWD/include/trikControl/brick.h \
	$$PWD/include/trikControl/declSpec.h \
	$$PWD/include/trikControl/digitalSensor.h \
	$$PWD/include/trikControl/display.h \
	$$PWD/include/trikControl/encoder.h \
	$$PWD/include/trikControl/keys.h \
	$$PWD/include/trikControl/led.h \
	$$PWD/include/trikControl/sensor.h \
	$$PWD/include/trikControl/sensor3d.h \
	$$PWD/include/trikControl/gamepad.h \
	$$PWD/include/trikControl/pwmCapture.h \
	$$PWD/include/trikControl/motor.h \
	$$PWD/include/trikControl/cameraLineDetectorSensor.h \
	$$PWD/src/cameraLineDetectorSensorWorker.h \
	$$PWD/src/powerMotor.h \
	$$PWD/src/servoMotor.h \
	$$PWD/src/configurer.h \
	$$PWD/src/i2cCommunicator.h \
	$$PWD/src/guiWorker.h \
	$$PWD/src/tcpConnector.h \
	$$PWD/src/keysWorker.h \
    src/graphicsWidget.h

SOURCES += \
	$$PWD/src/analogSensor.cpp \
	$$PWD/src/battery.cpp \
	$$PWD/src/brick.cpp \
	$$PWD/src/configurer.cpp \
	$$PWD/src/digitalSensor.cpp \
	$$PWD/src/display.cpp \
	$$PWD/src/encoder.cpp \
	$$PWD/src/gamepad.cpp \
	$$PWD/src/guiWorker.cpp \
	$$PWD/src/led.cpp \
	$$PWD/src/powerMotor.cpp \
	$$PWD/src/servoMotor.cpp \
	$$PWD/src/tcpConnector.cpp \
	$$PWD/src/pwmCapture.cpp \
	$$PWD/src/cameraLineDetectorSensorWorker.cpp \
	$$PWD/src/keysWorker.cpp \
	$$PWD/src/$$PLATFORM/i2cCommunicator.cpp \
	$$PWD/src/$$PLATFORM/keys.cpp \
	$$PWD/src/$$PLATFORM/sensor3d.cpp \
	$$PWD/src/$$PLATFORM/cameraLineDetectorSensor.cpp \
    src/graphicsWidget.cpp

OTHER_FILES += \
	config.xml \
