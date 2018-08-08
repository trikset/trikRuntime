#!/bin/bash
set -euxo pipefail
case $TRAVIS_OS_NAME ; in
  osx)
    brew update
    brew install qt
    ;;
  linux)
    if [[ "$CONFIG" == "debug" ]]
    then
      pip install --user codecov
    fi
    ;;
  *) exit 1 ;;
esac
