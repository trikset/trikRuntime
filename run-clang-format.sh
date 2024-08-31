#!/bin/bash
set -xeuo pipefail
which -a clang-format-15 > /dev/null \
	||  (echo -e "15.0.0\n$(clang-format${1+-$1} --version | grep -Eo '([0-9]+\.[0-9]+\.[0-9]+)')" | sort -CV ) \
		|| { echo "use '${0} <clang-format-version-not-less-than-15>' to call exact proper version" ; exit 1 ; }

IGNORED_DIRS=(
    "./trikScriptRunner/generated_cpp/pytrikcontrol"
    "./trikHal/src/trik/usbMsp"
)

IGNORE_CMD=""

for DIR in "${IGNORED_DIRS[@]}"; do
    IGNORE_CMD+=" -path $DIR -o"
done

IGNORE_CMD=${IGNORE_CMD% -o}

find . \( $IGNORE_CMD \) -prune -o -name '*.cpp' -o -name '*.h' -print0 | xargs -0 clang-format${1+-$1} --style=file -i

