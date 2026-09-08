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

#include <trikControl/utilities.h>

using namespace trikScriptRunner;

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
	QObject::connect(sec, &TrikScriptControlInterface::stopWaiting, &loop, [&loop]() { loop.exit(-1); });
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
		mBrick->startVideoTranslation(QStringLiteral("video1"), params);
		return true;
	}

	if (command.contains(QStringLiteral("mjpg-streamer-ov7670 stop"))) {
		QLOG_INFO() << "Redirecting legacy mjpg-streamer stop to stopVideoTranslation(video1)";
		mBrick->stopVideoTranslation(QStringLiteral("video1"));
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

	if (!synchronously) {
		QStringList args{"-c", command};
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
