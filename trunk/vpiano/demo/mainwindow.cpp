/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2014,      Davy Triponney         <davy.triponney@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QColorDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _customColor(this->palette().highlight().color()),
    _signalMapper(new QSignalMapper(this))
{
    ui->setupUi(this);
    initColors();
    initKeyTable();

    // Custom labels editable
    ui->listWidget->blockSignals(true);
    for (int i = 0; i < ui->listWidget->count(); i++)
        ui->listWidget->item(i)->setFlags (ui->listWidget->item(i)->flags () | Qt::ItemIsEditable);
    ui->listWidget->blockSignals(false);
    on_listWidget_itemChanged(NULL);

    _labelMouseOver = new QLabel();
    ui->statusBar->addPermanentWidget(_labelMouseOver);

    connect(ui->virtualPianoKeyboard, SIGNAL(noteOn(int,int)), this, SLOT(noteChanged(int,int)));
    connect(ui->virtualPianoKeyboard, SIGNAL(noteOff(int)), this, SLOT(noteChanged(int)));
    connect(ui->virtualPianoKeyboard, SIGNAL(mouseOver(int)), this, SLOT(onMouseOver(int)));
    connect(_signalMapper, SIGNAL(mapped(QString)), this, SLOT(cellChanged(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initColors()
{
    QString str = "QPushButton{border: 1px solid #888; background-color: ";
    ui->pushColor1->setStyleSheet(str + ui->virtualPianoKeyboard->get(PianoKeybd::PROPERTY_COLOR_1).value<QColor>().name() + ";}");
    ui->pushColor2->setStyleSheet(str + ui->virtualPianoKeyboard->get(PianoKeybd::PROPERTY_COLOR_2).value<QColor>().name() + ";}");
    ui->pushColor3->setStyleSheet(str + ui->virtualPianoKeyboard->get(PianoKeybd::PROPERTY_COLOR_3).value<QColor>().name() + ";}");
    ui->pushColor4->setStyleSheet(str + ui->virtualPianoKeyboard->get(PianoKeybd::PROPERTY_COLOR_4).value<QColor>().name() + ";}");
    ui->pushColor5->setStyleSheet(str + ui->virtualPianoKeyboard->get(PianoKeybd::PROPERTY_COLOR_5).value<QColor>().name() + ";}");
    ui->pushColor6->setStyleSheet(str + ui->virtualPianoKeyboard->get(PianoKeybd::PROPERTY_COLOR_6).value<QColor>().name() + ";}");
    ui->pushColor7->setStyleSheet(str + ui->virtualPianoKeyboard->get(PianoKeybd::PROPERTY_COLOR_7).value<QColor>().name() + ";}");
    ui->pushColor8->setStyleSheet(str + ui->virtualPianoKeyboard->get(PianoKeybd::PROPERTY_COLOR_8).value<QColor>().name() + ";}");
    ui->pushColor9->setStyleSheet(str + ui->virtualPianoKeyboard->get(PianoKeybd::PROPERTY_COLOR_9).value<QColor>().name() + ";}");
    ui->pushColor10->setStyleSheet(str + ui->virtualPianoKeyboard->get(PianoKeybd::PROPERTY_COLOR_10).value<QColor>().name() + ";}");
    ui->pushColor11->setStyleSheet(str + ui->virtualPianoKeyboard->get(PianoKeybd::PROPERTY_COLOR_11).value<QColor>().name() + ";}");
    ui->pushColor12->setStyleSheet(str + ui->virtualPianoKeyboard->get(PianoKeybd::PROPERTY_COLOR_12).value<QColor>().name() + ";}");
    ui->pushColor13->setStyleSheet(str + ui->virtualPianoKeyboard->get(PianoKeybd::PROPERTY_COLOR_13).value<QColor>().name() + ";}");
    ui->pushColor14->setStyleSheet(str + ui->virtualPianoKeyboard->get(PianoKeybd::PROPERTY_COLOR_14).value<QColor>().name() + ";}");
    ui->pushColor15->setStyleSheet(str + ui->virtualPianoKeyboard->get(PianoKeybd::PROPERTY_COLOR_15).value<QColor>().name() + ";}");
    ui->pushColor16->setStyleSheet(str + ui->virtualPianoKeyboard->get(PianoKeybd::PROPERTY_COLOR_16).value<QColor>().name() + ";}");
    ui->pushColorWhiteKeys->setStyleSheet(str + ui->virtualPianoKeyboard
                                          ->get(PianoKeybd::PROPERTY_COLOR_WHITE_KEYS).value<QColor>().name() + ";}");
    ui->pushColorBlackKeys->setStyleSheet(str + ui->virtualPianoKeyboard
                                          ->get(PianoKeybd::PROPERTY_COLOR_BLACK_KEYS).value<QColor>().name() + ";}");
    ui->pushColorTextWhite->setStyleSheet(str + ui->virtualPianoKeyboard
                                          ->get(PianoKeybd::PROPERTY_COLOR_TEXT_WHITE_KEYS).value<QColor>().name() + ";}");
    ui->pushColorTextBlack->setStyleSheet(str + ui->virtualPianoKeyboard
                                          ->get(PianoKeybd::PROPERTY_COLOR_TEXT_BLACK_KEYS).value<QColor>().name() + ";}");
    ui->pushCustomColor->setStyleSheet(str + _customColor.name() + ";}");

}

void MainWindow::initKeyTable()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 13; j++)
        {
            EditKey * editKey = new EditKey();
            editKey->setStyleSheet("EditKey{border: 0px; padding-left: 2px;}");
            editKey->setCombinaison(ui->virtualPianoKeyboard->getMapping((PianoKeybd::Key)j, i));
            ui->tableWidget->setCellWidget(i, j, editKey);

            _signalMapper->setMapping(editKey, QString::number(i) + ":" + QString::number(j));
            connect(editKey, SIGNAL(combinaisonChanged(QKeySequence)), _signalMapper, SLOT(map()));
        }
    }
}

void MainWindow::cellChanged(QString txt)
{
    int posSeparator = txt.indexOf(":");
    int octave = txt.left(posSeparator).toInt();
    PianoKeybd::Key key = (PianoKeybd::Key)txt.right(txt.length() - posSeparator - 1).toInt();
    EditKey * editKey = (EditKey*)ui->tableWidget->cellWidget(octave, key);
    ui->virtualPianoKeyboard->setMapping(key, octave, editKey->getSequence());
}

void MainWindow::noteChanged(int key, int vel)
{
    int timeOut = 1000;
    if (vel > 0)
        ui->statusBar->showMessage("note on: " + QString::number(key) + ", velocity: " + QString::number(vel), timeOut);
    else
        ui->statusBar->showMessage("note off: " + QString::number(key), timeOut);
}
void MainWindow::onMouseOver(int key)
{
    if (key == -1)
        _labelMouseOver->clear();
    else
        _labelMouseOver->setText(trUtf8("Mouse over key: ") + QString::number(key));
}

void MainWindow::on_radio5octaves_clicked()
{
    ui->spinStartingKey->blockSignals(true);
    ui->spinStartingKey->setValue(36);
    ui->spinNumberOfKeys->setMaximum(92);
    ui->spinStartingKey->blockSignals(false);

    ui->spinNumberOfKeys->blockSignals(true);
    ui->spinNumberOfKeys->setValue(61);
    ui->spinNumberOfKeys->blockSignals(false);

    keyRangeChanged();
}

void MainWindow::on_radioPianoRange_clicked()
{
    ui->spinStartingKey->blockSignals(true);
    ui->spinStartingKey->setValue(21);
    ui->spinNumberOfKeys->setMaximum(107);
    ui->spinStartingKey->blockSignals(false);

    ui->spinNumberOfKeys->blockSignals(true);
    ui->spinNumberOfKeys->setValue(88);
    ui->spinNumberOfKeys->blockSignals(false);

    keyRangeChanged();
}

void MainWindow::on_radioFullRange_clicked()
{
    ui->spinStartingKey->blockSignals(true);
    ui->spinStartingKey->setValue(0);
    ui->spinNumberOfKeys->setMaximum(128);
    ui->spinStartingKey->blockSignals(false);

    ui->spinNumberOfKeys->blockSignals(true);
    ui->spinNumberOfKeys->setValue(128);
    ui->spinNumberOfKeys->blockSignals(false);

    keyRangeChanged();
}

void MainWindow::on_spinStartingKey_valueChanged(int arg1)
{
    Q_UNUSED(arg1)
    ui->radioCustomRange->setChecked(true);
    keyRangeChanged();
}

void MainWindow::on_spinNumberOfKeys_valueChanged(int arg1)
{
    Q_UNUSED(arg1)
    ui->radioCustomRange->setChecked(true);
    keyRangeChanged();
}

void MainWindow::keyRangeChanged()
{
    ui->spinNumberOfKeys->blockSignals(true);
    ui->spinNumberOfKeys->setMaximum(128 - ui->spinStartingKey->value());
    ui->spinNumberOfKeys->blockSignals(false);

    ui->virtualPianoKeyboard->set(PianoKeybd::PROPERTY_KEY_MIN, ui->spinStartingKey->value());
    ui->virtualPianoKeyboard->set(PianoKeybd::PROPERTY_KEY_NUMBER, ui->spinNumberOfKeys->value());
}

void MainWindow::on_pushColor1_clicked()  { colorChange(PianoKeybd::PROPERTY_COLOR_1);  }
void MainWindow::on_pushColor2_clicked()  { colorChange(PianoKeybd::PROPERTY_COLOR_2);  }
void MainWindow::on_pushColor3_clicked()  { colorChange(PianoKeybd::PROPERTY_COLOR_3);  }
void MainWindow::on_pushColor4_clicked()  { colorChange(PianoKeybd::PROPERTY_COLOR_4);  }
void MainWindow::on_pushColor5_clicked()  { colorChange(PianoKeybd::PROPERTY_COLOR_5);  }
void MainWindow::on_pushColor6_clicked()  { colorChange(PianoKeybd::PROPERTY_COLOR_6);  }
void MainWindow::on_pushColor7_clicked()  { colorChange(PianoKeybd::PROPERTY_COLOR_7);  }
void MainWindow::on_pushColor8_clicked()  { colorChange(PianoKeybd::PROPERTY_COLOR_8);  }
void MainWindow::on_pushColor9_clicked()  { colorChange(PianoKeybd::PROPERTY_COLOR_9);  }
void MainWindow::on_pushColor10_clicked() { colorChange(PianoKeybd::PROPERTY_COLOR_10); }
void MainWindow::on_pushColor11_clicked() { colorChange(PianoKeybd::PROPERTY_COLOR_11); }
void MainWindow::on_pushColor12_clicked() { colorChange(PianoKeybd::PROPERTY_COLOR_12); }
void MainWindow::on_pushColor13_clicked() { colorChange(PianoKeybd::PROPERTY_COLOR_13); }
void MainWindow::on_pushColor14_clicked() { colorChange(PianoKeybd::PROPERTY_COLOR_14); }
void MainWindow::on_pushColor15_clicked() { colorChange(PianoKeybd::PROPERTY_COLOR_15); }
void MainWindow::on_pushColor16_clicked() { colorChange(PianoKeybd::PROPERTY_COLOR_16); }
void MainWindow::on_pushColorWhiteKeys_clicked()  { colorChange(PianoKeybd::PROPERTY_COLOR_WHITE_KEYS); }
void MainWindow::on_pushColorBlackKeys_clicked()  { colorChange(PianoKeybd::PROPERTY_COLOR_BLACK_KEYS); }
void MainWindow::on_pushColorTextWhite_clicked()  { colorChange(PianoKeybd::PROPERTY_COLOR_TEXT_WHITE_KEYS); }
void MainWindow::on_pushColorTextBlack_clicked()  { colorChange(PianoKeybd::PROPERTY_COLOR_TEXT_BLACK_KEYS); }

void MainWindow::colorChange(PianoKeybd::KeyboardProperty colorProperty)
{
    QColorDialog::ColorDialogOptions option = 0;
    QColor color = QColorDialog::getColor(ui->virtualPianoKeyboard->get(colorProperty).value<QColor>(),
                                          this, trUtf8("Choose a color"), option);
    if (color.isValid())
    {
        ui->virtualPianoKeyboard->set(colorProperty, color);
        initColors();
    }
}

void MainWindow::on_comboColorationType_currentIndexChanged(int index)
{
    PianoKeybd::ColorationType type = PianoKeybd::COLORATION_TYPE_NONE;
    switch (index)
    {
    case 0: type = PianoKeybd::COLORATION_TYPE_UNIQUE;  break;
    case 1: type = PianoKeybd::COLORATION_TYPE_DUAL;    break;
    case 2: type = PianoKeybd::COLORATION_TYPE_CHANNEL; break;
    case 3: type = PianoKeybd::COLORATION_TYPE_DEGREE;  break;
    default: break;
    }
    ui->virtualPianoKeyboard->set(PianoKeybd::PROPERTY_COLORATION_TYPE, type);
}

void MainWindow::on_spinChannel_valueChanged(int arg1)
{
    ui->virtualPianoKeyboard->set(PianoKeybd::PROPERTY_CHANNEL, arg1);
}

void MainWindow::on_spinVelocity_valueChanged(int arg1)
{
    ui->virtualPianoKeyboard->set(PianoKeybd::PROPERTY_VELOCITY, arg1);
}

void MainWindow::on_pushCustomColor_clicked()
{
    QColor color = QColorDialog::getColor(_customColor, this, trUtf8("Choose a color"));
    if (color.isValid())
    {
        _customColor = color;
        initColors();
    }
}

void MainWindow::on_checkEnableKeyboard_clicked(bool checked)
{
    ui->virtualPianoKeyboard->set(PianoKeybd::PROPERTY_ENABLE_COMPUTER_KEYBOARD, checked);
}

void MainWindow::on_checkEnableMouse_clicked(bool checked)
{
    ui->virtualPianoKeyboard->set(PianoKeybd::PROPERTY_ENABLE_MOUSE, checked);
}

void MainWindow::on_checkEnableTouch_clicked(bool checked)
{
    ui->virtualPianoKeyboard->set(PianoKeybd::PROPERTY_ENABLE_TOUCH, checked);
}

void MainWindow::on_pushApplyColor_clicked()
{
    ui->virtualPianoKeyboard->customize(ui->spinCustomKey->value(), PianoKeybd::CUSTOMIZATION_TYPE_COLOR, _customColor);
}

void MainWindow::on_pushAddMarker_clicked()
{
    PianoKeybd::MarkerType type = PianoKeybd::MARKER_TYPE_NONE;
    switch (ui->comboMarkerType->currentIndex())
    {
    case 0:  type = PianoKeybd::MARKER_TYPE_ARROW_BLUE;   break;
    case 1:  type = PianoKeybd::MARKER_TYPE_ARROW_RED;    break;
    case 2:  type = PianoKeybd::MARKER_TYPE_ARROW_YELLOW; break;
    case 3:  type = PianoKeybd::MARKER_TYPE_ARROW_BLACK;  break;
    case 4:  type = PianoKeybd::MARKER_TYPE_ARROW_WHITE;  break;
    case 5:  type = PianoKeybd::MARKER_TYPE_CROSS_BLUE;   break;
    case 6:  type = PianoKeybd::MARKER_TYPE_CROSS_RED;    break;
    case 7:  type = PianoKeybd::MARKER_TYPE_CROSS_YELLOW; break;
    case 8:  type = PianoKeybd::MARKER_TYPE_CROSS_BLACK;  break;
    case 9:  type = PianoKeybd::MARKER_TYPE_CROSS_WHITE;  break;
    case 10: type = PianoKeybd::MARKER_TYPE_DOT_BLUE;     break;
    case 11: type = PianoKeybd::MARKER_TYPE_DOT_RED;      break;
    case 12: type = PianoKeybd::MARKER_TYPE_DOT_YELLOW;   break;
    case 13: type = PianoKeybd::MARKER_TYPE_DOT_BLACK;    break;
    case 14: type = PianoKeybd::MARKER_TYPE_DOT_WHITE;    break;
    case 15: type = PianoKeybd::MARKER_TYPE_STAR_BLUE;    break;
    case 16: type = PianoKeybd::MARKER_TYPE_STAR_RED;     break;
    case 17: type = PianoKeybd::MARKER_TYPE_STAR_YELLOW;  break;
    case 18: type = PianoKeybd::MARKER_TYPE_STAR_BLACK;   break;
    case 19: type = PianoKeybd::MARKER_TYPE_STAR_WHITE;   break;
    default: break;
    }
    ui->virtualPianoKeyboard->customize(ui->spinCustomKey->value(), PianoKeybd::CUSTOMIZATION_TYPE_MARKER, type);
}

void MainWindow::on_pushCustomReset_clicked()
{
    ui->virtualPianoKeyboard->clearCustomization();
}

void MainWindow::on_radioLabelNone_clicked()
{
    ui->virtualPianoKeyboard->set(PianoKeybd::PROPERTY_LABEL_TYPE, PianoKeybd::LABEL_TYPE_NONE);
}

void MainWindow::on_radioLabelNameSharp_clicked()
{
    ui->virtualPianoKeyboard->set(PianoKeybd::PROPERTY_LABEL_TYPE, PianoKeybd::LABEL_TYPE_KEY_NAME_WITH_SHARPS);
}

void MainWindow::on_radioLabelNameFlat_clicked()
{
    ui->virtualPianoKeyboard->set(PianoKeybd::PROPERTY_LABEL_TYPE, PianoKeybd::LABEL_TYPE_KEY_NAME_WITH_FLATS);
}

void MainWindow::on_radioLabelMapping_clicked()
{
    ui->virtualPianoKeyboard->set(PianoKeybd::PROPERTY_LABEL_TYPE, PianoKeybd::LABEL_TYPE_MAPPING);
}

void MainWindow::on_radioLabelOctaves_clicked()
{
    ui->virtualPianoKeyboard->set(PianoKeybd::PROPERTY_LABEL_TYPE, PianoKeybd::LABEL_TYPE_OCTAVES);
}

void MainWindow::on_radioLabelNumber_clicked()
{
    ui->virtualPianoKeyboard->set(PianoKeybd::PROPERTY_LABEL_TYPE, PianoKeybd::LABEL_TYPE_NUMBER);
}

void MainWindow::on_radioLabelCustom_clicked()
{
    ui->virtualPianoKeyboard->set(PianoKeybd::PROPERTY_LABEL_TYPE, PianoKeybd::LABEL_TYPE_CUSTOM);
}

void MainWindow::on_radioLabelCustomOctave_clicked()
{
    ui->virtualPianoKeyboard->set(PianoKeybd::PROPERTY_LABEL_TYPE, PianoKeybd::LABEL_TYPE_CUSTOM_WITH_OCTAVE_NUMBER);
}

void MainWindow::on_radioLabelC3_clicked()
{
    ui->virtualPianoKeyboard->set(PianoKeybd::PROPERTY_MIDDLE_C, PianoKeybd::MIDDLE_KEY_C3);
}

void MainWindow::on_radioLabelC4_clicked()
{
    ui->virtualPianoKeyboard->set(PianoKeybd::PROPERTY_MIDDLE_C, PianoKeybd::MIDDLE_KEY_C4);
}

void MainWindow::on_radioLabelC5_clicked()
{
    ui->virtualPianoKeyboard->set(PianoKeybd::PROPERTY_MIDDLE_C, PianoKeybd::MIDDLE_KEY_C5);
}

void MainWindow::on_checkLabelIndices_clicked(bool checked)
{
    ui->virtualPianoKeyboard->set(PianoKeybd::PROPERTY_LABEL_OCTAVE_INDICE, checked);
}

void MainWindow::on_spinTranspose_valueChanged(int arg1)
{
    ui->virtualPianoKeyboard->set(PianoKeybd::PROPERTY_TRANSPOSE, arg1);
}

void MainWindow::on_comboFirstC_currentIndexChanged(int index)
{
    ui->virtualPianoKeyboard->set(PianoKeybd::PROPERTY_MAPPING_FIRST_NOTE, index * 12);
}

void MainWindow::on_listWidget_itemChanged(QListWidgetItem *item)
{
    Q_UNUSED(item)
    QStringList customLabels;
    for (int i = 0; i < ui->listWidget->count(); i++)
        customLabels << ui->listWidget->item(i)->text();
    ui->virtualPianoKeyboard->set(PianoKeybd::PROPERTY_CUSTOM_LABELS, customLabels);
}
