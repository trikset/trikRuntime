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

SnapshotTaker::SnapshotTaker() : mFrameBufferPath("/dev/fb0") {
}

SnapshotTaker::~SnapshotTaker() {
	if (mFrameBufferFileDescriptor != -1) {
		close(mFrameBufferFileDescriptor);
	}

	if (mPngWriteStructPointer && mPngInfoStructPointer) {
		png_destroy_write_struct(&mPngWriteStructPointer, &mPngInfoStructPointer);
	}

	if (!mPngImage.isNull()) {
		mPngImage->clear();
	}
}

bool SnapshotTaker::init() {

	if (mFrameBufferFileDescriptor == -1) {
		mFrameBufferFileDescriptor = open(mFrameBufferPath.data(), O_RDWR);
	}

	if (mFrameBufferFileDescriptor == -1) {
		/// Cannot open framebuffer.
		return false;
	}

	/// Reading framebuffer fixed information.
	if (ioctl(mFrameBufferFileDescriptor, FBIOGET_FSCREENINFO, &mFixedFrameBufferInfo) == -1) {
		/// Failed to read framebuffer fixed information.
		return false;
	}

	return true;
}

bool SnapshotTaker::mapFramebuffer() {

	/// Reading framebuffer variable information.
	if (ioctl(mFrameBufferFileDescriptor, FBIOGET_VSCREENINFO, &mVariableFrameBufferInfo) == -1) {
		/// Failed to read framebuffer variable information.
		return false;
	}

	/// Mapping framebuffer to memory.
	mMappedFrameBufferPointer.reset(
			static_cast<uint8_t *>(mmap(0
										, mFixedFrameBufferInfo.smem_len
										, PROT_READ | PROT_WRITE
										, MAP_SHARED
										, mFrameBufferFileDescriptor
										, 0)));
	if (mMappedFrameBufferPointer.data() == MAP_FAILED) {
		/// Failed to map framebuffer device to memory.
		return false;
	}

	return true;
}

bool SnapshotTaker::initImageBuffer() {

	/// Allocating PNG write struct.
	mPngWriteStructPointer = png_create_write_struct(
				PNG_LIBPNG_VER_STRING
				, NULL
				, NULL
				, NULL
	);
	if (mPngWriteStructPointer == NULL) {
		/// Failed to allocate PNG write struct.
		return false;
	}

	/// Allocating PNG info struct.
	mPngInfoStructPointer = png_create_info_struct(mPngWriteStructPointer);
	if (mPngInfoStructPointer == NULL) {
		/// Failed to allocate PNG info struct.
		return false;
	}

	mPngImage.reset(new std::vector<unsigned char>(mFixedFrameBufferInfo.smem_len));

	/// Setting custom function for writing a PNG stream.
	png_set_write_fn(mPngWriteStructPointer, mPngImage.data(), PngWriteCallback, NULL);

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

bool SnapshotTaker::writeImageToBuffer() {

	std::vector<unsigned char> pngBuffer(mVariableFrameBufferInfo.xres * 3 * sizeof(png_byte));

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

			pixel = *(reinterpret_cast<uint32_t *>(mMappedFrameBufferPointer.data() + frameBufferOffset));

			png_byte red = (pixel >> mVariableFrameBufferInfo.red.offset) & redMask;
			png_byte green = (pixel >> mVariableFrameBufferInfo.green.offset) & greenMask;
			png_byte blue = (pixel >> mVariableFrameBufferInfo.blue.offset) & blueMask;

			pngBuffer[pngBufferOffset] = (red * 0xFF) / redMask;
			pngBuffer[pngBufferOffset + 1] = (green * 0xFF) / greenMask;
			pngBuffer[pngBufferOffset + 2] = (blue * 0xFF) / blueMask;
		}

		png_write_row(mPngWriteStructPointer, pngBuffer.data());
	}

	png_write_end(mPngWriteStructPointer, NULL);

	pngBuffer.clear();

	return true;
}

std::vector<unsigned char> SnapshotTaker::takeSnapshot() {

	if (!mInitSuccessfull) {
		if (!(mInitSuccessfull = init())) {
			mPngImage->clear();
			return *(mPngImage.data());
		}
	}

	if (!mapFramebuffer()) {
		mPngImage->clear();
		return *(mPngImage.data());
	}

	if (!initImageBuffer()) {
		mPngImage->clear();
		return *(mPngImage.data());
	}

	if (!writeImageToBuffer()) {
		mPngImage->clear();
		return *(mPngImage.data());
	}

	png_destroy_write_struct(&mPngWriteStructPointer, &mPngInfoStructPointer);

	munmap(mMappedFrameBufferPointer.data(), mFixedFrameBufferInfo.smem_len);
	mMappedFrameBufferPointer.reset(nullptr);

	return *(mPngImage.data());
}

void PngWriteCallback(png_structp pngPtr, png_bytep data, png_size_t length) {
	std::vector<unsigned char> *pngImage = (std::vector<unsigned char>*)png_get_io_ptr(pngPtr);
	pngImage->insert(pngImage->end(), data, data + length);
}

}
