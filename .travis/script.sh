#!/bin/bash
set -euxo pipefail
case $TRAVIS_OS_NAME in
  osx)
     export PATH="/usr/local/opt/qt@5.5/bin:$PATH"
     EXECUTOR=
    ;;
  linux)
      docker run -d -v `pwd`:`pwd` -w `pwd` --name builder trikset/linux-builder Xvfb :0
      EXECUTOR="docker exec builder "
   ;;
  *) exit 1 ;;
esac

if [ "$VERA" = "true" ]; then $EXECUTOR ./runVera++.sh ; fi
$EXECUTOR qmake -r CONFIG+=$CONFIG CONFIG+=no-sanitizers
$EXECUTOR make -j2
$EXECUTOR sh -c "cd tests && qmake -r CONFIG+=$CONFIG CONFIG+=no-sanitizers"
$EXECUTOR sh -c "cd tests && make -j2"
$EXECUTOR sh -c "cd bin/x86-$CONFIG && ls"
$EXECUTOR sh -c "export DISPLAY=:0 && cd bin/x86-$CONFIG && ./trikScriptRunnerTests$SUFFIX"
$EXECUTOR sh -c "export DISPLAY=:0 && cd bin/x86-$CONFIG && ./trikCommunicatorTests$SUFFIX"
$EXECUTOR sh -c "export DISPLAY=:0 && cd bin/x86-$CONFIG && ./trikKernelTests$SUFFIX"
