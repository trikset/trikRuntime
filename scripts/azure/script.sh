#!/bin/bash -x
set -euxo pipefail
case $AGENT_OS in
  Darwin)
#    export PATH="$TRIK_QT/5.12.7/clang_64/bin:$PATH"
    export PATH="/usr/local/opt/ccache/libexec:$PATH"
    export PATH="/usr/local/opt/coreutils/libexec/gnubin:$PATH"
    export PATH="/usr/local/opt/qt@5/bin:$PATH"
    export TRIK_PYTHON3_VERSION_MINOR="$(python3 -V | sed 's#^Python [0-9]\.\([0-9]\)\.[0-9]*$#\1#g')"
    EXECUTOR=
    ;;
  Linux)
    EXECUTOR="docker exec --interactive builder "
   ;;
  *) exit 1 ;;
esac
export EXECUTOR
if "$VERA" ; then $EXECUTOR scripts/azure/runVera++.sh ; fi

if [ "$TRANSLATIONS" = "true" ] ; then $EXECUTOR bash -lic 'lupdate trikRuntime.pro' && $EXECUTOR scripts/checkStatus.sh ; fi

$EXECUTOR bash -lixc " \
   export CCACHE_DIR=$CCACHE_DIR \
&& export CCACHE_CPP2=yes \
&& export CCACHE_SLOPPINESS=time_macros \
&& eval 'export PKG_CONFIG_PATH=\`python3.\${TRIK_PYTHON3_VERSION_MINOR}-config --prefix\`/lib/pkgconfig' \
&& which g++ \
&& g++ --version \
&& which qmake \
&& qmake -query \
&& ccache -M 0 \
&& pkg-config --list-all \
&& { which python3 && python3 -V || true ; } \
&& { which python && python -V || true ; } \
&&  cd $BUILDDIR && qmake -r PYTHON3_VERSION_MINOR=\$TRIK_PYTHON3_VERSION_MINOR CONFIG+=$CONFIG -Wall $BUILD_SOURCESDIRECTORY/trikRuntime.pro $QMAKE_EXTRA \
&&  make -k -j2 \
&& cd bin/x86-$CONFIG && ls "

exec timeout -k 10s 100s scripts/azure/runtests.sh trikKernelTests trikCameraPhotoTests trikCommunicatorTests trikJsRunnerTests trikPyRunnerTests
