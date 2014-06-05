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

TRIKCONTROL_DIR = ../trikControl/
TRIKCOMMUNICATOR_DIR = ../trikCommunicator/
TRIKSCRIPTRUNNER_DIR = ../trikScriptRunner/
TRIKWIFI_DIR = ../trikWiFi/

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

TEMPLATE = app

QT += network

if (equals(QT_MAJOR_VERSION, 5)) {
	QT += widgets
}

FILES_TO_COPY = \
	wpa-config.xml \

include(../global.pri)

INCLUDEPATH = \
	$$PWD \
	$$TRIKCOMMUNICATOR_DIR/include \
	$$TRIKSCRIPTRUNNER_DIR/include \
	$$TRIKWIFI_DIR/include \
	$$TRIKCONTROL_DIR/include \

LIBS += \
	-L$$DESTDIR \
	-ltrikCommunicator$$CONFIGURATION_SUFFIX \
	-ltrikScriptRunner$$CONFIGURATION_SUFFIX \
	-ltrikWiFi$$CONFIGURATION_SUFFIX \
	-ltrikControl$$CONFIGURATION_SUFFIX \
