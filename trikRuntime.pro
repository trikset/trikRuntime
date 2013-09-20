# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/. */

TEMPLATE = subdirs

SUBDIRS = \
	trikControl \
	trikScriptRunner \
	trikCommunicator \
	trikRun \
	trikServer \
	trikGui \

trikScriptRunner.depends = trikControl
trikCommunicator.depends = trikScriptRunner
trikRun.depends = trikScriptRunner
trikServer.depends = trikCommunicator
trikGui.depends = trikCommunicator trikScriptRunner
