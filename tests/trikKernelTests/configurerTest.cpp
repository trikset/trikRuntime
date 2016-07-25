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
