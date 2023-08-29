#!/bin/bash
set -euxo pipefail

docker pull trikset/linux-builder
docker run -d --privileged -v $BUILD_SOURCESDIRECTORY:$BUILD_SOURCESDIRECTORY:rw -w `pwd` --name builder trikset/linux-builder Xvfb :0
docker exec builder bash -c 'export PATH=/usr/bin:/bin:/usr/sbin:/sbin ; python -V ; python3 -V ; rm -f ~/.bashrc ; git config --global --add safe.directory "*"'

mkdir -p "$BUILDDIR"
