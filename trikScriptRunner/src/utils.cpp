#include "utils.h"

#include <QtCore/QDateTime>
#include <QtCore/QRegExp>
#include <QtScript/QScriptValueIterator>

using namespace trikScriptRunner;

QScriptValue Utils::clone(QScriptValue const &prototype, QScriptEngine * const engine)
{
	QScriptValue copy;
	if (prototype.isObject()) {
		copy = engine->newObject();
		copy.setData(prototype.data());
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
	} else if (prototype.isFunction()) {
		copy = prototype;
	} else {
		copy = prototype;
	}

	copyRecursivelyTo(prototype, copy, engine);
	return copy;
}

bool Utils::hasProperty(QScriptValue const &object, QString const &property)
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

void Utils::copyRecursivelyTo(QScriptValue const &prototype, QScriptValue &target, QScriptEngine *engine)
{
	QScriptValueIterator iterator(prototype);
	while (iterator.hasNext()) {
		iterator.next();
		if (!hasProperty(target, iterator.name())) {
			QScriptValue const value = clone(iterator.value(), engine);
			if (value.engine() == target.engine()) {
				target.setProperty(iterator.name(), value);
			}
		}
	}
}
