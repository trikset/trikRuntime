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

QT += xml

CONFIG(debug, debug | release) {
	CONFIGURATION = debug
	CONFIGURATION_SUFFIX = d
} else {
	CONFIGURATION = release
	CONFIGURATION_SUFFIX =
}

TARGET = trikControl$$CONFIGURATION_SUFFIX

DESTDIR = bin/$$CONFIGURATION/

OBJECTS_DIR = .obj/$$CONFIGURATION
MOC_DIR = .moc/$$CONFIGURATION
RCC_DIR = .rcc/$$CONFIGURATION
UI_DIR = .ui/$$CONFIGURATION

INCLUDEPATH = \
	$$PWD \
	$$PWD/include/trikControl \

HEADERS += \
	$$PWD/include/trikControl/brick.h \
	$$PWD/include/trikControl/servoMotor.h \
	$$PWD/include/trikControl/powerMotor.h \
	$$PWD/include/trikControl/sensor.h \
	$$PWD/include/trikControl/declSpec.h \
	$$PWD/src/configurer.h \

SOURCES += \
	$$PWD/src/brick.cpp \
	$$PWD/src/servoMotor.cpp \
	$$PWD/src/powerMotor.cpp \
	$$PWD/src/sensor.cpp \
	$$PWD/src/configurer.cpp \
