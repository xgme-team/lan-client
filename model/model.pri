#-------------------------------------------------
#
# Project created by QtCreator 2016-02-21T00:12:43
#
#-------------------------------------------------

INCLUDEPATH += $$PWD
DEPENDPATH  += $$PWD


include(library/library.pri)
include(news/news.pri)
include(torrent/torrent.pri)

SOURCES += $$PWD/model.cpp \
    $$PWD/message.cpp \
    $$PWD/messagelistmodel.cpp

HEADERS  += $$PWD/model.h \
    $$PWD/message.h \
    $$PWD/messagelistmodel.h
