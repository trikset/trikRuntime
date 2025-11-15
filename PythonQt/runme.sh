#!/bin/sh
# Usage:
# QTDIR - Root directory of Qt.
# GENERATORPATH - Path to the built PythonQt generator .

# Build generator with correct qmake and run this than check diffs
 QTDIR="$QTDIR" "$GENERATORPATH" \
  --no-suppress-warnings \
  --output-directory=../trikScriptRunner/ \
  --include-paths=../trikControl/include:../trikKernel/include:../trikHal/include \
 ../PythonQt/trikControl_export.h \
 typesystem_trikControl.xml > pyqt_generator.log 2>&1
