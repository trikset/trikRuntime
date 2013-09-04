TEMPLATE = app
QT += script network
CONFIG += console

CONFIGURATION = debug

CONFIG -= debug release
CONFIG += $$CONFIGURATION

DESTDIR = $$CONFIGURATION/bin

INCLUDEPATH = ../trikControl/include

LIBS += -L../trikControl/$$CONFIGURATION/bin -ltrikControl

OBJECTS_DIR = $$CONFIGURATION/.obj
MOC_DIR = $$CONFIGURATION/.moc
RCC_DIR = $$CONFIGURATION/.moc

!macx {
	QMAKE_LFLAGS += -Wl,-O1,-rpath,.
}

HEADERS += \
	qRealCommunicator.h \
	runner.h \
	scriptableParts.h \

SOURCES += \
	main.cpp \
	qRealCommunicator.cpp \
	runner.cpp \
	scriptableParts.cpp \

win32 {
	QMAKE_POST_LINK = "xcopy ..\\trikControl\\$$CONFIGURATION\\bin $$CONFIGURATION\\bin /s /e /q /y /i && \
	xcopy ..\\media $$CONFIGURATION\\bin\\media /s /e /q /y /i"
}
else {
	QMAKE_POST_LINK = "cp -r ../trikControl/$$CONFIGURATION/bin/* $$CONFIGURATION/bin"
}
