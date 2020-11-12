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
TEMPLATE = app
include($$PWD/../global.pri)
CONFIG += testcase testcase_no_bundle no_testcase_installs

QT += widgets

INCLUDEPATH += \
	$$_PRO_FILE_PWD_/ \

QMAKE_CXXFLAGS += \
	-isystem $$PWD/thirdparty/googletest/googlemock/include \
	-isystem $$PWD/thirdparty/googletest/googletest/include \

LIBS += -L$$DESTDIR

LIBS += -lgoogletest$$CONFIGURATION_SUFFIX

SOURCES = $$PWD/mainTest.cpp

include(thirdparty/googletest.pri)

CONFIG(clang) {
	QMAKE_CXXFLAGS += -Wno-unused-local-typedef -Wno-error=gnu-zero-variadic-macro-arguments
} else {
	QMAKE_CXXFLAGS += -Wno-unused-local-typedefs -Wno-error=variadic-macros
}


QMAKE_CXXFLAGS += -Wno-error=pedantic
#DEFINES += GTEST_USE_OWN_TR1_TUPLE


OTHER_FILES += \
	$$PWD/test-system-config.xml \
	$$PWD/test-model-config.xml \

copyToDestdir($$PWD/test-system-config.xml, now)
copyToDestdir($$PWD/test-model-config.xml, now)
