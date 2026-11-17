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

#include "trikRknnSession.h"

#include <chrono>

#include <QtCore/QByteArray>

using namespace trikHal;
using namespace trikHal::trik;

namespace {

RknnDataType mapType(rknn_tensor_type type)
{
	switch (type) {
	case RKNN_TENSOR_INT8:
		return RknnDataType::Int8;
	case RKNN_TENSOR_FLOAT16:
		return RknnDataType::Float16;
	case RKNN_TENSOR_FLOAT32:
		return RknnDataType::Float32;
	case RKNN_TENSOR_UINT8:
	default:
		return RknnDataType::Uint8;
	}
}

RknnLayout mapLayout(rknn_tensor_format format)
{
	return format == RKNN_TENSOR_NCHW ? RknnLayout::Nchw : RknnLayout::Nhwc;
}

RknnTensorInfo makeTensorInfo(const rknn_tensor_attr &attr)
{
	auto info = RknnTensorInfo {};
	for (uint32_t i = 0; i < attr.n_dims; ++i) {
		info.dims.append(attr.dims[i]);
	}
	info.layout = mapLayout(attr.fmt);
	info.type = mapType(attr.type);
	info.size = attr.size;
	info.sizeWithStride = attr.size_with_stride;
	info.wStride = attr.w_stride;
	return info;
}

QString errorText(const char *what, int code)
{
	return QStringLiteral("%1 failed: %2").arg(QString::fromLatin1(what)).arg(code);
}

} // namespace

TrikRknnSession::TrikRknnSession() = default;

TrikRknnSession::~TrikRknnSession()
{
	reset();
}

void TrikRknnSession::reset()
{
	if (mCtx != 0) {
		for (auto *mem : mOutputMems) {
			if (mem) {
				rknn_destroy_mem(mCtx, mem);
			}
		}
		if (mInputMem) {
			rknn_destroy_mem(mCtx, mInputMem);
		}
		rknn_destroy(mCtx);
	}

	mOutputMems.clear();
	mOutputAttrs.clear();
	mInputMem = nullptr;
	mInputAttr = rknn_tensor_attr {};
	mCtx = 0;
	mLoaded = false;
	mInput = RknnTensorInfo {};
	mOutputs.clear();
	mLastRunUs = 0;
}

bool TrikRknnSession::load(const QString &modelPath, QString *error)
{
	reset();

	auto path = modelPath.toLocal8Bit();
	// With size == 0 the second argument is a file path to the .rknn model.
	const auto initRet = rknn_init(&mCtx, path.data(), 0, 0, nullptr);
	if (initRet != RKNN_SUCC) {
		mCtx = 0;
		if (error) {
			*error = errorText("rknn_init", initRet);
		}
		return false;
	}

	auto ioNum = rknn_input_output_num {};
	auto ret = rknn_query(mCtx, RKNN_QUERY_IN_OUT_NUM, &ioNum, sizeof(ioNum));
	if (ret != RKNN_SUCC) {
		if (error) {
			*error = errorText("rknn_query(IN_OUT_NUM)", ret);
		}
		reset();
		return false;
	}

	if (ioNum.n_input != 1) {
		if (error) {
			*error = QStringLiteral("only single-input models are supported (got %1)").arg(ioNum.n_input);
		}
		reset();
		return false;
	}

	auto inputAttr = rknn_tensor_attr {};
	inputAttr.index = 0;
	ret = rknn_query(mCtx, RKNN_QUERY_INPUT_ATTR, &inputAttr, sizeof(inputAttr));
	if (ret != RKNN_SUCC) {
		if (error) {
			*error = errorText("rknn_query(INPUT_ATTR)", ret);
		}
		reset();
		return false;
	}
	mInput = makeTensorInfo(inputAttr);

	// Models exported with dynamic shapes (e.g. rknn_dynamic_shape_input_demo)
	// reject io-memory binding until a concrete input shape is committed. Static
	// models return an error to the query below, so this only runs for dynamic
	// ones. The queried input attr already carries the model's default shape.
	auto dynamicRange = rknn_input_range {};
	dynamicRange.index = 0;
	if (rknn_query(mCtx, RKNN_QUERY_INPUT_DYNAMIC_RANGE, &dynamicRange, sizeof(dynamicRange)) == RKNN_SUCC) {
		if (rknn_set_input_shapes(mCtx, 1, &inputAttr) == RKNN_SUCC) {
			auto refreshed = rknn_tensor_attr {};
			refreshed.index = 0;
			if (rknn_query(mCtx, RKNN_QUERY_INPUT_ATTR, &refreshed, sizeof(refreshed)) == RKNN_SUCC) {
				inputAttr = refreshed;
				mInput = makeTensorInfo(inputAttr);
			}
		}
	}

	// Bind the input buffer as uint8 RGB NHWC and let the runtime convert it to
	// the model's native type/layout, applying whatever preprocessing was baked
	// into the model at conversion (normalization and/or quantization).
	// pass_through MUST be 0 for that conversion to happen: with pass_through=1
	// the raw bytes would go straight into the input node, so an fp16 node would
	// misinterpret uint8 pixels. The RGA-written buffer is tightly packed uint8,
	// so the native byte size is rescaled to uint8 elements (2x for fp16, etc.).
	const auto nativeElementBytes = inputAttr.n_elems != 0 ? inputAttr.size / inputAttr.n_elems : 1;
	const auto nativeBytes = inputAttr.size_with_stride != 0 ? inputAttr.size_with_stride : inputAttr.size;
	const auto inputBytes = nativeElementBytes != 0 ? nativeBytes / nativeElementBytes : nativeBytes;

	mInputAttr = inputAttr;
	mInputAttr.type = RKNN_TENSOR_UINT8;
	mInputAttr.fmt = RKNN_TENSOR_NHWC;
	mInputAttr.pass_through = 0;
	mInputAttr.size = inputBytes;
	mInputAttr.size_with_stride = inputBytes;
	mInputMem = rknn_create_mem(mCtx, inputBytes);
	if (!mInputMem) {
		if (error) {
			*error = QStringLiteral("rknn_create_mem(input) failed");
		}
		reset();
		return false;
	}
	ret = rknn_set_io_mem(mCtx, mInputMem, &mInputAttr);
	if (ret != RKNN_SUCC) {
		if (error) {
			*error = errorText("rknn_set_io_mem(input)", ret);
		}
		reset();
		return false;
	}

	mOutputs.clear();
	mOutputs.reserve(static_cast<int>(ioNum.n_output));
	mOutputAttrs.clear();
	mOutputAttrs.reserve(static_cast<int>(ioNum.n_output));
	mOutputMems.clear();
	mOutputMems.reserve(static_cast<int>(ioNum.n_output));
	for (uint32_t i = 0; i < ioNum.n_output; ++i) {
		auto outputAttr = rknn_tensor_attr {};
		outputAttr.index = i;
		ret = rknn_query(mCtx, RKNN_QUERY_OUTPUT_ATTR, &outputAttr, sizeof(outputAttr));
		if (ret != RKNN_SUCC) {
			if (error) {
				*error = errorText("rknn_query(OUTPUT_ATTR)", ret);
			}
			reset();
			return false;
		}
		mOutputs.append(makeTensorInfo(outputAttr));

		// Ask the runtime to write de-quantized float32 straight into our buffer.
		// pass_through=0 is required to actually convert the native output
		// (int8/fp16) to float32 instead of handing us the raw native bytes.
		auto ioAttr = outputAttr;
		ioAttr.type = RKNN_TENSOR_FLOAT32;
		ioAttr.pass_through = 0;
		const auto outputBytes = static_cast<uint32_t>(ioAttr.n_elems * sizeof(float));
		auto *mem = rknn_create_mem(mCtx, outputBytes);
		if (!mem) {
			if (error) {
				*error = QStringLiteral("rknn_create_mem(output %1) failed").arg(i);
			}
			reset();
			return false;
		}
		ret = rknn_set_io_mem(mCtx, mem, &ioAttr);
		if (ret != RKNN_SUCC) {
			rknn_destroy_mem(mCtx, mem);
			if (error) {
				*error = errorText("rknn_set_io_mem(output)", ret);
			}
			reset();
			return false;
		}
		mOutputAttrs.append(ioAttr);
		mOutputMems.append(mem);
	}

	mLoaded = true;
	return true;
}

bool TrikRknnSession::isLoaded() const
{
	return mLoaded;
}

const RknnTensorInfo &TrikRknnSession::input() const
{
	return mInput;
}

const QVector<RknnTensorInfo> &TrikRknnSession::outputs() const
{
	return mOutputs;
}

int TrikRknnSession::inputBufferFd() const
{
	return mInputMem ? mInputMem->fd : -1;
}

uint8_t *TrikRknnSession::inputBufferData() const
{
	return mInputMem && mInputMem->virt_addr ? static_cast<uint8_t *>(mInputMem->virt_addr) : nullptr;
}

bool TrikRknnSession::run(QVector<RknnOutputView> *out, QString *error)
{
	if (!mLoaded || !mInputMem) {
		if (error) {
			*error = QStringLiteral("no model loaded");
		}
		return false;
	}

	if (out) {
		out->clear();
	}

	// The producer wrote the input through RGA (device) or CPU; flush it for
	// the NPU. This is a no-op for non-cacheable buffers.
	rknn_mem_sync(mCtx, mInputMem, RKNN_MEMORY_SYNC_TO_DEVICE);

	const auto start = std::chrono::steady_clock::now();
	const auto ret = rknn_run(mCtx, nullptr);
	if (ret != RKNN_SUCC) {
		if (error) {
			*error = errorText("rknn_run", ret);
		}
		return false;
	}
	mLastRunUs =
		std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();

	if (out) {
		out->reserve(mOutputMems.size());
		for (int i = 0; i < mOutputMems.size(); ++i) {
			auto *mem = mOutputMems[i];
			rknn_mem_sync(mCtx, mem, RKNN_MEMORY_SYNC_FROM_DEVICE);

			auto view = RknnOutputView {};
			view.index = i;
			view.type = RknnDataType::Float32;
			view.data = mem ? mem->virt_addr : nullptr;
			view.count = mOutputAttrs[i].n_elems;
			view.size = view.count * sizeof(float);
			out->append(view);
		}
	}

	return true;
}

qint64 TrikRknnSession::lastRunTimeUs() const
{
	return mLastRunUs;
}
