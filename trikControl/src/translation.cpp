/* Copyright 2026 CyberTech Labs Ltd.
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

#include "translation.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QMetaObject>
#include <QtCore/QProcess>
#include <QtCore/QThread>

#include <QsLog.h>

#include <functional>

namespace trikControl {

namespace {

/// QProcess is thread-affine: its internal socket notifiers are bound to the
/// thread that created it. startVideoTranslation() runs on the script thread,
/// while Brick::stop() (which terminates the process) runs on the GUI thread,
/// so a naive stop() from there made QSocketNotifier warn ("cannot be enabled
/// or disabled from another thread"). Run every QProcess operation on the app's
/// main thread, where Brick lives, so create/terminate always match.
void runOnMainThread(const std::function<void()> &fn)
{
	auto *app = QCoreApplication::instance();
	if (!app || QThread::currentThread() == app->thread()) {
		fn();
		return;
	}

	QMetaObject::invokeMethod(app, fn, Qt::BlockingQueuedConnection);
}

} // namespace

Translation::StreamerProcess::~StreamerProcess()
{
	stop();
}

bool Translation::StreamerProcess::start(const QString &script, const QString &port, const QString &device)
{
	bool ok = false;
	runOnMainThread([&]() {
		mProcess.reset(new QProcess());
		mProcess->setProgram(script);
		mProcess->setArguments({QStringLiteral("start"), port, device});
		mProcess->start();
		ok = mProcess->waitForStarted();
	});
	return ok;
}

void Translation::StreamerProcess::stop()
{
	if (!mProcess) {
		return;
	}

	runOnMainThread([this]() {
		if (mProcess->state() != QProcess::NotRunning) {
			// terminate() sends SIGTERM and waitForFinished() blocks on waitpid()
			// until the process actually exits - no busy-waiting. mjpg_streamer's
			// signal handler closes the V4L2 device, so the camera is released only
			// after this returns. Escalate to SIGKILL if it misbehaves.
			mProcess->terminate();
			if (!mProcess->waitForFinished(5000)) {
				QLOG_WARN() << "Brick: mjpg-streamer did not exit after SIGTERM, killing";
				mProcess->kill();
				mProcess->waitForFinished(1000);
			}
		}

		mProcess.reset();
	});
}

} // namespace trikControl
