#pragma once

#include <gtest/gtest.h>
#include <trikKernel/configurer.h>

using namespace trikKernel;

/// Test fixture for Configurer class.
class ConfigurerTest : public testing::Test
{
protected:
	void SetUp()
	{
		mConfigurer = new Configurer(QString("./test-system-config.xml"), QString("./test-model-config.xml"));
	}

	void TearDown()
	{
		delete mConfigurer;
	}

	Configurer *mConfigurer;
};


