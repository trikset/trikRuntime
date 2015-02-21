/* Copyright 2013 - 2014 CyberTech Labs Ltd.
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

var KeysEnum = {
    Left : 105,
    Up : 103,
    Down : 108,
    Enter : 28,
    Right : 106,
    Power : 116,
    Esc : 1
};

// Aliases for passing ports without quotes
var aliases = ["A1", "A2", "A3", "A4", "A5", "A6"
        , "D1", "D2", "F1"
        , "M1", "M2", "M3", "M4"
        , "B1", "B2", "B3", "B4"
        , "E1", "E2", "E3"
	, "C1", "C2", "C3"];

aliases.forEach(function(port) { this[port] = port; });

// String.format function implementation.
// Taken from http://stackoverflow.com/questions/610406/javascript-equivalent-to-printf-string-format
if (!String.prototype.format) {
  String.prototype.format = function() {
    var args = arguments;
    return this.replace(/{(\d+)}/g, function(match, number) {
      return typeof args[number] != 'undefined'
        ? args[number]
        : match
      ;
    });
  };
}

script.repeat = function(mscs, f) {
  var timer = script.timer(mscs);
  timer.timeout.connect(f);
}

brick.smile = function() {
  brick.display().showImage('media/trik_smile_normal.png');
}

brick.sadSmile = function() {
  brick.display().showImage('media/trik_smile_sad.png');
}

