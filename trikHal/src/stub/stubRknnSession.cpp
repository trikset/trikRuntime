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

#include "stubRknnSession.h"

using namespace trikHal;
using namespace trikHal::stub;

bool StubRknnSession::load(const QString &modelPath, QString *error)
{
	Q_UNUSED(modelPath);
	if (error) {
		*error = QStringLiteral("NPU is not available in this build");
	}
	return false;
}

bool StubRknnSession::isLoaded() const
{
	return false;
}

const RknnTensorInfo &StubRknnSession::input() const
{
	return mInput;
}

const QVector<RknnTensorInfo> &StubRknnSession::outputs() const
{
	return mOutputs;
}

bool StubRknnSession::run(QVector<RknnOutputView> *out, QString *error)
{
	if (out) {
		out->clear();
	}
	if (error) {
		*error = QStringLiteral("NPU is not available in this build");
	}
	return false;
}

int StubRknnSession::inputBufferFd() const
{
	return -1;
}

uint8_t *StubRknnSession::inputBufferData() const
{
	return nullptr;
}

qint64 StubRknnSession::lastRunTimeUs() const
{
	return 0;
}
