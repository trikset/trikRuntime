#!/bin/bash
set -euxo pipefail
case $TRAVIS_OS_NAME in
  osx)
    export PATH="/usr/local/opt/qt/bin:$PATH"
    export PATH="/usr/local/opt/ccache/libexec:$PATH"
    export PATH="/usr/local/opt/coreutils/libexec/gnubin:$PATH"
    export PATH="$(pyenv root)/bin:$PATH"
    eval "$(pyenv init -)"
    export PKG_CONFIG_PATH="$(python3-config --prefix)/lib/pkgconfig"
    EXECUTOR="scripts/with_pyenv "
    ;;
  linux)
    EXECUTOR="docker exec --interactive builder "
   ;;
  *) exit 1 ;;
esac

if [ "$VERA" = "true" ]; then $EXECUTOR ./runVera++.sh ; fi
if [ "$VERA" = "true" ]; then git diff --name-only ${TRAVIS_COMMIT_RANGE} \
	| xargs -r file -i | sed -e "s|\(.*\):.*text/x-c.*|\1|g" -e "/:/d"  \
	| $EXECUTOR vera++ --error --root vera++ --profile strict ; fi
if [ "$TRANSLATIONS" = "true" ] ; then $EXECUTOR lupdate trikRuntime.pro && $EXECUTOR scripts/checkStatus.sh ; fi

$EXECUTOR bash -ic "{ [ -r /root/.bashrc ] && source /root/.bashrc || true ; } ; \
   export CCACHE_DIR=$HOME/.ccache/$TRAVIS_OS_NAME-$CONFIG \
&& export CCACHE_CPP2=yes \
&& export CCACHE_SLOPPINESS=time_macros \
&& eval \"\`pyenv init -\`\" \
&& eval 'export PKG_CONFIG_PATH=\`python3-config --prefix\`/lib/pkgconfig' \
&& which g++ \
&& g++ --version \
&& which qmake \
&& qmake -query \
&& ccache -M 0 \
&& pyenv root \
&& pyenv versions \
&& pkg-config --list-all \
&& { which python3 && python3 -V || true ; } \
&& { which python && python -V || true ; } \
&&  cd $BUILDDIR && qmake -r CONFIG+=$CONFIG -Wall $TRAVIS_BUILD_DIR/trikRuntime.pro $QMAKE_EXTRA \
&&  make -k -j2 \
&& cd bin/x86-$CONFIG && ls "

for t in trikKernelTests trikCameraPhotoTests trikCommunicatorTests trikScriptRunnerTests
  do
    $EXECUTOR env DISPLAY=:0 \
    ASAN_OPTIONS="$( [[ $TRAVIS_OS_NAME == linux ]] && echo detect_leaks=1 || :):detect_stack_use_after_return=1:fast_unwind_on_malloc=0:disable_coredump=0:symbolize=1:use_signalstack=0" \
    LSAN_OPTIONS="suppressions=lsan.supp:fast_unwind_on_malloc=0" sh -xc \
    "cd  $BUILDDIR/bin/x86-$CONFIG && \
     { \
       errCode=0 ; \
       ulimit -c unlimited ; \
       ./$t -platform minimal || errCode=\$? ; \
       [ "$TRAVIS_OS_NAME" = linux -a -e core ] && gdb ./$t core -ex 'thread apply all bt' -ex 'quit'  || true ; \
       rm -f core ; \
       ( exit \$errCode ) ; \
     } "
  done
