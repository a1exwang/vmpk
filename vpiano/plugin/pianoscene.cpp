/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2014, Pedro Lopez-Cabanillas <plcl@users.sf.net>
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

#include "pianoscene.h"
#include <QApplication>
#include <QPalette>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <qmath.h>


int PianoScene::MIN_NOTE = 0;
int PianoScene::MAX_NOTE = 127;
int PianoScene::KEYWIDTH = 18;
int PianoScene::KEYHEIGHT = 72;
KeyboardMap * PianoScene::m_keybdMap = NULL;

qreal PianoScene::sceneWidth(int startKey, int keys)
{
    int nbWidth = 0;
    if (getOffsetX(startKey) != 0)
    {
        nbWidth++;
        startKey++;
    }
    if (getOffsetX(startKey + keys - 1) != 0)
    {
        nbWidth++;
        keys--;
    }
    for (int i = startKey; i < startKey + keys; i++)
        if (getOffsetX(i) == 0)
            nbWidth++;

    return KEYWIDTH * nbWidth;
}

float PianoScene::getOffsetX(int numKey)
{
    float offset;
    switch (numKey % 12)
    {
    case 1:  offset = .425; break;
    case 6:  offset = .35;  break;
    case 8:  offset = .5;   break;
    case 3:  offset = .575; break;
    case 10: offset = .65;  break;
    default: offset = 0;    break;
    }
    return offset;
}

PianoScene::PianoScene (const int startKey, const int numKeys, PianoScene *previousScene, QObject * parent ) :
    QGraphicsScene(QRectF(0, 0, sceneWidth(startKey, numKeys), KEYHEIGHT), parent),
    m_numKeys(numKeys),
    m_startKey(startKey),
    m_minNote(0),
    m_maxNote(127),
    m_rawkbd(false)
{
    // Initialisation
    initConfiguration(previousScene);
    initLabels();

    // Keyboard creation
    createKeyboard();

    refreshKeys();
    refreshLabels();
}

void PianoScene::initConfiguration(PianoScene * previousScene)
{
    if (previousScene)
    {
        // recovering configuration
        m_mouseEnabled = previousScene->m_mouseEnabled;
        m_touchEnabled = previousScene->m_touchEnabled;
        m_keyboardEnabled = previousScene->m_keyboardEnabled;
        m_mousePressed = previousScene->m_mousePressed;
        m_channel = previousScene->m_channel;
        m_velocity = previousScene->m_velocity;
        m_colorationType = previousScene->m_colorationType;
        m_transpose = previousScene->m_transpose;
        for (int i = -2; i < 16; i++)
            m_palette[i] = previousScene->m_palette.value(i);
        m_customColors = previousScene->m_customColors;
        m_markers = previousScene->m_markers;
        m_labelType = previousScene->m_labelType;
        m_middleC = previousScene->m_middleC;
        m_indicesInLabels = previousScene->m_indicesInLabels;
    }
    else
    {
        // default configuration
        m_mouseEnabled = true;
        m_touchEnabled = true;
        m_keyboardEnabled = true;
        m_mousePressed = false;
        m_channel = 0;
        m_velocity = 100;
        m_colorationType = PianoKeybd::COLORATION_TYPE_UNIQUE;
        m_transpose = 0;
        m_labelType = PianoKeybd::LABEL_TYPE_NONE;
        m_middleC = PianoKeybd::MIDDLE_KEY_C4;
        m_indicesInLabels = true;

        // color initialisation
        m_palette[-4] = QColor(255, 255, 255);
        m_palette[-3] = QColor(  0,   0,   0);
        m_palette[-2] = QColor(  0,   0,   0);
        m_palette[-1] = QColor(255, 255, 255);
        m_palette[0]  = QColor(  0,   0, 255);
        m_palette[1]  = QColor(255,   0,   0);
        m_palette[2]  = QColor(  0, 255,   0);
        m_palette[3]  = QColor(  0, 127, 127);
        m_palette[4]  = QColor(127, 127,   0);
        m_palette[5]  = QColor(  0,   0, 127);
        m_palette[6]  = QColor(127,   0,   0);
        m_palette[7]  = QColor(  0, 127,   0);
        m_palette[8]  = QColor(  0, 255, 255);
        m_palette[9]  = QColor(255, 255,   0);
        m_palette[10] = QColor(255,   0, 255);
        m_palette[11] = QColor(127, 127, 127);
        m_palette[12] = QColor( 85, 170, 127);
        m_palette[13] = QColor(255, 110, 110);
        m_palette[14] = QColor( 85,  40, 210);
        m_palette[15] = QColor(  0, 230, 160);
    }
}

void PianoScene::initLabels()
{
    m_names_s.clear();
    m_names_f.clear();
    m_names_s << trUtf8("C")
              << trUtf8("C#")
              << trUtf8("D")
              << trUtf8("D#")
              << trUtf8("E")
              << trUtf8("F")
              << trUtf8("F#")
              << trUtf8("G")
              << trUtf8("G#")
              << trUtf8("A")
              << trUtf8("A#")
              << trUtf8("B");
    m_names_f << trUtf8("C")
              << trUtf8("Db")
              << trUtf8("D")
              << trUtf8("Eb")
              << trUtf8("E")
              << trUtf8("F")
              << trUtf8("Gb")
              << trUtf8("G")
              << trUtf8("Ab")
              << trUtf8("A")
              << trUtf8("Bb")
              << trUtf8("B");
    while (m_names_custom.size() != 12)
        m_names_custom << "";
}

void PianoScene::createKeyboard()
{
    QFont lblFont("Tahoma", 8);

    int skippedWhiteKeys = m_startKey % 12;
    if (skippedWhiteKeys >= 5)
        skippedWhiteKeys = (skippedWhiteKeys + 1) / 2;
    else
        skippedWhiteKeys = skippedWhiteKeys / 2;
    skippedWhiteKeys += 7 * (m_startKey / 12);

    for (int i = m_startKey; i < m_startKey + m_numKeys; i++)
    {
        // White key number (or the one just before a black key)
        int degree = i % 12;
        int numKeyX = (i / 12) * 7;
        if (degree >= 5)
            numKeyX += (degree + 1) / 2;
        else
            numKeyX += degree / 2;
        numKeyX -= skippedWhiteKeys;

        float offsetX = getOffsetX(i);
        float coefWidth, coefHeight;
        if (offsetX == 0)
        {
            coefWidth = 1;
            coefHeight = 1;
        }
        else
        {
            coefWidth = .8;
            coefHeight = .6;
        }

        // Dimensions and creation
        float x = numKeyX * KEYWIDTH;
        if (offsetX != 0)
            x += ((1. - coefWidth) / 2 + offsetX) * KEYWIDTH;
        PianoKey* key = new PianoKey(QRectF(x, 0, KEYWIDTH * coefWidth, KEYHEIGHT * coefHeight), offsetX != 0, i);
        KeyLabel* lbl = new KeyLabel(key);
        if (offsetX == 0)
        {
            lbl->setDefaultTextColor(m_palette.value(-4));
            lbl->setPos(x - 2, KEYHEIGHT);
        }
        else
        {
            lbl->setDefaultTextColor(m_palette.value(-5));
            lbl->setPos(x - 3, KEYHEIGHT * coefHeight - 3);
        }

        key->setAcceptTouchEvents(true);
        m_keys.insert(i, key);
        addItem(key);
        lbl->setFont(lblFont);
        m_labels.insert(i, lbl);
    }
}

QSize PianoScene::sizeHint() const
{
    return QSize(sceneWidth(m_startKey, m_numKeys), KEYHEIGHT);
}

void PianoScene::showKeyOn(PianoKey* key, int vel, int channel)
{
    if (vel == -1)
        vel = m_velocity;
    if (channel == -1)
        channel = m_channel;
    if (vel >= 0)
    {
        QColor color;
        switch (m_colorationType)
        {
        case PianoKeybd::COLORATION_TYPE_UNIQUE:
            color = m_palette.value(0);
            break;
        case PianoKeybd::COLORATION_TYPE_DUAL:
            color = m_palette.value(key->isBlack());
            break;
        case PianoKeybd::COLORATION_TYPE_CHANNEL:
            color = m_palette.value(channel % 16);
            break;
        case PianoKeybd::COLORATION_TYPE_DEGREE:
            color = m_palette.value(key->getNote() % 12);
            break;
        default:
            break;
        }

        if (m_colorationType == PianoKeybd::COLORATION_TYPE_NONE)
        {
            if (key->isBlack())
                key->setPressedBrush(PianoKey::BLACK_BRUSH);
            else
                key->setPressedBrush(PianoKey::WHITE_BRUSH);
        }
        else
            key->setPressedBrush(color.lighter(227 - vel));
    }
    key->setPressed(true);
}

void PianoScene::showKeyOff(PianoKey* key)
{
    key->setPressed(false);
}

void PianoScene::showNoteOn(const int note, int vel, int channel)
{
    int n = note - m_transpose;
    if ((note >= m_minNote) && (note <= m_maxNote) && m_keys.contains(n))
            showKeyOn(m_keys.value(n), vel, channel);
}

void PianoScene::showNoteOff(const int note, int channel)
{
    Q_UNUSED(channel)

    int n = note - m_transpose;
    if ((note >= m_minNote) && (note <= m_maxNote) && m_keys.contains(n))
        showKeyOff(m_keys.value(n));
}

void PianoScene::triggerNoteOn(const int note, const int vel)
{
    int n = note + m_transpose;
    if ((n >= m_minNote) && (n <= m_maxNote))
        emit noteOn(n, vel);
}

void PianoScene::triggerNoteOff( const int note )
{
    int n = note + m_transpose;
    if ((n >= m_minNote) && (n <= m_maxNote))
        emit noteOff(n);
}

void PianoScene::keyOn(PianoKey* key, qreal pressure)
{
    int vel;
    if (pressure < 0)
        vel = m_velocity;
    else
        vel = m_velocity * pressure;
    triggerNoteOn(key->getNote(), vel);
    showKeyOn(key, vel);
}

void PianoScene::keyOff( PianoKey* key )
{
    triggerNoteOff(key->getNote());
    showKeyOff(key);
}

void PianoScene::keyOn(const int note)
{
    if (m_keys.contains(note))
        keyOn(m_keys.value(note));
    else
        triggerNoteOn(note, m_velocity);
}

void PianoScene::keyOff(const int note)
{
    if (m_keys.contains(note))
        keyOff(m_keys.value(note));
    else
        triggerNoteOff(note);
}

PianoKey* PianoScene::getKeyForPos( const QPointF& p ) const
{
    PianoKey* key = 0;
    QList<QGraphicsItem *> ptitems = this->items(p, Qt::IntersectsItemShape, Qt::DescendingOrder);
    foreach(QGraphicsItem *itm, ptitems)
    {
        key = dynamic_cast<PianoKey*>(itm);
        if (key != 0)
            break;
    }
    return key;
}

void PianoScene::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    if (m_mouseEnabled)
    {
        if (m_mousePressed)
        {
            PianoKey* key = getKeyForPos(mouseEvent->scenePos());
            PianoKey* lastkey = getKeyForPos(mouseEvent->lastScenePos());
            if ((lastkey != NULL) && (lastkey != key) && lastkey->isPressed())
                keyOff(lastkey);

            if ((key != NULL) && !key->isPressed())
                keyOn(key);

            mouseEvent->accept();
            return;
        }
        else
        {
            PianoKey* key = getKeyForPos(mouseEvent->scenePos());
            if (key)
                mouseOver(key->getNote() + m_transpose);
            else
                mouseOver(-1);
        }
    }
}

void PianoScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    if (m_mouseEnabled)
    {
        PianoKey* key = getKeyForPos(mouseEvent->scenePos());
        if (key != NULL && !key->isPressed())
        {
            keyOn(key);
            m_mousePressed = true;
            mouseEvent->accept();
            return;
        }
    }
}

void PianoScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    if (m_mouseEnabled)
    {
        m_mousePressed = false;
        PianoKey* key = getKeyForPos(mouseEvent->scenePos());
        if (key != NULL && key->isPressed())
        {
            keyOff(key);
            mouseEvent->accept();
            return;
        }
    }
}

void PianoScene::keyPressEvent(QKeyEvent * keyEvent)
{
    if ( m_keyboardEnabled)
    {
        if ( !m_rawkbd && !keyEvent->isAutoRepeat() ) // ignore auto-repeats
        {
            QString modifier;
            if (keyEvent->modifiers() & Qt::ShiftModifier)
                modifier = "Shift+";
            QString key = QKeySequence(keyEvent->key()).toString();
            QKeySequence(modifier + key);
            int note = m_keybdMap->getKey(QKeySequence(modifier + key));
            if (note > -1)
                keyOn(note);
        }
        keyEvent->accept();
        return;
    }
    keyEvent->ignore();
}

void PianoScene::keyReleaseEvent(QKeyEvent * keyEvent)
{
    if (m_keyboardEnabled)
    {
        if ( !m_rawkbd && !keyEvent->isAutoRepeat() ) // ignore auto-repeats
        {
            QString modifier;
            if (keyEvent->modifiers() & Qt::ShiftModifier)
                modifier = "Shift+";
            QString key = QKeySequence(keyEvent->key()).toString();
            QKeySequence(modifier + key);
            int note = m_keybdMap->getKey(QKeySequence(modifier + key));
            if (note > -1)
                keyOff(note);
        }
        keyEvent->accept();
        return;
    }
    keyEvent->ignore();
}

bool PianoScene::event(QEvent *event)
{
    switch(event->type())
    {
    case QEvent::TouchBegin:
    case QEvent::TouchEnd:
    case QEvent::TouchUpdate:
    {
        if (m_touchEnabled)
        {
            QTouchEvent *touchEvent = static_cast<QTouchEvent*>(event);
            QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
            foreach(const QTouchEvent::TouchPoint& touchPoint, touchPoints)
            {
                switch (touchPoint.state())
                {
                //case Qt::TouchPointPrimary:
                case Qt::TouchPointStationary:
                    continue;
                case Qt::TouchPointReleased: {
                        PianoKey* key = getKeyForPos(touchPoint.scenePos());
                        if (key != NULL && key->isPressed())
                            keyOff(key);
                        break;
                    }
                case Qt::TouchPointPressed: {
                        PianoKey* key = getKeyForPos(touchPoint.scenePos());
                        if (key != NULL && !key->isPressed()) {
                            keyOn(key, touchPoint.pressure());
                            key->ensureVisible();
                        }
                        break;
                    }
                case Qt::TouchPointMoved: {
                        PianoKey* key = getKeyForPos(touchPoint.scenePos());
                        PianoKey* lastkey = getKeyForPos(touchPoint.lastScenePos());
                        if ((lastkey != NULL) && (lastkey != key) && lastkey->isPressed())
                            keyOff(lastkey);
                        if ((key != NULL) && !key->isPressed())
                            keyOn(key, touchPoint.pressure());
                        break;
                    }
                default:
                    break;
                }
            }
            event->accept();
            return true;
        }
        break;
    }
    case QEvent::Leave:
        mouseOver(-1);
        break;
    default:
        break;
    }

    return QGraphicsScene::event(event);
}

void PianoScene::allKeysOff()
{
    foreach(PianoKey* key, m_keys)
    {
        key->setPressed(false);
    }
}

QString PianoScene::noteName(int note)
{
    QString str;

    note += m_transpose;
    if (note < m_minNote || note > m_maxNote)
        return "";

    int degree = note % 12;
    int oct;
    if (m_labelType == PianoKeybd::LABEL_TYPE_KEY_NAME_WITH_FLATS)
        oct = (note + 4) / 12;
    else
        oct = (note + 3) / 12;
    if (m_middleC == PianoKeybd::MIDDLE_KEY_C3)
        oct--;
    else if (m_middleC == PianoKeybd::MIDDLE_KEY_C5)
        oct++;

    switch (m_labelType)
    {
    case PianoKeybd::LABEL_TYPE_NONE:
        str = "";
        break;
    case PianoKeybd::LABEL_TYPE_KEY_NAME_WITH_SHARPS:
        str = m_names_s.at(degree);
        break;
    case PianoKeybd::LABEL_TYPE_KEY_NAME_WITH_FLATS:
        str = m_names_f.at(degree);
        break;
    case PianoKeybd::LABEL_TYPE_NUMBER:
        str = QString::number(note);
        break;
    case PianoKeybd::LABEL_TYPE_OCTAVES:
        if (degree == 0)
            str = m_names_s.first();
        break;
    case PianoKeybd::LABEL_TYPE_MAPPING:{
        QList<QKeySequence> listSequences = m_keybdMap->getSequences(note);
        if (!listSequences.isEmpty())
        {
            str = formatSequence(listSequences.first());
            if (listSequences.size() > 1)
                str += " / " + formatSequence(listSequences.at(1));
        }
    }break;
    case PianoKeybd::LABEL_TYPE_CUSTOM:
    case PianoKeybd::LABEL_TYPE_CUSTOM_WITH_OCTAVE_NUMBER:
        str = m_names_custom.at(degree);
        break;
    }

    // Octave number
    if (m_labelType != PianoKeybd::LABEL_TYPE_NONE &&
            m_labelType != PianoKeybd::LABEL_TYPE_NUMBER &&
            m_labelType != PianoKeybd::LABEL_TYPE_MAPPING &&
            m_labelType != PianoKeybd::LABEL_TYPE_CUSTOM &&
            (m_labelType != PianoKeybd::LABEL_TYPE_OCTAVES || degree == 0))
    {
        if (m_indicesInLabels)
            str += "<span style='vertical-align:sub;'>" + QString::number(oct) + "</span>";
        else
            str += QString::number(oct);
    }

    return str;
}

QString PianoScene::formatSequence(QKeySequence sequence)
{
    int key = sequence[0];
    int keyWithNoModifier = key & ~Qt::ShiftModifier;
    QString str = QKeySequence(keyWithNoModifier).toString();
    if (key != keyWithNoModifier)
    {
        str.prepend("<u>");
        str.append("</u>");
    }
    return str;
}

void PianoScene::refreshLabels()
{
    foreach(KeyLabel* lbl, m_labels)
    {
        PianoKey* key = dynamic_cast<PianoKey*>(lbl->parentItem());
        if (key != NULL)
        {
            if (key->isBlack())
                lbl->setDefaultTextColor(m_palette.value(-4));
            else
                lbl->setDefaultTextColor(m_palette.value(-3));
            lbl->setHtml(noteName(key->getNote()));
            lbl->setVisible(m_labelType != PianoKeybd::LABEL_TYPE_NONE);
        }
    }
}

void PianoScene::refreshKeys()
{
    PianoKey::WHITE_BRUSH = QBrush(m_palette.value(-1));
    PianoKey::BLACK_BRUSH = QBrush(m_palette.value(-2));

    foreach(PianoKey* key, m_keys)
    {
        if (m_customColors.contains(key->getNote()))
            key->setBrush(m_customColors.value(key->getNote()));
        else
            key->resetBrush();

        if (m_markers.contains(key->getNote()))
            key->setMarker(m_markers.value(key->getNote()));
        else
            key->setMarker(PianoKeybd::MARKER_TYPE_NONE);

        key->setPressed(false);
    }

    invalidate();
}

void PianoScene::setLabelType(PianoKeybd::LabelType type)
{
    if (m_labelType != type)
    {
        m_labelType = type;
        refreshLabels();
    }
}

void PianoScene::setCustomNoteNames(const QStringList& names)
{
    if (names.size() < 12)
        return;
    m_names_custom = names;
    while (m_names_custom.size() > 12)
        m_names_custom.removeLast();
    refreshLabels();
}

void PianoScene::setMiddleC(PianoKeybd::MiddleKey middleC)
{
    if (m_middleC != middleC)
    {
        m_middleC = middleC;
        refreshLabels();
    }
}

void PianoScene::setIndicesInLabels(bool enabled)
{
    if (m_indicesInLabels != enabled)
    {
        m_indicesInLabels = enabled;
        refreshLabels();
    }
}

void PianoScene::setTranspose(const int transpose)
{
    if (m_transpose != transpose && transpose > -12 && transpose < 12)
    {
        m_transpose = transpose;
        refreshLabels();
    }
}

void PianoScene::setRawKeyboardMode(bool b)
{
    if (m_rawkbd != b)
        m_rawkbd = b;
}

void PianoScene::setKeyboardEnabled(const bool enable)
{
    if (enable != m_keyboardEnabled)
        m_keyboardEnabled = enable;
}

void PianoScene::setMouseEnabled(const bool enable)
{
    if (enable != m_mouseEnabled)
        m_mouseEnabled = enable;
}

void PianoScene::setTouchEnabled(const bool enable)
{
    if (enable != m_touchEnabled)
        m_touchEnabled = enable;
}

void PianoScene::setColorationType(PianoKeybd::ColorationType colorationType)
{
    if (m_colorationType != colorationType)
        m_colorationType = colorationType;
}

void PianoScene::setColor(int num, QColor color)
{
    m_palette[num] = color;
    if (num == -1 || num == -2)
        refreshKeys();

    else if (num == -3 || num == -4)
        refreshLabels();
}

// CUSTOMIZATION //

void PianoScene::addCustomColor(int key, QColor color)
{
    if (color.isValid())
        m_customColors[key] = color;
    else
        m_customColors.remove(key);
    refreshKeys();
}

void PianoScene::addMarker(int key, PianoKeybd::MarkerType markerType)
{
    if (markerType != PianoKeybd::MARKER_TYPE_NONE)
        m_markers[key] = markerType;
    else
        m_markers.remove(key);
    refreshKeys();
}

void PianoScene::clearCustomization()
{
    m_customColors.clear();
    m_markers.clear();
    refreshKeys();
}

void PianoScene::resetCustomization(int key, PianoKeybd::CustomizationType type)
{
    switch (type)
    {
    case PianoKeybd::CUSTOMIZATION_TYPE_COLOR:
        m_customColors.remove(key);
        break;
    case PianoKeybd::CUSTOMIZATION_TYPE_MARKER:
        m_markers.remove(key);
        break;
    }
    refreshKeys();
}
