# Copyright 2013 - 2015 Yurii Litvinov and CyberTech Labs Ltd.
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

include(../global.pri)

PUBLIC_HEADERS += \
	$$PWD/include/trikCommunicator/trikCommunicator.h \

HEADERS += \
	$$PWD/src/connection.h \

SOURCES += \
	$$PWD/src/trikCommunicator.cpp \
	$$PWD/src/connection.cpp \

QT += network

DEFINES += TRIKCOMMUNICATOR_LIBRARY

interfaceIncludes(trikNetwork)
implementationIncludes(trikKernel trikScriptRunner trikControl)
PythonQtIncludes()

links(trikQsLog trikScriptRunner trikControl trikKernel trikNetwork)

installs()
