# Copyright 2013 Roman Kurbatov, Yurii Litvinov
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

TEMPLATE = lib

QT += xml

DEFINES += TRIKWIFI_LIBRARY

PUBLIC_HEADERS += \
	$$PWD/include/trikWiFi/trikWiFi.h \
	$$PWD/include/trikWiFi/wpaConfigurer.h \
	$$PWD/include/trikWiFi/declSpec.h \

HEADERS += \
	$$PWD/src/wpaSupplicantCommunicator.h \

SOURCES += \
	$$PWD/src/trikWiFi.cpp \
	$$PWD/src/wpaConfigurer.cpp \
	$$PWD/src/$$PLATFORM/wpaSupplicantCommunicator.cpp \

installs()
noPch()
