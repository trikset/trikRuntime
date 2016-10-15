/* Copyright 2016 Evgeny Sergeev
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

#include <stdint.h>
#include <fcntl.h>
#include <png.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <zlib.h>
#include <QtCore/QString>
#include <QtCore/QVector>

namespace trikTelemetry {

/// Provides a possibility to get a png snapshot of framebuffer.
class SnapshotTaker
{
public:
	SnapshotTaker();

	~SnapshotTaker();

	/// Returns pointer to PNG framebuffer image in case of success
	/// or null QByteArray otherwise.
	QByteArray takeSnapshot();

private:
	static void pngWriteCallback(png_structp pngPtr, png_bytep data, png_size_t length);

	/// Initialisation of data that will not be changed in runtime.
	bool init();

	bool mapFramebuffer();

	bool initImageBuffer();

	bool writeImageToBuffer();

	/// Setted true after successfull initialisation. False otherwise.
	bool mInitSuccessfull = false;

	const QByteArray mFrameBufferPath;

	fb_fix_screeninfo mFixedFrameBufferInfo;

	fb_var_screeninfo mVariableFrameBufferInfo;

	uint8_t *mMappedFrameBufferPointer;  // Has ownership.

	png_structp mPngWriteStructPointer;

	png_infop mPngInfoStructPointer;

	int mFrameBufferFileDescriptor = -1;

	QByteArray *mPngImagePointer;  // Does not have ownership.
};

}
