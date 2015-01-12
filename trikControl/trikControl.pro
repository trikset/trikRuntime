# Copyright 2013 - 2015 Yurii Litvinov and CyberTech Labs Ltd.
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

include(../global.pri)

HEADERS += \
	$$PWD/include/trikControl/batteryInterface.h \
	$$PWD/include/trikControl/brickFactory.h \
	$$PWD/include/trikControl/brickInterface.h \
	$$PWD/include/trikControl/colorSensorInterface.h \
	$$PWD/include/trikControl/declSpec.h \
	$$PWD/include/trikControl/displayInterface.h \
	$$PWD/include/trikControl/encoderInterface.h \
	$$PWD/include/trikControl/keysInterface.h \
	$$PWD/include/trikControl/ledInterface.h \
	$$PWD/include/trikControl/lineSensorInterface.h \
	$$PWD/include/trikControl/motorInterface.h \
	$$PWD/include/trikControl/objectSensorInterface.h \
	$$PWD/include/trikControl/pwmCaptureInterface.h \
	$$PWD/include/trikControl/sensorInterface.h \
	$$PWD/include/trikControl/vectorSensorInterface.h \

HEADERS += \
	$$PWD/src/abstractVirtualSensorWorker.h \
	$$PWD/src/analogSensor.h \
	$$PWD/src/angularServoMotor.h \
	$$PWD/src/battery.h \
	$$PWD/src/brick.h \
	$$PWD/src/colorSensor.h \
	$$PWD/src/colorSensorWorker.h \
	$$PWD/src/configurer.h \
	$$PWD/src/continiousRotationServoMotor.h \
	$$PWD/src/digitalSensor.h \
	$$PWD/src/display.h \
	$$PWD/src/encoder.h \
	$$PWD/src/graphicsWidget.h \
	$$PWD/src/guiWorker.h \
	$$PWD/src/i2cCommunicator.h \
	$$PWD/src/keys.h \
	$$PWD/src/keysWorker.h \
	$$PWD/src/led.h \
	$$PWD/src/lineSensor.h \
	$$PWD/src/lineSensorWorker.h \
	$$PWD/src/objectSensor.h \
	$$PWD/src/objectSensorWorker.h \
	$$PWD/src/powerMotor.h \
	$$PWD/src/pwmCapture.h \
	$$PWD/src/rangeSensor.h \
	$$PWD/src/rangeSensorWorker.h \
	$$PWD/src/servoMotor.h \
	$$PWD/src/vectorSensor.h \
	$$PWD/src/vectorSensorWorker.h \

SOURCES += \
	$$PWD/src/analogSensor.cpp \
	$$PWD/src/angularServoMotor.cpp \
	$$PWD/src/battery.cpp \
	$$PWD/src/brick.cpp \
	$$PWD/src/brickFactory.cpp \
	$$PWD/src/colorSensor.cpp \
	$$PWD/src/colorSensorWorker.cpp \
	$$PWD/src/configurer.cpp \
	$$PWD/src/continiousRotationServoMotor.cpp \
	$$PWD/src/digitalSensor.cpp \
	$$PWD/src/display.cpp \
	$$PWD/src/encoder.cpp \
	$$PWD/src/graphicsWidget.cpp \
	$$PWD/src/guiWorker.cpp \
	$$PWD/src/keys.cpp \
	$$PWD/src/led.cpp \
	$$PWD/src/lineSensor.cpp \
	$$PWD/src/lineSensorWorker.cpp \
	$$PWD/src/objectSensor.cpp \
	$$PWD/src/objectSensorWorker.cpp \
	$$PWD/src/powerMotor.cpp \
	$$PWD/src/pwmCapture.cpp \
	$$PWD/src/rangeSensor.cpp \
	$$PWD/src/servoMotor.cpp \
	$$PWD/src/vectorSensor.cpp \
	$$PWD/src/$$PLATFORM/abstractVirtualSensorWorker.cpp \
	$$PWD/src/$$PLATFORM/i2cCommunicator.cpp \
	$$PWD/src/$$PLATFORM/keysWorker.cpp \
	$$PWD/src/$$PLATFORM/rangeSensorWorker.cpp \
	$$PWD/src/$$PLATFORM/vectorSensorWorker.cpp \

OTHER_FILES += \
#	config_capture.xml \
	model-config.xml \
	system-config.xml \

TEMPLATE = lib

DEFINES += TRIKCONTROL_LIBRARY

QT += xml gui network

if (equals(QT_MAJOR_VERSION, 5)) {
	QT += widgets
}

uses(trikKernel)

copyToDestdir( \
	$$PWD/model-config.xml  \
	$$PWD/system-config.xml  \
#	$$PWD/config_capture.xml \
	$$PWD/../media/ \
)
