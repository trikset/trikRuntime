# Copyright 2014 - 2016 CyberTech Co. Ltd., Yurii Litvinov, Iakov Kirilenko
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#	  http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

TEMPLATE = subdirs

include(../global.pri)

QMAKE_CXXFLAGS -= -Werror=pedantic -pedantic-errors
!trik_nopython {
	SUBDIRS += PythonQtCore PythonQtFull
	PythonQtFull.depends += PythonQtCore
	PythonQtCore.subdir = PythonQt/src
	PythonQtFull.file = PythonQtFull.pro
}

OTHER_FILES += \
	$$PWD/runme.sh \
	$$PWD/trikControl_export.h \
