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

#pragma once

#include <QtCore/QSharedPointer>

namespace trikKernel {

/// Smart pointer that can hold two different kinds of pointers: where ownership belongs to someone else and
/// where ownership belongs to this object (some mix of shared pointer and weak pointer).
/// Different constructors provide different behavior.
template<typename T> class DifferentOwnerPointer
{
public:
	~DifferentOwnerPointer() = default;

	/// Copy constructor. Preserves ownership semantics of a copied object.
	DifferentOwnerPointer(const DifferentOwnerPointer<T> &other)
		: mPointer(other.mPointer)
		, mPointerGuard(other.mPointerGuard)
	{
	}

	/// Constructor which takes a reference to object owned by someone else, so we need only to store the reference
	/// on it.
	explicit DifferentOwnerPointer(T &foreignObject)
		: mPointer(&foreignObject)
	{
	}

	/// Constructor which takes shared pointer to object and makes DifferentOwnerPointer behave like shared pointer
	/// (it is preferred way to pass ownership from object to object to prevent memory leaks if exception will be
	/// thrown somewhere before ownership is passed).
	explicit DifferentOwnerPointer(const QSharedPointer<T> &sharedObject)
		: mPointer(sharedObject.data())
		, mPointerGuard(sharedObject)
	{
	}

	/// Constructor which takes raw pointer and takes ownership over it.
	explicit DifferentOwnerPointer(T * const ownObject)
		: mPointer(ownObject)
		, mPointerGuard(ownObject)
	{
	}

	/// Reset/detach (as in the shared_pointer) if we have (shared) ownership.
	inline void reset() {
		mPointerGuard.reset();
		mPointer = nullptr;
	}

	/// Operator that allows to access members of stored object.
	inline T *operator->()
	{
		return mPointer;
	}

	/// Provides reference to stored object.
	inline T &operator *()
	{
		return *mPointer;
	}

	/// Returns raw pointer to an object, does not transfer ownership.
	inline T *data() const
	{
		return mPointer;
	}

	/// Assignment operator to provide value semantics.
	inline DifferentOwnerPointer<T> &operator =(const DifferentOwnerPointer<T> &other) = default;

private:
	/// Does not directly have ownership, owned by mPointerGuard when needed.
	T *mPointer {};

	QSharedPointer<T> mPointerGuard;
};

/// Helper function that creates DifferentOwnerPointer that does not own object.
template<typename T>
inline DifferentOwnerPointer<T> createDifferentOwnerPointer(T &foreignObject)
{
	return DifferentOwnerPointer<T>(foreignObject);
}

/// Helper function that creates DifferentOwnerPointer that shares object.
template<typename T>
inline DifferentOwnerPointer<T> createDifferentOwnerPointer(const QSharedPointer<T> &sharedObject)
{
	return DifferentOwnerPointer<T>(sharedObject);
}

/// Helper function that creates DifferentOwnerPointer that owns object.
template<typename T>
inline DifferentOwnerPointer<T> createDifferentOwnerPointer(T * const ownObject)
{
	return DifferentOwnerPointer<T>(ownObject);
}

}
