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
#include <QtCore/QLibrary>

using namespace trikHal;
using namespace trikHal::trik;

namespace trikHal {
namespace trik {

namespace {

template <typename T>
bool resolve(QLibrary &library, T &function, const char *name)
{
	function = reinterpret_cast<T>(library.resolve(name));
	return function != nullptr;
}

} // namespace

/// Internal loader for the RKNN C API.
///
/// The entry points are resolved from librknnrt.so at runtime, so libtrikHal
/// has no DT_NEEDED dependency on the runtime. QLibrary/dlopen reference-count
/// the shared object: the first session loads it, later sessions reuse the same
/// mapping. When the library is missing, load() fails and createRknnSession()
/// reports the NPU as unavailable.
struct RknnApi {
	QLibrary library;

	decltype(&rknn_init) init = nullptr;
	decltype(&rknn_destroy) destroy = nullptr;
	decltype(&rknn_query) query = nullptr;
	decltype(&rknn_set_input_shapes) setInputShapes = nullptr;
	decltype(&rknn_create_mem) createMem = nullptr;
	decltype(&rknn_destroy_mem) destroyMem = nullptr;
	decltype(&rknn_set_io_mem) setIoMem = nullptr;
	decltype(&rknn_mem_sync) memSync = nullptr;
	decltype(&rknn_run) run = nullptr;

	/// Loads librknnrt.so from the standard library path and resolves the API.
	bool load()
	{
		if (mLoaded) {
			return true;
		}

		library.setFileName(QStringLiteral("librknnrt.so"));
		if (!library.load()) {
			mError = library.errorString();
			return false;
		}

		if (!resolve(library, init, "rknn_init")
			|| !resolve(library, destroy, "rknn_destroy")
			|| !resolve(library, query, "rknn_query")
			|| !resolve(library, setInputShapes, "rknn_set_input_shapes")
			|| !resolve(library, createMem, "rknn_create_mem")
			|| !resolve(library, destroyMem, "rknn_destroy_mem")
			|| !resolve(library, setIoMem, "rknn_set_io_mem")
			|| !resolve(library, memSync, "rknn_mem_sync")
			|| !resolve(library, run, "rknn_run")) {
			mError = QStringLiteral("librknnrt.so does not export the required RKNN API");
			library.unload();
			return false;
		}

		mLoaded = true;
		return true;
	}

	const QString &errorString() const
	{
		return mError;
	}

private:
	bool mLoaded = false;
	QString mError;
};

} // namespace trik
} // namespace trikHal

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

TrikRknnSession::TrikRknnSession()
	: mApi(std::make_unique<RknnApi>())
{
}

TrikRknnSession::~TrikRknnSession()
{
	reset();
}

bool TrikRknnSession::isAvailable() const
{
	return ensureApi(nullptr);
}

bool TrikRknnSession::ensureApi(QString *error) const
{
	if (!mApi) {
		mApi = std::make_unique<RknnApi>();
	}

	if (mApi->load()) {
		return true;
	}

	if (error) {
		*error = QStringLiteral("librknnrt.so is not available: %1").arg(mApi->errorString());
	}
	return false;
}

void TrikRknnSession::reset()
{
	if (mCtx != 0 && mApi) {
		for (auto *mem : mOutputMems) {
			if (mem) {
				mApi->destroyMem(mCtx, mem);
			}
		}
		if (mInputMem) {
			mApi->destroyMem(mCtx, mInputMem);
		}
		mApi->destroy(mCtx);
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

	if (!ensureApi(error)) {
		return false;
	}

	auto path = modelPath.toLocal8Bit();
	// With size == 0 the second argument is a file path to the .rknn model.
	const auto initRet = mApi->init(&mCtx, path.data(), 0, 0, nullptr);
	if (initRet != RKNN_SUCC) {
		mCtx = 0;
		if (error) {
			*error = errorText("rknn_init", initRet);
		}
		return false;
	}

	auto ioNum = rknn_input_output_num {};
	auto ret = mApi->query(mCtx, RKNN_QUERY_IN_OUT_NUM, &ioNum, sizeof(ioNum));
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
	ret = mApi->query(mCtx, RKNN_QUERY_INPUT_ATTR, &inputAttr, sizeof(inputAttr));
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
	if (mApi->query(mCtx, RKNN_QUERY_INPUT_DYNAMIC_RANGE, &dynamicRange, sizeof(dynamicRange)) == RKNN_SUCC) {
		if (mApi->setInputShapes(mCtx, 1, &inputAttr) == RKNN_SUCC) {
			auto refreshed = rknn_tensor_attr {};
			refreshed.index = 0;
			if (mApi->query(mCtx, RKNN_QUERY_INPUT_ATTR, &refreshed, sizeof(refreshed)) == RKNN_SUCC) {
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
	mInputMem = mApi->createMem(mCtx, inputBytes);
	if (!mInputMem) {
		if (error) {
			*error = QStringLiteral("rknn_create_mem(input) failed");
		}
		reset();
		return false;
	}
	ret = mApi->setIoMem(mCtx, mInputMem, &mInputAttr);
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
		ret = mApi->query(mCtx, RKNN_QUERY_OUTPUT_ATTR, &outputAttr, sizeof(outputAttr));
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
		auto *mem = mApi->createMem(mCtx, outputBytes);
		if (!mem) {
			if (error) {
				*error = QStringLiteral("rknn_create_mem(output %1) failed").arg(i);
			}
			reset();
			return false;
		}
		ret = mApi->setIoMem(mCtx, mem, &ioAttr);
		if (ret != RKNN_SUCC) {
			mApi->destroyMem(mCtx, mem);
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
	mApi->memSync(mCtx, mInputMem, RKNN_MEMORY_SYNC_TO_DEVICE);

	const auto start = std::chrono::steady_clock::now();
	const auto ret = mApi->run(mCtx, nullptr);
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
			mApi->memSync(mCtx, mem, RKNN_MEMORY_SYNC_FROM_DEVICE);

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
