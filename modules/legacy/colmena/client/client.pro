HEADERS =	../clientmasterdef.hpp \
			../inputstream.hpp \
			clientsocket.hpp
SOURCES =	main.cpp \
			../inputstream.cpp \
			clientsocket.cpp
QT           += network
DEFINES += _FILE_OFFSET_BITS=64
CONFIG += console
CONFIG -= gui

UI_DIR = compilation
MOC_DIR = compilation
OBJECTS_DIR = compilation


INCLUDEPATH += . ..
