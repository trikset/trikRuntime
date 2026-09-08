/* Copyright 2026 CyberTech Labs Ltd.
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

#include <trikHal/fbOutputInterface.h>

namespace trikHal {
namespace stub {

/// No-op framebuffer output for tests and non-target builds.
class StubFbOutput : public trikHal::FbOutputInterface
{
	Q_OBJECT

public:
	explicit StubFbOutput(QObject *parent = nullptr);

	/// Pretends to open the framebuffer.
	bool open() override;

	/// Pretends to close the framebuffer.
	void close() override;

	/// Whether the stub considers itself open.
	bool isOpen() const override;

	/// No-op frame write.
	void writeFrame(const uint8_t *rgb565) override;

	/// Mirrors the real 240x240 DSP output size.
	uint32_t frameWidth() const override;

	/// Mirrors the real 240x240 DSP output size.
	uint32_t frameHeight() const override;

private:
	bool mOpen = false;
};

} // namespace stub
} // namespace trikHal
