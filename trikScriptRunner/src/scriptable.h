/* Copyright 2013 - 2016 Yurii Litvinov and CyberTech Labs Ltd.
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

/// Helper class that registers converters from and to script values for a given script engine.
template<typename T> class Scriptable
{
public:
	Scriptable() = delete;

	/// Registers converters from and to script values for a given script engine.
	static void registerMetatype(QScriptEngine *engine)
	{
		qScriptRegisterMetaType(engine, toScriptValue, fromScriptValue);
	}

private:
	static QScriptValue toScriptValue(QScriptEngine *engine, T * const &in)
	{
		return engine->newQObject(in);
	}

	static void fromScriptValue(const QScriptValue &object, T * &out)
	{
		out = qobject_cast<T *>(object.toQObject());
	}
};

}
