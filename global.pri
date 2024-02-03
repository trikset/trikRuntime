# Copyright 2014 - 2018 CyberTech Co. Ltd., Yurii Litvinov, Iakov Kirilenko
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

!isEmpty(_PRO_FILE_):!isEmpty(CONFIG):isEmpty(RUNTIME_GLOBAL_PRI_INCLUDED){
# commented out, seems like there is no need for this var, but let's wait
RUNTIME_GLOBAL_PRI_INCLUDED=1

COMPILER = $$(CXX)
!isEmpty(QMAKE_GCC_MAJOR_VERSION) {
	COMPILER_VERSION=$${QMAKE_GCC_MAJOR_VERSION}.$${QMAKE_GCC_MINOR_VERSION}
}

COMPILER_IS_ARM = $$find(COMPILER, arm-.*)

PYTHONQTALL_CONFIG=PythonQtCore

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

CONFIG *= qt

CONFIG -= app_bundle

!win32:CONFIG *= use_gold_linker
#CONFIG *= fat-lto

#deal with mixed configurations
CONFIG -= debug_and_release debug_and_release_target
CONFIG(debug, debug | release): CONFIG -= release
else:!debug:CONFIG *= release
release:CONFIG -= debug
no-sanitizers: CONFIG *= nosanitizers
CONFIG = $$unique(CONFIG)

QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO *= -Og
QMAKE_CXXFLAGS_DEBUG *= -Og

CONFIG(debug) {
	isEmpty(CONFIGURATION): CONFIGURATION = $$ARCHITECTURE-debug
	unix {
		QMAKE_CXXFLAGS += -coverage
		QMAKE_LFLAGS += -coverage
	}
	CONFIGURATION_SUFFIX=
} else {
	isEmpty(CONFIGURATION): CONFIGURATION = $$ARCHITECTURE-release
	CONFIGURATION_SUFFIX=
}

!gcc4:!gcc5:!clang:!win32:gcc:*-g++*:system($$QMAKE_CXX --version | grep -qEe '"\\<5\\.[0-9]+\\."' ){ CONFIG += gcc5 }
!gcc4:!gcc5:!clang:!win32:gcc:*-g++*:system($$QMAKE_CXX --version | grep -qEe '"\\<4\\.[0-9]+\\."' ){ CONFIG += gcc4 }

GLOBAL_PWD = $$absolute_path($$PWD)
GLOBAL_OUTPWD = $$absolute_path($$OUT_PWD)

isEmpty(GLOBAL_DESTDIR) {
	GLOBAL_DESTDIR = $$GLOBAL_OUTPWD/bin/$$CONFIGURATION
}
isEmpty(DESTDIR) {
	DESTDIR = $$GLOBAL_DESTDIR
}

PROJECT_BASENAME = $$basename(_PRO_FILE_)
PROJECT_NAME = $$section(PROJECT_BASENAME, ".", 0, 0)

isEmpty(TARGET) {
	TARGET = $$PROJECT_NAME$$CONFIGURATION_SUFFIX
} else {
	R=$$find( TARGET, "$$CONFIGURATION_SUFFIX$" )
	isEmpty(R):TARGET = $$TARGET$$CONFIGURATION_SUFFIX
}

!no_rpath {
	unix:!macx {
		QMAKE_LFLAGS += -Wl,-rpath-link,$$GLOBAL_DESTDIR
		QMAKE_LFLAGS += -Wl,-O1,-rpath,\'\$$ORIGIN\'
	} macx {
		QMAKE_LFLAGS += -rpath @executable_path
		QMAKE_LFLAGS += -rpath @executable_path/../Lib
		QMAKE_LFLAGS += -rpath @executable_path/../Frameworks
		QMAKE_LFLAGS += -rpath @executable_path/../../../
	}
}

equals(TEMPLATE, lib){
	CONFIG += create_pc create_libtool
	QMAKE_PKGCONFIG_NAME=$$TARGET
	QMAKE_PKGCONFIG_PREFIX = $$INSTALLBASE
	QMAKE_PKGCONFIG_LIBDIR = $$target.path
	QMAKE_PKGCONFIG_INCDIR = $$headers.path
	QMAKE_PKGCONFIG_VERSION = $$VERSION
#	QMAKE_PKGCONFIG_DESCRIPTION =
}

#Workaround for a known gcc/ld (before 7.3/bionic) issue
#use_gold_linker:!clang: QMAKE_LFLAGS += -Wl,--disable-new-dtags

macx-clang {
#	QMAKE_MACOSX_DEPLOYMENT_TARGET=10.12
	QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/
}

!nosanitizers:!clang:gcc:*-g++*:gcc4{
	warning("Disabled sanitizers, failed to detect compiler version or too old compiler: $$QMAKE_CXX")
	CONFIG += nosanitizers
}

!nosanitizers {
	CONFIG += sanitizer
}

unix:!nosanitizers {

	# seems like we want USan always, but are afraid of ....
	!CONFIG(sanitize_address):!CONFIG(sanitize_thread):!CONFIG(sanitize_memory):!CONFIG(sanitize_kernel_address) {
		# Ubsan is turned on by default
		#CONFIG += sanitizer sanitize_undefined
		#QMAKE_SANITIZE_UNDEFINED_CXXFLAGS += -fsanitize-trap=undefined -fsanitize-undefined-trap-on-error
	}

	CONFIG(sanitize_address) {
	# GCC 5.5 does not know this
	#	QMAKE_SANITIZE_ADDRESS_CXXFLAGS += -fsanitize-address-use-after-scope
		#!clang:QMAKE_LFLAGS_RELEASE *= -static-libasan
	}
	#LSan can be used without performance degrade even in release build
	#But at the moment we can not, because of Qt  problems
        CONFIG(debug):!CONFIG(sanitize_address):!CONFIG(sanitize_memory):!CONFIG(sanitize_thread):!macx-clang {
             CONFIG += sanitize_leak
        }

        sanitize_leak {
		QMAKE_CFLAGS *= -fsanitize=leak
		QMAKE_CXXFLAGS *= -fsanitize=leak
		QMAKE_LFLAGS *= -fsanitize=leak
		#!clang:QMAKE_LFLAGS_RELEASE *= -static-liblsan
	}

        sanitize_memory {
                QMAKE_CFLAGS *= -fsanitize-memory-use-after-dtor -fsanitize-memory-track-origins
                QMAKE_CXXFLAGS *= -fsanitize-memory-use-after-dtor -fsanitize-memory-track-origins
        }

	sanitize_undefined {
		macx-clang {
		# sometimes runtime is missing in clang. this hack allows to avoid runtime dependency.
		#QMAKE_SANITIZE_UNDEFINED_CFLAGS += -fsanitize-trap=undefined
		#QMAKE_SANITIZE_UNDEFINED_CXXFLAGS += -fsanitize-trap=undefined
		#QMAKE_SANITIZE_UNDEFINED_LFLAGS += -fsanitize-trap=undefined
		}
		#!clang:QMAKE_LFLAGS_RELEASE *= -static-libubsan
	}

	sanitize_thread {
		#!clang:QMAKE_LFLAGS_RELEASE *= -static-libtsan
	}

	gcc5 {
		CONFIG(sanitize_undefined){
		# Ubsan has (had at least) known issues with false errors about calls of methods of the base class.
		# That must be disabled. Variables for confguring ubsan are taken from here:
		# https://codereview.qt-project.org/#/c/43420/17/mkspecs/common/sanitize.conf
		# They can change in some version of Qt, keep track of it.
		# By the way, simply setting QMAKE_CFLAGS, QMAKE_CXXFLAGS and QMAKE_LFLAGS instead of those used below
		# will not work due to arguments order ("-fsanitize=undefined" must be declared before "-fno-sanitize=vptr").

# Useless since 2019? Commented out.
#			QMAKE_SANITIZE_UNDEFINED_CFLAGS += -fno-sanitize=vptr
#			QMAKE_SANITIZE_UNDEFINED_CXXFLAGS += -fno-sanitize=vptr
#			QMAKE_SANITIZE_UNDEFINED_LFLAGS += -fno-sanitize=vptr
		}
	}

	QMAKE_CFLAGS += -fno-sanitize-recover=all
	QMAKE_CXXFLAGS += -fno-sanitize-recover=all
}

OBJECTS_DIR = .build/$$CONFIGURATION/obj
MOC_DIR = .build/$$CONFIGURATION/moc
RCC_DIR = .build/$$CONFIGURATION/rcc
UI_DIR = .build/$$CONFIGURATION/ui

CONFIG += precompile_header
precompile_header {
	PRECOMPILED_HEADER = $$PWD/pch.h
	QMAKE_CXXFLAGS *= -Wno-error=invalid-pch
}

INCLUDEPATH += $$_PRO_FILE_PWD_ \
	$$_PRO_FILE_PWD_/include \
	$$_PRO_FILE_PWD_/include/$$PROJECT_NAME \


THIS_IS_QS_LOG=$$find(PROJECT_NAME, [qQ]s[lL]og)
isEmpty(THIS_IS_QS_LOG) {
        INCLUDEPATH += $$GLOBAL_PWD/qslog/qslog
}

CONFIG += c++14

DEFINES += QT_FORCE_ASSERTS

QMAKE_CXXFLAGS += -pedantic-errors -Wextra -Werror

!clang: QMAKE_CXXFLAGS += -ansi

gcc5 | clang {
	QMAKE_CXXFLAGS +=-Werror=pedantic -Werror=delete-incomplete
}

clang {
	#treat git submodules as system path
	SYSTEM_INCLUDE_PREFIX_OPTION += $$system(git submodule status 2>/dev/null | sed $$shell_quote('s/^.[0-9a-fA-F]* \\([^ ]*\\).*$/--system-header-prefix=\\1/g'))

	#treat Qt includes as system headers
	SYSTEM_INCLUDE_PREFIX_OPTION += --system-header-prefix=$$[QT_INSTALL_HEADERS]
}

gcc {
	#treat Qt includes as system headers
	#but -isystem causes problems in OE thud
	!count(COMPILER_IS_ARM, 1):SYSTEM_INCLUDE_PREFIX_OPTION *= -isystem $$[QT_INSTALL_HEADERS]
}

QMAKE_CXXFLAGS += $$SYSTEM_INCLUDE_PREFIX_OPTION

gcc:versionAtLeast(QT_VERSION, 5.14.2):QMAKE_CXXFLAGS *= -Wno-error=deprecated-declarations

false:clang {
# Problem from Qt system headers
	QMAKE_CXXFLAGS += -Wno-error=expansion-to-defined -Wno-error=c++98-compat -Wno-error=sign-conversion \
			-Wno-error=covered-switch-default -Wno-error=c++98-compat-pedantic \
			-Wno-error=documentation-unknown-command -Wno-error=inconsistent-missing-destructor-override \
			-Wno-error=used-but-marked-unused -Wno-error=extra-semi-stmt -Wno-error=padded \
			-Wno-error=float-equal -Wno-error=float-conversion -Wno-error=implicit-float-conversion \
			-Wno-error=redundant-parens -Wno-error=deprecated -Wno-error=shift-sign-overflow \
			-Wno-error=zero-as-null-pointer-constant -Wno-error=exit-time-destructors \
			-Wno-error=double-promotion -Wno-error=shadow-field -Wno-error=documentation \
			-Wno-error=reserved-id-macro -Wno-error=extra-semi -Wno-error=comma
}


QMAKE_CXXFLAGS += -Werror=cast-qual -Werror=write-strings -Werror=redundant-decls -Werror=unreachable-code \
			-Werror=non-virtual-dtor -Wno-error=overloaded-virtual \
			-Werror=uninitialized -Werror=init-self

gcc4:QMAKE_CXXFLAGS += -Wno-error=missing-field-initializers

# Simple function that checks if given argument is a file or directory.
# Returns false if argument 1 is a file or does not exist.
defineTest(isDir) {
	exists($$system_path($$1/*)):return(true)
	return(false)
}

# Useful function to copy additional files to destination,
# from http://stackoverflow.com/questions/3984104/qmake-how-to-copy-a-file-to-the-output
defineTest(copyToDestdir) {
	FILES = $$1
	NOW = $$2

	for(FILE, FILES) {
		DESTDIR_SUFFIX =
		AFTER_SLASH = $$section(FILE, "/", -1, -1)
		isDir($$FILE) {
			ABSOLUTE_PATH = $$absolute_path($$FILE, $$GLOBAL_PWD)
			BASE_NAME = $$section(ABSOLUTE_PATH, "/", -1, -1)
			DESTDIR_SUFFIX = $$BASE_NAME/
			FILE = $$FILE/*
		}

		DDIR = $$quote($$system_path($$DESTDIR/$$3$$DESTDIR_SUFFIX))
		FILE = $$quote($$system_path($$FILE))

		mkpath($$DDIR)

		win32 {
			# probably, xcopy needs /s and /e for directories
			COPY_DIR = "cmd.exe /C xcopy /y /i /s /e "
			!silent: COPY_DIR += /f
		} else {
			COPY_DIR = rsync -a
			!silent: COPY_DIR += -v
		}

		COPY_COMMAND = $$COPY_DIR $$FILE $$DDIR
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
		INCLUDEPATH *= $$GLOBAL_PWD/$$PROJECT/include
	}

	export(INCLUDEPATH)
}

defineTest(implementationIncludes) {
	interfaceIncludes($$1)
}

defineTest(transitiveIncludes) {
	interfaceIncludes($$1)
}

defineTest(PythonQtIncludes) {
	INCLUDEPATH += $$GLOBAL_PWD/PythonQt/PythonQt/src
	export(INCLUDEPATH)
}

defineTest(links) {
	LIBS *= -L$$GLOBAL_DESTDIR
	LIBS *= -L$$DESTDIR
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

		INSTALLS *= additionalSharedFiles

		export(additionalSharedFiles.path)
		export(additionalSharedFiles.files)
		export(INSTALLS)
	}
}

defineTest(noPch) {
	CONFIG -= precompile_header
	PRECOMPILED_HEADER =
	export(CONFIG)
	export(PRECOMPILED_HEADER)
}

defineTest(enableFlagIfCan) {
  system(bash -c $$system_quote(echo $$shell_quote(int main(){return 0;}) | $$QMAKE_CXX $$QMAKE_CXXFLAGS $$1 -x c++ -c - -o $$system(bash -c mktemp) 2>/dev/null) ) {
	QMAKE_CXXFLAGS += $$1
	export(QMAKE_CXXFLAGS)
  } else {
	message(Cannot enable $$1)
  }
}


CONFIG(noPch) {
	noPch()
}
} # GLOBAL_PRI_INCLUDED
