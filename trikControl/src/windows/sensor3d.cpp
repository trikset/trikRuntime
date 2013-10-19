/* Copyright 2013 Matvey Bryksin, Yurii Litvinov
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

#include "sensor3d.h"

using namespace trikControl;

Sensor3d::Sensor3d(int min, int max, QString const &controlFile)
{
	Q_UNUSED(min);
	Q_UNUSED(max);
	Q_UNUSED(controlFile);
}

QVector<int> Sensor3d::read()
{
	return QVector<int>(3);
}

void Sensor3d::readFile()
{
}
