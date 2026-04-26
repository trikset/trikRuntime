/* Copyright 2024 CyberTech Labs Ltd.
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

#include "trikCppProgramInterface.h"

/// User C++ program template.
/// Rename the class, implement startProgram(), and call TRIK_REGISTER_CPP_PROGRAM.
/// The runtime instantiates the class via the factory and runs startProgram()
/// in a dedicated thread; set mBrick, mMailbox, mScriptControl are available as fields.
class MyProgram : public trikScriptRunner::TrikCppProgram
{
public:
	using TrikCppProgram::TrikCppProgram; // inherit constructor

	void povorot_lev()
	{
		mBrick->motor("M1")->brake();
		mBrick->motor("M2")->brake();
		mBrick->motor("M3")->brake();
		mBrick->motor("M4")->brake();

		mScriptControl->wait(555);

		mBrick->encoder("E1")->reset();
		mBrick->encoder("E2")->reset();
		mBrick->encoder("E3")->reset();
		mBrick->encoder("E4")->reset();

		mBrick->motor("M3")->setPower(70);
		mBrick->motor("M4")->setPower(-70);

		while (!(mBrick->encoder("E3")->read() < -150)) {
			mScriptControl->wait(10);
		}

		mBrick->motor("M1")->powerOff();
		mBrick->motor("M2")->powerOff();
		mBrick->motor("M3")->powerOff();
		mBrick->motor("M4")->powerOff();

		mScriptControl->wait(555);

		mBrick->encoder("E1")->reset();
		mBrick->encoder("E2")->reset();
		mBrick->encoder("E3")->reset();
		mBrick->encoder("E4")->reset();

		mBrick->motor("M3")->setPower(50);
		mBrick->motor("M4")->setPower(50);

		while (!(mBrick->encoder("E3")->read() < -275)) {
			mScriptControl->wait(10);
		}

		mBrick->motor("M1")->brake();
		mBrick->motor("M2")->brake();
		mBrick->motor("M3")->brake();
		mBrick->motor("M4")->brake();

		start = 1;
	}

	void startProgram(const std::atomic<bool> &isTerminate) override
	{
		mBrick->motor("M3")->setPower(70);
		mBrick->motor("M4")->setPower(-70);

		mScriptControl->wait(500);

		mBrick->motor("M1")->brake();
		mBrick->motor("M2")->brake();
		mBrick->motor("M3")->brake();
		mBrick->motor("M4")->brake();

		start = 0;
		int cth_bk = 0;

		mBrick->encoder("E1")->reset();
		mBrick->encoder("E2")->reset();
		mBrick->encoder("E3")->reset();
		mBrick->encoder("E4")->reset();

		while (!isTerminate) {
			if (mBrick->sensor("A1")->read() > 10) {
				if (mBrick->sensor("A2")->read() > 10) {
					mBrick->motor("M1")->brake();
					mBrick->motor("M2")->brake();
					mBrick->motor("M3")->brake();
					mBrick->motor("M4")->brake();

					if (start == 0) {
						povorot_lev();
					} else {
						if (cth_bk == 1) {  // Исправлено: в JS было = вместо ==
							povorot_lev();
						} else {
							mBrick->motor("M4")->setPower(-20);
							mBrick->motor("M3")->setPower(20);
							mScriptControl->wait(10);
							cth_bk = cth_bk + 1;
						}
						mBrick->motor("M1")->powerOff();
						mBrick->motor("M2")->powerOff();
						mBrick->motor("M3")->powerOff();
						mBrick->motor("M4")->powerOff();
					}
				} else {
					mBrick->motor("M3")->setPower(0);
					mBrick->motor("M4")->setPower(-70);
				}
			} else {
				if (mBrick->sensor("A2")->read() > 10) {
					mBrick->motor("M3")->setPower(70);
					mBrick->motor("M4")->setPower(0);
				} else {
					mBrick->motor("M3")->setPower(70);
					mBrick->motor("M4")->setPower(-70);
				}
			}
		}
	}

private:
	int start = 0;
};

// Register MyProgram as the entry point of this shared library.
TRIK_REGISTER_CPP_PROGRAM(MyProgram)
