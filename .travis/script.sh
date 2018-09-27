#!/bin/bash
set -euxo pipefail
case $TRAVIS_OS_NAME in
  osx)
     export PATH="/usr/local/opt/qt/bin:$PATH"
     export PATH="/usr/local/opt/ccache/libexec:$PATH"
     EXECUTOR=
    ;;
  linux)
      EXECUTOR="docker exec builder "
   ;;
  *) exit 1 ;;
esac

if [ "$VERA" = "true" ]; then $EXECUTOR ./runVera++.sh ; fi
$EXECUTOR bash -c "export CCACHE_DIR=$HOME/.ccache/$TRAVIS_OS_NAME-$CONFIG \
&& export CCACHE_CPP2=yes \
&& export CCACHE_SLOPPINESS=time_macros \
&& which g++ \
&& g++ --version \
&& which qmake \
&& qmake -query \
&& ccache -M 0 \
&& pkg-config --list-all \
&&  qmake -r CONFIG+=$CONFIG -Wall \
&&  make -k -j2 \
&& pushd tests && qmake -r CONFIG+=$CONFIG && make -j2 && popd \
&& cd bin/x86-$CONFIG && ls "

$EXECUTOR sh -c "export DISPLAY=:0 && cd bin/x86-$CONFIG && ./trikScriptRunnerTests$SUFFIX"
$EXECUTOR sh -c "export DISPLAY=:0 && cd bin/x86-$CONFIG && ./trikCommunicatorTests$SUFFIX"
$EXECUTOR sh -c "export DISPLAY=:0 && cd bin/x86-$CONFIG && ./trikKernelTests$SUFFIX"
