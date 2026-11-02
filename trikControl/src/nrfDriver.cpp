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

#include "nrfDriver.h"
#include <QsLog.h>
using namespace trikControl;

NrfDriver::NrfDriver(PeripheryCommunicatorInterface *usb, PeripheryCommunicatorInterface *i2c, QObject *parent)
	: QObject(parent)
	, mUsb(usb)
	, mI2c(i2c)
{
	mFlushTimer.setInterval(kDefaultFlushMs);
	connect(&mFlushTimer, &QTimer::timeout, this, &NrfDriver::flush);
	mFlushTimer.start();
}

NrfDriver::~NrfDriver()
{
	flush();
}

void NrfDriver::send(const CommunicatorCommand &cmd)
{
	if (mUsb) {
		// Immediate (fire-and-forget): flush the queue first, then send right now.
		flush();
		const QByteArray wire = serialize(cmd);
		if (!wire.isEmpty()) {
			mUsb->sendBatch(wire);
		}
		return;
	}

	if (mI2c) {
		sendI2c(cmd);
	}
}

void NrfDriver::sendBatch(const CommunicatorCommand &cmd)
{
	if (mUsb) {
		enqueue(cmd);
	} else if (mI2c) {
		// The I2C path has no batching, send immediately.
		sendI2c(cmd);
	}
}

int NrfDriver::sendI2c(const CommunicatorCommand &cmd)
{
	if (!mI2c) {
		return -1;
	}

	const int idx = portIndex(cmd.portName);
	if (idx < 0) {
		return -1;
	}

	const auto encoded = static_cast<uint8_t>((static_cast<uint8_t>(idx) << 4) | cmd.operation);
	QByteArray data(1 + cmd.data.size(), '\0');
	data[0] = static_cast<char>(encoded);
	for (int i = 0; i < cmd.data.size(); ++i) {
		data[1 + i] = cmd.data[i];
	}

	return mI2c->send(data);
}

void NrfDriver::enqueue(const CommunicatorCommand &cmd)
{
	const QByteArray wire = serialize(cmd);
	if (!wire.isEmpty()) {
		mBatchQueue.append(wire);
	}
}

int NrfDriver::read(const CommunicatorCommand &cmd)
{
	Q_UNUSED(cmd)
	return 0;
}

DeviceInterface::Status NrfDriver::status() const
{
	if (mUsb && mUsb->status() == DeviceInterface::Status::ready) {
		return DeviceInterface::Status::ready;
	}

	if (mI2c && mI2c->status() == DeviceInterface::Status::ready) {
		return DeviceInterface::Status::ready;
	}

	return DeviceInterface::Status::off;
}

int NrfDriver::portIndex(const QString &portName) const
{
	static const QHash<QString, int> portMap = {{"E1", 0}, {"E2", 1}, {"E3", 2}, {"E4", 3}, {"A1", 4}, {"A2", 5},
		{"A3", 6}, {"A4", 7}, {"A5", 8}, {"A6", 9}, {"A7", 10}, {"A8", 11}, {"A9", 12}, {"A10", 13}};

	return portMap.value(portName, -1);
}

QByteArray NrfDriver::serialize(const CommunicatorCommand &cmd) const
{
	const int idx = portIndex(cmd.portName);
	if (idx < 0) {
		return {};
	}

	QByteArray wire(3 + cmd.data.size(), '\0');
	wire[0] = static_cast<char>(cmd.device);
	wire[1] = static_cast<char>(idx);
	wire[2] = static_cast<char>(cmd.operation);
	wire.replace(3, cmd.data.size(), cmd.data);

	return wire;
}

int NrfDriver::readAsync(const QString &port)
{
	const int idx = portIndex(port);
	if (idx < 0 || !mUsb) {
		return 0;
	}

	QByteArray command(2, '\0');
	command[0] = static_cast<char>(idx & 0xFF);
	command[1] = static_cast<char>((idx >> 8) & 0xFF);
	return mUsb->read(command);
}

void NrfDriver::flush()
{
	if (mBatchQueue.isEmpty() || !mUsb) {
		return;
	}

	QByteArray batch;
	int consumed = 0;
	int total = 0;
	const auto maxPayload = mUsb->batchMaxPayload();

	for (const auto &wire : mBatchQueue) {
		if (total + wire.size() > static_cast<int>(maxPayload)) {
			break;
		}

		batch.append(wire);
		total += wire.size();
		++consumed;
	}

	if (consumed == 0) {
		batch = mBatchQueue.front();
		consumed = 1;
	}

	mUsb->sendBatch(batch);
	mBatchQueue.erase(mBatchQueue.begin(), mBatchQueue.begin() + consumed);
}