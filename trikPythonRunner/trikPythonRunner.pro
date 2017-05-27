include(../global.pri)
include(generated_cpp/PyTrikControl/PyTrikControl.pri)

TEMPLATE = app

INCLUDEPATH += \
	/home/rb/Documents/python-trik-build/pp2/src \
	/opt/trik-sdk/sysroots/arm926ejse-oe-linux-gnueabi/usr/include/python2.7/ \
	./generated_cpp/PyTrikControl \
	../trikControl/src \
	../trikControl/include/trikControl \

LIBS += \
	-L/home/rb/Documents/python-trik-build/pp2/lib -lPythonQt \
	-lpython2.7 -lpthread -ldl -lutil -lm \


links(trikControl trikKernel trikHal)
implementationIncludes(trikControl trikKernel trikHal)

QT += widgets

SOURCES += \
	trikPythonRunnerMain.cpp \
