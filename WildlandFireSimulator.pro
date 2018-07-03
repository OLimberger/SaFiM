#-------------------------------------------------
#
# Project created by QtCreator 2017-08-28T12:27:11
#
#-------------------------------------------------

QT       += core

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

g++ {
    QMAKE_CXXFLAGS += -Wextra
}
win32-msvc* {
    QMAKE_CXXFLAGS += /W3
}

TARGET = WildlandFireSimulator
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp\
    fire.cpp \
    firemap.cpp \
    landscape.cpp \
    simulation.cpp \
    utility.cpp \
    csvreader.cpp \
    fireweather.cpp

HEADERS  += \
    fire.h \
    firemap.h \
    landscape.h \
    cell.h \
    globals.h \
    simulation.h \
    utility.h \
    csvreader.h \
    fireweather.h

FORMS    +=
