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

namespace trikControl {

/// How aggressively stopping a video sensor tears the camera down.
///
/// These are bit flags OR-ed into the @c stop() argument. They are kept as a
/// plain enum (passed as @c int) so the PythonQt/JS bindings can forward them
/// without a custom QFlags metatype.
enum VideoSensorStopFlag {
	/// Only deactivate the DSP channel. The camera keeps streaming and stays
	/// acquired (the legacy stop(false) behaviour).
	StopNone = 0,

	/// streamoff the camera: stop streaming and drop the push subscription,
	/// but keep the device acquired (open). Other pull subscribers may still
	/// re-start streaming and grab frames without a full reopen.
	StopStream = 1 << 0,

	/// Full teardown: unsubscribe, release the camera and forget the port.
	StopAll = 1 << 1,
};

} // namespace trikControl
