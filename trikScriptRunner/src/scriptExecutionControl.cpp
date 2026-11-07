/* Copyright 2015 CyberTech Labs Ltd.
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

#include "scriptExecutionControl.h"

#include <QtCore/QDateTime>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QProcess>
#include <QtCore/QFileInfo>
#include <QtCore/QRegularExpression>
#include <QtCore/QStringList>
#include <QtCore/QVariantMap>

#include <QElapsedTimer>
#include <QOperatingSystemVersion>
#include <QRandomGenerator>
#include <QThread>
#include <QsLog.h>

#ifdef Q_OS_LINUX
#include <dirent.h>
#include <csignal>
#include <sys/types.h>
#include <unistd.h>
#endif

#include <cstdlib>

#include <trikControl/utilities.h>

using namespace trikScriptRunner;

#ifdef Q_OS_LINUX
namespace {

// TODO(Qt >= 6.2): remove this native killall workaround together with the
// interception in ScriptExecutionControl::system(). See QTBUG-114863.
void killProcessesByName(const QString &processName)
{
	const auto targetName = processName.toLocal8Bit();
	auto *procDir = ::opendir("/proc");
	if (!procDir) {
		QLOG_ERROR() << "killall: cannot open /proc";
		return;
	}

	const auto selfPid = ::getpid();
	auto killedCount = 0;

	while (auto *entry = ::readdir(procDir)) {
		auto ok = false;
		const auto pid = QString::fromLatin1(entry->d_name).toLong(&ok);
		if (!ok || pid <= 0 || pid == selfPid) {
			continue;
		}

		QFile commFile(QStringLiteral("/proc/%1/comm").arg(pid));
		if (!commFile.open(QIODevice::ReadOnly)) {
			continue;
		}

		if (commFile.readAll().trimmed() == targetName
				&& ::kill(static_cast<pid_t>(pid), SIGTERM) == 0) {
			++killedCount;
		}
	}

	::closedir(procDir);
	QLOG_INFO() << "killall" << processName << ": terminated" << killedCount << "process(es)";
}

} // namespace
#endif

ScriptExecutionControl::ScriptExecutionControl(trikControl::BrickInterface *brick): mBrick(brick) {
	qRegisterMetaType<QVector<int32_t>>("QVector<int32_t>");
}

ScriptExecutionControl::~ScriptExecutionControl()
{
	qDeleteAll(mTimers);
}

void ScriptExecutionControl::reset()
{
	mInEventDrivenMode = false;
	Q_EMIT stopWaiting();
	for (auto &&timer : mTimers) {
		QMetaObject::invokeMethod(timer, &QTimer::stop, Qt::QueuedConnection);
		timer->deleteLater();
	}

	mTimers.clear();
}

QObject* ScriptExecutionControl::timer(int milliseconds)
{
	auto result = new QTimer();
	mTimers.append(result);
	result->start(milliseconds);
	return result;
}

static inline int waitWithTimerType(ScriptExecutionControl *sec, int ms, Qt::TimerType tt) {
	QEventLoop loop;
	QObject::connect(sec, &ScriptExecutionControl::stopWaiting, &loop, [&loop]() { loop.exit(-1); });
	QTimer t;
	t.setTimerType(tt);
	QObject::connect(&t, &QTimer::timeout, &loop, &QEventLoop::quit);
	t.start(ms);
	return loop.exec();
}

void ScriptExecutionControl::wait(const int &milliseconds)
{
	QElapsedTimer elapsed;
	elapsed.start();
	auto precision = 0;

	// Try to send all posted events even if timer in ms is very short
	QCoreApplication::sendPostedEvents();
	auto diff = milliseconds - elapsed.elapsed();
	if (diff <= precision) {
		return;
	}
	constexpr auto preciseTimerDelta = 20;

	if (diff > 100
			&& waitWithTimerType(this, static_cast<int>(std::max(diff - preciseTimerDelta, 0ll)), Qt::TimerType::CoarseTimer)) {
		return;
	}
	diff = milliseconds - elapsed.elapsed();

	// QThread::usleep does not work for Windows, sleeps too long, about 20 ms
	constexpr auto usleepDelta = QOperatingSystemVersion::currentType() != QOperatingSystemVersion::Windows ? 3 : 0;
	constexpr auto spinLockDelta = QOperatingSystemVersion::currentType() != QOperatingSystemVersion::Windows ? 2 : 3;

	static_assert(preciseTimerDelta > usleepDelta, "Use timer for longer sleep");

	if (waitWithTimerType(this, static_cast<int>(std::max(0ll, diff - (usleepDelta+spinLockDelta))), Qt::TimerType::PreciseTimer)) {
			return;
	}

	diff = milliseconds - elapsed.elapsed();
	if (diff <= precision) {
		// This is enough
		return;
	}


	if (diff > usleepDelta && usleepDelta > spinLockDelta) {
		QThread::usleep( (diff - spinLockDelta) * 1000);
	}
	// Ok, spin-lock to wait for a few milliseconds
	while ((milliseconds - elapsed.elapsed()) > precision ) {
		/* do nothing */
	}
}

qint64 ScriptExecutionControl::time() const
{
	return QDateTime::currentMSecsSinceEpoch();
}

int ScriptExecutionControl::random(int from, int to) const
{
	if (from > to) {
		qSwap(from, to);
	}

	return QRandomGenerator::global()->bounded(from, to+1);
}

void ScriptExecutionControl::run()
{
	mInEventDrivenMode = true;
}

bool ScriptExecutionControl::isInEventDrivenMode() const
{
	return mInEventDrivenMode;
}

void ScriptExecutionControl::quit()
{
	Q_EMIT quitSignal();
}

bool ScriptExecutionControl::redirectLegacyMjpgStreaming(const QString &command)
{
	// The old generated code started/stopped video streaming by running the
	// mjpg-encoder-ov7670 / mjpg-streamer-ov7670 init scripts directly:
	//   script.system("/etc/init.d/mjpg-encoder-ov7670 start --jpeg-qual 30 --white-black false"
	//                 " && /etc/init.d/mjpg-streamer-ov7670 start");
	//   script.system("/etc/init.d/mjpg-streamer-ov7670 stop"
	//                 " && /etc/init.d/mjpg-encoder-ov7670 stop");
	// These are intercepted here and redirected to the Brick translation API for
	// the ov7670 port (video1), so old scripts keep working on the new runtime.

	if (command.contains(QStringLiteral("mjpg-encoder-ov7670 start"))) {
		QVariantMap params;
		const static QRegularExpression qualRe(QStringLiteral("--jpeg-qual\\s+(\\d+)"));
		const auto &qualMatch = qualRe.match(command);
		if (qualMatch.hasMatch()) {
			params.insert(QStringLiteral("jpeg-qual"), qualMatch.captured(1).toInt());
		}

		const static QRegularExpression bwRe(QStringLiteral("--white-black\\s+(\\w+)"));
		const auto &bwMatch = bwRe.match(command);
		if (bwMatch.hasMatch()) {
			const auto &value = bwMatch.captured(1);
			params.insert(QStringLiteral("white-black"),
			              value == QStringLiteral("true") || value == QStringLiteral("1"));
		}

		QLOG_INFO() << "Redirecting legacy mjpg-encoder start to startVideoTranslation(video1)";
		mBrick->startVideoTranslation(QStringLiteral("video2"), params);
		return true;
	}

	if (command.contains(QStringLiteral("mjpg-streamer-ov7670 stop"))) {
		QLOG_INFO() << "Redirecting legacy mjpg-streamer stop to stopVideoTranslation(video2)";
		mBrick->stopVideoTranslation(QStringLiteral("video2"));
		return true;
	}

	return false;
}

// NOLINTNEXTLINE(google-default-arguments)
void ScriptExecutionControl::system(const QString &command, bool synchronously)
{
	if (redirectLegacyMjpgStreaming(command)) {
		return;
	}

#ifdef Q_OS_LINUX
	// "killall aplay"/"killall vlc" are sent by the client to stop leftover
	// media players. Forking a helper script for this from the script thread
	// can deadlock in QtCore's post-fork code:
	// https://bugreports.qt.io/browse/QTBUG-114863
	// Fixed in Qt 6.2, where QProcess::startDetached resolves the executable
	// path and prepares argv/envp before fork. Remove this native workaround
	// when the runtime is built against Qt >= 6.2.
	const auto killallPrefix = QStringLiteral("killall ");
	if (command.startsWith(killallPrefix)) {
		const auto processName = command.mid(killallPrefix.size()).trimmed();
		if (!processName.isEmpty()
				&& !processName.startsWith(QLatin1Char('-'))
				&& !processName.contains(QLatin1Char(' '))
				&& !processName.contains(QLatin1Char('\t'))) {
			QLOG_INFO() << "Running natively:" << command;
			killProcessesByName(processName);
			return;
		}
	}
#endif

	if (!synchronously) {
		auto args = QStringList{"-c", command};
		QLOG_INFO() << "Running: " << "sh" << args;
		QProcess::startDetached("sh", args);
	} else {
		QLOG_INFO() << "Running synchronously: " << command;
		auto rc = ::system(command.toStdString().c_str());
		QLOG_INFO() << "System result code: " << rc;
	}
}

void ScriptExecutionControl::writeToFile(const QString &file, const QString &text)
{
	QFile out(file);
	out.open(QIODevice::WriteOnly | QIODevice::Append);
	out.write(text.toUtf8());
}

void ScriptExecutionControl::writeData(const QString &file, const QVector<uint8_t> &bytes)
{
	QFile out(file);
	out.open(QIODevice::WriteOnly | QIODevice::Append);
	out.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
}

QStringList ScriptExecutionControl::readAll(const QString &file) const
{
	QFile in(file);
	if (!in.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QLOG_INFO() << "Trying to read from file" << file << "failed";
		return {};
	}

	QStringList result;

	while (!in.atEnd()) {
		const auto line = in.readLine();
		result << QString::fromUtf8(line);
	}

	return result;
}

void ScriptExecutionControl::removeFile(const QString &file)
{
	QFile out(file);
	out.remove();
}

int ScriptExecutionControl::timeInterval(int packedTimeLeft, int packedTimeRight)
{
	return trikKernel::TimeVal::timeInterval(packedTimeLeft, packedTimeRight);
}

// NOLINTNEXTLINE(google-default-arguments)
QVector<int32_t> ScriptExecutionControl::getPhoto(const QString &port)
{
	return trikControl::Utilities::rescalePhoto(mBrick->getStillImage(port));
}
