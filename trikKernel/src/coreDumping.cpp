#include "coreDumping.h"

#include <QtCore/QDir>

#include <sys/resource.h>
#include <signal.h>

void (*oldHandler)(int);
QString dumpDirPath = "/home/root/trik";

void dumpHandler(int signal)
{
	QDir::setCurrent(dumpDirPath);
	oldHandler(signal);
}

void initSignals()
{
	QList<int> const signalsList({ SIGQUIT, SIGILL, SIGABRT, SIGFPE, SIGSEGV, SIGBUS, SIGSYS, SIGTRAP, SIGXCPU, SIGXFSZ
			, SIGIOT});

	struct sigaction oldAction;
	sigaction(SIGSEGV, nullptr, &oldAction);
	oldHandler = oldAction.sa_handler;

	struct sigaction action;
	action.sa_handler = dumpHandler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;

	foreach (int signal, signalsList) {
		sigaddset(&action.sa_mask, signal);
		sigaction(signal, &action, nullptr);
	}
}

void setCoreLimits()
{
	rlimit core_limits;
	core_limits.rlim_cur = core_limits.rlim_max = RLIM_INFINITY;
	setrlimit(RLIMIT_CORE, &core_limits);
}

void trikKernel::coreDumping::initCoreDumping(QString const &dumpDir)
{
	dumpDirPath = dumpDir;
	initSignals();
	setCoreLimits();
}
