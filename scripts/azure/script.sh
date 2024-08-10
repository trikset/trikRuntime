#!/bin/bash -x
set -euxo pipefail
case $AGENT_OS in
  Darwin)
#    export PATH="$TRIK_QT/5.12.7/clang_64/bin:$PATH"
    export PATH="/usr/local/opt/ccache/libexec:$PATH"
    export PATH="/usr/local/opt/coreutils/libexec/gnubin:$PATH"
    export PATH="$( ls -1d $HOME/Qt/${TRIK_QT_VERSION}*/*/bin | head -n 1 ):$PATH"
    EXECUTOR="env TRIK_PYTHON3_VERSION_MINOR=$TRIK_PYTHON3_VERSION_MINOR "
    ;;
  Linux)
    EXECUTOR="docker exec --interactive -e BUILDDIR=$BUILDDIR -e CONFIG=$CONFIG -e AGENT_OS=$AGENT_OS builder "
   ;;
  *) exit 1 ;;
esac
export EXECUTOR

$EXECUTOR env VERA=$VERA TRANSLATIONS=$TRANSLATIONS scripts/azure/vera_translation.sh

$EXECUTOR bash -lic " set -x; \
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
&&  make -j 2 all \
&& env TRIK_PYTHONPATH=\`python3.\${TRIK_PYTHON3_VERSION_MINOR} -c 'import sys; import os; print(os.pathsep.join(sys.path))'\` \
    PYTHONMALLOC=malloc \
    ASAN_OPTIONS=disable_coredump=0:detect_stack_use_after_return=1:fast_unwind_on_malloc=0:use_sigaltstack=0 \
    LSAN_OPTIONS=suppressions=\$PWD/bin/lsan.supp:fast_unwind_on_malloc=0 \
    MSAN_OPTIONS=poison_in_dtor=1 \
    QT_QPA_PLATFORM=minimal \
    make -k -j 2 check \
&& ls bin"


exec bash -c 'eval timeout -k 10s 100s $EXECUTOR scripts/azure/runtests.sh trikKernelTests trikCameraPhotoTests trikCommunicatorTests trikJsRunnerTests $([ -x ./trikPyRunnerTests ] && echo trikPyRunnerTests || : )'
