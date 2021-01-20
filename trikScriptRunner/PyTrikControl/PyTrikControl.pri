DEBUG_EXT=$${CONFIGURATION_SUFFIX}
include($$PWD/../../PythonQt/PythonQt/build/PythonQt_QtAll.prf)
#PythonQt generated files have problems, but I have no time to fix 3rdparty tools
clang:QMAKE_CXXFLAGS *= -Wno-error -Wno-error=sometimes-uninitialized -Wno-error=writable-strings
QMAKE_CXXFLAGS *= -Wno-error=cast-qual -Wno-error=redundant-decls
QMAKE_CXXFLAGS -= -Werror -Werror=pedantic -pedantic-errors -Werror=write-strings

QT += widgets

INCLUDEPATH *= $$PWD/../../trikControl/include/trikControl

HEADERS += \
           $$PWD/PyTrikControl0.h \

SOURCES += \
           $$PWD/PyTrikControl0.cpp \
           $$PWD/PyTrikControl_init.cpp
