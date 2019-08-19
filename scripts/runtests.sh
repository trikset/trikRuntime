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

PYTHONPATH=$($EXECUTOR bash -ic "{ [ -r /root/.bashrc ] && source /root/.bashrc || true ; } ; \
	python3 -c 'import sys; import os; print(os.pathsep.join(sys.path))'")

for t in "$@"
  do
    $EXECUTOR env DISPLAY=:0 \
    ASAN_OPTIONS="$( [[ $TRAVIS_OS_NAME == linux ]] && echo 'detect_leaks=1:disable_coredump=0:' || :)detect_stack_use_after_return=1:fast_unwind_on_malloc=0:symbolize=1:use_signalstack=0" \
    LSAN_OPTIONS=suppressions=lsan.supp:fast_unwind_on_malloc=0 \
    PYTHONPATH=$PYTHONPATH \
    sh -xc \
    "cd  $BUILDDIR/bin/x86-$CONFIG && \
     { \
       errCode=0 ; \
       ulimit -c unlimited ; \
       if [ $TRAVIS_OS_NAME = linux ] ; then echo core > /proc/sys/kernel/core_pattern ; echo 0 > /proc/sys/kernel/core_uses_pid; fi ; \
       ./$t -platform minimal || errCode=\$? ; \
       [ $TRAVIS_OS_NAME = linux -a -e core ] && gdb ./$t core -ex 'thread apply all bt' -ex 'quit'  || true ; \
       rm -f core* ; \
       ( exit \$errCode ) ; \
     } "
  done
