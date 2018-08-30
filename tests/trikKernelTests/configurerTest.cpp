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

#include "configurerTest.h"

TEST_F(ConfigurerTest, deviceClassParseTest)
{
	ASSERT_TRUE(mConfigurer->attributeByDevice("servoMotor", "controlMax") == "90");
	ASSERT_TRUE(mConfigurer->attributeByPort("S1", "period") == "20000000");
}

TEST_F(ConfigurerTest, deviceTypeParseTest)
{
	ASSERT_TRUE(mConfigurer->attributeByPort("S1", "min") == "700000");
}

TEST_F(ConfigurerTest, changeAttributeTest)
{
	ASSERT_TRUE(mConfigurer->attributeByPort("S1", "min") == "700000");
	mConfigurer->changeAttributeByPort("S1", "min", "7");
	ASSERT_TRUE(mConfigurer->attributeByPort("S1", "min") == "7");
}
