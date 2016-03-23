#-------------------------------------------------
#
# Project created by QtCreator 2016-02-21T00:12:43
#
#-------------------------------------------------

QT       += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG   += C++11

TARGET = lan-client
TEMPLATE = app

exists(custom.pri):include(custom.pri)

LIBS += -ltorrent -lboost_system
win32-g++:LIBS += -lWs2_32 -lMswsock

RC_ICONS = lan-client.ico


include(common/common.pri)
include(gui/gui.pri)
include(launcher/launcher.pri)
include(model/model.pri)
