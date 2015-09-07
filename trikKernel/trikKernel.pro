# Copyright 2014 - 2015 CyberTech Labs Ltd.
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

QT += widgets xml

HEADERS += \
	$$PWD/include/trikKernel/applicationInitHelper.h \
	$$PWD/include/trikKernel/configurer.h \
	$$PWD/include/trikKernel/coreDumping.h \
	$$PWD/include/trikKernel/debug.h \
	$$PWD/include/trikKernel/fileUtils.h \
	$$PWD/include/trikKernel/loggingHelper.h \
	$$PWD/include/trikKernel/commandLineParser.h \
	$$PWD/include/trikKernel/paths.h \
	$$PWD/include/trikKernel/rcReader.h \
	$$PWD/include/trikKernel/translationsHelper.h \
	$$PWD/include/trikKernel/version.h \
	$$PWD/include/trikKernel/exceptions/failedToOpenFileException.h \
	$$PWD/include/trikKernel/exceptions/failedToParseXmlException.h \
	$$PWD/include/trikKernel/exceptions/internalErrorException.h \
	$$PWD/include/trikKernel/exceptions/malformedConfigException.h \
	$$PWD/include/trikKernel/exceptions/trikRuntimeException.h \

SOURCES += \
	$$PWD/src/applicationInitHelper.cpp \
	$$PWD/src/configurer.cpp \
	$$PWD/src/debug.cpp \
	$$PWD/src/fileUtils.cpp \
	$$PWD/src/loggingHelper.cpp \
	$$PWD/src/commandLineParser.cpp \
	$$PWD/src/paths.cpp \
	$$PWD/src/rcReader.cpp \
	$$PWD/src/translationsHelper.cpp \
	$$PWD/src/$$PLATFORM/coreDumping.cpp \

TEMPLATE = lib

DEFINES += TRIKKERNEL_LIBRARY

TRANSLATIONS = \
	$$PWD/../translations/ru/trikKernel_ru.ts \
	$$PWD/../translations/fr/trikKernel_fr.ts \

installs()
