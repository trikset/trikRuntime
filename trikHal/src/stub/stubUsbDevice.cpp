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

#include "stubUsbDevice.h"

#include <QtCore/QDebug>

using namespace trikHal;
using namespace trikHal::stub;

StubUsbDevice::StubUsbDevice() = default;

bool StubUsbDevice::connect()
{
	qDebug() << "StubUsbDevice: connect";
	mConnected = true;
	return true;
}

void StubUsbDevice::disconnect()
{
	qDebug() << "StubUsbDevice: disconnect";
	mConnected = false;
}

bool StubUsbDevice::isConnected() const
{
	return mConnected;
}

int StubUsbDevice::send(const QByteArray &data)
{
	qDebug() << "StubUsbDevice: send" << data.toHex();
	return 0;
}

void StubUsbDevice::setDataCallback(DataCallback callback, void *userData)
{
	Q_UNUSED(callback)
	Q_UNUSED(userData)
	qDebug() << "StubUsbDevice: setDataCallback";
}

bool StubUsbDevice::controlWrite(uint8_t bRequest, const QByteArray &data, int timeoutMs)
{
	Q_UNUSED(timeoutMs)
	qDebug() << "StubUsbDevice: controlWrite request=0x" << QString::number(bRequest, 16)
		 << "data=" << data.toHex();
	return true;
}

void StubUsbDevice::setTelemetryPeriodUs(uint32_t periodUs, uint32_t isoPacketsPerUrb)
{
	Q_UNUSED(periodUs)
	Q_UNUSED(isoPacketsPerUrb)
	qDebug() << "StubUsbDevice: setTelemetryPeriodUs" << periodUs;
}

int StubUsbDevice::read(const QByteArray &data)
{
	Q_UNUSED(data)
	qDebug() << "StubUsbDevice: read";
	return 0;
}
