/* Copyright 2026 CyberTech Labs Ltd.
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

#include "portLease.h"

#include <utility>

#include "port.h"

using namespace trikControl;

PortLease::PortLease(Port *port, quint64 token, QString device)
	: mPort(port)
	, mToken(token)
	, mDevice(std::move(device))
{
}

PortLease::PortLease(PortLease &&other) noexcept
	: mPort(other.mPort)
	, mToken(other.mToken)
	, mDevice(std::move(other.mDevice))
{
	other.clear();
}

PortLease &PortLease::operator=(PortLease &&other) noexcept
{
	if (this == &other) {
		return *this;
	}

	release();

	mPort = other.mPort;
	mToken = other.mToken;
	mDevice = std::move(other.mDevice);

	other.clear();

	return *this;
}

PortLease::~PortLease()
{
	release();
}

PortLease::operator bool() const
{
	return mPort != nullptr && mToken != 0;
}

QString PortLease::device() const
{
	return mDevice;
}

void PortLease::release()
{
	if (mPort != nullptr && mToken != 0) {
		mPort->stop(mToken);
	}

	clear();
}

void PortLease::clear()
{
	mPort = nullptr;
	mToken = 0;
	mDevice.clear();
}
