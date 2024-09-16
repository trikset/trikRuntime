#!/bin/bash -x
set -euxo pipefail
case $TRAVIS_OS_NAME in
  osx)
#    export PATH="$TRIK_QT/5.12.7/clang_64/bin:$PATH"
    export PATH="/usr/local/opt/ccache/libexec:$PATH"
    export PATH="/usr/local/opt/coreutils/libexec/gnubin:$PATH"
    export PATH="/usr/local/opt/qt/bin:$PATH"
    EXECUTOR=
    ;;
  linux)
    EXECUTOR="docker exec -e TRAVIS_COMMIT_RANGE=${TRAVIS_COMMIT_RANGE} --interactive builder "
   ;;
  *) exit 1 ;;
esac
export EXECUTOR
if "$VERA" ; then $EXECUTOR ./runVera++.sh ; fi

if [ "$TRANSLATIONS" = "true" ] ; then $EXECUTOR bash -lic 'lupdate trikRuntime.pro' && $EXECUTOR scripts/checkStatus.sh ; fi

$EXECUTOR bash -lic " set -x; \
   export CCACHE_DIR=$HOME/.ccache/$TRAVIS_OS_NAME-$CONFIG \
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
&&  cd $BUILDDIR && qmake -r PYTHON3_VERSION_MINOR=\$TRIK_PYTHON3_VERSION_MINOR CONFIG+=$CONFIG -Wall $TRAVIS_BUILD_DIR/trikRuntime.pro $QMAKE_EXTRA \
&& env TRIK_PYTHONPATH=\`python3.\${TRIK_PYTHON3_VERSION_MINOR} -c 'import sys; import os; print(os.pathsep.join(sys.path))'\` \
    DISPLAY=:0 \
    PYTHONMALLOC=malloc \
    ASAN_OPTIONS=disable_coredump=0:detect_stack_use_after_return=1:fast_unwind_on_malloc=0:symbolize=1:use_sigaltstack=0 \
    LSAN_OPTIONS=suppressions=\$PWD/bin/lsan.supp:fast_unwind_on_malloc=0 \
    MSAN_OPTIONS=poison_in_dtor=1 \
    make check -k -j2 \
&& ls bin/x86-$CONFIG"

#exec timeout -k 10s 100s scripts/runtests.sh trikKernelTests trikCameraPhotoTests trikCommunicatorTests trikJsRunnerTests trikPyRunnerTests
