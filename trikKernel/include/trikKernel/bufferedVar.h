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

#pragma once

#include <QtCore/QScopedPointer>

#include "commandLineParser.h"

namespace trikKernel {

/// Helper template for syncing reader and writer without monitors. Compound value can be written to buffer using
/// "->" operator, then, when "sync" is called, it is swapped with current value, which can be accessed using "get".
/// For example,
/// BufferedVar<Point> var;
/// var->x = 10;
/// var.sync();
/// var->x = 20;
/// EXPECT_EQ(10, var.get().x);
/// var.sync();
/// EXPECT_EQ(20, var.get().x);
template<typename T> class BufferedVar
{
public:
	/// Constructor. Creates var with default buffer and value.
	BufferedVar()
		: mBuffer(new T())
		, mValue(new T())
	{
	}

	/// Returns current synced value.
	/// May be called from reader thread.
	inline T get()
	{
//		QScopedPointer<T> temp(new T());
//		temp.swap(mValue);
//		return *temp;
		return *mValue;
	}

	/// Returns pointer to unsynced buffer.
	/// Shall be called only from writer thread.
	inline T *operator->()
	{
		return mBuffer.data();
	}

	/// Copies value from buffer to synced value, invalidates buffer.
	/// Shall be called from writer thread.
	inline void sync()
	{
		mValue.swap(mBuffer);
	}

	/// Resets buffer to initial state.
	/// Shall be called from writer thread.
	inline void reset()
	{
		mValue.reset(new T());
		mBuffer.reset(new T());
	}

private:
	/// Current unsynced buffer.
	QScopedPointer<T> mBuffer;

	/// Current synced value.
	QScopedPointer<T> mValue;
};

}
