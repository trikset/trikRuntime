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

include(../global.pri)

SOURCES += \
	$$PWD/trikRunMain.cpp \

OTHER_FILES += \
	$$PWD/test.qts \

copyToDestdir($$OTHER_FILES)

implementationIncludes(trikKernel trikControl trikScriptRunner trikNetwork)
PythonQtIncludes()
links(trikQsLog trikKernel trikControl trikScriptRunner trikNetwork)


TEMPLATE = app
CONFIG += console

QT += gui

if (equals(QT_MAJOR_VERSION, 5)) {
	QT += widgets
}

installs()
