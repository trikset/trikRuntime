# Copyright 2014 CyberTech Co. Ltd.
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

# Build settings common to all projects in TrikRuntime.
# Provides:
# CONFIGURATION_SUFFIX variable that shall be consistently used in TARGET and LIBS variables in all projects.
#
# Depends on:
# FILES_TO_COPY variable for a list of additional files and directories which needed to be copied after build to
#		target directory.

QT_INSTALL_DIR = $$[QT_INSTALL_PREFIX]
TRIK_SDK_USAGE = $$find(QT_INSTALL_DIR, "trik-sdk")

count(TRIK_SDK_USAGE, 1) {
	ARCHITECTURE = arm
} else {
	ARCHITECTURE = x86
}

CONFIG(debug, debug | release) {
	CONFIGURATION = $$ARCHITECTURE-debug
	CONFIGURATION_SUFFIX = -$$ARCHITECTURE-d
} else {
	CONFIGURATION = $$ARCHITECTURE-release
	equals(ARCHITECTURE, "arm") {
		CONFIGURATION_SUFFIX =
	} else {
		CONFIGURATION_SUFFIX = -$$ARCHITECTURE
	}
}

DESTDIR = $$PWD/bin/$$CONFIGURATION

equals(TEMPLATE, app) {
	!macx {
		QMAKE_LFLAGS += -Wl,-O1,-rpath,.
		QMAKE_LFLAGS += -Wl,-rpath-link,$$DESTDIR
	}
}

OBJECTS_DIR = .build/$$CONFIGURATION/.obj
MOC_DIR = .build/$$CONFIGURATION/.moc
RCC_DIR = .build/$$CONFIGURATION/.rcc
UI_DIR = .build/$$CONFIGURATION/.ui

unix {
	target.path = $$[INSTALL_ROOT]/
	INSTALLS += target
}

!isEmpty(FILES_TO_COPY) {
	win32 {
		QMAKE_POST_LINK = "xcopy $$replace(FILES_TO_COPY, /, \\) $$replace(DESTDIR, /, \\) /s /e /q /y /i"
	} else {
		QMAKE_POST_LINK = "cp -rf $$FILES_TO_COPY $$DESTDIR"
	}
}
