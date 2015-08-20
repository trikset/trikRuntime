# Copyright 2015 Yurii Litvinov and CyberTech Labs Ltd.
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
	$$PWD/include/trikHal/declSpec.h \
	$$PWD/include/trikHal/hardwareAbstractionInterface.h \
	$$PWD/include/trikHal/hardwareAbstractionFactory.h \
	$$PWD/include/trikHal/i2cInterface.h \
	$$PWD/include/trikHal/fifoInterface.h \
	$$PWD/include/trikHal/eventFileInterface.h \
	$$PWD/include/trikHal/inputDeviceFileInterface.h \
	$$PWD/include/trikHal/outputDeviceFileInterface.h \
	$$PWD/include/trikHal/systemConsoleInterface.h \

HEADERS += \
	$$PWD/src/real/hardwareAbstraction.h \
	$$PWD/src/real/i2c.h \
	$$PWD/src/real/systemConsole.h \
	$$PWD/src/real/eventFile.h \
	$$PWD/src/real/inputDeviceFile.h \
	$$PWD/src/real/outputDeviceFile.h \
	$$PWD/src/real/fifo.h \

SOURCES += \
	$$PWD/src/real/hardwareAbstractionFactory.cpp \
	$$PWD/src/real/hardwareAbstraction.cpp \
	$$PWD/src/real/i2c.cpp \
	$$PWD/src/real/systemConsole.cpp \
	$$PWD/src/real/eventFile.cpp \
	$$PWD/src/real/inputDeviceFile.cpp \
	$$PWD/src/real/outputDeviceFile.cpp \
	$$PWD/src/real/fifo.cpp \

TEMPLATE = lib

DEFINES += TRIKHAL_LIBRARY

if (equals(QT_MAJOR_VERSION, 5)) {
	QT += widgets
}
