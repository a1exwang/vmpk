/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2009, Pedro Lopez-Cabanillas <plcl@users.sf.net>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; If not, see <http://www.gnu.org/licenses/>.
*/

#include "preferences.h"
#include "constants.h"
#include "vpiano.h"

#include <QPushButton>
#include <QShowEvent>
#include <QFileDialog>
#include <QColorDialog>

Preferences::Preferences(QWidget *parent)
    : QDialog(parent),
    m_numOctaves(5),
    m_grabKb(false),
    m_styledKnobs(true),
    m_alwaysOnTop(false),
    m_showLabels(false),
    m_rawKeyboard(false)
{
    ui.setupUi( this );
    ui.txtFileInstrument->setText(QSTR_DEFAULT);
    ui.txtFileKmap->setText(QSTR_DEFAULT);
    ui.txtFileRawKmap->setText(QSTR_DEFAULT);
    m_keymap.setRawMode(false);
    m_rawmap.setRawMode(true);
    connect(ui.btnInstrument, SIGNAL(clicked()), SLOT(slotOpenInstrumentFile()));
    connect(ui.btnColor, SIGNAL(clicked()), SLOT(slotSelectColor()));
    connect(ui.btnKmap, SIGNAL(clicked()), SLOT(slotOpenKeymapFile()));
    connect(ui.btnRawKmap, SIGNAL(clicked()), SLOT(slotOpenRawKeymapFile()));
    QPushButton *btnDefaults = ui.buttonBox->button(QDialogButtonBox::RestoreDefaults);
    connect(btnDefaults, SIGNAL(clicked()), SLOT(slotRestoreDefaults()));
}

void Preferences::showEvent ( QShowEvent *event )
{
    if (event->type() == QEvent::Show) {
        ui.spinNumOctaves->setValue( m_numOctaves );
        ui.chkGrabKb->setChecked( m_grabKb );
        ui.chkStyledKnobs->setChecked( m_styledKnobs );
        ui.chkAlwaysOnTop->setChecked( m_alwaysOnTop );
        ui.chkShowNames->setChecked( m_showLabels );
        ui.chkRawKeyboard->setChecked( m_rawKeyboard );
    }
}

void Preferences::apply()
{
    m_numOctaves = ui.spinNumOctaves->value();
    m_keyPressedColor = QColor(ui.lblColorName->text());
    m_grabKb = ui.chkGrabKb->isChecked();
    m_styledKnobs = ui.chkStyledKnobs->isChecked();
    m_alwaysOnTop = ui.chkAlwaysOnTop->isChecked();
    m_showLabels = ui.chkShowNames->isChecked();
    m_rawKeyboard = ui.chkRawKeyboard->isChecked();
    if ( ui.txtFileRawKmap->text().isEmpty() ||
         ui.txtFileRawKmap->text() == QSTR_DEFAULT)
        m_rawmap.setFileName(QSTR_DEFAULT);
    if ( ui.txtFileKmap->text().isEmpty() ||
         ui.txtFileKmap->text() == QSTR_DEFAULT)
        m_keymap.setFileName(QSTR_DEFAULT);
    if ( ui.txtFileInstrument->text().isEmpty() ||
         ui.txtFileInstrument->text() == QSTR_DEFAULT )
        m_insFileName = QSTR_DEFAULT;
}

void Preferences::accept()
{
    apply();
    QDialog::accept();
}

void Preferences::slotOpenInstrumentFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                tr("Open instruments definition"),
                                VPiano::dataDirectory(),
                                tr("Instrument definitions (*.ins)"));
    if (!fileName.isEmpty()) {
        setInstrumentsFileName(fileName);
    }
}

void Preferences::slotSelectColor()
{
    QColor color = QColorDialog::getColor(m_keyPressedColor, this);
    if (color.isValid()) {
        ui.lblColorName->setText(color.name());
        ui.lblColorName->setPalette(QPalette(color));
        ui.lblColorName->setAutoFillBackground(true);
    }
}

Instrument* Preferences::getInstrument()
{
    QString key = ui.cboInstrument->currentText();
    if (key.isEmpty())
        return NULL;
    return &m_ins[key];
}

void Preferences::setInstrumentsFileName( const QString fileName )
{
    QFileInfo f(fileName);
    if (f.isReadable()) {
        m_ins.clear();
        ui.cboInstrument->clear();
        if (m_ins.load(fileName)) {
            ui.txtFileInstrument->setText(f.fileName());
            InstrumentList::ConstIterator it;
            for(it = m_ins.begin(); it != m_ins.end(); ++it) {
                ui.cboInstrument->addItem(it.key());
            }
            ui.cboInstrument->setCurrentIndex(-1);
            m_insFileName = fileName;
        } else {
            m_insFileName.clear();
            ui.txtFileInstrument->setText(m_insFileName);
        }
    }
}

QString Preferences::getInstrumentsFileName()
{
    return m_insFileName;
}

void Preferences::setInstrumentName( const QString name )
{
    int index = ui.cboInstrument->findText( name );
    ui.cboInstrument->setCurrentIndex( index );
}

QString Preferences::getInstrumentName()
{
    return ui.cboInstrument->currentText();
}

void Preferences::setKeyPressedColor(QColor value)
{
    if (m_keyPressedColor != value) {
        m_keyPressedColor = value;
        ui.lblColorName->setText(value.name());
        ui.lblColorName->setPalette(QPalette(value));
        ui.lblColorName->setAutoFillBackground(true);
    }
}

void Preferences::slotOpenKeymapFile()
{
    QString fileName = QFileDialog::getOpenFileName(0,
                                tr("Open keyboard map definition"),
                                VPiano::dataDirectory(),
                                tr("Keyboard map (*.xml)"));
    if (!fileName.isEmpty()) {
        setKeyMapFileName(fileName);
    }
}

void Preferences::slotOpenRawKeymapFile()
{
    QString fileName = QFileDialog::getOpenFileName(0,
                                tr("Open keyboard map definition"),
                                VPiano::dataDirectory(),
                                tr("Keyboard map (*.xml)"));
    if (!fileName.isEmpty()) {
        setRawKeyMapFileName(fileName);
    }
}

void Preferences::setRawKeyMapFileName( const QString fileName )
{
    QFileInfo f(fileName);
    if (f.isReadable()) {
        m_rawmap.loadFromXMLFile(fileName);
        ui.txtFileRawKmap->setText(f.fileName());
    } else {
        m_rawmap.clear();
        m_rawmap.setFileName(QSTR_DEFAULT);
    }
}

void Preferences::setKeyMapFileName( const QString fileName )
{
    QFileInfo f(fileName);
    if (f.isReadable()) {
        m_keymap.loadFromXMLFile(fileName);
        ui.txtFileKmap->setText(f.fileName());
    } else {
        m_keymap.clear();
        m_keymap.setFileName(QSTR_DEFAULT);
    }
}

void Preferences::slotRestoreDefaults()
{
    ui.chkAlwaysOnTop->setChecked(false);
    ui.chkGrabKb->setChecked(false);
    ui.chkRawKeyboard->setChecked(false);
    ui.chkShowNames->setChecked(false);
    ui.chkStyledKnobs->setChecked(true);
    ui.spinNumOctaves->setValue(5);
    ui.txtFileInstrument->setText(QSTR_DEFAULT);
    ui.txtFileKmap->setText(QSTR_DEFAULT);
    ui.txtFileRawKmap->setText(QSTR_DEFAULT);
    ui.cboInstrument->setCurrentIndex(-1);
    setKeyPressedColor(QApplication::palette().highlight().color());
}
