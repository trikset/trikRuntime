#PythonQt generated files have problems, but I have no time to fix 3rdparty tools
include($$PWD/../../PythonQt/PythonQt/build/python.prf)
clang:QMAKE_CXXFLAGS += -Wno-error=sometimes-uninitialized -Wno-error=writable-strings
QMAKE_CXXFLAGS += -Wno-error=cast-qual -Wno-error=redundant-decls

QT += widgets

HEADERS += \
           $$PWD/PyTrikControl0.h \

SOURCES += \
           $$PWD/PyTrikControl0.cpp \
           $$PWD/PyTrikControl_init.cpp
