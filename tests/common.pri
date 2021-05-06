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
isEmpty(TEMPLATE): TEMPLATE = app
include($$PWD/../global.pri)

QT += widgets

#Workaround for MinGW build. Qt incorrectly sets it to empty string on Win32 for bash
mingw: TEST_TARGET_DIR = .

# after global.pri sets DESTDIR

equals(TEMPLATE, app) {
	CONFIG += testcase testcase_no_bundle no_testcase_installs
}

INCLUDEPATH += \
	$$_PRO_FILE_PWD_/ \

SOURCES = $$PWD/mainTest.cpp

include(thirdparty/googletest.pri)

OTHER_FILES += \
	$$PWD/test-system-config.xml \
	$$PWD/test-model-config.xml \

copyToDestdir($$PWD/test-system-config.xml, now)
copyToDestdir($$PWD/test-model-config.xml, now)
