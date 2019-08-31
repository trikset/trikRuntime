/* Copyright 2014 Dmitry Mordvinov, CyberTech Labs Ltd.
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

#include <QtScript/QScriptEngine>

namespace trikScriptRunner {

/// Contains common useful methods for QtScript usage.
class Utils
{
public:
	/// Creates new instance of script value copying there all insufficient data from prototype.
	/// @param prototype - object to copy.
	/// @paragraph engine - an engine that should own new object.
	static QScriptValue clone(const QScriptValue &prototype, QScriptEngine * engine);

	/// Copies data into target from prototype.
	/// @param prototype - copy source.
	/// @param target - copy target.
	/// @paragraph engine - an engine that should own new objects.
	static void copyRecursivelyTo(const QScriptValue &prototype, QScriptValue &target, QScriptEngine *engine);

	/// Returns true if a given script value is an object and if it has a property with a given name.
	static bool hasProperty(const QScriptValue &object, const QString &property);
};

}
