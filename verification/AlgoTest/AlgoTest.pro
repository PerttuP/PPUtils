#-------------------------------------------------
#
# Project created by QtCreator 2015-07-18T19:56:11
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_algotest
CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_algotest.cc
HEADERS += ../../source/PPUtils/algo.hh

DEFINES += SRCDIR=\\\"$$PWD/\\\"

INCLUDEPATH += ../../source/PPUtils
