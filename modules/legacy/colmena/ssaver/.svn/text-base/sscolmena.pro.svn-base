HEADERS	      = ../slavessaverdef.hpp 

QT           += network

win32{

HEADERS       = saver.hpp \
                                config.hpp \
                                bhttp.hpp \
                                ../inputstream.hpp



SOURCES       = saver.cpp \
                config.cpp \
                bhttp.cpp \
                ../inputstream.cpp \
		main_win.cpp
QT		+= webkit
}

!win32{

SOURCES = main_unix.cpp

CONFIG += console
CONFIG -= gui


}
RESOURCES       = sscolmena.qrc

UI_DIR = compilation
MOC_DIR = compilation
OBJECTS_DIR = compilation

INCLUDEPATH += .. .
