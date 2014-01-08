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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSignalMapper>
#include "pianokeybd.h"
#include "editkey.h"

namespace Ui {
class MainWindow;
}
class QLabel;
class QListWidgetItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void noteChanged(int key, int vel = 0);
    void onMouseOver(int key);
    void cellChanged(QString txt);
    void on_radio5octaves_clicked();
    void on_radioPianoRange_clicked();
    void on_radioFullRange_clicked();
    void on_spinStartingKey_valueChanged(int arg1);
    void on_spinNumberOfKeys_valueChanged(int arg1);
    void on_pushColor1_clicked();
    void on_pushColor2_clicked();
    void on_pushColor3_clicked();
    void on_pushColor4_clicked();
    void on_pushColor5_clicked();
    void on_pushColor6_clicked();
    void on_pushColor7_clicked();
    void on_pushColor8_clicked();
    void on_pushColor9_clicked();
    void on_pushColor10_clicked();
    void on_pushColor11_clicked();
    void on_pushColor12_clicked();
    void on_pushColor13_clicked();
    void on_pushColor14_clicked();
    void on_pushColor15_clicked();
    void on_pushColor16_clicked();
    void on_pushColorWhiteKeys_clicked();
    void on_pushColorBlackKeys_clicked();
    void on_pushColorTextWhite_clicked();
    void on_pushColorTextBlack_clicked();
    void on_spinChannel_valueChanged(int arg1);
    void on_comboColorationType_currentIndexChanged(int index);
    void on_spinVelocity_valueChanged(int arg1);
    void on_pushApplyColor_clicked();
    void on_pushAddMarker_clicked();
    void on_pushCustomReset_clicked();
    void on_pushCustomColor_clicked();
    void on_checkEnableKeyboard_clicked(bool checked);
    void on_checkEnableMouse_clicked(bool checked);
    void on_checkEnableTouch_clicked(bool checked);
    void on_radioLabelNone_clicked();
    void on_radioLabelNameSharp_clicked();
    void on_radioLabelNameFlat_clicked();
    void on_radioLabelMapping_clicked();
    void on_radioLabelOctaves_clicked();
    void on_radioLabelNumber_clicked();
    void on_radioLabelCustom_clicked();
    void on_radioLabelCustomOctave_clicked();
    void on_radioLabelC3_clicked();
    void on_radioLabelC4_clicked();
    void on_radioLabelC5_clicked();
    void on_checkLabelIndices_clicked(bool checked);
    void on_spinTranspose_valueChanged(int arg1);
    void on_comboFirstC_currentIndexChanged(int index);
    void on_listWidget_itemChanged(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
    QColor _customColor;
    QLabel * _labelMouseOver;
    QSignalMapper * _signalMapper;

    void keyRangeChanged();
    void initColors();
    void initKeyTable();
    void colorChange(PianoKeybd::KeyboardProperty colorProperty);
};

#endif // MAINWINDOW_H
