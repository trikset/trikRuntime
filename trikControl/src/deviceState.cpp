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

DeviceInterface::Status DeviceState::status() const
{
	return mStatus;
}

bool DeviceState::isReady() const
{
	return mStatus == DeviceInterface::Status::ready;
}

bool DeviceState::isFailed() const
{
	return mStatus == DeviceInterface::Status::failure;
}

void DeviceState::fail()
{
    mStatus = DeviceInterface::Status::failure;
}

void DeviceState::start()
{
    mLock.lockForWrite();
    if (mStatus == DeviceInterface::Status::failure) {
        mLock.unlock();
        return;
    }

    if (mStatus == DeviceInterface::Status::off) {
        mStatus = DeviceInterface::Status::starting;
    } else {
        mLock.unlock();
        throw IncorrectStateChangeException(mStatus, DeviceInterface::Status::starting);
    }

    mLock.unlock();
}

void DeviceState::ready()
{
	mLock.lockForWrite();
	if (mStatus == DeviceInterface::Status::failure) {
		mLock.unlock();
		return;
	}

	if (mStatus == DeviceInterface::Status::off || mStatus == DeviceInterface::Status::starting) {
		mStatus = DeviceInterface::Status::ready;
	} else {
		mLock.unlock();
		throw IncorrectStateChangeException(mStatus, DeviceInterface::Status::ready);
	}

	mLock.unlock();
}

void DeviceState::stop()
{
	mLock.lockForWrite();
	if (mStatus == DeviceInterface::Status::failure) {
		mLock.unlock();
		return;
	}

	if (mStatus == DeviceInterface::Status::ready) {
		mStatus = DeviceInterface::Status::stopping;
	} else {
		mLock.unlock();
		throw IncorrectStateChangeException(mStatus, DeviceInterface::Status::stopping);
	}

	mLock.unlock();
}

void DeviceState::off()
{
	mLock.lockForWrite();
	if (mStatus == DeviceInterface::Status::failure) {
		mLock.unlock();
		return;
	}

	if (mStatus == DeviceInterface::Status::ready || mStatus == DeviceInterface::Status::stopping) {
		mStatus = DeviceInterface::Status::off;
	} else {
		mLock.unlock();
		throw IncorrectStateChangeException(mStatus, DeviceInterface::Status::off);
	}

	mLock.unlock();
}
