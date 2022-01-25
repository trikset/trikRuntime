/* Copyright 2014 - 2015 CyberTech Labs Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#include "coreDumping.h"

#include <QtCore/QDir>
#include <QsLog.h>

#include <sys/resource.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <csignal>
#include <unistd.h>
#include <cstdio>
#include "paths.h"

#include <QApplication>
#include <QSocketNotifier>
#include <coreDumping.h>

void (*oldHandler)(int);
static int sigtermFd[2];

static void dumpHandler_impl(int signal, char *p)
{
	static char* path = nullptr;
	if (p) {
		// Special case: set path for future dumps
		path = p;
	} else {
		// dumpHandler called, path should be set in advance
		auto rc = 0;
		if (path) {
			rc = chdir(path);
		} else {
			// path was not set in advance
			perror("path was not set before core dump");
		}
		if (rc) {
			perror("FAILED to chdir");
		}
		oldHandler(signal);
	}
}

static void dumpHandler(int signal)
{
	dumpHandler_impl(signal, nullptr);
}

static void initSignals()
{
	QList<int> const signalsList({SIGQUIT, SIGILL, SIGABRT, SIGFPE, SIGSEGV, SIGBUS, SIGSYS, SIGTRAP, SIGXCPU, SIGXFSZ
			, SIGIOT});

	struct sigaction oldAction {};
	sigaction(SIGSEGV, nullptr, &oldAction);
	oldHandler = oldAction.sa_handler;

	struct sigaction action {};
	action.sa_handler = dumpHandler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;

	for (int signal : signalsList) {
		sigaddset(&action.sa_mask, signal);
		sigaction(signal, &action, nullptr);
	}
}

static void setCoreLimits()
{
	rlimit core_limits {};
	core_limits.rlim_cur = core_limits.rlim_max = RLIM_INFINITY;
	setrlimit(RLIMIT_CORE, &core_limits);
}

static int setup_unix_signal_handlers()
{
	struct sigaction term;

	term.sa_handler = trikKernel::coreDumping::CoreDumpingDaemon::CoreDumpingDaemon::termSignalHandler;
	sigemptyset(&term.sa_mask);
	term.sa_flags = 0;
	term.sa_flags |= SA_RESTART;

	if (sigaction(SIGTERM, &term, 0))
	   return 2;

	return 0;
}

trikKernel::coreDumping::CoreDumpingDaemon::CoreDumpingDaemon(const QString &coreDumpPath, QObject *parent)
	: QObject(parent)
	, mCoreDumpPath(coreDumpPath)
{
}

void trikKernel::coreDumping::CoreDumpingDaemon::init()
{
	setup_unix_signal_handlers();

	if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigtermFd))
		   qFatal("Couldn't create TERM socketpair");
		snTerm = new QSocketNotifier(sigtermFd[1], QSocketNotifier::Read, this);
		connect(snTerm, &QSocketNotifier::activated, this, &CoreDumpingDaemon::handleSigTerm);

		initCoreDumping(mCoreDumpPath);
}

void trikKernel::coreDumping::CoreDumpingDaemon::termSignalHandler(int unused)
{
	Q_UNUSED(unused);
	char a = 1;
	auto rc = ::write(sigtermFd[0], &a, sizeof(a));
	Q_UNUSED(rc);
}

void trikKernel::coreDumping::CoreDumpingDaemon::initCoreDumping(const QString &coreDumpPath)
{
	auto const & p = coreDumpPath.toLocal8Bit();
	dumpHandler_impl(0, strndup(p.constData(), p.size()));
	initSignals();
	setCoreLimits();
}

void trikKernel::coreDumping::CoreDumpingDaemon::handleSigTerm()
{
	snTerm->setEnabled(false);
	char tmp;
	auto rc = ::read(sigtermFd[1], &tmp, sizeof(tmp));
	Q_UNUSED(rc);
	QLOG_INFO() << "GOOOD QUITE";
	QCoreApplication::quit();

	snTerm->setEnabled(true);
}
