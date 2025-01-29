/* Copyright 2024, Iakov Kirilenko
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

#include "eventFilter.h"
#include <QDebug>
#include <QtCore/private/qobject_p.h>
#include <QMetaMethod>

bool tests::utils::EventFilter::eventFilter(QObject *o, QEvent *e) {
	QMessageLogger log;
	auto print = log.debug();
	print << o << e;
	switch(e->type()) {
	default:break;
	case QEvent::Type::MetaCall:
		QMetaCallEvent * mev = static_cast<QMetaCallEvent*>(e);
		QMetaMethod slot = o->metaObject()->method(mev->id());
		print << slot.methodSignature() << slot.methodType() << slot.name() << slot.typeName();
		break;
	}


	return false;
}
