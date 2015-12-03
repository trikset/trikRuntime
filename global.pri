# Copyright 2014 - 2015 CyberTech Co. Ltd.
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
# copyToDestdir function to copy arbitrary files and directories to DESTDIR
# uses function to automatically add a library to INCLUDEPATH and LIBS.
#
# Uses environment variable INSTALL_ROOT as a root of a file structure for install rules.

COMPILER = $$(CXX)

COMPILER_IS_ARM = $$find(COMPILER, arm-.*)

count(COMPILER_IS_ARM, 1) {
	ARCHITECTURE = arm
} else {
	ARCHITECTURE = x86
}

win32 {
	PLATFORM = windows
} else {
	PLATFORM = linux
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

PROJECT_BASENAME = $$basename(_PRO_FILE_)
PROJECT_NAME = $$section(PROJECT_BASENAME, ".", 0, 0)

TARGET = $$PROJECT_NAME$$CONFIGURATION_SUFFIX

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

INCLUDEPATH += $$_PRO_FILE_PWD_ \
	$$_PRO_FILE_PWD_/include/$$PROJECT_NAME \

INCLUDEPATH += \
	$$PWD/qslog \

PRECOMPILED_HEADER = $$PWD/pch.h
CONFIG += precompile_header

LIBS += -L$$DESTDIR

IS_QSLOG = $$find(PROJECT_NAME, [qQ]s[lL]og)

isEmpty(IS_QSLOG) {
	LIBS += -lqslog$$CONFIGURATION_SUFFIX
}

QMAKE_CXXFLAGS += -std=c++1y
QMAKE_CXXFLAGS += -Wextra -Wcast-qual -Wwrite-strings -Wredundant-decls -Wunreachable-code -Wnon-virtual-dtor -Woverloaded-virtual

GLOBAL_PWD = $$PWD

# Useful function to copy additional files to destination,
# from http://stackoverflow.com/questions/3984104/qmake-how-to-copy-a-file-to-the-output
defineTest(copyToDestdir) {
	FILES = $$1

	for(FILE, FILES) {
		# This ugly code is needed because xcopy requires to add source directory name to target directory name when copying directories
		win32:AFTER_SLASH = $$section(FILE, "/", -1, -1)
		win32:BASE_NAME = $$section(FILE, "/", -2, -2)
		win32:equals(AFTER_SLASH, ""):DESTDIR_SUFFIX = /$$BASE_NAME

		win32:FILE ~= s,/$,,g

		win32:FILE ~= s,/,\,g
		DDIR = $$DESTDIR$$DESTDIR_SUFFIX/
		win32:DDIR ~= s,/,\,g

		QMAKE_POST_LINK += $(COPY_DIR) $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
	}

	export(QMAKE_POST_LINK)
}

defineTest(interfaceIncludes) {
	PROJECTS = $$1

	for(PROJECT, PROJECTS) {
		INCLUDEPATH += $$GLOBAL_PWD/$$PROJECT/include
	}

	export(INCLUDEPATH)
}

defineTest(implementationIncludes) {
	interfaceIncludes($$1)
}

defineTest(transitiveIncludes) {
	interfaceIncludes($$1)
}

defineTest(links) {
	LIBS += -L$$DESTDIR
	PROJECTS = $$1

	for(PROJECT, PROJECTS) {
		LIBS += -l$$PROJECT$$CONFIGURATION_SUFFIX
	}

	export(LIBS)
}

defineTest(installs) {
	unix {
		equals(TEMPLATE, lib) {
			target.path = $$INSTALL_ROOT/usr/lib/
			isEmpty(PUBLIC_HEADERS) {
				headers.files = $$HEADERS
				headers.path = $$INSTALL_ROOT/usr/include/$$PROJECT_NAME/
			} else {
				headers.files = $$PUBLIC_HEADERS
				headers.path = $$INSTALL_ROOT/usr/include/$$PROJECT_NAME/
			}
		}

		equals(TEMPLATE, app) {
			target.path = $$INSTALL_ROOT/usr/bin/
		}

		INSTALLS += target
		export(target.path)

		equals(TEMPLATE, lib) {
			INSTALLS += headers
			export(headers.files)
			export(headers.path)
		}

		export(INSTALLS)
	}

	HEADERS += $$PUBLIC_HEADERS
	export(HEADERS)
}

defineTest(installAdditionalConfigs) {
	FILES = $$1

	unix {
		additionalConfigs.files += $$FILES
		additionalConfigs.path = $$INSTALL_ROOT/etc/trik/trikRuntime/

		INSTALLS += additionalConfigs

		export(additionalConfigs.path)
		export(additionalConfigs.files)
		export(INSTALLS)
	}
}

defineTest(installAdditionalSharedFiles) {
	FILES = $$1

	unix {
		additionalSharedFiles.files += $$FILES
		additionalSharedFiles.path = $$INSTALL_ROOT/usr/share/trikRuntime/

		INSTALLS += additionalSharedFiles

		export(additionalSharedFiles.path)
		export(additionalSharedFiles.files)
		export(INSTALLS)
	}
}

defineTest(noPch) {
	CONFIG -= precompiled_header
	PRECOMPILED_HEADER =
	export(CONFIG)
	export(PRECOMPILED_HEADER)
}
