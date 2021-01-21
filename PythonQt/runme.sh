#!/bin/sh
./PythonQt/generator/pythonqt_generator \
  --no-suppress-warnings \
  --output-directory=../trikScriptRunner/ \
  --include-paths=../trikControl/include:../trikKernel/include:../trikHal/include \
 ../PythonQt/trikControl_export.h \
 typesystem_trikControl.xml
