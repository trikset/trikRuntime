#!/bin/bash
set -euxo pipefail
case $TRAVIS_OS_NAME in
  osx)
    ;;
  linux)
    docker stop builder
    ;;
  *) exit 1 ;;
esac

[ -r build.log ] && tail -n 1000 build.log || true
