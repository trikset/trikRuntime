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

#include <cstddef>
#include <cstdint>

#include <QVector>

#include <trikHal/rknnSessionInterface.h>

namespace trikHal {
namespace stub {

/// NPU session stub used when the RKNN backend is not built in.
class StubRknnSession : public RknnSessionInterface
{
public:
	bool load(const QString &modelPath, QString *error) override;
	bool isLoaded() const override;

	const RknnTensorInfo &input() const override;
	const QVector<RknnTensorInfo> &outputs() const override;

	bool run(QVector<RknnOutputView> *out, QString *error) override;
	int inputBufferFd() const override;
	uint8_t *inputBufferData() const override;

	qint64 lastRunTimeUs() const override;

private:
	RknnTensorInfo mInput;
	QVector<RknnTensorInfo> mOutputs;
};

} // namespace stub
} // namespace trikHal
