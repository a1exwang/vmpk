# Virtual MIDI Piano Keyboard
# Copyright (C) 2008-2010 Pedro Lopez-Cabanillas <plcl@users.sourceforge.net>
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License along
# with this program; If not, see <http://www.gnu.org/licenses/>.
TEMPLATE = app
TARGET = vmpk
VERSION = 0.3.3
DESTDIR = build
OBJECTS_DIR = $$DESTDIR
UI_DIR = $$DESTDIR
MOC_DIR = $$DESTDIR
RCC_DIR = $$DESTDIR
CONFIG -= debug_and_release
CONFIG -= debug
CONFIG += release
unix:QMAKE_POST_LINK=strip $(TARGET)
win32:QMAKE_POST_LINK=strip $$DESTDIR/$(TARGET)
QT += core \
    gui \
    xml \
    svg
contains(DEFINES, ENABLE_DBUS) {
    CONFIG += qdbus
    DBUS_ADAPTORS += src/net.sourceforge.vmpk.xml
}
VERSIONH = $$sprintf(const QString PGM_VERSION(\"%1\");,$$VERSION)
system($$QMAKE_MKDIR $$DESTDIR)
win32 { 
    DEFINES += __WINDOWS_MM__
    LIBS += -lwinmm
    RC_FILE = src/vpianoico.rc
    system(echo $$VERSIONH > $$DESTDIR/vmpk_version.h)
}
linux* { 
    DEFINES += __LINUX_ALSASEQ__
    DEFINES += AVOID_TIMESTAMPING
    CONFIG += link_pkgconfig x11
    PKGCONFIG += alsa
    system(echo \'$$VERSIONH\' > $$DESTDIR/vmpk_version.h)
}
macx { 
    CONFIG += x86 \
        ppc
    ICON = data/vmpk.icns
    DEFINES += __MACOSX_CORE__
    BUNDLE_RES.files = data/help.html \
#        data/help_de.html \
        data/help_es.html \
        data/help_ru.html \
#        data/help_tr.html \
        data/gmgsxg.ins \
        data/spanish.xml \
        data/german.xml \
        data/azerty.xml \
        data/it-qwerty.xml \
        data/vkeybd-default.xml \
        data/pc102mac.xml \
        $$DESTDIR/translations/vmpk_cs.qm \
#        $$DESTDIR/translations/vmpk_de.qm \
        $$DESTDIR/translations/vmpk_es.qm \
#        $$DESTDIR/translations/vmpk_fr.qm \
        $$DESTDIR/translations/vmpk_ru.qm \
#        $$DESTDIR/translations/vmpk_tr.qm \
        $$DESTDIR/translations/vmpk_zh_CN.qm
    BUNDLE_RES.path = Contents/Resources
    QMAKE_BUNDLE_DATA += BUNDLE_RES
    LIBS += -framework \
        CoreMidi \
        -framework \
        CoreAudio \
        -framework \
        CoreFoundation \
        -framework \
        Carbon
    system(echo \'$$VERSIONH\' > $$DESTDIR/vmpk_version.h)
}
irix* { 
    DEFINES += __IRIX_MD__
    LIBS += -laudio \
        -lpthread
    system(echo \'$$VERSIONH\' > $$DESTDIR/vmpk_version.h)
}
debug:DEFINES += __RTMIDI_DEBUG__
INCLUDEPATH += src
# Input

FORMS += src/about.ui \
    src/extracontrols.ui \
    src/kmapdialog.ui \
    src/midisetup.ui \
    src/preferences.ui \
    src/riffimportdlg.ui \
    src/shortcutdialog.ui \
    src/vpiano.ui \

HEADERS += src/about.h \
    src/classicstyle.h \
    src/constants.h \
    src/events.h \
    src/extracontrols.h \
    src/instrument.h \
    src/keyboardmap.h \
    src/keylabel.h \
    src/kmapdialog.h \
    src/knob.h \
    src/mididefs.h \
    src/midisetup.h \
    src/pianodefs.h \
    src/pianokeybd.h \
    src/pianokey.h \
    src/pianoscene.h \
    src/preferences.h \
    src/qticonloader.h \
    src/rawkeybdapp.h \
    src/riff.h \
    src/riffimportdlg.h \
    src/RtError.h \
    src/RtMidi.h \
    src/shortcutdialog.h \
    src/vpiano.h
    
SOURCES += src/about.cpp \
    src/classicstyle.cpp \
    src/extracontrols.cpp \
    src/instrument.cpp \
    src/keyboardmap.cpp \
    src/keylabel.cpp \
    src/kmapdialog.cpp \
    src/knob.cpp \
    src/main.cpp \
    src/midisetup.cpp \
    src/pianokeybd.cpp \
    src/pianokey.cpp \
    src/pianoscene.cpp \
    src/preferences.cpp \
    src/qticonloader.cpp \
    src/rawkeybdapp.cpp \
    src/riff.cpp \
    src/riffimportdlg.cpp \
    src/RtMidi.cpp \
    src/shortcutdialog.cpp \
    src/vpiano.cpp

RESOURCES += data/vmpk.qrc

# unmaintained translations, not distributed
# translations/vmpk_de.ts \
# translations/vmpk_fr.ts \
# translations/vmpk_tr.ts \
TRANSLATIONS +=  translations/vmpk_cs.ts \
    translations/vmpk_es.ts \
    translations/vmpk_ru.ts \
    translations/vmpk_zh_CN.ts

include(updateqm.pri)
