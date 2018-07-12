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

# Build settings common to all projects in TrikRuntime.
# Provides:
# CONFIGURATION_SUFFIX variable that shall be consistently used in TARGET and LIBS variables in all projects.
# copyToDestdir function to copy arbitrary files and directories to DESTDIR
# uses function to automatically add a library to INCLUDEPATH and LIBS.
#
# Uses environment variable INSTALL_ROOT as a root of a file structure for install rules.
#
# Uses CONFIG variable to control support for Google Sanitizers:
# - CONFIG+=sanitize-address will enable address sanitizer
# - CONFIG+=sanitize-undefined will enable undefined behavior sanitizer
# - CONFIG+=sanitize-thread will enable thread sanitizer

COMPILER = $$(CXX)

COMPILER_IS_ARM = $$find(COMPILER, arm-.*)

count(COMPILER_IS_ARM, 1) {
	ARCHITECTURE = arm
} else {
	ARCHITECTURE = x86
}

win32 {
	PLATFORM = windows
}

unix:!macx {
	PLATFORM = linux
}

macx {
	PLATFORM = mac
}

CONFIG(debug, debug | release) {
	CONFIGURATION = $$ARCHITECTURE-debug
	CONFIGURATION_SUFFIX = -$$ARCHITECTURE-d
	unix {
		QMAKE_CXXFLAGS += -coverage
		QMAKE_LFLAGS += -coverage
	}
} else {
	CONFIGURATION = $$ARCHITECTURE-release
	equals(ARCHITECTURE, "arm") {
		CONFIGURATION_SUFFIX =
	} else {
		CONFIGURATION_SUFFIX = -$$ARCHITECTURE
	}
}

#	CHECK_GCC_VERSION_5="test \"x$$CHECK_GCC_VERSION\" != x && echo \"$$CHECK_GCC_VERSION\" | grep -qe \'\\<5\\.[0-9]\\+\\.\'"
#	!clang:gcc:*-g++*:system($$CHECK_GCC_VERSION_5){

#CHECK_GCC_VERSION=$$system("$$QMAKE_CXX --version")
#!CONFIG(nosanitizers):!clang:gcc:*-g++*:system(test \"x$${CHECK_GCC_VERSION}\" = x  || echo \"$$CHECK_GCC_VERSION\" | grep -qe \'\\<4\\.[0-9]\\+\\.\') 
!clang:gcc:*-g++*:system($$QMAKE_CXX --version | grep -oe \\\"\\<5\\.[0-9]\\+\\.\\\" ){ CONFIG += gcc5 }
!clang:gcc:*-g++*:system($$QMAKE_CXX --version | grep -oe \\\"\\<4\\.[0-9]\\+\\.\\\" ){ CONFIG += gcc4 }

DESTDIR = $$PWD/bin/$$CONFIGURATION

PROJECT_BASENAME = $$basename(_PRO_FILE_)
PROJECT_NAME = $$section(PROJECT_BASENAME, ".", 0, 0)

isEmpty(TARGET) {
	TARGET = $$PROJECT_NAME$$CONFIGURATION_SUFFIX
} else {
	TARGET = $$TARGET$$CONFIGURATION_SUFFIX
}

equals(TEMPLATE, app) {
	unix:!macx {
		QMAKE_LFLAGS += -Wl,-rpath-link,$$DESTDIR
		!CONFIG(no_rpath) QMAKE_LFLAGS += -Wl,-O1,-rpath,.
	}
	macx:!CONFIG(no_rpath) {
		QMAKE_LFLAGS += -rpath . -rpath @executable_path/../Lib -rpath @executable_path/../Frameworks -rpath @executable_path/../../../
	}
}

macx-clang {
	QMAKE_MACOSX_DEPLOYMENT_TARGET=10.9
	QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/
}

!CONFIG(nosanitizers):!clang:gcc:*-g++*:gcc4{
	warning("Disabled sanitizers, failed to detect compiler version or too old compiler: $$QMAKE_CXX")
	CONFIG += nosanitizers
}

unix:!CONFIG(nosanitizers):!CONFIG(no-sanitizers) {

	# seems like we want USan always, but are afraid of ....
	!CONFIG(sanitize_address):!CONFIG(sanitize_thread):!CONFIG(sanitize_memory):!CONFIG(sanitize_kernel_address) {
		# Ubsan is turned on by default
		CONFIG += sanitizer sanitize_undefined
	}


	#LSan can be used without performance degrade even in release build
	#But at the moment we can not, because of Qt  problems
	CONFIG(debug, debug | release):!CONFIG(sanitize_address):!macx-clang { CONFIG += sanitize_leak }

	CONFIG(sanitize_leak) {
		QMAKE_CFLAGS += -fsanitize=leak
		QMAKE_CXXFLAGS += -fsanitize=leak
		QMAKE_LFLAGS += -fsanitize=leak
	}

	CONFIG(sanitize_undefined):macx-clang {
		# sometimes runtime is missing in clang. this hack allows to avoid runtime dependency.
		QMAKE_SANITIZE_UNDEFINED_CFLAGS += -fsanitize-trap=undefined
		QMAKE_SANITIZE_UNDEFINED_CXXFLAGS += -fsanitize-trap=undefined
		QMAKE_SANITIZE_UNDEFINED_LFLAGS += -fsanitize-trap=undefined
	}

	!clang:gcc:*-g++*:gcc5{
		CONFIG(sanitize_undefined){
		# Ubsan has (had at least) known issues with false errors about calls of methods of the base class.
		# That must be disabled. Variables for confguring ubsan are taken from here:
		# https://codereview.qt-project.org/#/c/43420/17/mkspecs/common/sanitize.conf
		# They can change in some version of Qt, keep track of it.
		# By the way, simply setting QMAKE_CFLAGS, QMAKE_CXXFLAGS and QMAKE_LFLAGS instead of those used below
		# will not work due to arguments order ("-fsanitize=undefined" must be declared before "-fno-sanitize=vptr").
			QMAKE_SANITIZE_UNDEFINED_CFLAGS += -fno-sanitize=vptr
			QMAKE_SANITIZE_UNDEFINED_CXXFLAGS += -fno-sanitize=vptr
			QMAKE_SANITIZE_UNDEFINED_LFLAGS += -fno-sanitize=vptr
		}
	}

	CONFIG(release, debug | release){
			QMAKE_CFLAGS += -fsanitize-recover=all
			QMAKE_CXXFLAGS += -fsanitize-recover=all
	}

}

OBJECTS_DIR = .build/$$CONFIGURATION/obj
MOC_DIR = .build/$$CONFIGURATION/moc
RCC_DIR = .build/$$CONFIGURATION/rcc
UI_DIR = .build/$$CONFIGURATION/ui

PRECOMPILED_HEADER = $$PWD/pch.h
CONFIG += precompile_header

INCLUDEPATH += $$_PRO_FILE_PWD_ \
	$$_PRO_FILE_PWD_/include/$$PROJECT_NAME \

INCLUDEPATH += \
	$$PWD/qslog \



LIBS += -L$$DESTDIR

IS_QSLOG = $$find(PROJECT_NAME, [qQ]s[lL]og)

isEmpty(IS_QSLOG) {
	LIBS += -lqslog$$CONFIGURATION_SUFFIX
}

CONFIG += c++11

QMAKE_CXXFLAGS += -pedantic-errors -ansi -Wextra

CONFIG(gcc5) | clang {
	QMAKE_CXXFLAGS +=-Werror=pedantic -Werror=delete-incomplete
}

QMAKE_CXXFLAGS += -Werror=cast-qual -Werror=write-strings -Werror=redundant-decls -Werror=unreachable-code \
			-Werror=non-virtual-dtor -Wno-error=overloaded-virtual \
			-Werror=uninitialized -Werror=init-self


GLOBAL_PWD = $$PWD

# Useful function to copy additional files to destination,
# from http://stackoverflow.com/questions/3984104/qmake-how-to-copy-a-file-to-the-output
defineTest(isDir) {
	exists($$system_path($$1/*)):return(true)
	return(false)
}

defineTest(copyToDestdir) {
	FILES = $$1
	NOW = $$2

	for(FILE, FILES) {
		DESTDIR_SUFFIX =
		AFTER_SLASH = $$section(FILE, "/", -1, -1)
		isDir($$FILE) {
			ABSOLUTE_PATH = $$absolute_path($$FILE, $$GLOBAL_PWD)
			BASE_NAME = $$section(ABSOLUTE_PATH, "/", -1, -1)
			DESTDIR_SUFFIX = /$$BASE_NAME
			FILE = $$FILE/*
		}

		DDIR = $$system_path($$DESTDIR/$$3$$DESTDIR_SUFFIX)
		FILE = $$system_path($$FILE)

		mkpath($$DDIR)

		win32 {
			# probably, xcopy needs /s and /e for directories
			COPY_DIR = "cmd.exe /C xcopy /f /y /i "
		} else {
		 	COPY_DIR = "rsync -avz "
		}
		COPY_COMMAND = $$COPY_DIR $$quote($$FILE) $$quote($$DDIR/)
		isEmpty(NOW) {
			QMAKE_POST_LINK += $$COPY_COMMAND $$escape_expand(\\n\\t)
		} else {
			system($$COPY_COMMAND)
		}
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

#seems obsolete, atleast for qmake-qt5
unix:equals(ARCHITECTURE, "x86") {
	CONFIG(debug) {
		QMAKE_CXXFLAGS += -fno-omit-frame-pointer
		CONFIG(sanitize_address) {
			QMAKE_CXXFLAGS += -fsanitize=address
			QMAKE_LFLAGS += -fsanitize=address
		}
		CONFIG(sanitize_undefined) {
			# UBSan does not play well with precompiled headers for some reason.
			noPch()
			QMAKE_CXXFLAGS += -fsanitize=undefined
			QMAKE_LFLAGS += -fsanitize=undefined
		}
		CONFIG(sanitize_thread) {
			QMAKE_CXXFLAGS += -fsanitize=thread
			QMAKE_LFLAGS += -fsanitize=thread
			LIBS += -ltsan
		}
	}
}

CONFIG(noPch) {
	noPch()
}
