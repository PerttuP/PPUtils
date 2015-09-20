#-------------------------------------------------
#
# Project created by QtCreator 2015-08-30T15:12:21
#
#-------------------------------------------------

QT       -= core

QT       -= gui

TARGET = ActiveObjectExample
CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../../source/PPUtils

SOURCES += main.cpp \
    ../../source/PPUtils/activeobject.cc

HEADERS += \
    ../../source/PPUtils/activeobject.hh
