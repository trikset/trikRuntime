/* Copyright 2018 Ivan Tyulyandin and CyberTech Labs Ltd.
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

#include "cameraDeviceInterface.h"

using namespace trikControl;

QVector<uint8_t> CameraDeviceInterface::qImageToQVector(const QImage &imgOrig) {
	// Some possible formats:
	// QImage::Format_RGB32
	// QImage::Format_RGB888
	// QImage::Format_RGB16
	// QImage::Format_Grayscale8
	// QImage::Format_Mono
	constexpr auto DESIRED_FORMAT = QImage::Format_RGB888;

	constexpr auto SIZE_X = 320;
	constexpr auto SIZE_Y = 240;

	const QImage &img = imgOrig.format() == DESIRED_FORMAT ? imgOrig : imgOrig.convertToFormat(DESIRED_FORMAT);
	const QImage &scaledImg = img.height() == SIZE_X && img.width() == SIZE_Y ? img : img.scaled(SIZE_X, SIZE_Y);
	auto cb = scaledImg.constBits();
	auto size = scaledImg.sizeInBytes();
	QVector<uint8_t> imageByteVector (size);
	std::copy(cb, cb + size, imageByteVector.begin());

	return imageByteVector;
}
