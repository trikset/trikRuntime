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
#include "dspServer_p.h"
#include "dspConverters.h"

#include <QsLog.h>
#include <QEventLoop>
#include <QTimer>

#include <trikHal/fbOutputInterface.h>

#include <algorithm>
#include <cstring>

namespace {
const int _registerDspMetaTypes = []() {
	qRegisterMetaType<trikDsp::Algorithm>("trikDsp::Algorithm");
	qRegisterMetaType<trikDsp::InArgs>("trikDsp::InArgs");
	qRegisterMetaType<trikDsp::OutArgs>("trikDsp::OutArgs");
	qRegisterMetaType<uint32_t>("uint32_t");
	return 0;
}();

/// The DSP video output is a fixed 240x240 RGB565 framebuffer (see BUFFER_SIZE_FOR_FB).
constexpr int DSP_FB_DIM = 240;

} // namespace

namespace trikDsp {

DspServer::DspServer(uint16_t rprocId, QObject *parent)
	: QObject(parent)
	, d(new Impl)
{
	d->rprocId = rprocId;
}

DspServer::~DspServer()
{
	// Tear down the IPC stack while the LAD daemon is still alive: the
	// MessageQ teardown blocks on MessageQ_FOREVER and needs the daemon to
	// respond, so killing LAD first could hang forever.
	d.reset();

	mLadProcess.terminate();
	if (!mLadProcess.waitForFinished(3000)) {
		QLOG_ERROR() << "DspServer: LAD daemon did not finish, killing";
		mLadProcess.kill();
		mLadProcess.waitForFinished(1000);
	}
}

bool DspServer::processFrame(const DspChannel &channel, OutArgs &out, uint32_t bufferIdx)
{
	const auto dspAlgo = toDspAlgo(channel.algorithm);
	// Re-register when the algorithm, the pixel format or the line length
	// changes. The DSP's setup() selects the format converter and fixes the
	// stride from these, so a stale registration would decode the new channel's
	// frames with the previous session's/port's settings.
	if (dspAlgo != d->currentAlgo() || channel.format != d->currentFormat()
		|| channel.lineLength != d->currentLineLength()) {
		QLOG_INFO() << "DspServer: (re)registering algorithm" << dspAlgo << "format"
			    << static_cast<int>(channel.format) << "lineLength" << channel.lineLength;
		const AlgoDescriptor desc = {channel.format, channel.lineLength};
		d->registerAlgorithm(channel.algorithm, desc);
		d->setCurrentAlgo(dspAlgo, channel.format, channel.lineLength);
	}

	const bool ok = d->step(channel.inArgs, out, channel.inputBufferBase + bufferIdx);

	if (ok && dspAlgo == TRIK_CV_ALGORITHM_JPEG_ENCODER) {
		// Point at the shared output buffer: the bytes stay valid until the
		// next step() overwrites them, so a JPEG consumer must run synchronously
		// on the DSP thread right after this call (the pipeline delivers Jpeg
		// results via a DirectConnection) - no copy.
		out.jpegData = d->outStart();
	}

	if (ok && channel.videoOut && d->mFbOutput && d->mFbOutput->isOpen()) {
		d->mFbOutput->writeFrame(d->outStart());
	}

	return ok;
}

void DspServer::setVideoOutput(bool enabled)
{
	if (enabled && d->mFbOutput && !d->mFbOutput->isOpen()) {
		QLOG_INFO() << "DspServer: opening video display via HAL fb output";
		d->mFbOutput->open();
	} else if (!enabled && d->mFbOutput && d->mFbOutput->isOpen()) {
		QLOG_INFO() << "DspServer: closing video display via HAL fb output";
		d->mFbOutput->close();
	}
}

void DspServer::init()
{
	QLOG_INFO() << "DspServer: initializing";

	QEventLoop loop;
	bool ladOk = false;

	QTimer::singleShot(15000, &loop, [&]() {
		QLOG_ERROR() << "DspServer: timed out waiting for LAD daemon";
		loop.quit();
	});

	connect(&mLadProcess, &QProcess::started, &loop, [&]() {
		QLOG_INFO() << "DspServer: LAD daemon started";
		ladOk = true;
		loop.quit();
	});

	connect(&mLadProcess, &QProcess::errorOccurred, &loop, [&](QProcess::ProcessError error) {
		QLOG_ERROR() << "DspServer: failed to start LAD daemon:" << error;
		loop.quit();
	});

	using ExitStatus = QProcess::ExitStatus;
	connect(&mLadProcess, QOverload<int, ExitStatus>::of(&QProcess::finished), &loop,
		[&](int exitCode, ExitStatus status) {
		if (exitCode != 0 || status != ExitStatus::NormalExit) {
			QLOG_ERROR() << "DspServer: LAD daemon exited with code" << exitCode << "status" << status;
		} else {
			QLOG_INFO() << "DspServer: LAD daemon started (parent exited)";
			ladOk = true;
		}
		loop.quit();
	});

	mLadProcess.start(QStringLiteral("lad_omapl138"), QStringList(), QIODevice::ReadOnly);
	loop.exec();

	if (!ladOk) {
		QLOG_ERROR() << "DspServer: LAD daemon start failed, aborting init";
		Q_EMIT errorOccurred(QStringLiteral("LAD daemon start failed"));
		return;
	}

	if (!d->startIpc()) {
		QLOG_ERROR() << "DspServer: Ipc_start failed, aborting init";
		Q_EMIT errorOccurred(QStringLiteral("Ipc_start failed"));
		return;
	}

	if (d->setupMessageQueue() && d->mapSharedBuffers()) {
		QLOG_INFO() << "DspServer: message queue and shared buffers set up successfully";
		Q_EMIT successfullyInited();
	} else {
		QLOG_ERROR() << "DspServer: message queue or shared buffer setup failed";
		Q_EMIT errorOccurred(QStringLiteral("message queue or shared buffer setup failed"));
	}
}

void DspServer::setFbOutput(trikHal::FbOutputInterface *fb)
{
	d->mFbOutput.reset(fb);
}

}
