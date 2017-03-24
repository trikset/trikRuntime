# Copyright 2013 - 2016 Yurii Litvinov and CyberTech Labs Ltd.
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

PUBLIC_HEADERS += \
	$$PWD/include/trikControl/batteryInterface.h \
	$$PWD/include/trikControl/brickFactory.h \
	$$PWD/include/trikControl/brickInterface.h \
	$$PWD/include/trikControl/colorSensorInterface.h \
	$$PWD/include/trikControl/declSpec.h \
	$$PWD/include/trikControl/deviceInterface.h \
	$$PWD/include/trikControl/displayInterface.h \
	$$PWD/include/trikControl/displayWidgetInterface.h \
	$$PWD/include/trikControl/encoderInterface.h \
	$$PWD/include/trikControl/eventCodeInterface.h \
	$$PWD/include/trikControl/eventDeviceInterface.h \
	$$PWD/include/trikControl/eventInterface.h \
	$$PWD/include/trikControl/fifoInterface.h \
	$$PWD/include/trikControl/gamepadInterface.h \
	$$PWD/include/trikControl/keysInterface.h \
	$$PWD/include/trikControl/ledInterface.h \
	$$PWD/include/trikControl/lineSensorInterface.h \
	$$PWD/include/trikControl/motorInterface.h \
	$$PWD/include/trikControl/objectSensorInterface.h \
	$$PWD/include/trikControl/pwmCaptureInterface.h \
	$$PWD/include/trikControl/sensorInterface.h \
	$$PWD/include/trikControl/soundSensorInterface.h \
	$$PWD/include/trikControl/vectorSensorInterface.h \
	$$PWD/include/trikControl/gyroSensorInterface.h \
#	$$PWD/include/trikControl/headingSensorInterface.h \

HEADERS += \
	$$PWD/src/abstractVirtualSensorWorker.h \
	$$PWD/src/analogSensor.h \
	$$PWD/src/audioSynthDevices.h \
	$$PWD/src/battery.h \
	$$PWD/src/brick.h \
	$$PWD/src/colorSensor.h \
	$$PWD/src/colorSensorWorker.h \
	$$PWD/src/configurerHelper.h \
	$$PWD/src/deviceState.h \
	$$PWD/src/digitalSensor.h \
	$$PWD/src/display.h \
	$$PWD/src/encoder.h \
	$$PWD/src/event.h \
	$$PWD/src/eventCode.h \
	$$PWD/src/eventDevice.h \
	$$PWD/src/eventDeviceWorker.h \
	$$PWD/src/fifo.h \
	$$PWD/src/gamepad.h \
	$$PWD/src/graphicsWidget.h \
	$$PWD/src/guiWorker.h \
	$$PWD/src/keys.h \
	$$PWD/src/keysWorker.h \
	$$PWD/src/led.h \
	$$PWD/src/lineSensor.h \
	$$PWD/src/lineSensorWorker.h \
	$$PWD/src/moduleLoader.h \
	$$PWD/src/mspCommunicatorInterface.h \
	$$PWD/src/mspBusAutoDetector.h \
	$$PWD/src/mspI2cCommunicator.h \
	$$PWD/src/mspUsbCommunicator.h \
	$$PWD/src/objectSensor.h \
	$$PWD/src/objectSensorWorker.h \
	$$PWD/src/powerMotor.h \
	$$PWD/src/pwmCapture.h \
	$$PWD/src/rangeSensor.h \
	$$PWD/src/rangeSensorWorker.h \
	$$PWD/src/servoMotor.h \
	$$PWD/src/soundSensor.h \
	$$PWD/src/soundSensorWorker.h \
	$$PWD/src/tonePlayer.h \
	$$PWD/src/vectorSensor.h \
	$$PWD/src/vectorSensorWorker.h \
	$$PWD/src/exceptions/incorrectDeviceConfigurationException.h \
	$$PWD/src/exceptions/incorrectStateChangeException.h \
	$$PWD/src/shapes/arc.h \
	$$PWD/src/shapes/ellipse.h \
	$$PWD/src/shapes/line.h \
	$$PWD/src/shapes/point.h \
	$$PWD/src/shapes/rectangle.h \
	$$PWD/src/shapes/shape.h \
	$$PWD/src/gyroSensor.h \
#	$$PWD/src/headingSensor.h \

SOURCES += \
	$$PWD/src/abstractVirtualSensorWorker.cpp \
	$$PWD/src/analogSensor.cpp \
	$$PWD/src/battery.cpp \
	$$PWD/src/brick.cpp \
	$$PWD/src/brickFactory.cpp \
	$$PWD/src/colorSensor.cpp \
	$$PWD/src/colorSensorWorker.cpp \
	$$PWD/src/configurerHelper.cpp \
	$$PWD/src/deviceState.cpp \
	$$PWD/src/digitalSensor.cpp \
	$$PWD/src/display.cpp \
	$$PWD/src/encoder.cpp \
	$$PWD/src/event.cpp \
	$$PWD/src/eventCode.cpp \
	$$PWD/src/eventDevice.cpp \
	$$PWD/src/eventDeviceWorker.cpp \
	$$PWD/src/fifo.cpp \
	$$PWD/src/gamepad.cpp \
	$$PWD/src/graphicsWidget.cpp \
	$$PWD/src/guiWorker.cpp \
	$$PWD/src/keys.cpp \
	$$PWD/src/keysWorker.cpp \
	$$PWD/src/led.cpp \
	$$PWD/src/lineSensor.cpp \
	$$PWD/src/lineSensorWorker.cpp \
	$$PWD/src/moduleLoader.cpp \
	$$PWD/src/mspBusAutoDetector.cpp \
	$$PWD/src/mspI2cCommunicator.cpp \
	$$PWD/src/mspUsbCommunicator.cpp \
	$$PWD/src/objectSensor.cpp \
	$$PWD/src/objectSensorWorker.cpp \
	$$PWD/src/powerMotor.cpp \
	$$PWD/src/pwmCapture.cpp \
	$$PWD/src/rangeSensor.cpp \
	$$PWD/src/rangeSensorWorker.cpp \
	$$PWD/src/servoMotor.cpp \
	$$PWD/src/soundSensor.cpp \
	$$PWD/src/soundSensorWorker.cpp \
	$$PWD/src/tonePlayer.cpp \
	$$PWD/src/vectorSensor.cpp \
	$$PWD/src/vectorSensorWorker.cpp \
	$$PWD/src/shapes/ellipse.cpp \
	$$PWD/src/shapes/point.cpp \
	$$PWD/src/shapes/line.cpp \
	$$PWD/src/shapes/rectangle.cpp \
	$$PWD/src/shapes/arc.cpp \
	$$PWD/src/audioSynthDevices.cpp \
	$$PWD/src/gyroSensor.cpp \
#	$$PWD/src/headingSensor.cpp \

CONFIGS += \
	$$PWD/model-config-usb.xml \
	$$PWD/system-config-usb.xml \
	$$PWD/model-config.xml \
	$$PWD/system-config.xml \
	$$PWD/model-config-v6.xml \
	$$PWD/system-config-v6.xml \
	$$PWD/model-config-usb.xml \
	$$PWD/system-config-usb.xml \

OTHER_FILES += \
	$$CONFIGS \
	$$PWD/trikControlExport.pri \

TEMPLATE = lib

DEFINES += TRIKCONTROL_LIBRARY

QT += xml gui multimedia

if (equals(QT_MAJOR_VERSION, 5)) {
	QT += widgets
}

links(trikKernel trikHal)
implementationIncludes(trikKernel trikHal)

copyToDestdir( \
	$$CONFIGS \
	$$PWD/../media/ \
	, now \
)

installs()

installAdditionalConfigs($$CONFIGS)
installAdditionalSharedFiles($$PWD/../media)
