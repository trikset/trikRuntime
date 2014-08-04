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
	$$PWD/fileManagerWidget.h \
	$$PWD/runningWidget.h \
	$$PWD/wiFiClientWidget.h \
	$$PWD/sensorsSelectionWidget.h \
	$$PWD/startWidget.h \
	$$PWD/trikGuiApplication.h \
	$$PWD/controller.h \
	$$PWD/motorLever.h \
	$$PWD/motorsWidget.h \
	$$PWD/sensorIndicator.h \
	$$PWD/sensorsWidget.h \
	$$PWD/wiFiModeWidget.h \
	$$PWD/wiFiInitWidget.h \
	$$PWD/wiFiAPWidget.h \
	$$PWD/rcReader.h \
	$$PWD/trikGuiMessageBox.h \
	$$PWD/trikGuiDialog.h \

SOURCES += \
	$$PWD/main.cpp \
	$$PWD/fileManagerWidget.cpp \
	$$PWD/runningWidget.cpp \
	$$PWD/wiFiClientWidget.cpp \
	$$PWD/sensorsSelectionWidget.cpp \
	$$PWD/startWidget.cpp \
	$$PWD/trikGuiApplication.cpp \
	$$PWD/controller.cpp \
	$$PWD/motorLever.cpp \
	$$PWD/motorsWidget.cpp \
	$$PWD/sensorIndicator.cpp \
	$$PWD/sensorsWidget.cpp \
	$$PWD/wiFiModeWidget.cpp \
	$$PWD/wiFiInitWidget.cpp \
	$$PWD/wiFiAPWidget.cpp \
	$$PWD/rcReader.cpp \
	$$PWD/trikGuiMessageBox.cpp \
	$$PWD/trikGuiDialog.cpp \

TRANSLATIONS = trikGui_ru.ts
RESOURCES = trikGui.qrc

copyToDestdir(wpa-config.xml)

TEMPLATE = app

QT += network

if (equals(QT_MAJOR_VERSION, 5)) {
	QT += widgets
}

uses(trikKernel trikControl trikCommunicator trikScriptRunner trikWiFi)

INCLUDEPATH += \
	../trikKernel/include/ \
	../trikControl/include/ \
	../trikCommunicator/include/ \
	../trikScriptRunner/include/ \
	../trikWiFi/include/ \
