#!/usr/bin/env bash
set -xeuo
# Usage:
# QTDIR - Root directory of Qt.
# GENERATORPATH - Path to the built PythonQt generator .

SCRIPT_DIR="$(dirname "$(realpath "${BASH_SOURCE[0]}")")"
RUNTIME_ROOT="$SCRIPT_DIR"/../

# Build generator with correct qmake and run this than check diffs
QTDIR="$QTDIR" "$GENERATORPATH" \
  --no-suppress-warnings \
  --output-directory="$RUNTIME_ROOT"/trikScriptRunner/ \
  --include-paths="$RUNTIME_ROOT"/trikControl/include:"$RUNTIME_ROOT"/trikKernel/include:"$RUNTIME_ROOT"/trikHal/include \
 "$SCRIPT_DIR"/trikControl_export.h \
 "$SCRIPT_DIR"/typesystem_trikControl.xml > pyqt_generator.log 2>&1
 
QTDIR="$QTDIR" "$GENERATORPATH" \
  --no-suppress-warnings \
  --output-directory="$RUNTIME_ROOT"/trikScriptRunner/ \
  --include-paths="$SCRIPT_DIR":"$RUNTIME_ROOT"/trikControl/src:"$RUNTIME_ROOT"/trikControl/include \
 "$SCRIPT_DIR"/trikControlInternal_export.h \
 "$SCRIPT_DIR"/typesystem_trikControlInternal.xml > pyqt_generator_internal.log 2>&1
