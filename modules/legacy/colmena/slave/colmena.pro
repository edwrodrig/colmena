HEADERS       = ../communicator.hpp \
		slavemastercommunicator.hpp \
		slavemanager.hpp \
		slavekernel.hpp \
		task.hpp \
		taskindex.hpp \
		tasklist.hpp \
		../slavessaverdef.hpp \
		../inputstream.hpp \
		../message.hpp \
		../messagerepeater.hpp \
		../datareceiver.hpp \
		../datasender.hpp
SOURCES       = ../communicator.cpp \
		slavemastercommunicator.cpp \
		slavemanager.cpp \
		slavekernel.cpp \
		task.cpp \
		tasklist.cpp \
		../inputstream.cpp \
		../message.cpp \
		../messagerepeater.cpp \
		../datareceiver.cpp \
		../datasender.cpp \
		main.cpp

win32{
	SOURCES += task_win.cpp
}

!win32{
	SOURCES += task_unix.cpp
}
		
QT           += network sql

RESOURCES       = colmena.qrc

CONFIG += console
CONFIG -= gui

UI_DIR = compilation
MOC_DIR = compilation
OBJECTS_DIR = compilation

INCLUDEPATH += . ..