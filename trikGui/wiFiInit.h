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

#include <QtCore/qglobal.h>

#include <QtCore/QEventLoop>
#include <QtCore/QProcess>

#include "wiFiMode.h"

namespace trikGui {

/// Widget for initializing wi-fi on the controller.
class WiFiInit : public QObject
{
	Q_OBJECT

public:
	/// Constructor
	/// @param parent - parent of this widget in Qt object hierarchy.
	explicit WiFiInit(QObject *parent = nullptr);

	~WiFiInit() override;

	enum class Result {
		success   // script exited 0 — mode switch complete
		, cancelled // user cancelled, sigterm_handler restored previous mode (exit 2)
		, fail    // script error (exit 1 or crash)
	};

	Q_PROPERTY(bool restoring READ isRestoring NOTIFY restoringChanged)
	bool isRestoring() const { return mRestoring; }

	/// Initialize wi-fi on the controller.
	/// @param mode - wi-fi mode which we want to initialize.
	Result init(WiFiMode::Mode mode);

	Q_INVOKABLE void exit();

	Q_INVOKABLE void setQmlParent(QObject *parent);

Q_SIGNALS:
	void restoringChanged();

private:
	bool mRestoring = false;

	QEventLoop mEventLoop;
	QProcess mProcess;

private Q_SLOTS:
	void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
	void onProcessError(QProcess::ProcessError error);
};

}
