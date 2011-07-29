#-------------------------------------------------
#
# Project created by QtCreator 2011-07-24T11:54:31
#
#-------------------------------------------------

QT       += core
QT       += network
QT       -= gui

TARGET = bhttpd
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    server.cpp \
    log.cpp \
    request.cpp \
    settings.cpp \
    serverthread.cpp \
    mime.cpp \
    response.cpp \
    responsefile.cpp \
    responsedirectory.cpp \
    common.cpp

HEADERS += \
    server.h \
    log.h \
    request.h \
    settings.h \
    serverthread.h \
    common.h \
    mime.h \
    response.h \
    responsefile.h \
    responsedirectory.h \
    httpstatus.h
