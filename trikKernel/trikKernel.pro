# Copyright 2014 CyberTech Labs Ltd.
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

QT += network

INCLUDEPATH += $$PWD/include \
	$$PWD/../qslog \

HEADERS += \
	$$PWD/include/trikKernel/connection.h \
	$$PWD/include/trikKernel/debug.h \
	$$PWD/include/trikKernel/fileUtils.h \
	$$PWD/include/trikKernel/trikServer.h \
	$$PWD/include/trikKernel/coreDumping.h \
        $$PWD/include/trikKernel/version.h \
        $$PWD/include/trikKernel/mainWidget.h \
        $$PWD/include/trikKernel/lazyMainWidget.h

SOURCES += \
	$$PWD/src/connection.cpp \
	$$PWD/src/debug.cpp \
	$$PWD/src/fileUtils.cpp \
	$$PWD/src/trikServer.cpp \
	$$PWD/src/coreDumping.cpp

TEMPLATE = lib

DEFINES += TRIKKERNEL_LIBRARY

uses(qslog)
