/* Copyright 2015 Yurii Litvinov and CyberTech Labs Ltd.
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

#include "trikHardwareAbstraction.h"

#include "trikI2c.h"
#include "trikMspUsb.h"
#include "trikSystemConsole.h"
#include "trikEventFile.h"
#include "trikInputDeviceFile.h"
#include "trikOutputDeviceFile.h"
#include "trikFifo.h"
#include "trikIIOFile.h"
#include "trikVideoDevice.h"
#include "QsLog.h"
#include "commonI2c.h"
#include "trikFbOutput.h"
#include <trikKernel/videoUtils.h>

#include <QtCore/QScopedPointer>
#include <unistd.h>

using namespace trikHal;
using namespace trikHal::trik;

namespace {

/// A single (register, value) pair of the ov7670 initialization sequence, taken
/// verbatim from the init-ov7670-320x240.sh script (handle_init_ov7670()).
struct Ov7670Reg {
	uint8_t reg;
	uint8_t value;
};

const Ov7670Reg ov7670RegisterDefaults[] = {
	{0x12, 0x80}, // reset all registers to defaults
	{0x12, 0x00},
	{0x04, 0x40}, // CCIR656 enable
	{0x40, 0x80}, // output range [01]..[FE]
	{0x12, 0x00}, // YUV
	{0x0c, 0x04},
	{0x3e, 0x19},
	{0x70, 0x3A},
	{0x71, 0x35},
	{0x72, 0x11},
	{0x73, 0xf1},
	{0xa2, 0x00},
	{0x3a, 0x08},
	{0x32, 0xb0}, // 0xb0 = 0x80 + (6 << 3)
	{0xb0, 0x84}, // fix green/purple tint
	{0x09, 0x00}, // output drive 1x
	{0x14, 0x1a}, // automatic gain ceiling 4x
	{0x13, 0x87}, // turn on awb/agc/aec
	{0x6f, 0x6f}, // simple AWB
	{0xa5, 0x05}, // agc/aec
	{0x24, 0x95},
	{0x25, 0x33},
	{0x26, 0xe3},
	{0x9f, 0x78},
	{0xa0, 0x68},
	{0xa6, 0xd8},
	{0xa7, 0xd8},
	{0xa9, 0x90},
	{0xaa, 0x94},
	{0x9d, 0x98}, // banding filter
	{0x9e, 0x7f},
	{0xa5, 0x02},
	{0xab, 0x03},
	{0x3b, 0x12},
	{0x41, 0x0a}, // double YUV
	{0x4f, 0x80}, // YUV matrix
	{0x50, 0x70},
	{0x51, 0x1a},
	{0x52, 0x28},
	{0x53, 0x15},
	{0x54, 0x40},
};

} // namespace

TrikHardwareAbstraction::TrikHardwareAbstraction()
	: mI2c(new TrikI2c())
	, mUsb(new TrikMspUsb())
	, mSystemConsole(new TrikSystemConsole())
{
}

TrikHardwareAbstraction::~TrikHardwareAbstraction() = default;

MspI2cInterface &TrikHardwareAbstraction::mspI2c()
{
	return *mI2c.data();
}

MspI2cInterface *TrikHardwareAbstraction::createMspI2c()
{
	return new TrikI2c();
}

MspI2cInterface *TrikHardwareAbstraction::createCommonI2c(uint8_t regSize)
{
	return new CommonI2c(regSize);
}

MspUsbInterface &TrikHardwareAbstraction::mspUsb()
{
	return *mUsb.data();
}

SystemConsoleInterface &TrikHardwareAbstraction::systemConsole()
{
	return *mSystemConsole.data();
}

EventFileInterface *TrikHardwareAbstraction::createEventFile(const QString &fileName) const
{
	return new TrikEventFile(fileName);
}

FifoInterface *TrikHardwareAbstraction::createFifo(const QString &fileName) const
{
	return new TrikFifo(fileName);
}

IIOFileInterface *TrikHardwareAbstraction::createIIOFile(const QString &fileName, const QString &scanType) const
{
	return new TrikIIOFile(fileName, scanType);
}

InputDeviceFileInterface *TrikHardwareAbstraction::createInputDeviceFile(const QString &fileName) const
{
	return new TrikInputDeviceFile(fileName);
}

OutputDeviceFileInterface *TrikHardwareAbstraction::createOutputDeviceFile(const QString &fileName) const
{
	return new TrikOutputDeviceFile(fileName);
}

VideoDeviceFileInterface *TrikHardwareAbstraction::createVideoDeviceFile( // NOLINT(google-default-arguments)
	const QString &devicePath, uint32_t width, uint32_t height, uint32_t fourcc, bool isWebcam) const
{
	return new TrikVideoDevice(devicePath, width, height, fourcc, trikKernel::dspInputBuffersPerRegion, isWebcam);
}

bool TrikHardwareAbstraction::initVideoSensor(const QString &deviceFile, int i2cBus, int i2cAddress,
	int gpioNumber) const
{
	Q_UNUSED(deviceFile);

	// The kernel driver exposes a `reinit` sysfs node that powers the sensor up,
	// re-detects it (recovering a possibly frozen I2C bus after hot-plug) and
	// restores the default controls. Idempotent, so it is safe to run on every
	// fresh open. Must complete before the device is opened, so it stays on the
	// caller's thread.
	const QString reinitFile = QStringLiteral("/sys/bus/i2c/devices/%1-%2/reinit")
	                                   .arg(i2cBus)
	                                   .arg(i2cAddress, 4, 16, QLatin1Char('0'));
	{
		QScopedPointer<OutputDeviceFileInterface> reinit(createOutputDeviceFile(reinitFile));
		if (reinit->open()) {
			reinit->write(QStringLiteral("1\n"));
			QLOG_INFO() << "TrikHardwareAbstraction: reinitialized sensor on bus" << i2cBus << "address"
				    << Qt::hex << i2cAddress;
			return true;
		}
		QLOG_DEBUG() << "TrikHardwareAbstraction: no reinit node at" << reinitFile
			     << "- falling back to GPIO+I2C register programming";
	}

	// Fallback for kernels without the `reinit` node: pulse the reset GPIO, then
	// program the sensor registers over I2C and lock the exposure after a second
	// (see init-ov7670-320x240.sh). This path is legacy (the target firmware has
	// the `reinit` node). It is fully synchronous: the caller is the CameraManager
	// worker thread, so the sleeps here never block the GUI thread, and the init
	// completes before the device is opened.
	if (gpioNumber > 0) {
		QScopedPointer<OutputDeviceFileInterface> gpio(
			createOutputDeviceFile(QStringLiteral("/sys/class/gpio/gpio%1/value").arg(gpioNumber)));
		if (gpio->open()) {
			gpio->write(QStringLiteral("0\n"));
			usleep(10000); // reset pulse width (as in the script)
			gpio->write(QStringLiteral("1\n"));
		}
	}

	CommonI2c i2c(1); // ov7670 has 8-bit registers
	if (!i2c.connect(QStringLiteral("/dev/i2c-%1").arg(i2cBus), i2cAddress)) {
		QLOG_ERROR() << "TrikHardwareAbstraction: cannot open I2C bus" << i2cBus << "address" << i2cAddress;
		return false;
	}

	for (const auto &reg : ov7670RegisterDefaults) {
		if (i2c.writeRegister(reg.reg, reg.value) < 0) {
			QLOG_WARN() << "TrikHardwareAbstraction: failed to write ov7670 register" << Qt::hex << reg.reg
				    << "value" << reg.value;
		}
	}
	QLOG_DEBUG() << "TrikHardwareAbstraction: programmed"
		     << sizeof(ov7670RegisterDefaults) / sizeof(ov7670RegisterDefaults[0]) << "ov7670 registers on bus"
		     << i2cBus;

	// fix_ov7670(): let the auto-exposure stabilize (0x13 0x87 is on), then
	// lock it (0x13 0x85).
	usleep(1000000);
	i2c.writeRegister(0x13, 0x85);
	i2c.disconnect();
	return true;
}

FbOutputInterface *TrikHardwareAbstraction::createFbOutput() const
{
	return new trik::TrikFbOutput();
}
