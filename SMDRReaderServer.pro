#-------------------------------------------------
#
# Project created by QtCreator 2015-03-25T16:10:59
#
#-------------------------------------------------

QT       += core sql network

QT       -= gui

# for Service
include(../../QtService/src/qtservice.pri)

TARGET = SMDRReaderServer
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    server.cpp \
    daemon.cpp \
    infoserver.cpp \
    infosocket.cpp

HEADERS += \
    server.h \
    daemon.h \
    infoserver.h \
    infosocket.h
