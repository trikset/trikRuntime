# Copyright 2014 - 2015 CyberTech Labs Ltd.
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

include(../common.pri)

HEADERS += \
	$$PWD/trikPyRunnerTest.h \

SOURCES += \
	$$PWD/trikPyRunnerTest.cpp \

OTHER_FILES += \
	$$PWD/data/file-test.py \

implementationIncludes(trikKernel trikControl trikScriptRunner trikNetwork tests/testUtils)
links(trikKernel trikControl trikScriptRunner trikNetwork trikHal testUtils)

copyToDestdir($$PWD/data/, now)

#DISTFILES += \
#	data/sync-system-test.js \
#	data/async-system-test.js
