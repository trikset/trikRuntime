#trikRuntime

[![Join the chat at https://gitter.im/trikset/trikRuntime](https://badges.gitter.im/trikset/trikRuntime.svg)](https://gitter.im/trikset/trikRuntime?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
[![Build Status](https://travis-ci.org/trikset/trikRuntime.svg?branch=master)](https://travis-ci.org/trikset/trikRuntime)
[![codecov.io](https://codecov.io/github/trikset/trikRuntime/coverage.svg?branch=master)](https://codecov.io/github/trikset/trikRuntime?branch=master)[![CoverityScan](https://scan.coverity.com/projects/7496/badge.svg)](https://scan.coverity.com/projects/trikset-trikruntime)

===========

Runtime for TRIK controller.

Consists of following subprojects:
- trikControl: library for interacting with robot hardware, provides interface for hardware as Qt classes with signals and slots, that can be used from other Qt programs or Qt Script.
- trikScriptRunner: library providing Qt Script interpreter that uses trikControl to enable interaction with hardware from scripts.
- trikCommunicator: library that provides network interface to run programs on a robot.
- trikRun: command-line utility to execute Qt Script files.
- trikServer: command-line server for network communications, uses trikCommunicator library.
- trikGui: user interface that can show various settings (like IP address), file system, run scripts, act as a server with trikCommunicator and so on.
- trikKernel: library with common code for all other projects.

Special thanks to:
- Nikita Batov (https://github.com/Batov) for I2C direct access example.
- Roman Kurbatov (https://github.com/romankurbatov) for his desktop environment code used in trikGui.
- Anna Semakova for various artwork in "media" folder.
