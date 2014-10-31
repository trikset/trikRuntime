include(../global.pri)

HEADERS += \
	$$PWD/include/trikTelemetry/trikTelemetry.h \
	$$PWD/src/connection.h \

SOURCES += \
	$$PWD/src/trikTelemetry.cpp \
	$$PWD/src/connection.cpp \

TEMPLATE = lib

QT += network

DEFINES += TRIKTELEMETRY_LIBRARY

uses(trikControl trikKernel)

INCLUDEPATH += \
	../trikControl/include/ \
	../trikKernel/include/ \
