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

#include "deviceState.h"

#include "exceptions/incorrectStateChangeException.h"

using namespace trikControl;

DeviceState::DeviceState(const QString &deviceName)
	: mDeviceName(deviceName)
{
}

DeviceInterface::Status DeviceState::status() const
{
	// Operation is atomic so it does not require locking.
	return mStatus;
}

bool DeviceState::isReady() const
{
	// Read operation is atomic here, so it does not require locking.
	return mStatus == DeviceInterface::Status::ready;
}

bool DeviceState::isFailed() const
{
	// Read operation is atomic here, so it does not require locking.
	return mStatus == DeviceInterface::Status::permanentFailure;
}

void DeviceState::fail()
{
	mLock.lockForWrite();
	mStatus = DeviceInterface::Status::permanentFailure;
	mLock.unlock();
}

void DeviceState::start()
{
	mLock.lockForWrite();
	if (mStatus == DeviceInterface::Status::permanentFailure) {
		mLock.unlock();
		return;
	}

	if (mStatus == DeviceInterface::Status::off) {
		mStatus = DeviceInterface::Status::starting;
	} else {
		mLock.unlock();
		throw IncorrectStateChangeException(mDeviceName, mStatus, DeviceInterface::Status::starting);
	}

	mLock.unlock();
}

void DeviceState::ready()
{
	mLock.lockForWrite();
	if (mStatus == DeviceInterface::Status::permanentFailure) {
		mLock.unlock();
		return;
	}

	if (mStatus == DeviceInterface::Status::off || mStatus == DeviceInterface::Status::starting) {
		mStatus = DeviceInterface::Status::ready;
	} else {
		mLock.unlock();
		throw IncorrectStateChangeException(mDeviceName, mStatus, DeviceInterface::Status::ready);
	}

	mLock.unlock();
}

void DeviceState::stop()
{
	mLock.lockForWrite();
	if (mStatus == DeviceInterface::Status::permanentFailure) {
		mLock.unlock();
		return;
	}

	if (mStatus == DeviceInterface::Status::ready) {
		mStatus = DeviceInterface::Status::stopping;
	} else {
		mLock.unlock();
		throw IncorrectStateChangeException(mDeviceName, mStatus, DeviceInterface::Status::stopping);
	}

	mLock.unlock();
}

void DeviceState::off()
{
	mLock.lockForWrite();
	if (mStatus == DeviceInterface::Status::permanentFailure) {
		mLock.unlock();
		return;
	}

	if (mStatus == DeviceInterface::Status::ready || mStatus == DeviceInterface::Status::stopping) {
		mStatus = DeviceInterface::Status::off;
	} else {
		mLock.unlock();
		throw IncorrectStateChangeException(mDeviceName, mStatus, DeviceInterface::Status::off);
	}

	mLock.unlock();
}

void DeviceState::resetFailure()
{
	mLock.lockForWrite();

	if (mStatus == DeviceInterface::Status::permanentFailure) {
		mStatus = DeviceInterface::Status::off;
	} else {
		mLock.unlock();
		throw IncorrectStateChangeException(mDeviceName, mStatus);
	}

	mLock.unlock();
}

QString DeviceState::deviceName() const
{
	return mDeviceName;
}
