# This pro file will build QsLog as a shared library
include(../global.pri)

include(QsLog.pri)

TARGET = qslog$$CONFIGURATION_SUFFIX

QT -= gui

CONFIG -= console
CONFIG -= app_bundle
CONFIG += shared

TEMPLATE = lib

enableFlagIfCan(-Wno-error=terminate)

DEFINES += QSLOG_IS_SHARED_LIBRARY

installs()
