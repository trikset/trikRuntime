# Copyright 2013 - 2016 Yurii Litvinov, Mikhail Kita, Anna Kudryashova
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

TEMPLATE = app
QT += network xml core qml quick quickcontrols2 gui

QML_IMPORT_NAME = com.trikGui
QML_IMPORT_MAJOR_VERSION = 1
CONFIG += qmltypes

include(../global.pri)
include(Qt-QrCodeGenerator/Qt-QrCodeGenerator.pri)

HEADERS += \
	$$PWD/abstractIndicator.h \
	$$PWD/autoRunner.h \
	$$PWD/batteryIndicator.h \
	$$PWD/controller.h \
	$$PWD/encoderIndicator.h \
	$$PWD/fileManager.h \
	$$PWD/information.h \
	$$PWD/languageSelection.h \
	$$PWD/motorLever.h \
	$$PWD/motors.h \
	$$PWD/communicationSettings.h \
	$$PWD/runningCode.h \
	$$PWD/sensorIndicator.h \
	$$PWD/sensors.h \
	$$PWD/systemSettings.h \
	$$PWD/wiFiAP.h \
	$$PWD/wiFiClient.h \
	$$PWD/wiFiInit.h \
	$$PWD/wiFiMode.h \
	$$PWD/wiFiIndicator.h \
	$$PWD/openSocketIndicator.h \
	$$PWD/gamepadIndicator.h \
	$$PWD/powerLevel.h \
	$$PWD/gyroscopeIndicator.h \
	$$PWD/accelerometer.h \
	$$PWD/network.h \
	$$PWD/camera.h \
	$$PWD/pwmCaptureIndicator.h \
	$$PWD/mainMenuManager.h \
	$$PWD/managers.h \
	$$PWD/testingManager.h \
	$$PWD/accelerometerMock.h \
	$$PWD/encoderMock.h \
	$$PWD/gyroscopeMock.h \
	$$PWD/sensorMock.h \
	$$PWD/modeManager.h \
	$$PWD/wiFiAPMock.h \
	$$PWD/wiFiClientMock.h \
	$$PWD/wiFiModeMock.h \
	$$PWD/trikGuiApplication.h

SOURCES += \
	$$PWD/autoRunner.cpp \
	$$PWD/batteryIndicator.cpp \
	$$PWD/communicationSettings.cpp \
	$$PWD/controller.cpp \
	$$PWD/encoderIndicator.cpp \
	$$PWD/fileManager.cpp \
	$$PWD/information.cpp \
	$$PWD/languageSelection.cpp \
	$$PWD/trikGuiMain.cpp \
	$$PWD/motorLever.cpp \
	$$PWD/motors.cpp \
	$$PWD/runningCode.cpp \
	$$PWD/sensorIndicator.cpp \
	$$PWD/sensors.cpp \
	$$PWD/systemSettings.cpp \
	$$PWD/wiFiAP.cpp \
	$$PWD/wiFiClient.cpp \
	$$PWD/wiFiInit.cpp \
	$$PWD/wiFiMode.cpp \
	$$PWD/wiFiIndicator.cpp \
	$$PWD/openSocketIndicator.cpp \
	$$PWD/gamepadIndicator.cpp \
	$$PWD/gyroscopeIndicator.cpp \
	$$PWD/accelerometer.cpp \
	$$PWD/network.cpp \
	$$PWD/camera.cpp \
	$$PWD/pwmCaptureIndicator.cpp \
	$$PWD/mainMenuManager.cpp \
	$$PWD/testingManager.cpp \
	$$PWD/accelerometerMock.cpp \
	$$PWD/encoderMock.cpp \
	$$PWD/gyroscopeMock.cpp \
	$$PWD/sensorMock.cpp \
	$$PWD/modeManager.cpp \
	$$PWD/wiFiAPMock.cpp \
	$$PWD/wiFiClientMock.cpp \
	$$PWD/wiFiModeMock.cpp \
	$$PWD/trikGuiApplication.cpp



!trik_not_brick {
    SOURCES += $$PWD/powerLevel.cpp
} else {
    SOURCES += $$PWD/powerLevel_stub.cpp
    DEFINES += DESKTOP
}


RESOURCES = trikGui.qrc

OTHER_FILES += \

implementationIncludes(trikKernel trikControl trikCommunicator trikScriptRunner trikWiFi trikTelemetry)
transitiveIncludes(trikNetwork)
PythonQtIncludes()
links(trikRuntimeQsLog trikKernel trikControl trikCommunicator trikScriptRunner trikWiFi trikTelemetry trikNetwork)

installs()
