#!/bin/bash
set -xeo pipefail

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

if [ -n "$CHECK" ]; then
    OPTION="--check"
else
    OPTION="--replace"
fi

find . \( $IGNORE_CMD \) -prune -o -name '*.cpp' -print -o -name '*.h' -print | xargs uncrustify -c trikRuntime.cfg "$OPTION"
