#-------------------------------------------------
#
# Albatross Qt Project File
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

# icon information
macx {
    ICON = albatross.icns
}

win32 {
    RC_FILE = albatross.rc
}

# code stuff

SOURCES += main.cpp\
    mainwindow.cpp \
    server.cpp \
    client.cpp \
    connection.cpp \
    chatmsg.cpp

HEADERS  += mainwindow.h \
    server.h \
    client.h \
    connection.h \
    chatmsg.h

FORMS    += mainwindow.ui

RESOURCES += icon.qrc

win32 {
    LIBS += -lwsock32
}
