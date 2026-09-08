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

#include "../dspServer_p.h"
#include "dspServer.h"

#include <trikHal/fbOutputInterface.h>

#include <QsLog.h>

namespace trikDsp {

DspServer::Impl::~Impl()
{
	QLOG_INFO() << "DspServer: destroyed (stub)";
}

bool DspServer::Impl::startIpc()
{
	QLOG_INFO() << "DspServer: Ipc_start (stub)";
	return true;
}

bool DspServer::Impl::setupMessageQueue()
{
	QLOG_INFO() << "DspServer: MessageQ setup skipped (stub)";
	return true;
}

void DspServer::Impl::destroyMessageQueue()
{
	QLOG_INFO() << "DspServer: MessageQ destroy skipped (stub)";
}

bool DspServer::Impl::mapSharedBuffers()
{
	QLOG_INFO() << "DspServer: buffer mapping skipped (stub)";
	return true;
}

void DspServer::Impl::registerAlgorithm(Algorithm algo, const AlgoDescriptor &desc)
{
	Q_UNUSED(algo)
	Q_UNUSED(desc)
	QLOG_INFO() << "DspServer: algorithm registration skipped (stub)";
}

bool DspServer::Impl::step(const InArgs &in, OutArgs &out, uint32_t bufferIdx)
{
	Q_UNUSED(in)
	Q_UNUSED(out)
	Q_UNUSED(bufferIdx)
	return false;
}

::trik_msg *DspServer::Impl::sendAndWaitForResponse(::trik_msg *msg)
{
	Q_UNUSED(msg)
	return nullptr;
}

void DspServer::Impl::freeMessage(::trik_msg *msg)
{
	Q_UNUSED(msg)
}

}
