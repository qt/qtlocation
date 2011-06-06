/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativegeomapmouseevent_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass MapMouseEvent QDeclarativeGeoMapMouseEvent

    \brief The MapMouseEvent object provides information about a mouse event.

    \ingroup qml-location-maps

    The position of the mouse can be found via the \l x and \l y properties
    or the \l coordinate property.
    The button that caused the event is available via the \l button property.

    \sa MapMouseArea
*/

QDeclarativeGeoMapMouseEvent::QDeclarativeGeoMapMouseEvent(QObject *parent)
    : QObject(parent),
      accepted_(false),
      button_(Qt::NoButton),
      coordinate_(0)
{
}

QDeclarativeGeoMapMouseEvent::~QDeclarativeGeoMapMouseEvent()
{
}

/*!
    \qmlproperty bool MapMouseEvent::accepted

    Setting \a accepted to true prevents the mouse event from being
    propagated to items below this item.

    Generally, if the item acts on the mouse event then it should be accepted
    so that items lower in the stacking order do not also respond to the same ev
ent.
*/

void QDeclarativeGeoMapMouseEvent::setAccepted(bool accepted)
{
    accepted_ = accepted;
}

bool QDeclarativeGeoMapMouseEvent::accepted() const
{
    return accepted_;
}

/*!
    \qmlproperty enumeration MapMouseEvent::button

    This property holds the button that caused the event.  It can be one of:
    \list
    \o Qt.LeftButton
    \o Qt.RightButton
    \o Qt.MiddleButton
    \endlist
*/

void QDeclarativeGeoMapMouseEvent::setButton(int button)
{
    button_ = button;
}

int QDeclarativeGeoMapMouseEvent::button() const
{
    return button_;
}

/*!
    \qmlproperty int MapMouseEvent::modifiers

    This property holds the keyboard modifier flags that existed immediately
    before the event occurred.

    It contains a bitwise combination of:
    \list
    \o Qt.NoModifier - No modifier key is pressed.
    \o Qt.ShiftModifier - A Shift key on the keyboard is pressed.
    \o Qt.ControlModifier - A Ctrl key on the keyboard is pressed.
    \o Qt.AltModifier - An Alt key on the keyboard is pressed.
    \o Qt.MetaModifier - A Meta key on the keyboard is pressed.
    \o Qt.KeypadModifier - A keypad button is pressed.
    \endlist
*/

void QDeclarativeGeoMapMouseEvent::setModifiers(int modifiers)
{
    modifiers_ = modifiers;
}

int QDeclarativeGeoMapMouseEvent::modifiers() const
{
    return modifiers_;
}

//void QDeclarativeGeoMapMouseEvent::setWasHeld(bool wasHeld)
//{
//    wasHeld_ = wasHeld;
//}

//bool QDeclarativeGeoMapMouseEvent::wasHeld() const
//{
//    return wasHeld_;
//}

/*!
    \qmlproperty int MapMouseEvent::x
    \qmlproperty int MapMouseEvent::y

    These properties hold the screen coordinates of the position supplied
    by the mouse event.
*/

void QDeclarativeGeoMapMouseEvent::setX(int x)
{
    x_ = x;
}

int QDeclarativeGeoMapMouseEvent::x() const
{
    return x_;
}

void QDeclarativeGeoMapMouseEvent::setY(int y)
{
    y_ = y;
}

int QDeclarativeGeoMapMouseEvent::y() const
{
    return y_;
}

/*!
    \qmlproperty Coordinate MapMouseEvent::coordinate

    This property holds the coordinate corresponding to the latitude
    and longitude of the position on the map at which the mouse event
    occurred.
*/

void QDeclarativeGeoMapMouseEvent::setCoordinate(QDeclarativeCoordinate *coordinate)
{
    if (!coordinate || (coordinate == coordinate_))
        return;

    coordinate_ = coordinate;
}

QDeclarativeCoordinate* QDeclarativeGeoMapMouseEvent::coordinate()
{
    return coordinate_;
}

#include "moc_qdeclarativegeomapmouseevent_p.cpp"

QTM_END_NAMESPACE
