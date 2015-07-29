#-------------------------------------------------
#
# Project created by QtCreator 2015-07-29T20:40:20
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_concurrentuniformobjectpooltest
CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_concurrentuniformobjectpooltest.cc
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    ../../source/PPUtils/concurrentuniformobjectpool.hh \
    ../../source/PPUtils/concurrentuniformobjectpool_impl.hh
