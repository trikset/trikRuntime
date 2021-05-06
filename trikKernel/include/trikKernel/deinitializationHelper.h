/* Copyright 2016 Yurii Litvinov.
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
#include <QtGlobal>
#include <trikKernel/trikKernelDeclSpec.h>

namespace trikKernel {

/// RAII-style helper class that performs deferred deinitialization, intended to be used in main programs or test cases
/// (with Q_UNUSED macro). Runs zero-time event loop that processes all deleteLater() requests and so on, then
/// shuts down.
class TRIKKERNEL_EXPORT DeinitializationHelper
{
	Q_DISABLE_COPY(DeinitializationHelper)
public:
	DeinitializationHelper() = default;
	/// Here is all the magic - runs zero-time event loop when this object is destroyed.
	~DeinitializationHelper();
};

}
