/* Copyright 2021 CyberTech Labs Ltd.
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

#include "IIOFileInterface.h"

namespace trikHal {
namespace stub {

/// Empty implementation of event file, it only logs calls to its methods and doen't emit any signals.
class StubIIOFile : public IIOFileInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param fileName - file name (with path, relative or absolute) of an IIO file.
	StubIIOFile(const QString &fileName);

	bool open() override;
	bool close() override;
	QString fileName() const override;
	bool isOpened() const override;

private:
	QString mFileName;
};

}
}
