/* Copyright 2016 Yurii Litvinov.
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

#include <trikKernel/differentOwnerPointer.h>

#include <gtest/gtest.h>

using namespace trikKernel;

/// Helper class that provides something to construct and destruct and a means to verify its destruction.
class TestHelper
{
	Q_DISABLE_COPY(TestHelper)
public:
	/// Class that can report that it was destroyed.
	class Destructable
	{
		Q_DISABLE_COPY(Destructable)
	public:
		explicit Destructable(TestHelper &parent) : mParent(parent)
		{
		}

		~Destructable()
		{
			mParent.reportDestroyed();
		}

	private:
		TestHelper &mParent;
	};

	TestHelper() = default;
	~TestHelper()
	{
		if (!mDestroyed)
		{
			delete mDestructable;
		}
	}

	/// Notifies helper that Destructable was indeed destroyed.
	void reportDestroyed()
	{
		mDestroyed = true;
	}

	/// Returns current Destructable status.
	bool isDestroyed() const
	{
		return mDestroyed;
	}

	/// Returns pointer to destructable itself.
	Destructable *destructable() const
	{
		return mDestructable;
	}

private:
	bool mDestroyed = false;
	Destructable *mDestructable = new Destructable(*this);
};

TEST(differentOwnerPointertest, foreignPointerTest)
{
	TestHelper helper;
	DifferentOwnerPointer<TestHelper::Destructable> *ptr
			= new DifferentOwnerPointer<TestHelper::Destructable>(*helper.destructable());

	delete ptr;
	ASSERT_FALSE(helper.isDestroyed());
}

TEST(differentOwnerPointertest, ownPointerTest)
{
	TestHelper helper;
	DifferentOwnerPointer<TestHelper::Destructable> *ptr
			= new DifferentOwnerPointer<TestHelper::Destructable>(helper.destructable());

	delete ptr;
	ASSERT_TRUE(helper.isDestroyed());
}

TEST(differentOwnerPointertest, sharedPointerTest)
{
	TestHelper helper;
	DifferentOwnerPointer<TestHelper::Destructable> *ptr
			= new DifferentOwnerPointer<TestHelper::Destructable>(
					QSharedPointer<TestHelper::Destructable>(helper.destructable())
			);

	delete ptr;
	ASSERT_TRUE(helper.isDestroyed());
}
