#-------------------------------------------------
#
# Project created by QtCreator 2015-07-30T21:27:45
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_objectpooltest
CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_objectpooltest.cc
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    ../../source/PPUtils/objectpool.hh \
    ../../source/PPUtils/objectpool_impl.hh

INCLUDEPATH += ../../source/PPUtils
