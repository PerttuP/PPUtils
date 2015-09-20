#-------------------------------------------------
#
# Project created by QtCreator 2015-09-20T10:44:37
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_activeobjecttest
CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../../source/PPUtils

HEADERS += ../../source/PPUtils/activeobject.hh

SOURCES += tst_activeobjecttest.cpp \
           ../../source/PPUtils/activeobject.cc


DEFINES += SRCDIR=\\\"$$PWD/\\\"
