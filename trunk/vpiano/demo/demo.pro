#-------------------------------------------------
#
# Project created by QtCreator 2014-01-02T16:48:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = demo
TEMPLATE = app

INCLUDEPATH += ../plugin
LIBS += -L../plugin \
    -lvpianokeybd

macx {
    LIBS += -framework Cocoa
}

SOURCES += main.cpp\
        mainwindow.cpp \
    editkey.cpp

HEADERS  += mainwindow.h \
    editkey.h

FORMS    += mainwindow.ui
