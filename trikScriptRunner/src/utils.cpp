/* Copyright 2014 - 2015 Dmitry Mordvinov, CyberTech Labs Ltd.
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

#include "utils.h"

#include <QtCore/QDateTime>
#include <QtCore/QRegExp>
#include <QtScript/QScriptValueIterator>

using namespace trikScriptRunner;

QScriptValue Utils::clone(const QScriptValue &prototype, QScriptEngine * const engine)
{
	QScriptValue copy;
	if (prototype.isFunction()) {
		// Functions can not be copied across script engines, so they actually will not be copied.
		return prototype;
	} else if (prototype.isArray()) {
		copy = engine->newArray();
		copy.setData(prototype.data());
	} else if (prototype.isBool()) {
		copy = QScriptValue(engine, prototype.toBool());
	} else if (prototype.isNumber()) {
		copy = QScriptValue(engine, prototype.toNumber());
	} else if (prototype.isString()) {
		copy = QScriptValue(engine, prototype.toString());
	} else if (prototype.isRegExp()) {
		copy = engine->newRegExp(prototype.toRegExp());
	} else if (prototype.isDate()) {
		copy = engine->newDate(prototype.toDateTime());
	} else if (prototype.isQObject()) {
		copy = engine->newQObject(prototype.toQObject());
	} else if (prototype.isQMetaObject()) {
		copy = engine->newQMetaObject(prototype.toQMetaObject());
	} else if (prototype.isNull()) {
		copy = QScriptValue();
	} else if (prototype.isObject()) {
		if (prototype.toString() == "[object Math]"
				|| prototype.toString() == "[object Object]"
				|| prototype.toString() == "[object JSON]"
				)
		{
			// Do not copy intrinsic objects, we will not be able to copy their functions properly anyway.
			return prototype;
		}

		copy = engine->newObject();
		copy.setData(prototype.data());
	} else {
		copy = prototype;
	}

	copyRecursivelyTo(prototype, copy, engine);
	return copy;
}

bool Utils::hasProperty(const QScriptValue &object, const QString &property)
{
	QScriptValueIterator iterator(object);
	while (iterator.hasNext()) {
		iterator.next();
		if (iterator.name() == property) {
			return true;
		}
	}

	return false;
}

void Utils::copyRecursivelyTo(const QScriptValue &prototype, QScriptValue &target, QScriptEngine *engine)
{
	QScriptValueIterator iterator(prototype);
	while (iterator.hasNext()) {
		iterator.next();
		QScriptValue const value = clone(iterator.value(), engine);
		// Functions will not be copied to a new engine.
		if (value.engine() == engine) {
			target.setProperty(iterator.name(), value);
		}
	}
}
