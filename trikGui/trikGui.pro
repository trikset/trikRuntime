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
QT += network xml widgets

include(../global.pri)

HEADERS += \
	$$PWD/abstractIndicator.h \
	$$PWD/autoRunner.h \
	$$PWD/backgroundWidget.h \
	$$PWD/batteryIndicator.h \
	$$PWD/connectButton.h \
	$$PWD/controller.h \
	$$PWD/digitSelector.h \
	$$PWD/encoderIndicator.h \
	$$PWD/fileManagerWidget.h \
	$$PWD/informationWidget.h \
	$$PWD/languageSelectionWidget.h \
	$$PWD/lazyMainWidget.h \
	$$PWD/lazyMainWidgetWrapper.h \
	$$PWD/mainWidget.h \
	$$PWD/motorLever.h \
	$$PWD/motorsWidget.h \
	$$PWD/numberSelectionWidget.h \
	$$PWD/communicationSettingsWidget.h \
	$$PWD/runningWidget.h \
	$$PWD/sensorIndicator.h \
	$$PWD/sensorsWidget.h \
	$$PWD/startWidget.h \
	$$PWD/systemSettingsWidget.h \
	$$PWD/trikGuiApplication.h \
	$$PWD/trikGuiDialog.h \
	$$PWD/trikGuiMessageBox.h \
	$$PWD/updateWidget.h \
	$$PWD/wiFiAPWidget.h \
	$$PWD/wiFiClientWidget.h \
	$$PWD/wiFiInitWidget.h \
	$$PWD/wiFiModeWidget.h \
	$$PWD/wiFiIndicator.h \
	$$PWD/openSocketIndicator.h \
	$$PWD/commandSettingsWidget.h \
	$$PWD/commandsListWidget.h \
	$$PWD/programmingWidget.h \
	$$PWD/sensorSettingsWidget.h \
	$$PWD/sensorLever.h \
	$$PWD/scriptHolder.h \
	$$PWD/gamepadIndicator.h \
	$$PWD/powerLevel.h \
	$$PWD/gyroscopeIndicator.h \
	$$PWD/accelerometerWidget.h \
	$$PWD/networkWidget.h \
	$$PWD/fileSystemFilter.h \
	$$PWD/cameraWidget.h \
	$$PWD/pwmCaptureIndicator.h

SOURCES += \
	$$PWD/autoRunner.cpp \
	$$PWD/backgroundWidget.cpp \
	$$PWD/batteryIndicator.cpp \
	$$PWD/communicationSettingsWidget.cpp \
	$$PWD/connectButton.cpp \
	$$PWD/controller.cpp \
	$$PWD/digitSelector.cpp \
	$$PWD/encoderIndicator.cpp \
	$$PWD/fileManagerWidget.cpp \
	$$PWD/informationWidget.cpp \
	$$PWD/languageSelectionWidget.cpp \
	$$PWD/lazyMainWidgetWrapper.cpp \
	$$PWD/trikGuiMain.cpp \
	$$PWD/motorLever.cpp \
	$$PWD/motorsWidget.cpp \
	$$PWD/numberSelectionWidget.cpp \
	$$PWD/runningWidget.cpp \
	$$PWD/sensorIndicator.cpp \
	$$PWD/sensorsWidget.cpp \
	$$PWD/startWidget.cpp \
	$$PWD/systemSettingsWidget.cpp \
	$$PWD/trikGuiApplication.cpp \
	$$PWD/trikGuiDialog.cpp \
	$$PWD/trikGuiMessageBox.cpp \
	$$PWD/updateWidget.cpp \
	$$PWD/wiFiAPWidget.cpp \
	$$PWD/wiFiClientWidget.cpp \
	$$PWD/wiFiInitWidget.cpp \
	$$PWD/wiFiModeWidget.cpp \
	$$PWD/wiFiIndicator.cpp \
	$$PWD/openSocketIndicator.cpp \
	$$PWD/commandSettingsWidget.cpp \
	$$PWD/commandsListWidget.cpp \
	$$PWD/programmingWidget.cpp \
	$$PWD/sensorSettingsWidget.cpp \
	$$PWD/sensorLever.cpp \
	$$PWD/scriptHolder.cpp \
	$$PWD/gamepadIndicator.cpp \
	$$PWD/$$PLATFORM/powerLevel.cpp \
	$$PWD/gyroscopeIndicator.cpp \
	$$PWD/accelerometerWidget.cpp \
	$$PWD/networkWidget.cpp \
	$$PWD/fileSystemFilter.cpp \
	$$PWD/cameraWidget.cpp \
	$$PWD/pwmCaptureIndicator.cpp

RESOURCES = trikGui.qrc

OTHER_FILES += \

implementationIncludes(trikKernel trikControl trikCommunicator trikScriptRunner trikWiFi trikTelemetry)
transitiveIncludes(trikNetwork)
PythonQtIncludes()
links(trikQsLog trikKernel trikControl trikCommunicator trikScriptRunner trikWiFi trikTelemetry trikNetwork)

installs()
