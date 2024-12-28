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
#pragma once
#include <QObject>
#include "testUtilsDeclSpec.h"

namespace tests {
namespace utils {

/// Event filter for debug purposes
class TESTUTILS_EXPORT EventFilter: public QObject
{
	Q_OBJECT
public:
	/// ctor
	explicit EventFilter(QObject *parent = nullptr):QObject(parent) {}

	/// default method to filter events
	bool eventFilter(QObject *o, QEvent *e) override;
};

}
}
