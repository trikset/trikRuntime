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

TEMPLATE = lib

include(../global.pri)

QT += xml

DEFINES += TRIKWIFI_LIBRARY

PUBLIC_HEADERS += \
	$$PWD/include/trikWiFi/trikWiFi.h \
	$$PWD/include/trikWiFi/networkStructs.h \
	$$PWD/include/trikWiFi/trikWifiDeclSpec.h \

HEADERS += \
	$$PWD/src/wpaSupplicantCommunicator.h \
	$$PWD/src/trikWiFiWorker.h \

SOURCES += \
	$$PWD/src/trikWiFi.cpp \
	$$PWD/src/trikWiFiWorker.cpp \

trik_not_brick {
	SOURCES += $$PWD/src/wpaSupplicantCommunicator_stub.cpp
} else {
	SOURCES += $$PWD/src/wpaSupplicantCommunicator.cpp
}

implementationIncludes(trikKernel)
links(trikRuntimeQsLog trikKernel)

installs()
