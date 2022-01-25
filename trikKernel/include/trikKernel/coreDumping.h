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

#pragma once

#include <QtCore/QString>

#include <QObject>
#include <QSocketNotifier>

namespace trikKernel {

namespace coreDumping {
	class CoreDumpingDaemon : public QObject
	{
		Q_OBJECT

	  public:
		CoreDumpingDaemon(const QString &coreDumpPath, QObject *parent = 0);
		~CoreDumpingDaemon() = default;


		static void termSignalHandler(int unused);

		/// Installs signal handler and initializes core dump in a location specified by trikKernel::Paths.
		void initCoreDumping(const QString &coreDumpPath);

	  public slots:
		void init();

		void handleSigTerm();

	  private:
//		static int sigtermFd[2];

		QSocketNotifier *snTerm;

		const QString mCoreDumpPath;
	};
}
}
