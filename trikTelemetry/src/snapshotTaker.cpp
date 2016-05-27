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


#include "snapshotTaker.h"

namespace trikTelemetry {

SnapshotTaker::SnapshotTaker() : mImageSize(1024 * 15), mFrameBufferPath("/dev/fb0")
{
}

bool SnapshotTaker::mapFramebuffer() {
	int frameBufferFileDescriptor = open(mFrameBufferPath, O_RDWR);
	if (frameBufferFileDescriptor == -1) {
		/// Cannot open framebuffer
		return false;
	}

	/// Reading framebuffer fixed information.
	if (ioctl(frameBufferFileDescriptor, FBIOGET_FSCREENINFO, &mFixedFrameBufferInfo) == -1) {
		/// Failed to read framebuffer fixed information
		return false;
	}

	/// Reading framebuffer variable information.
	if (ioctl(frameBufferFileDescriptor, FBIOGET_VSCREENINFO, &mVariableFrameBufferInfo) == -1) {
		/// Failed to read framebuffer variable information
		return false;
	}

	/// Mapping framebuffer to memory.
	mMappedFrameBufferPointer =
			static_cast<uint8_t *>(mmap(0
										, mFixedFrameBufferInfo.smem_len
										, PROT_READ | PROT_WRITE
										, MAP_SHARED
										, frameBufferFileDescriptor
										, 0));

	close(frameBufferFileDescriptor);

	if (mMappedFrameBufferPointer == MAP_FAILED) {
		/// Failed to map framebuffer device to memory
		return false;
	}

	return true;
}

bool SnapshotTaker::initImageBuffer(char **bufferPointer, size_t *sizePointer) {
	/// Allocating PNG write struct.
	mPngWriteStructPointer = png_create_write_struct(
				PNG_LIBPNG_VER_STRING
				, NULL
				, NULL
				, NULL
	);
	if (mPngWriteStructPointer == NULL) {
		/// Failed to allocate PNG write struct
		return false;
	}

	/// Allocating PNG info struct.
	mPngInfoStructPointer = png_create_info_struct(mPngWriteStructPointer);
	if (mPngInfoStructPointer == NULL) {
		/// Failed to allocate PNG info struct
		return false;
	}

	/// Opening a dynamic memory buffer stream.
	/// PNG image will be written there.
	mImagePointer = open_memstream(bufferPointer, sizePointer);
	if (mImagePointer == NULL) {
		/// Failed to create memstream
		return false;
	}

	/// Initializing the default input/output functions for the PNG file.
	png_init_io(mPngWriteStructPointer, mImagePointer);

	/// Set image header information.
	png_set_IHDR(
		mPngWriteStructPointer,
		mPngInfoStructPointer,
		mVariableFrameBufferInfo.xres,
		mVariableFrameBufferInfo.yres,
		8,
		PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE,
		PNG_FILTER_TYPE_BASE
	);

	/// Writing the PNG information to file.
	png_write_info(mPngWriteStructPointer, mPngInfoStructPointer);

	return true;
}

bool SnapshotTaker::writePngToImageBuffer() {
	png_bytep pngBuffer =
			static_cast<unsigned char *>(malloc(mVariableFrameBufferInfo.xres * 3 * sizeof(png_byte)));

	if (pngBuffer == NULL) {
		/// Failed to allocate buffer
		return false;
	}

	int redMask = (1 << mVariableFrameBufferInfo.red.length) - 1;
	int greenMask = (1 << mVariableFrameBufferInfo.green.length) - 1;
	int blueMask = (1 << mVariableFrameBufferInfo.blue.length) - 1;

	int bytesPerPixel = mVariableFrameBufferInfo.bits_per_pixel / 8;

	unsigned int y = 0;

	for (y = 0; y < mVariableFrameBufferInfo.yres; y++)
	{
		unsigned int x;

		for (x = 0; x < mVariableFrameBufferInfo.xres; x++)
		{
			int pngBufferOffset = 3 * x;

			size_t frameBufferOffset = x * (bytesPerPixel) + y * mFixedFrameBufferInfo.line_length;

			uint32_t pixel = 0;

			pixel = *(reinterpret_cast<uint32_t *>(mMappedFrameBufferPointer + frameBufferOffset));

			png_byte red = (pixel >> mVariableFrameBufferInfo.red.offset) & redMask;
			png_byte green = (pixel >> mVariableFrameBufferInfo.green.offset) & greenMask;
			png_byte blue = (pixel >> mVariableFrameBufferInfo.blue.offset) & blueMask;

			pngBuffer[pngBufferOffset] = (red * 0xFF) / redMask;
			pngBuffer[pngBufferOffset + 1] = (green * 0xFF)  / greenMask;
			pngBuffer[pngBufferOffset + 2] = (blue * 0xFF)  / blueMask;
		}

		png_write_row(mPngWriteStructPointer, pngBuffer);
	}

	free(pngBuffer);
	pngBuffer = NULL;
	png_write_end(mPngWriteStructPointer, NULL);

	return true;
}

bool SnapshotTaker::takeSnapshot(char **bufferPointer, size_t *sizePointer) {
	*sizePointer = mImageSize;

	if (!mapFramebuffer()) {
		return false;
	}

	if (!initImageBuffer(bufferPointer, sizePointer)) {
		return false;
	}

	if (!writePngToImageBuffer()) {
		return false;
	}

	png_destroy_write_struct(&mPngWriteStructPointer, &mPngInfoStructPointer);

	fclose(mImagePointer);

	munmap(mMappedFrameBufferPointer, mFixedFrameBufferInfo.smem_len);

	return true;
}

}
