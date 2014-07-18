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

CROSS_COMPILE = $$(CROSS_COMPILE)

!isEmpty(CROSS_COMPILE) {
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

# Useful function to copy additional files to destination,
# from http://stackoverflow.com/questions/3984104/qmake-how-to-copy-a-file-to-the-output
defineTest(copyToDestdir) {
	FILES = $$1

	for (FILE, FILES) {
		# This ugly code is needed because xcopy requires to add source directory name to target directory name when copying directories
		win32:AFTER_SLASH = $$section(FILE, "/", -1, -1)
		win32:BASE_NAME = $$section(FILE, "/", -2, -2)
		win32:equals(AFTER_SLASH, ""):DESTDIR_SUFFIX = /$$BASE_NAME

		win32:FILE ~= s,/$,,g

		FILE = $$shell_path($$FILE)
		DDIR = $$shell_path($$DESTDIR$$DESTDIR_SUFFIX/)
        
		QMAKE_POST_LINK += $(COPY_DIR) $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
	}

	export(QMAKE_POST_LINK)
}
