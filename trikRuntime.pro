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