#!/bin/sh
../../pythonqt/generator/pythonqt_generator \
  --no-suppress-warnings \
  --output-directory=../trikScriptRunner/PyTrikControl/ \
  --include-paths=../trikControl/include/trikControl/:../trikKernel/include/trikKernel:../trikHal/include/trikHal:../trikControl/include:../trikHal/include:../trikKernel/include \
 ../PythonQt/trikControl_export.h \
 typesystem_trikControl.xml
