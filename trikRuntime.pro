# Copyright 2013-2015 Yurii Litvinov, CyberTech Labs Ltd.
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
TARGET = trikRuntime
TEMPLATE = subdirs
SUBDIRS = \
        initvars \
        qslog \
        trikKernel \
        trikWiFi \
        trikNetwork \
        trikHal \
        trikControl \
        trikTelemetry \
        trikCommunicator \
        trikScriptRunner \
        trikGui \
        trikRun \
        trikServer \
        translations \

initvars.file = $$PWD/initvars.pre

tests {
	SUBDIRS *= tests
	tests.depends = trikScriptRunner trikCommunicator trikKernel
}

qslog.file = qslog/QsLogSharedLibrary.pro

qslog.depends = initvars
translations.depends = initvars

trikKernel.depends = qslog translations
trikHal.depends = trikKernel
trikControl.depends = trikHal
trikWiFi.depends = trikKernel
trikNetwork.depends = trikKernel
trikRun.depends = trikScriptRunner
trikScriptRunner.depends = trikControl trikNetwork
trikCommunicator.depends = trikScriptRunner
trikServer.depends = trikCommunicator
trikTelemetry.depends = trikControl trikNetwork
trikGui.depends = trikCommunicator trikScriptRunner trikWiFi trikTelemetry

!trik_nopython {
    SUBDIRS += PythonQt
    trikScriptRunner.depends += PythonQt
    PythonQt.depends = qslog
}

TRANSLATIONS += \
        $$PWD/translations/trikRuntime_ru.ts \
        $$PWD/translations/trikRuntime_fr.ts \
        $$PWD/translations/trikRuntime_de.ts \

OTHER_FILES += \
	$$PWD/resources/changelog.txt \
	$$PWD/resources/lsan.supp \
        $$PWD/docker/Dockerfile \

include($$PWD/global.pri)

copyToDestdir($$PWD/resources/changelog.txt, now)
copyToDestdir($$PWD/resources/lsan.supp, now)

