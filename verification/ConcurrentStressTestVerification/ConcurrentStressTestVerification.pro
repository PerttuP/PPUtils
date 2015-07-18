#-------------------------------------------------
#
# Project created by QtCreator 2015-07-18T12:52:21
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_concurrentstresstestverification
CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += ../../source/PPTest/concurrentstresstest.hh

SOURCES += tst_concurrentstresstestverification.cc
DEFINES += SRCDIR=\\\"$$PWD/\\\"

INCLUDEPATH += ../../source/PPTest
DEPENDPATH += ../../source/PPTest
