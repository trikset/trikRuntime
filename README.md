# Runtime for TRIK controller.

[![Build Status](https://img.shields.io/travis/trikset/trikRuntime/master.svg?maxAge=3600&style=for-the-badge&logo=linux&logoColor=green)](https://travis-ci.com/trikset/trikRuntime)
[![Build status](https://img.shields.io/appveyor/ci/iakov/trikruntime-cgd5y/master.svg?maxAge=3600&style=for-the-badge&logo=windows&logoColor=green)](https://ci.appveyor.com/project/iakov/trikruntime-cgd5y/branch/master)

[![Join the chat at https://gitter.im/trikset/trikRuntime](https://badges.gitter.im/trikset/trikRuntime.svg)](https://gitter.im/trikset/trikRuntime?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
[![codecov.io](https://codecov.io/github/trikset/trikRuntime/coverage.svg?branch=master)](https://codecov.io/github/trikset/trikRuntime?branch=master)
[![CoverityScan](https://scan.coverity.com/projects/7496/badge.svg)](https://scan.coverity.com/projects/trikset-trikruntime)
[![Mergify](https://img.shields.io/endpoint.svg?url=https://gh.mergify.io/badges/trikset/trikRuntime&style=flat)](ADD_URL_HERE)

===========


# Pre-build step

  ```git submodule update --init --recursive```

# Build
  PythonQt should be built with corresponding version of Python in mind.
  One can install `libpython3-dev` from destribution package manager, or use `pyenv`, and there is a helper script for this.

  ```shell
    qmake -r
    make -j 4
  ```

# Add translations
  [Details here](https://github.com/kleo-53/trikRuntime/blob/master/translations/ADDING_TRANSLATIONS.en.md)

# Out-of-source-tree build (split-build) is supported

===========
Consists of following subprojects:
- trikControl: library for interacting with robot hardware, provides interface for hardware as Qt classes with signals and slots, that can be used from other Qt programs or Qt Script.
- trikScriptRunner: library providing Qt Script interpreter that uses trikControl to enable interaction with hardware from scripts.
- trikCommunicator: library that provides network interface to run programs on a robot.
- trikRun: command-line utility to execute Qt Script files.
- trikServer: command-line server for network communications, uses trikCommunicator library.
- trikGui: user interface that can show various settings (like IP address), file system, run scripts, act as a server with trikCommunicator and so on.
- trikHAL: library with hardware abstraction layer.
- trikKernel: library with common code for all other projects.



Special thanks to:
- Nikita Batov (https://github.com/Batov) for I2C direct access example.
- Roman Kurbatov (https://github.com/romankurbatov) for his desktop environment code used in trikGui.
- Anna Semakova for various artwork in "media" folder.
