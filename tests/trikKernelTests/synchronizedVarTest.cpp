/* Copyright 2015 CyberTech Labs Ltd.
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

#include "synchronizedVarTest.h"

#include <trikKernel/synchronizedVar.h>

#include <QtCore/QList>

using namespace tests;
using namespace trikKernel;

struct Point {
	int x;
	int y;
};

TEST_F(SynchronizedVarTest, structTest)
{
	SynchronizedVar<Point> var;
	var->x = 10;
	var->y = 10;
	var.sync();
	var->x = 20;
	var->y = 20;
	EXPECT_EQ(10, var.get().x);
	var.sync();
	EXPECT_EQ(20, var.get().x);
	var->x = 30;
	var->y = 30;
	EXPECT_EQ(20, var.get().x);
}

TEST_F(SynchronizedVarTest, listTest)
{
	SynchronizedVar<QList<Point>> list;
	EXPECT_TRUE(list.get().isEmpty());

	const Point point{10, 10};
	list->append(point);

	EXPECT_TRUE(list.get().isEmpty());

	list.sync();

	ASSERT_FALSE(list.get().isEmpty());
	EXPECT_EQ(10, list.get().at(0).x);
}

TEST_F(SynchronizedVarTest, resetTest)
{
	SynchronizedVar<QList<Point>> list;
	list->append({10, 10});

	list.sync();

	EXPECT_FALSE(list.get().isEmpty());

	list.reset();

	EXPECT_TRUE(list.get().isEmpty());
	list.sync();
	EXPECT_TRUE(list.get().isEmpty());
}
