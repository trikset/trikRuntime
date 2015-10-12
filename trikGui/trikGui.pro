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
	$$PWD/abstractIndicator.h \
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
	$$PWD/sensorsSelectionWidget.h \
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

SOURCES += \
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
	$$PWD/sensorsSelectionWidget.cpp \
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

TRANSLATIONS = \
	$$PWD/../translations/ru/trikGui_ru.ts \
	$$PWD/../translations/fr/trikGui_fr.ts \

RESOURCES = trikGui.qrc

copyToDestdir($$PWD/wpa-config.xml)

TEMPLATE = app

QT += network xml

if (equals(QT_MAJOR_VERSION, 5)) {
	QT += widgets
}

implementationIncludes(trikKernel trikControl trikCommunicator trikScriptRunner trikWiFi trikTelemetry)
transitiveIncludes(trikNetwork)
links(trikKernel trikControl trikCommunicator trikScriptRunner trikWiFi trikTelemetry trikNetwork)

installs()
