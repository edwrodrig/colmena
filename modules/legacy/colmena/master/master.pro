HEADERS = mastermanager.hpp \
    masterkernel.hpp \
    slave.hpp \
    slavelist.hpp \
    job.hpp \
    joblist.hpp \
    messagerepeaterlist.hpp \
    datasenderlist.hpp \
    datareceiverlist.hpp \
    ../communicator.hpp \
    ../message.hpp \
    ../messagerepeater.hpp \
    ../datasender.hpp \
    ../datareceiver.hpp \
    ../clientmasterdef.hpp \
    ../inputstream.hpp \
    masterslavecommunicator.hpp \
    masterclientcommunicator.hpp \
    masterclientlistener.hpp \
    masterclientthread.hpp \
    masterslavethread.hpp
SOURCES = mastermanager.cpp \
    masterkernel.cpp \
    slave.cpp \
    slavelist.cpp \
    job.cpp \
    joblist.cpp \
    messagerepeaterlist.cpp \
    datasenderlist.cpp \
    datareceiverlist.cpp \
    ../communicator.cpp \
    ../message.cpp \
    ../messagerepeater.cpp \
    ../datasender.cpp \
    ../datareceiver.cpp \
    ../inputstream.cpp \
    masterslavecommunicator.cpp \
    masterclientcommunicator.cpp \
    masterclientlistener.cpp \
    main.cpp \
    masterclientthread.cpp \
    masterslavethread.cpp
QT += network \
    sql
CONFIG += console
CONFIG -= gui
UI_DIR = compilation
MOC_DIR = compilation
DEFINES += _FILE_OFFSET_BITS=64
OBJECTS_DIR = compilation
INCLUDEPATH += .. \
    .
