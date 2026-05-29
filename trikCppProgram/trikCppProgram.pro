# Template .pro file for a user C++ program that runs on TRIK robot.
# Build produces a shared library (.so) that trikRuntime loads via QLibrary.
#
# When built as part of trikRuntime (SUBDIRS), this target is excluded from the
# default build (no_default_target) — it is present only for Qt Creator navigation.
#
# To build your own program:
#   qmake trikCppProgram.pro && make
#
# The resulting .so can be transferred to the robot and run from the file manager.

TEMPLATE = lib
CONFIG  += shared
QT      += core
linux:CONFIG += unversioned_libname
win32:CONFIG += skip_target_version_ext

include(../global.pri)

DESTDIR = $$PWD/bin

HEADERS += \
    $$PWD/trikCppProgramInterface.h \

SOURCES += \
    $$PWD/main.cpp \

implementationIncludes(trikKernel trikControl trikNetwork trikScriptRunner)
links(trikKernel trikControl trikNetwork trikScriptRunner)
