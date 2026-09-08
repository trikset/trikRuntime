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

#include <trik/sensors/cv_algorithm.h>
#include <trik/sensors/cv_algorithm_args.h>
#include <trik/sensors/video_format.h>
#include <trikKernel/videoUtils.h>

#include "dspTypes.h"

namespace trikDsp {

// -- Algorithm conversion

inline Algorithm fromDspAlgo(enum trik_cv_algorithm a)
{
	switch (a) {
	case TRIK_CV_ALGORITHM_MOTION_SENSOR:     return Algorithm::Motion;
	case TRIK_CV_ALGORITHM_EDGE_LINE_SENSOR:  return Algorithm::EdgeLine;
	case TRIK_CV_ALGORITHM_LINE_SENSOR:       return Algorithm::Line;
	case TRIK_CV_ALGORITHM_OBJECT_SENSOR:     return Algorithm::Object;
	case TRIK_CV_ALGORITHM_MXN_SENSOR:        return Algorithm::Mxn;
	case TRIK_CV_ALGORITHM_JPEG_ENCODER:      return Algorithm::Jpeg;
	default:                                  return Algorithm::None;
	}
}

inline enum trik_cv_algorithm toDspAlgo(Algorithm a)
{
	switch (a) {
	case Algorithm::Motion:   return TRIK_CV_ALGORITHM_MOTION_SENSOR;
	case Algorithm::EdgeLine: return TRIK_CV_ALGORITHM_EDGE_LINE_SENSOR;
	case Algorithm::Line:     return TRIK_CV_ALGORITHM_LINE_SENSOR;
	case Algorithm::Object:   return TRIK_CV_ALGORITHM_OBJECT_SENSOR;
	case Algorithm::Mxn:      return TRIK_CV_ALGORITHM_MXN_SENSOR;
	case Algorithm::Jpeg:     return TRIK_CV_ALGORITHM_JPEG_ENCODER;
	default:                  return TRIK_CV_ALGORITHM_NONE;
	}
}

inline enum VideoFormat toDspVideoFormat(PixelFormat fmt)
{
	switch (fmt) {
	case PixelFormat::Nv16: return NV16;
	case PixelFormat::Yuyv: return YUV422;
	default:                return Unknown;
	}
}

// -- In/Out args conversion

inline InArgs fromDspInArgs(const struct trik_cv_algorithm_in_args &src)
{
	InArgs dst;
	dst.autoDetect = src.auto_detect_hsv;
	dst.params.hue.from = src.detect_hue_from;
	dst.params.hue.to = src.detect_hue_to;
	dst.params.saturation.from = src.detect_sat_from;
	dst.params.saturation.to = src.detect_sat_to;
	dst.params.value.from = src.detect_val_from;
	dst.params.value.to = src.detect_val_to;
	dst.m = src.extra_inArgs.mxnParams.m_m;
	dst.n = src.extra_inArgs.mxnParams.m_n;
	dst.jpegQuality = src.jpeg_image_quality;
	dst.ifBlackAndWhite = src.if_black_and_white;
	return dst;
}

inline struct trik_cv_algorithm_in_args toDspInArgs(const InArgs &src)
{
	struct trik_cv_algorithm_in_args dst = {};
	dst.auto_detect_hsv = src.autoDetect;
	dst.detect_hue_from = src.params.hue.from;
	dst.detect_hue_to = src.params.hue.to;
	dst.detect_sat_from = src.params.saturation.from;
	dst.detect_sat_to = src.params.saturation.to;
	dst.detect_val_from = src.params.value.from;
	dst.detect_val_to = src.params.value.to;
	dst.extra_inArgs.mxnParams.m_m = src.m;
	dst.extra_inArgs.mxnParams.m_n = src.n;
	dst.jpeg_image_quality = src.jpegQuality;
	dst.if_black_and_white = src.ifBlackAndWhite;
	return dst;
}

inline OutArgs fromDspOutArgs(const struct trik_cv_algorithm_out_args &src)
{
	OutArgs dst;
	dst.location.x = src.targets[0].out_target.targetLocation.x;
	dst.location.y = src.targets[0].out_target.targetLocation.y;
	dst.location.size = src.targets[0].out_target.targetLocation.size;
	dst.detected.hue.from = src.detect_hue_from;
	dst.detected.hue.to = src.detect_hue_to;
	dst.detected.saturation.from = src.detect_sat_from;
	dst.detected.saturation.to = src.detect_sat_to;
	dst.detected.value.from = src.detect_val_from;
	dst.detected.value.to = src.detect_val_to;
	for (int i = 0; i < 9; ++i)
		dst.colors[i] = src.targets[0].out_target.targetColors.m_colors[i];
	dst.jpegSize = src.jpeg_size;
	return dst;
}

}
