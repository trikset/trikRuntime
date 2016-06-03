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


#include "snapshotTaker.h"

namespace trikTelemetry {

SnapshotTaker::SnapshotTaker() : mFrameBufferPath() 
{
}

SnapshotTaker::~SnapshotTaker()
{
}

bool SnapshotTaker::init()
{
	return false;
}

bool SnapshotTaker::mapFramebuffer()
{
	return false;
}

bool SnapshotTaker::initImageBuffer()
{
	return false;
}

bool SnapshotTaker::writeImageToBuffer()
{
	return false;
}

QByteArray *SnapshotTaker::takeSnapshot()
{

	return new QByteArray();
}

void SnapshotTaker::PngWriteCallback(png_structp pngPtr, png_bytep data, png_size_t length)
{
	Q_UNUSED(pngPtr);
	Q_UNUSED(data);
	Q_UNUSED(length);
}

}
