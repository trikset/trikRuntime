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

include(../global.pri)

HEADERS += \
	$$PWD/backgroundWidget.h \
	$$PWD/batteryIndicator.h \
	$$PWD/connectButton.h \
	$$PWD/controller.h \
	$$PWD/digitSelector.h \
	$$PWD/fileManagerWidget.h \
	$$PWD/motorLever.h \
	$$PWD/motorsWidget.h \
	$$PWD/numberSelectionWidget.h \
	$$PWD/rcReader.h \
	$$PWD/communicationSettingsWidget.h \
	$$PWD/runningWidget.h \
	$$PWD/sensorIndicator.h \
	$$PWD/sensorsSelectionWidget.h \
	$$PWD/sensorsWidget.h \
	$$PWD/startWidget.h \
	$$PWD/trikGuiApplication.h \
	$$PWD/trikGuiDialog.h \
	$$PWD/trikGuiMessageBox.h \
	$$PWD/wiFiAPWidget.h \
	$$PWD/wiFiClientWidget.h \
	$$PWD/wiFiInitWidget.h \
	$$PWD/wiFiModeWidget.h \
	$$PWD/informationWidget.h \
	$$PWD/fileManagerMessageBox.h \
	$$PWD/updateWidget.h \
	$$PWD/systemSettingsWidget.h \

SOURCES += \
	$$PWD/digitSelector.cpp \
	$$PWD/fileManagerWidget.cpp \
	$$PWD/runningWidget.cpp \
	$$PWD/connectButton.cpp \
	$$PWD/wiFiClientWidget.cpp \
	$$PWD/sensorsSelectionWidget.cpp \
	$$PWD/startWidget.cpp \
	$$PWD/trikGuiApplication.cpp \
	$$PWD/controller.cpp \
	$$PWD/main.cpp \
	$$PWD/motorLever.cpp \
	$$PWD/motorsWidget.cpp \
	$$PWD/numberSelectionWidget.cpp \
	$$PWD/sensorIndicator.cpp \
	$$PWD/sensorsWidget.cpp \
	$$PWD/wiFiModeWidget.cpp \
	$$PWD/wiFiInitWidget.cpp \
	$$PWD/wiFiAPWidget.cpp \
	$$PWD/rcReader.cpp \
	$$PWD/communicationSettingsWidget.cpp \
	$$PWD/trikGuiMessageBox.cpp \
	$$PWD/trikGuiDialog.cpp \
	$$PWD/backgroundWidget.cpp \
	$$PWD/batteryIndicator.cpp \
	$$PWD/informationWidget.cpp \
	$$PWD/fileManagerMessageBox.cpp \
	$$PWD/updateWidget.cpp \
	$$PWD/systemSettingsWidget.cpp \

TRANSLATIONS = trikGui_ru.ts
RESOURCES = trikGui.qrc

copyToDestdir($$PWD/wpa-config.xml)

TEMPLATE = app

QT += network xml

if (equals(QT_MAJOR_VERSION, 5)) {
	QT += widgets
}

uses(trikKernel trikControl trikCommunicator trikScriptRunner trikWiFi trikTelemetry trikNetwork)
