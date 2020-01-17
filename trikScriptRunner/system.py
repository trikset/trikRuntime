# Copyright 2019 CyberTech Labs Ltd. & Andrei Khodko
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
import types

/* global script_cpp */
script = types.SimpleNamespace()
script.random = script_cpp.random
script.wait = script_cpp.wait
script.time = script_cpp.time
script.timer = script_cpp.timer
script.system = script_cpp.system
script.writeToFile = script_cpp.writeToFile
script.writeData = script_cpp.writeData
script.readAll = script_cpp.readAll
script.removeFile = script_cpp.removeFile
script.run = script_cpp.run
script.quit = script_cpp.quit
script.reset = script_cpp.reset
script.getPhoto = script_cpp.getPhoto

class KeysEnum():
    Left = 105
    Up = 103
    Down = 108
    Enter = 28
    Right = 106
    Power = 116
    Esc = 1

class Events():
    Sync = 0
    Key = 1
    Relative = 2
    Absolute = 3
    Misc = 4

class MouseEventCodes():
    X = 0
    Y = 1
    Wheel = 8

    LeftBtn = 272
    RightBtn = 273
    MiddleBtn = 274

class PadEventCodes():
    BtnA = 304
    BtnB = 305
    BtnC = 306
    BtnX = 307
    BtnY = 308
    BtnZ = 309

    BtnTL = 310
    BtnTR = 311
    BtnTL2 = 312
    BtnTR2 = 313
    BtnSelect = 314
    BtnStart = 315

    X = 0
    Y = 1
    Z = 2
    Rx = 3
    Ry = 4
    Rz = 5

    Throttle = 6
    Rudder = 7
    Wheel = 8
    Gas = 9
    Break = 10

    Hat0X = 16
    Hat0Y = 17
    Hat1X = 18
    Hat1Y = 19
