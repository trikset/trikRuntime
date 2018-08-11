#!/bin/bash
set -euxo pipefail
case $TRAVIS_OS_NAME in
  osx)
    brew update
    brew install qt@5.5 python@2 ccache
    ;;
  linux)
    docker pull trikset/linux-builder
    if [[ "$CONFIG" == "debug" ]]
    then
      pip install --user codecov
    fi
    ;;
  *) exit 1 ;;
esac
pkg-config --list-all
