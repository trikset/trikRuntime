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

#include "dspServer_p.h"
#include "dspConverters.h"

#include <trikHal/fbOutputInterface.h>
#include <trikHal/physicalMemoryMapper.h>

#include <array>

#include <ti/ipc/Std.h>
#include <ti/ipc/Ipc.h>
#include <ti/ipc/MessageQ.h>
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/transports/TransportRpmsg.h>

#include <trik/sensors/cmd.h>
#include <trik/sensors/msg.h>

#include <QsLog.h>

namespace trikDsp {

namespace {

constexpr int MSG_QUEUE_RETRIES = 10;

enum trik_cmd algoToDspCmd(enum trik_cv_algorithm algo)
{
	switch (algo) {
	case TRIK_CV_ALGORITHM_MOTION_SENSOR:
		return TRIK_CMD_MOTION_SENSOR;
	case TRIK_CV_ALGORITHM_EDGE_LINE_SENSOR:
		return TRIK_CMD_EDGE_LINE_SENSOR;
	case TRIK_CV_ALGORITHM_LINE_SENSOR:
		return TRIK_CMD_LINE_SENSOR;
	case TRIK_CV_ALGORITHM_OBJECT_SENSOR:
		return TRIK_CMD_OBJECT_SENSOR;
	case TRIK_CV_ALGORITHM_MXN_SENSOR:
		return TRIK_CMD_MXN_SENSOR;
	case TRIK_CV_ALGORITHM_JPEG_ENCODER:
		return TRIK_CMD_JPEG_ENCODER;
	default:
		return TRIK_CMD_NOP;
	}
}

::trik_msg *allocRequest(MessageQ_Handle hostQue, UInt16 heapId, UInt32 msgSize, enum trik_cmd cmd)
{
	auto *msg = reinterpret_cast<::trik_msg *>(MessageQ_alloc(heapId, msgSize));
	if (!msg) {
		QLOG_ERROR() << "DspServer: MessageQ_alloc failed (heap" << heapId << "size" << msgSize << "cmd" << cmd
			     << ")";
		return nullptr;
	}
	msg->cmd = cmd;
	MessageQ_setReplyQueue(hostQue, reinterpret_cast<MessageQ_Msg>(msg));
	return msg;
}

} // namespace

DspServer::Impl::~Impl()
{
	destroyMessageQueue();
	Ipc_stop();
	// mOutMap unmaps the DSP output region via RAII.

	QLOG_INFO() << "DspServer: destroyed";
}

bool DspServer::Impl::startIpc()
{
	Ipc_transportConfig(&TransportRpmsg_Factory);

	const int status = Ipc_start();
	if (status < 0) {
		QLOG_ERROR() << "DspServer: Ipc_start failed:" << status;
		return false;
	}

	QLOG_INFO() << "DspServer: Ipc_start ok, MultiProc: self=" << MultiProc_self()
		    << "name(0)=" << MultiProc_getName(0) << "name(1)=" << MultiProc_getName(1);

	return true;
}

::trik_msg *DspServer::Impl::sendAndWaitForResponse(::trik_msg *msg)
{
	if (MessageQ_put(mSlaveQue, reinterpret_cast<MessageQ_Msg>(msg)) < 0) {
		QLOG_ERROR() << "DspServer: MessageQ_put failed for cmd" << msg->cmd;
		return nullptr;
	}

	::trik_msg *res = nullptr;
	MessageQ_get(mHostQue, reinterpret_cast<MessageQ_Msg *>(&res), MessageQ_FOREVER);
	return res;
}

void DspServer::Impl::freeMessage(::trik_msg *msg)
{
	MessageQ_free(reinterpret_cast<MessageQ_Msg>(msg));
}

bool DspServer::Impl::setupMessageQueue()
{
	MessageQ_Params params;
	MessageQ_Params_init(&params);

	mHostQue = MessageQ_create(const_cast<char *>(TRIK_HOST_MSG_QUE_NAME), &params);
	if (!mHostQue) {
		QLOG_ERROR() << "DspServer: MessageQ_create failed";
		return false;
	}
	std::array<char, 32> name {};
	snprintf(name.data(), name.size(), TRIK_SLAVE_MSG_QUE_NAME, "DSP");
	QLOG_INFO() << "DspServer: opening remote queue" << name.data();

	int status = 0;
	for (int retry = 0; retry < MSG_QUEUE_RETRIES; ++retry) {
		status = MessageQ_open(name.data(), &mSlaveQue);
		QLOG_INFO() << "DspServer: MessageQ_open retry" << retry << "status" << status;
		if (status != MessageQ_E_NOTFOUND)
			break;
		sleep(1);
	}

	if (status == MessageQ_E_NOTFOUND) {
		QLOG_ERROR() << "DspServer: MessageQ_open timed out after" << MSG_QUEUE_RETRIES << "retries";
		return false;
	}

	if (status < 0) {
		QLOG_ERROR() << "DspServer: MessageQ_open failed:" << status;
		return false;
	}

	QLOG_INFO() << "DspServer: MessageQ ready";
	return true;
}

void DspServer::Impl::destroyMessageQueue()
{
	if (mSlaveQue != MessageQ_INVALIDMESSAGEQ) {
		QLOG_INFO() << "DspServer: closing slave message queue";
		MessageQ_close(&mSlaveQue);
		mSlaveQue = MessageQ_INVALIDMESSAGEQ;
	}
	if (mHostQue) {
		QLOG_INFO() << "DspServer: deleting host message queue";
		MessageQ_delete(&mHostQue);
		mHostQue = nullptr;
	}
}

bool DspServer::Impl::mapSharedBuffers()
{
	auto *req = allocRequest(mHostQue, TRIK_MSG_HEAP_ID, TRIK_MSG_SIZE, TRIK_CMD_INIT);
	if (!req) {
		QLOG_ERROR() << "DspServer: failed to allocate INIT msg";
		return false;
	}

	auto *res = sendAndWaitForResponse(req);
	if (!res) {
		QLOG_ERROR() << "DspServer: no response to INIT";
		return false;
	}

	auto *initRes = reinterpret_cast<struct trik_res_init_msg *>(res);
	mOutMap = trikHal::mapPhysicalMemory(reinterpret_cast<uintptr_t>(initRes->dsp_out_buffer), BUFFER_SIZE);

	// The DSP input region is no longer mapped here: CameraManager maps it
	// itself up front (fixed physical address) so the camera can capture into
	// DSP memory independently of this INIT exchange. Only the output buffer is
	// still obtained from the INIT response.
	QLOG_DEBUG() << "DspServer: INIT response dsp_in_phys=" << initRes->dsp_in_buffer
		     << "dsp_out_phys=" << initRes->dsp_out_buffer
		     << "out_virt=" << static_cast<void *>(mOutMap.data());

	if (mOutMap) {
		QLOG_INFO() << "DspServer: mapped DSP output buffer at" << mOutMap.data();
		mDspOut.start = mOutMap.data();
		mDspOut.length = BUFFER_SIZE;
	}

	freeMessage(res);

	if (mDspOut.start) {
		QLOG_INFO() << "DspServer: DSP output buffer mapped";
		return true;
	}

	QLOG_ERROR() << "DspServer: failed to mmap DSP output buffer";
	return false;
}

void DspServer::Impl::registerAlgorithm(Algorithm algo, const AlgoDescriptor &desc)
{
	const auto dspAlgo = toDspAlgo(algo);
	const auto cmd = algoToDspCmd(dspAlgo);
	if (cmd == TRIK_CMD_NOP) {
		QLOG_ERROR() << "DspServer: unknown algorithm";
		return;
	}

	const auto vfmt = toDspVideoFormat(desc.format);
	if (vfmt == Unknown) {
		QLOG_ERROR() << "DspServer: unknown pixel format";
		return;
	}

	auto *req = reinterpret_cast<struct trik_req_cv_algorithm_msg *>(
		allocRequest(mHostQue, TRIK_MSG_HEAP_ID, TRIK_MSG_SIZE, cmd));
	if (!req) {
		QLOG_ERROR() << "DspServer: failed to allocate algo msg";
		return;
	}

	req->video_format = vfmt;
	req->line_length = desc.lineLength;

	auto *res = sendAndWaitForResponse(&req->header);
	if (!res) {
		QLOG_ERROR() << "DspServer: no ack for algo reg";
		return;
	}

	freeMessage(res);
}

bool DspServer::Impl::step(const InArgs &in, OutArgs &out, uint32_t bufferIdx)
{
	auto *req = reinterpret_cast<struct trik_res_step_msg *>(
		allocRequest(mHostQue, TRIK_MSG_HEAP_ID, TRIK_MSG_SIZE, TRIK_CMD_STEP));
	if (!req) {
		QLOG_ERROR() << "DspServer: failed to allocate STEP msg";
		return false;
	}

	req->buffer_idx = bufferIdx;
	req->in_args = toDspInArgs(in);

	auto *res = sendAndWaitForResponse(&req->header);
	if (!res) {
		QLOG_ERROR() << "DspServer: no response to STEP";
		return false;
	}

	out = fromDspOutArgs(reinterpret_cast<struct trik_res_step_msg *>(res)->out_args);
	freeMessage(res);
	return true;
}

} // namespace trikDsp
