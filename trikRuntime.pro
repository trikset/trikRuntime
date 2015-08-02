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

TEMPLATE = subdirs

SUBDIRS = \
	qslog \
	trikRuntimeKiller \
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

qslog.file = qslog/QsLogSharedLibrary.pro

trikCommunicator.depends = trikScriptRunner trikNetwork qslog
trikControl.depends = trikKernel trikHal qslog
trikGui.depends = trikCommunicator trikScriptRunner trikWiFi trikKernel trikTelemetry qslog
trikKernel.depends = qslog
trikNetwork.depends = trikKernel qslog
trikRun.depends = trikScriptRunner trikKernel qslog
trikScriptRunner.depends = trikControl trikKernel trikNetwork qslog
trikServer.depends = trikCommunicator qslog
trikTelemetry.depends = trikControl trikNetwork trikKernel qslog
trikWiFi.depends = qslog
trikRuntimeKiller.depends = qslog
trikHal.depends = qslog
