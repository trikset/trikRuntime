/* Copyright 2020 CyberTech Labs Ltd., Andrei Khodko
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
#include <functional>

/// Data filter for TRIK sensors
class DataFilter
{
public:
	/// Constructor
	/// @param minValue - minimal possible value returned by sensor
	/// @param maxValue - maximal possible value returned by sensor
	/// @param filterName - name of filter to be applied
	DataFilter(int minValue, int maxValue, const QString &filterName = "");

	/// Applies the specified filter to the data
	int applyFilter(int data);

private:
	int getMedian3(int data);

	std::function<int(int)> mFilterFunction;

	int mReadData1 {-1};
	int mReadData2 {-1};
};
