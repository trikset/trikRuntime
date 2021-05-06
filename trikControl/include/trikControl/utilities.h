/* Copyright 2019 Andrei Khodko
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

#include <QImage>

#include <trikControl/trikControlDeclSpec.h>

namespace trikControl {

/// The class that makes utility code reusable.
class TRIKCONTROL_EXPORT Utilities
{
public:
	/// Creates image for show method from data according to format. Image is scaled to fill the screen.
	static QImage imageFromBytes(const QVector<int32_t> &array, int width, int height, const QString &format);

	/// Reworks data from brick.getStillImage() to better getPhoto()
	static QVector<int32_t> rescalePhoto(const QVector<uchar> &data);
};
}
