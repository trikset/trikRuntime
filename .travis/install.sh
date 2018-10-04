#!/bin/bash
set -euxo pipefail
case $TRAVIS_OS_NAME in
  osx)
    REQURED_PACKAGES="qt ccache python@2 python@3"
    for p in $REQURED_PACKAGES ; do
      rmdir $CELLAR_CACHE_DIR/$p && brew install $p \
      || brew link --force $p
    done
  ;;

  linux)
    docker pull trikset/linux-builder
    docker run -d -v $HOME:$HOME:rw -w `pwd` --name builder trikset/linux-builder Xvfb :0
    if [[ "$CONFIG" == "debug" ]]
    then
      pip install --user codecov
    fi
    ;;
  *) exit 1 ;;
esac
