# Copyright 2015 CyberTech Labs Ltd.
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

# This file is intended to be used from C++ applications that need to use TrikControl. It includes everything that is
# needed to compile with TrikControl and link to it.

# Expects TRIK_RUNTIME_DIR variable to be set pointing to a root of trikRuntime sources directory and trikRuntime
# built for correct configuration and architecture (for example, if you build your C++ application for ARM9 in Release
# configuration, trikRuntime must be already built for ARM9-release, qmake and make trikRuntime.pro for this).

CONFIG -= debug_and_release debug_and_release_target

QT += xml gui

if (equals(QT_MAJOR_VERSION, 5)) {
	QT += widgets
}

# Some magic related to configuration detection taken from global.pri

COMPILER = $$(CXX)

equals(TEMPLATE, app) {
	!macx {
		QMAKE_LFLAGS += -Wl,-O1,-rpath,.
		QMAKE_LFLAGS += -Wl,-rpath-link,$$DESTDIR
	}
}

isEmpty(TRIK_RUNTIME_BIN_DIR):TRIK_RUNTIME_BIN_DIR = $$TRIK_RUNTIME_DIR/bin

LIBS += -L$$TRIK_RUNTIME_BIN_DIR

LIBS += -lqslog$$CONFIGURATION_SUFFIX

if (equals(QT_MAJOR_VERSION, 5)) {
	CONFIG += c++11
}

QMAKE_CXXFLAGS += -std=c++11

defineTest(copyToDestdir) {
	FILES = $$1

	for(FILE, FILES) {
		# This ugly code is needed because xcopy requires to add source directory name to target directory name when copying directories
		win32:AFTER_SLASH = $$section(FILE, "/", -1, -1)
		win32:BASE_NAME = $$section(FILE, "/", -2, -2)
		win32:equals(AFTER_SLASH, ""):DESTDIR_SUFFIX = /$$BASE_NAME

		win32:FILE ~= s,/$,,g

		win32:FILE ~= s,/,\\,g
		DDIR = $$DESTDIR
		win32:DDIR ~= s,/,\\,g

		QMAKE_POST_LINK += $(COPY_DIR) $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
	}

	export(QMAKE_POST_LINK)
}

defineTest(uses) {
	PROJECTS = $$1

	for(PROJECT, PROJECTS) {
		LIBS += -l$$PROJECT$$CONFIGURATION_SUFFIX
		INCLUDEPATH += $$TRIK_RUNTIME_DIR/$$PROJECT/include
		!win32 {
			macx {
				copyToDestdir($$TRIK_RUNTIME_BIN_DIR/lib$$PROJECT$${CONFIGURATION_SUFFIX}.dylib)
				copyToDestdir($$TRIK_RUNTIME_BIN_DIR/lib$$PROJECT$${CONFIGURATION_SUFFIX}.1.dylib)
				copyToDestdir($$TRIK_RUNTIME_BIN_DIR/lib$$PROJECT$${CONFIGURATION_SUFFIX}.1.0.dylib)
				copyToDestdir($$TRIK_RUNTIME_BIN_DIR/lib$$PROJECT$${CONFIGURATION_SUFFIX}.1.0.0.dylib)
			} else {
				copyToDestdir($$TRIK_RUNTIME_BIN_DIR/lib$$PROJECT$${CONFIGURATION_SUFFIX}.so)
				copyToDestdir($$TRIK_RUNTIME_BIN_DIR/lib$$PROJECT$${CONFIGURATION_SUFFIX}.so.1)
				copyToDestdir($$TRIK_RUNTIME_BIN_DIR/lib$$PROJECT$${CONFIGURATION_SUFFIX}.so.1.0)
				copyToDestdir($$TRIK_RUNTIME_BIN_DIR/lib$$PROJECT$${CONFIGURATION_SUFFIX}.so.1.0.0)
			}
		} else {
			copyToDestdir($$TRIK_RUNTIME_BIN_DIR/$$PROJECT$${CONFIGURATION_SUFFIX}.dll)
		}
	}

	export(LIBS)
	export(INCLUDEPATH)
}

# Libs and includepathes for trikControl and related.
uses(trikControl trikKernel trikRuntimeQsLog trikHal)

# Files to be present in destdir for trikRuntime to work.
copyToDestdir( \
	$$PWD/model-config.xml  \
	$$PWD/system-config.xml  \
	$$PWD/../media/ \
)
