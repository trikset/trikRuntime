#!/bin/bash
set -euxo pipefail
case $TRAVIS_OS_NAME in
  osx)
    REQUIRED_PACKAGES="ccache coreutils python3"
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
    [ -d $TRIK_QT/5.12.4 ] || env TRIK_QT_INSTALL_DIR="$TRIK_QT" scripts/install_qt_mac.sh >> build.log 2>&1
  ;;

  linux)
    docker pull trikset/linux-builder
    docker run -d --privileged -v $HOME:$HOME:rw -w `pwd` --name builder trikset/linux-builder Xvfb :0
    docker exec builder bash -c 'export PATH=/usr/bin:/bin:/usr/sbin:/sbin ; python -V ; python3 -V ; rm -f ~/.bashrc'
  ;;
  *) exit 1 ;;
esac
mkdir -p "$BUILDDIR"
