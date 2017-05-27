include(../global.pri)
include(generated_cpp/PyTrikControl/PyTrikControl.pri)

TEMPLATE = app

INCLUDEPATH += \
	/home/rb/Documents/pp2/src \
	/usr/include/python2.7 \
	./generated_cpp/PyTrikControl \
	../trikControl/src \
	../trikControl/include/trikControl \

LIBS += \
	-L/home/rb/Documents/pp2/lib -lPythonQt_d \
	-lpython2.7 -lpthread -ldl -lutil -lm \


links(trikControl trikKernel trikHal)
implementationIncludes(trikControl trikKernel trikHal)

QT += widgets

SOURCES += \
	trikPythonRunnerMain.cpp \
    PyExampleDecorators.cpp

HEADERS += \
    PyExampleDecorators.h
