#pragma once

#include <QtScript/QScriptEngine>

namespace trikScriptRunner {

/// Contains common useful methods for QtScript usage.
class Utils
{
public:
	/// Creates new instance of script value copying there all insufficient data from @arg prototype.
	/// @param prototype - object to copy.
	/// @paragraph engine - an engine that should own new object.
	static QScriptValue clone(QScriptValue const &prototype, QScriptEngine * const engine);

	/// Copies insufficient data into @arg target from @arg prototype.
	/// @param prototype - copy source.
	/// @param target - copy target.
	/// @paragraph engine - an engine that should own new objects.
	static void copyRecursivelyTo(QScriptValue const &prototype, QScriptValue &target, QScriptEngine *engine);

	/// Returns true if a given script value is an object and if it has a property with a given name.
	static bool hasProperty(QScriptValue const &object, QString const &property);
};

}
