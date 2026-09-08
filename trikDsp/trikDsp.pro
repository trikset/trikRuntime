# Copyright 2026 CyberTech Labs Ltd.
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

TEMPLATE = lib
TARGET = trikDsp

include(../global.pri)

DEFINES += TRIKDSP_LIBRARY

INCLUDEPATH += \
	$$PWD/trik-media-sensors/shared/include

!trik_not_brick {
	INCLUDEPATH += \
		$$PWD/trik-media-sensors/ipc-libs/include \
		$$PWD/trik-media-sensors/ipc-libs/specific-trik-include

	LIBS += \
		-L$$PWD/trik-media-sensors/ipc-libs/lib \
		-ltiipc -ltiipcutils -ltitransportrpmsg
}

PUBLIC_HEADERS += \
	$$PWD/include/trikDsp/trikDspDeclSpec.h \
	$$PWD/include/trikDsp/dspTypes.h \
	$$PWD/include/trikDsp/dspServer.h \
	$$PWD/include/trikDsp/dspSource.h

HEADERS += \
	$$PWD/include/trikDsp/trikDspDeclSpec.h \
	$$PWD/include/trikDsp/dspTypes.h \
	$$PWD/include/trikDsp/dspServer.h \
	$$PWD/include/trikDsp/dspSource.h \
	$$PWD/src/dspServer_p.h \
	$$PWD/src/dspConverters.h

!trik_not_brick {
	SOURCES += $$PWD/src/dspServer.cpp
	SOURCES += $$PWD/src/dspServerImpl.cpp
} else {
	DEFINES += TRIK_DSP_STUB
	SOURCES += $$PWD/src/stubs/dspServerStub.cpp
	SOURCES += $$PWD/src/stubs/dspServerStubImpl.cpp
}

QMAKE_CXXFLAGS += \
	-Wno-error=redundant-decls \
	-Wno-error=missing-field-initializers

links(trikRuntimeQsLog trikHal)
interfaceIncludes(trikHal trikKernel)

installs()
