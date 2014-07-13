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
    Menu : 139
};

// Aliases for passing ports without quotes
var A1 = "A1";
var A2 = "A2";
var A3 = "A3";
var A4 = "A4";
var A5 = "A5";
var A6 = "A6";

var D1 = "D1";
var D2 = "D2";
var F1 = "F1";

var M1 = "M1";
var M2 = "M2";
var M3 = "M3";
var M4 = "M4";
var B1 = "B1";
var B2 = "B2";
var B3 = "B3";
var B4 = "B4";

var E1 = "E1";
var E2 = "E2";
var E3 = "E3";
var C1 = "C1";
var C2 = "C2";
var C3 = "C3";

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
