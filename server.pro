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
    socket-thread.cpp \
    database-manager.cpp

HEADERS += \
    util.hpp \
    server.hpp \
    socket-thread.hpp \
    database-manager.hpp

DISTFILES += \
    conf.xml

macx: LIBS += -L/usr/local/lib -lmongoclient -lboost_thread-mt -lboost_system -lboost_regex
macx: INCLUDEPATH += /usr/local/include

unix:!macx: LIBS += -L$$PWD/../../../mongo-client-driver/lib/ -lmongoclient -lmongoclient -lboost_thread -lboost_system -lboost_regex

unix:!macx:INCLUDEPATH += $$PWD/../../../mongo-client-driver/include
unix:!macx:DEPENDPATH += $$PWD/../../../mongo-client-driver/include

unix:!macx: PRE_TARGETDEPS += $$PWD/../../../mongo-client-driver/lib/libmongoclient.a
