/* Copyright 2026 CyberTech Labs Ltd.
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

#include "deviceFactory.h"

#include <utility>

using namespace trikControl;

void DeviceFactory::registerDevice(const QString &deviceClass, Creator creator)
{
	mCreators[deviceClass] = std::move(creator);
}

DeviceInterface *DeviceFactory::create(const QString &deviceClass, const QString &port) const
{
	const auto creator = mCreators.constFind(deviceClass);
	if (creator == mCreators.constEnd()) {
		return nullptr;
	}

	return creator.value()(port);
}
