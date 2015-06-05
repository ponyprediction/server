#-------------------------------------------------
#
# Project created by QtCreator 2015-06-05T15:02:04
#
#-------------------------------------------------

QT       += core
QT += network
QT       -= gui

TARGET = server
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    util.cpp \
    server.cpp \
    socket-thread.cpp

HEADERS += \
    util.hpp \
    server.hpp \
    socket-thread.hpp

DISTFILES += \
    conf.xml
