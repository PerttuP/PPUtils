#-------------------------------------------------
#
# Project created by QtCreator 2015-07-28T22:57:57
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_uniformobjectpooltest
CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_uniformobjectpooltest.cc
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    ../../source/PPUtils/uniformobjectpool.hh \
    ../../source/PPUtils/uniformobjectpool_impl.hh

INCLUDEPATH += ../../source/PPUtils
