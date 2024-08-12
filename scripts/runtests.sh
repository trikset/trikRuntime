#!/bin/bash
set -euxo pipefail

TRIK_PYTHONPATH=$($EXECUTOR python3 -c 'import sys; import os; print(os.pathsep.join(sys.path))')

for t in "$@"
  do
    $EXECUTOR env DISPLAY=:0 \
    ASAN_OPTIONS="detect_leaks=0:disable_coredump=0:detect_stack_use_after_return=1:fast_unwind_on_malloc=0:symbolize=1:use_sigaltstack=0" \
    LSAN_OPTIONS=suppressions=lsan.supp:fast_unwind_on_malloc=0 \
    MSAN_OPTIONS=poison_in_dtor=1 \
    TRIK_PYTHONPATH=$TRIK_PYTHONPATH \
    sh -xc \
    "cd  $BUILDDIR/bin && \
     { \
       errCode=0 ; \
       if [ $TRAVIS_OS_NAME = linux ] ; then ulimit -c unlimited ; echo core > /proc/sys/kernel/core_pattern ; echo 0 > /proc/sys/kernel/core_uses_pid; fi ; \
       ./$t -platform minimal || errCode=\$? ; \
       [ $TRAVIS_OS_NAME = linux -a -e core ] && gdb ./$t core -ex 'thread apply all bt' -ex 'quit'  || true ; \
       rm -f core* ; \
       ( exit \$errCode ) ; \
     } "
  done
