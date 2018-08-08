#!/bin/bash
set -euxo pipefail
case $TRAVIS_OS_NAME in
  osx)
      qmake -r CONFIG+=$CONFIG CONFIG+=no-sanitizers
      make -j 2
      cd tests
      qmake -r CONFIG+=$CONFIG CONFIG+=no-sanitizers
      make -j 2
      cd .../bin/x86-$CONFIG
      ls
    ;;
  linux)
      docker run -d -v `pwd`:`pwd` -w `pwd` --name builder trikset/linux-builder Xvfb :0
      if [ "$VERA" = "true" ]; then docker exec builder tclsh vera++/generatePaths.tcl; fi
      if [ "$VERA" = "true" ]; then docker exec -i builder vera++ --error -p allRules --root vera++ <vera++/params; fi
      docker exec builder qmake -r CONFIG+=$CONFIG CONFIG+=no-sanitizers
      docker exec builder make -j2
      docker exec builder sh -c "cd tests && qmake -r CONFIG+=$CONFIG CONFIG+=no-sanitizers"
      docker exec builder sh -c "cd tests && make -j2"
      docker exec builder sh -c "cd bin/x86-$CONFIG && ls"
      docker exec builder sh -c "export DISPLAY=:0 && cd bin/x86-$CONFIG && ./trikScriptRunnerTests$SUFFIX"
      docker exec builder sh -c "export DISPLAY=:0 && cd bin/x86-$CONFIG && ./trikCommunicatorTests$SUFFIX"
      docker exec builder sh -c "export DISPLAY=:0 && cd bin/x86-$CONFIG && ./trikKernelTests$SUFFIX"
      docker stop builder
    ;;
  *) exit 1 ;;
esac
