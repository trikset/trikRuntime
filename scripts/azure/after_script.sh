#!/bin/bash
set -euxo pipefail
case $AGENT_OS in
  Darwin)
    ;;
  Linux)
    docker stop builder
    ;;
  *) exit 1 ;;
esac

[ -r build.log ] && tail -n 1000 build.log || true