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

TEMPLATE = lib

include(../../global.pri)

clang:QMAKE_CXXFLAGS += -Wno-error=undef -Wno-error=switch-enum -Wno-error=missing-prototypes \
		-Wno-error=shadow -Wno-error=disabled-macro-expansion

QMAKE_CXXFLAGS += \
	-isystem $$PWD/googletest/googlemock \
	-isystem $$PWD/googletest/googlemock/include \
	-isystem $$PWD/googletest/googletest \
	-isystem $$PWD/googletest/googletest/include \

HEADERS += \
	$$files(googletest/googletest/src/*.h) \
	$$files(googletest/googletest/include/gtest/*.h) \
	$$files(googletest/googletest/include/gtest/internal/*.h) \
	$$files(googletest/googlemock/include/gmock/*.h) \
	$$files(googletest/googlemock/include/gmock/internal/*.h) \

SOURCES += \
	$$files($$PWD/googletest/googlemock/src/*-all.cc) \
	$$files($$PWD/googletest/googletest/src/*-all.cc) \

TARGET = trikRuntime-googletest$$CONFIGURATION_SUFFIX
