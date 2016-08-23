/* Copyright 2016 CyberTech Labs Ltd.
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

namespace trikGui {

/// Performs platform-specific querying of current low power threshold that must be set as boot option.
class PowerLevel
{
public:
	/// Contains values indicating settings for power threshold. Default value for TRIK is 'twentyVolt' meaning
	/// that board will automatically switch off when power voltage is lower than 10V (or 9V on older boards).
	enum class Level
	{
		/// Voltage settings for 6V scheme (used by VEX motors and other strange peripherals).
		sixVolt

		/// Voltage settings for 12V scheme (default).
		, twelveVolt
	};

	/// Retuns currently selected voltage threshold level by querying GPIO pin 56.
	static Level currentLevel();
};

}
