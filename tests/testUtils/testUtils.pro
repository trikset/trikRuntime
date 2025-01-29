# Copyright 2016 CyberTech Labs Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

#     http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

include(../../global.pri)

TEMPLATE = lib

DEFINES += TESTUTILS_LIBRARY

QT += network core-private

interfaceIncludes(trikNetwork)
links(trikNetwork)

HEADERS += \
	$$PWD/include/testUtils/tcpClientSimulator.h \
	$$PWD/include/testUtils/wait.h \
	$$PWD/include/testUtils/testUtilsDeclSpec.h \
	$$PWD/include/testUtils/eventFilter.h

SOURCES += \
	$$PWD/src/tcpClientSimulator.cpp \
	$$PWD/src/wait.cpp \
	$$PWD/src/eventFilter.cpp \	
