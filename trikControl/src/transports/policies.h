/* Copyright 2025 CyberTech Labs Ltd.
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

/// Tag type: device talks over a periphery communicator
/// using the periphery driver (USB or I2C).
struct CommunicatorPolicy {};

/// Tag type: device accesses on-module hardware directly via HAL
/// (e.g. sysfs PWM for servos, IIO for sensors on the same board).
struct InternalPolicy {};

} // namespace trikControl
