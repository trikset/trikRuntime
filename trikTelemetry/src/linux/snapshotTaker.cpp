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

SnapshotTaker::SnapshotTaker() : mFrameBufferPath("/dev/fb0")
{
}

SnapshotTaker::~SnapshotTaker()
{
	if (mFrameBufferFileDescriptor != -1) {
		close(mFrameBufferFileDescriptor);
	}

	if (mPngWriteStructPointer && mPngInfoStructPointer) {
		png_destroy_write_struct(&mPngWriteStructPointer, &mPngInfoStructPointer);
	}

	if (mMappedFrameBufferPointer != nullptr) {
		munmap(mMappedFrameBufferPointer, mFixedFrameBufferInfo.smem_len);
	}

	delete mPngImagePointer;
}


bool SnapshotTaker::init()
{
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

bool SnapshotTaker::mapFramebuffer()
{
	/// Reading framebuffer variable information.
	if (ioctl(mFrameBufferFileDescriptor, FBIOGET_VSCREENINFO, &mVariableFrameBufferInfo) == -1) {
		/// Failed to read framebuffer variable information.
		return false;
	}

	/// Mapping framebuffer to memory.
	mMappedFrameBufferPointer =
			static_cast<uint8_t *>(mmap(0
					, mFixedFrameBufferInfo.smem_len
					, PROT_READ | PROT_WRITE
					, MAP_SHARED
					, mFrameBufferFileDescriptor
					, 0));
	return mMappedFrameBufferPointer != MAP_FAILED;
}

bool SnapshotTaker::initImageBuffer()
{
	/// Allocating PNG write struct.
	mPngWriteStructPointer = png_create_write_struct(
				PNG_LIBPNG_VER_STRING
				, nullptr
				, nullptr
				, nullptr
	);
	if (mPngWriteStructPointer == nullptr) {
		/// Failed to allocate PNG write struct.
		return false;
	}

	/// Allocating PNG info struct.
	mPngInfoStructPointer = png_create_info_struct(mPngWriteStructPointer);
	if (mPngInfoStructPointer == nullptr) {
		/// Failed to allocate PNG info struct.
		return false;
	}

	mPngImagePointer = new QByteArray();

	/// Setting custom function for writing a PNG.
	png_set_write_fn(mPngWriteStructPointer, mPngImagePointer, pngWriteCallback, nullptr);

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

bool SnapshotTaker::writeImageToBuffer()
{
	QVector<unsigned char> pngBuffer(mVariableFrameBufferInfo.xres * 3 * sizeof(png_byte));

	int redMask = (1 << mVariableFrameBufferInfo.red.length) - 1;
	int greenMask = (1 << mVariableFrameBufferInfo.green.length) - 1;
	int blueMask = (1 << mVariableFrameBufferInfo.blue.length) - 1;

	int bytesPerPixel = mVariableFrameBufferInfo.bits_per_pixel / 8;

	for (unsigned int y = 0; y < mVariableFrameBufferInfo.yres; y++) {
		for (unsigned int x = 0; x < mVariableFrameBufferInfo.xres; x++) {
			int pngBufferOffset = 3 * x;

			size_t frameBufferOffset = x * (bytesPerPixel) + y * mFixedFrameBufferInfo.line_length;

			uint32_t pixel = *(reinterpret_cast<uint32_t *>(mMappedFrameBufferPointer + frameBufferOffset));

			png_byte red = (pixel >> mVariableFrameBufferInfo.red.offset) & redMask;
			png_byte green = (pixel >> mVariableFrameBufferInfo.green.offset) & greenMask;
			png_byte blue = (pixel >> mVariableFrameBufferInfo.blue.offset) & blueMask;

			pngBuffer[pngBufferOffset] = (red * 0xFF) / redMask;
			pngBuffer[pngBufferOffset + 1] = (green * 0xFF) / greenMask;
			pngBuffer[pngBufferOffset + 2] = (blue * 0xFF) / blueMask;
		}

		png_write_row(mPngWriteStructPointer, pngBuffer.data());
	}

	png_write_end(mPngWriteStructPointer, nullptr);

	pngBuffer.clear();

	return true;
}

QByteArray SnapshotTaker::takeSnapshot()
{
	if (!mInitSuccessfull) {
		mInitSuccessfull = init();
		if (!mInitSuccessfull) {
			return QByteArray();
		}
	}

	if (!mapFramebuffer()) {
		return QByteArray();
	}

	if (!initImageBuffer()) {
		delete mPngImagePointer;
		mPngImagePointer = nullptr;
		return QByteArray();
	}

	if (!writeImageToBuffer()) {
		delete mPngImagePointer;
		mPngImagePointer = nullptr;
		return QByteArray();
	}

	png_destroy_write_struct(&mPngWriteStructPointer, &mPngInfoStructPointer);

	munmap(mMappedFrameBufferPointer, mFixedFrameBufferInfo.smem_len);
	mMappedFrameBufferPointer = nullptr;

	QByteArray tempPngImagePointer = *mPngImagePointer;
	mPngImagePointer = nullptr;
	return tempPngImagePointer;
}

void pngWriteCallback(png_structp pngPtr, png_bytep data, png_size_t length)
{
	QByteArray *pngImage = (QByteArray *)png_get_io_ptr(pngPtr);
	pngImage->append(reinterpret_cast<char *>(data), length);
}

}
