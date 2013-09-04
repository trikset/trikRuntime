#pragma once

#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <QtScript/QScriptEngine>

#include <trikControl/brick.h>

namespace scriptRunner {

/// Executes scripts in Qt Scripting Engine.
class Runner : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	Runner();

	/// Executes given script.
	/// @param script Script in Qt Script language to be executed.
	void run(QString const &script);

	/// Aborts script execution.
	void abort();

private:
	QScriptEngine mEngine;  // Has ownership.
	trikControl::Brick mBrick;
};

}
