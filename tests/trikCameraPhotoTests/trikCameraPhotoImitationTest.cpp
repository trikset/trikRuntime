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

#include "trikCameraPhotoImitationTest.h"

#include <trikControl/brickFactory.h>

using namespace tests;
using namespace trikControl;

void trikCameraPhotoImitationTest::SetUp() {
	testBrick.reset(trikControl::BrickFactory::create("./test-system-config.xml"
			, "./test-model-config.xml", "./media/"));
}

namespace {
	QVector<uint8_t> qImageToQVector(const QImage &imgOrig) {
		constexpr auto DESIRED_FORMAT = QImage::Format_RGB888;

		constexpr auto SIZE_X = 320;
		constexpr auto SIZE_Y = 240;
		const QImage &img = imgOrig.format() == DESIRED_FORMAT ? imgOrig : imgOrig.convertToFormat(DESIRED_FORMAT);
		const QImage &scaledImg = img.height() == SIZE_X && img.width() == SIZE_Y ? img : img.scaled(SIZE_X, SIZE_Y);
		auto cb = scaledImg.constBits();
		QVector<uint8_t> imageByteVector = QVector<uint8_t>(scaledImg.byteCount());
		std::copy(cb, cb + scaledImg.byteCount(), imageByteVector.begin());

		return imageByteVector;
	}
}

TEST_F(trikCameraPhotoImitationTest, cameraImitationTest)
{
	QVector<uint8_t> expectedPhoto = qImageToQVector(QImage("./media/trik_smile_normal.png"));
	QVector<uint8_t> currentPhoto = testBrick->getStillImage();
	ASSERT_EQ(expectedPhoto, currentPhoto);

	currentPhoto = testBrick->getStillImage();
	expectedPhoto = qImageToQVector(QImage("./media/trik_smile_sad.png"));
	ASSERT_EQ(expectedPhoto, currentPhoto);

	currentPhoto = testBrick->getStillImage();
	expectedPhoto = qImageToQVector(QImage("./media/trik_smile_normal.png"));
	ASSERT_EQ(expectedPhoto, currentPhoto);
}
