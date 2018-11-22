# Copyright 2014
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

TEMPLATE = subdirs

SUBDIRS = \
	minimalCppApp \
	selftest \
	thirdparty \
	trikCameraPhotoTests \
	trikCommunicatorTests \
	trikKernelTests \
	trikScriptRunnerTests \
	testUtils \

thirdparty.file = thirdparty/googletest.pro

trikKernelTests.depends = thirdparty testUtils
trikScriptRunnerTests.depends = thirdparty testUtils
trikCommunicatorTests.depends = thirdparty testUtils
selftest.depends = thirdparty testUtils
trikCameraPhotoTests.depends = thirdparty testUtils
