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

#pragma once

#include <QtCore/QScopedPointer>
#include <QtCore/QSharedPointer>
#include <QtCore/QString>

class QProcess;

namespace trikControl {

/// State of an active video translation (mjpg-streamer) on a port.
struct Translation {
	QString streamerScript;
	bool detached = false;
	bool isUsb = false;

	/// RAII owner of the mjpg_streamer process. QProcess is thread-affine (its
	/// socket notifiers are bound to the creating thread), and start() may run on
	/// the script thread while Brick::stop() runs on the GUI thread. Every QProcess
	/// operation is therefore marshalled to the app's main thread, so create and
	/// terminate always happen on the same thread.
	class StreamerProcess
	{
	public:
		StreamerProcess() = default;
		~StreamerProcess();

		StreamerProcess(const StreamerProcess &) = delete;
		StreamerProcess &operator=(const StreamerProcess &) = delete;

		bool start(const QString &script, const QString &port, const QString &device);
		void stop();

	private:
		QScopedPointer<QProcess> mProcess;
	};

	QSharedPointer<StreamerProcess> streamerProcess; ///< Owns the mjpg_streamer process.
};

} // namespace trikControl
