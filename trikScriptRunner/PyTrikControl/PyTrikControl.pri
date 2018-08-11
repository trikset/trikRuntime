#PythonQt generated files have problems, but I have no time to fix 3rdparty tools
clang:QMAKE_CXXFLAGS += -Wno-error=sometimes-uninitialized -Wno-error=writable-strings

QT += widgets

HEADERS += \
           $$PWD/PyTrikControl0.h \

SOURCES += \
           $$PWD/PyTrikControl0.cpp \
           $$PWD/PyTrikControl_init.cpp
