#!/bin/bash
set -euxo pipefail
case $TRAVIS_OS_NAME in
  osx)
    brew update
    brew install qt
    export PATH="/usr/local/opt/qt/bin:$PATH"
    ;;
  linux)
    if [[ "$CONFIG" == "debug" ]]
    then
      pip install --user codecov
    fi
    ;;
  *) exit 1 ;;
esac
