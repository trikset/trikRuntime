trikRuntime  [![Build Status](https://travis-ci.org/trikset/trikRuntime.svg?branch=master)](https://travis-ci.org/trikset/trikRuntime)
===========

Runtime for TRIK controller.

Consists of following subprojects:
- trikControl: library for interacting with robot hardware, provides interface for hardware as Qt classes with signals and slots, that can be used from other Qt programs or Qt Script.
- trikScriptRunner: library providing Qt Script interpreter that uses trikControl to enable interaction with hardware from scripts.
- trikCommunicator: library that provides network interface to run programs on a robot.
- trikRun: command-line utility to execute Qt Script files.
- trikServer: command-line server for network communications, uses trikCommunicator library.
- trikGui: user interface that can show various settings (like IP address), file system, run scripts, act as a server with trikCommunicator and so on.

Special thanks to:
- Nikita Batov (https://github.com/Batov) for I2C direct access example.
- Roman Kurbatov (https://github.com/romankurbatov) for his desktop environment code used in trikGui.
- Anna Semakova for various artwork in "media" folder.
