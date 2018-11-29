#!/bin/bash
set -euxo pipefail
case $TRAVIS_OS_NAME in
  osx)
    REQURED_PACKAGES="qt ccache pyenv"
    export HOMEBREW_NO_AUTO_UPDATE=1
    for pkg in $REQURED_PACKAGES ; do
      p="${pkg##*/}"
      p="${p%.*}"
      rmdir $CELLAR_CACHE_DIR/$p && brew install $pkg \
      || { brew unlink $p ; brew link --force $p ; }
    done
    pyenv update
    eval "$(pyenv init -)"
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
