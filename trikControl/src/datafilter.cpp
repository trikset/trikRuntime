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

#include "datafilter.h"
#include <utility>

DataFilter::DataFilter(int minValue, int maxValue, const QString &filterName)
	: mReadData1(minValue)
	, mReadData2(maxValue)
{
	if (filterName == "median3") {
		mFilterFunction = [this](int data){return getMedian3(data);};
	}
	else {
		mFilterFunction = [](int data){return data;};
	}
}

int DataFilter::applyFilter(int data)
{
	return mFilterFunction(data);
}

int DataFilter::getMedian3(int c)
{
	int a = mReadData1;
	int b = mReadData2;
	mReadData1 = b;
	mReadData2 = c;

	if (a > b)
		std::swap(a, b);
	if (b > c)
		std::swap(b, c);
	if (a > b)
		std::swap(a, b);

	return b;
}
