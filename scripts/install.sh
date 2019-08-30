#!/bin/bash
set -euxo pipefail
case $TRAVIS_OS_NAME in
  osx)
    REQUIRED_PACKAGES="ccache pyenv coreutils"
    export HOMEBREW_TEMP="$HOME/homebrew.tmp"
    export HOMEBREW_LOGS="$HOMEBREW_TEMP"
    #To turn autoupdate on use `unset HOMEBREW_NO_AUTO_UPDATE` in a sub-shell before `breq install`
    export HOMEBREW_NO_AUTO_UPDATE=1
    for pkg in $REQUIRED_PACKAGES ; do
      p="${pkg##*/}"
      p="${p%.*}"
      rmdir $CELLAR_CACHE_DIR/$p && \
      ( brew install $pkg || echo "Failed to install $pkg" ) \
      || { brew unlink $p ; brew link --force $p ; }
    done
    [ -d $TRIK_QT/5.12.4 ] || env TRIK_QT_INSTALL_DIR="$TRIK_QT" scripts/install_qt_mac.sh
  ;;

  linux)
    docker pull trikset/linux-builder
    docker run -d -v $HOME:$HOME:rw -w `pwd` --name builder trikset/linux-builder Xvfb :0
  ;;
  *) exit 1 ;;
esac
mkdir -p "$BUILDDIR"
