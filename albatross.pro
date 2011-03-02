#-------------------------------------------------
#
# Project created by QtCreator 2011-03-02T10:18:51
#
#-------------------------------------------------

QT       += core gui

TARGET = albatross
TEMPLATE = app

macx {
    ICON = albatross.icns
}

SOURCES += main.cpp\
    mainwindow.cpp \
    server.cpp \
    client.cpp \
    connection.cpp

HEADERS  += mainwindow.h \
    server.h \
    client.h \
    connection.h \
    chatmsg.h

FORMS    += mainwindow.ui
