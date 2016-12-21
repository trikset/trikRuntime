# Copyright 2014 CyberTech Labs Ltd.
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

include($$PWD/../global.pri)

TEMPLATE = app

QT += widgets

INCLUDEPATH += \
	$$_PRO_FILE_PWD_/ \
	$$PWD/thirdparty/gmock-1.6.0/include \
	$$PWD/thirdparty/gmock-1.6.0/gtest/include \

LIBS += -L$$DESTDIR

LIBS += -lgmock$$CONFIGURATION_SUFFIX

SOURCES = $$PWD/mainTest.cpp

CONFIG(clang) {
	QMAKE_CXXFLAGS += -Wno-unused-local-typedef
} else {
	QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
} 


QMAKE_CXXFLAGS += -Wno-error=pedantic 

DEFINES += GTEST_USE_OWN_TR1_TUPLE


OTHER_FILES += \
	$$PWD/test-system-config.xml \
	$$PWD/test-model-config.xml \

copyToDestdir($$PWD/test-system-config.xml, now)
copyToDestdir($$PWD/test-model-config.xml, now)
