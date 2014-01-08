TEMPLATE = lib
TARGET = vpianokeybd
target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS += target
QT += core gui svg
CONFIG += designer plugin
DEFINES += VPIANO_PLUGIN

macx {
    LIBS += -framework Carbon
}

HEADERS += pianoscene.h \
    pianokey.h \
    pianokeybd.h \
    plugin.h \
    keyboardmap.h \
    keylabel.h

SOURCES += pianoscene.cpp \
    pianokey.cpp \
    pianokeybd.cpp \
    keyboardmap.cpp \
    plugin.cpp \
    keylabel.cpp

RESOURCES += pianokeybd.qrc
