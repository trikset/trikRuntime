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

PUBLIC_HEADERS += \
	$$PWD/include/trikHal/declSpec.h \
	$$PWD/include/trikHal/hardwareAbstractionInterface.h \
	$$PWD/include/trikHal/hardwareAbstractionFactory.h \
	$$PWD/include/trikHal/fifoInterface.h \
	$$PWD/include/trikHal/eventFileInterface.h \
	$$PWD/include/trikHal/inputDeviceFileInterface.h \
	$$PWD/include/trikHal/mspI2cInterface.h \
	$$PWD/include/trikHal/mspUsbInterface.h \
	$$PWD/include/trikHal/outputDeviceFileInterface.h \
	$$PWD/include/trikHal/systemConsoleInterface.h \

!win32:!macx {
	HEADERS += \
		$$PWD/src/trik/trikHardwareAbstraction.h \
		$$PWD/src/trik/trikMspI2c.h \
		$$PWD/src/trik/trikMspUsb.h \
		$$PWD/src/trik/trikSystemConsole.h \
		$$PWD/src/trik/trikEventFile.h \
		$$PWD/src/trik/trikInputDeviceFile.h \
		$$PWD/src/trik/trikOutputDeviceFile.h \
		$$PWD/src/trik/trikFifo.h \
		$$PWD/src/trik/usbMsp/usbMSP430Interface.h \
		$$PWD/src/trik/usbMsp/usbMSP430Defines.h \
}

HEADERS += \
	$$PWD/src/stub/stubHardwareAbstraction.h \
	$$PWD/src/stub/stubMspI2c.h \
	$$PWD/src/stub/stubMspUsb.h \
	$$PWD/src/stub/stubSystemConsole.h \
	$$PWD/src/stub/stubEventFile.h \
	$$PWD/src/stub/stubInputDeviceFile.h \
	$$PWD/src/stub/stubOutputDeviceFile.h \
	$$PWD/src/stub/stubFifo.h \

!win32:!macx {
	SOURCES += \
		$$PWD/src/trik/trikHardwareAbstraction.cpp \
		$$PWD/src/trik/trikMspI2c.cpp \
		$$PWD/src/trik/trikMspUsb.cpp \
		$$PWD/src/trik/trikSystemConsole.cpp \
		$$PWD/src/trik/trikEventFile.cpp \
		$$PWD/src/trik/trikInputDeviceFile.cpp \
		$$PWD/src/trik/trikOutputDeviceFile.cpp \
		$$PWD/src/trik/trikFifo.cpp \
		$$PWD/src/trik/usbMsp/usbMSP430Interface.cpp \
}

SOURCES += \
	$$PWD/src/stub/stubHardwareAbstraction.cpp \
	$$PWD/src/stub/stubMspI2c.cpp \
	$$PWD/src/stub/stubMspUsb.cpp \
	$$PWD/src/stub/stubSystemConsole.cpp \
	$$PWD/src/stub/stubEventFile.cpp \
	$$PWD/src/stub/stubInputDeviceFile.cpp \
	$$PWD/src/stub/stubOutputDeviceFile.cpp \
	$$PWD/src/stub/stubFifo.cpp \

equals(ARCHITECTURE, arm) {
	SOURCES += $$PWD/src/trik/hardwareAbstractionFactory.cpp
} else {
	SOURCES += $$PWD/src/stub/hardwareAbstractionFactory.cpp
}

TEMPLATE = lib

DEFINES += TRIKHAL_LIBRARY

links(trikKernel)
implementationIncludes(trikKernel)

if (equals(QT_MAJOR_VERSION, 5)) {
	QT += widgets
}

installs()
