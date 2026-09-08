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

#include "dspServer.h"
#include "../dspServer_p.h"
#include "../dspConverters.h"

#include <trikHal/fbOutputInterface.h>

#include <QsLog.h>

namespace {

const int _registerDspMetaTypes = []() {
	qRegisterMetaType<trikDsp::Algorithm>("trikDsp::Algorithm");
	qRegisterMetaType<trikDsp::InArgs>("trikDsp::InArgs");
	qRegisterMetaType<trikDsp::OutArgs>("trikDsp::OutArgs");
	qRegisterMetaType<uint32_t>("uint32_t");
	return 0;
}();

} // namespace

namespace trikDsp {

DspServer::DspServer(uint16_t rprocId, QObject *parent)
	: QObject(parent)
	, d(new Impl)
{
	d->rprocId = rprocId;
	QLOG_INFO() << "DspServer: init skipped (stub)";
}

DspServer::~DspServer() = default;

void DspServer::init()
{
	QLOG_INFO() << "DspServer: init (stub)";
	Q_EMIT successfullyInited();
}

bool DspServer::processFrame(const DspChannel &channel, OutArgs &out, uint32_t bufferIdx)
{
	Q_UNUSED(channel)
	Q_UNUSED(out)
	Q_UNUSED(bufferIdx)
	return true;
}

void DspServer::setVideoOutput(bool enabled)
{
	Q_UNUSED(enabled)
	QLOG_INFO() << "DspServer: setVideoOutput (stub)";
}

void DspServer::setFbOutput(trikHal::FbOutputInterface *fb)
{
	d->mFbOutput.reset(fb);
	QLOG_INFO() << "DspServer: setFbOutput (stub)";
}

}
