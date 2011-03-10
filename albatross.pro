#-------------------------------------------------
#
# Project created by QtCreator 2011-03-02T10:18:51
#
#-------------------------------------------------

QT       += core gui

TARGET = albatross
TEMPLATE = app

# compilation output directories
DESTDIR = ./bin
OBJECTS_DIR = ./obj
MOC_DIR = ./moc
UI_DIR = ./ui

macx {
    ICON = albatross.icns
}

winnt {
    RC_FILE = albatross.rc
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

RESOURCES += \
    icon.qrc
