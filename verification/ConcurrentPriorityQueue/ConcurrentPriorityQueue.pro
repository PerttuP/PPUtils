#-------------------------------------------------
#
# Project created by QtCreator 2016-07-10T18:25:13
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_concurrentpriorityqueuetest
CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../../source/PPUtils
INCLUDEPATH += ../../source/PPTest

HEADERS += \
    ../../source/PPUtils/concurrentpriorityqueue.hh \
    ../../source/PPTest/concurrentstresstest.hh

SOURCES += tst_concurrentpriorityqueuetest.cc
DEFINES += SRCDIR=\\\"$$PWD/\\\"
