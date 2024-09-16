#!/bin/bash
set -xeuo pipefail

IGNORED_DIRS=(
    "./trikScriptRunner/generated_cpp/pytrikcontrol"
    "./trikHal/src/trik/usbMsp"
    "./PythonQt"
    "./mlx90640-library"
    "./qslog"
    "./tests/thirdparty"
)

IGNORE_CMD=""

for DIR in "${IGNORED_DIRS[@]}"; do
    IGNORE_CMD+=" -path $DIR -o"
done

IGNORE_CMD=${IGNORE_CMD% -o}

# clang-format-15
find . \( $IGNORE_CMD \) -prune -o -name '*.cpp' -o -name '*.h' -print | xargs uncrustify -c trikRuntime.cfg --replace

