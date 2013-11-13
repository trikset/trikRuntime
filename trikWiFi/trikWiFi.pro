#-------------------------------------------------
#
# Project created by QtCreator 2013-10-29T05:05:11
#
#-------------------------------------------------

QT       -= gui

TARGET = trikWiFi
TEMPLATE = lib

DEFINES += TRIKWIFI_LIBRARY

SOURCES += \
    trikWPACtrlIface.cpp \
    trikWiFi.cpp

HEADERS += \
    trikWPACtrlIface.h \
    trikWiFi_global.h \
    trikWiFi.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
