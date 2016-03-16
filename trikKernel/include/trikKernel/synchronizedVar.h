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
#include <QtCore/QReadWriteLock>

#include "commandLineParser.h"

namespace trikKernel {

/// Helper template for syncing reader and writer. Compound value can be written to buffer using
/// "->" operator, then, when "sync" is called, it is swapped with current value, which can be accessed using "get".
/// For example,
/// SynchronizedVar<Point> var;
/// var->x = 10;
/// var.sync();
/// var->x = 20;
/// EXPECT_EQ(10, var.get().x);
/// var.sync();
/// EXPECT_EQ(20, var.get().x);
template<typename T> class SynchronizedVar
{
public:
	/// Constructor. Creates var with default buffer and value.
	SynchronizedVar()
		: mValue(new T())
		, mBuffer(new T())
	{
	}

	/// Returns synced value. May be called from reader thread.
	inline T get()
	{
		mLock.lockForRead();
		T temp = *mValue;
		mLock.unlock();
		return temp;
	}

	/// Returns pointer to unsynced buffer.
	/// Shall be called only from writer thread.
	inline T *operator->()
	{
		return mBuffer.data();
	}

	/// Returns contents of unsynced buffer.
	/// Shall be called only from writer thread.
	inline const T &operator*() const
	{
		return *mBuffer;
	}

	/// Copies value from buffer to synced value, invalidates buffer.
	/// Shall be called from writer thread.
	inline void sync()
	{
		mLock.lockForWrite();
		mValue.swap(mBuffer);
		mLock.unlock();
	}

	/// Resets buffer to initial state.
	/// Shall be called from writer thread.
	inline void reset()
	{
		mLock.lockForWrite();
		mValue.reset(new T());
		mBuffer.reset(new T());
		mLock.unlock();
	}

private:
	/// Synced value.
	QScopedPointer<T> mValue;

	/// Unsynced buffer.
	QScopedPointer<T> mBuffer;

	/// Read-write lock that protects the value.
	QReadWriteLock mLock;
};

}
