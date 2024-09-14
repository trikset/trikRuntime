#!/bin/bash
set -xeuo pipefail

IGNORED_DIRS=(
    "./trikScriptRunner/generated_cpp/PyTrikControl"
    "./trikHal/src/trik/usbMsp"
)

IGNORE_CMD=""

for DIR in "${IGNORED_DIRS[@]}"; do
    IGNORE_CMD+=" -path $DIR -o"
done

IGNORE_CMD=${IGNORE_CMD% -o}

# clang-format-15
find . \( $IGNORE_CMD \) -prune -o -name '*.cpp' -o -name '*.h' | xargs clang-format --style=file -i

