/* Copyright 2016 CyberTech Labs Ltd.
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

#include <cstdlib>
#include <stdint.h>
#include <fcntl.h>
#include <png.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <zlib.h>


namespace trikTelemetry {

/// Provides an opportunity to get a png snapshot of framebuffer.
class SnapshotTaker
{

public:
	SnapshotTaker();

	/// @param bufferPointer - in case of succeess function returns true
	/// and it contains the address of the buffer containing framebuffer png image.
	/// @param sizePointer - in case of succeess function returns true and it contains the
	/// bufferPointer length.
	bool takeSnapshot(char **bufferPointer, size_t *sizePointer);

private:
	bool mapFramebuffer();

	bool initImageBuffer(char **bufferPointer, size_t *sizePointer);

	bool writePngToImageBuffer();

	/// Initial value of png buffer size. If it is not enougth, the
	/// buffer will be reallocated automatically.
	const size_t mImageSize;

	char const *mFrameBufferPath;

	FILE *mImagePointer;

	fb_fix_screeninfo mFixedFrameBufferInfo;

	fb_var_screeninfo mVariableFrameBufferInfo;

	uint8_t *mMappedFrameBufferPointer;

	png_structp mPngWriteStructPointer;

	png_infop mPngInfoStructPointer;
};

}
