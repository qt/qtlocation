/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativegeomapmouseevent_p.h"

QT_BEGIN_NAMESPACE

/*!
    \qmltype MapMouseEvent
    \instantiates QDeclarativeGeoMapMouseEvent
    \inqmlmodule QtLocation
    \ingroup qml-QtLocation5-maps
    \since Qt Location 5.0

    \brief The MapMouseEvent object provides information about a mouse event.

    The position of the mouse can be found via the \l x and \l y properties
    or the \l coordinate property. Both \a x and \a y are relative to the
    \l MapMouseArea that generated the event.
    The button that caused the event is available via the \l button property.

    \sa MapMouseArea
*/

QDeclarativeGeoMapMouseEvent::QDeclarativeGeoMapMouseEvent(QObject *parent)
    : QObject(parent),
      accepted_(false),
      button_(Qt::NoButton),
      modifiers_(Qt::NoModifier),
      wasHeld_(false),
      x_(0),
      y_(0),
      buttons_(Qt::NoButton)
{
}

QDeclarativeGeoMapMouseEvent::~QDeclarativeGeoMapMouseEvent()
{
}

/*!
    \qmlproperty bool QtLocation::MapMouseEvent::accepted

    Setting \a accepted to true prevents the mouse event from being
    propagated to items below the item currently handling the event.

    Generally, if the item acts on the mouse event then it should be accepted
    so that items lower in the stacking order do not also respond to the same event.
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
    \qmlproperty enumeration QtLocation::MapMouseEvent::button

    This property holds the button that caused the event.  It can be one of:

    \list
    \li Qt.LeftButton
    \li Qt.RightButton
    \li Qt.MiddleButton
    \endlist

    \sa buttons
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
    \qmlproperty enumeration QtLocation::MapMouseEvent::buttons

    This property holds the mouse buttons pressed when the event was
    generated. For mouse move events, this is all buttons that are pressed
    down. For mouse press and double click events this includes the button
    that caused the event. For mouse release events this excludes the button
    that caused the event.

    It contains a bitwise combination of:

    \list
    \li Qt.LeftButton
    \li Qt.RightButton
    \li Qt.MiddleButton
    \endlist

    \sa button
*/

void QDeclarativeGeoMapMouseEvent::setButtons(int buttons)
{
    buttons_ = buttons;
}

int QDeclarativeGeoMapMouseEvent::buttons() const
{
    return buttons_;
}

/*!
    \qmlproperty int QtLocation::MapMouseEvent::modifiers

    This property holds the keyboard modifier flags that existed immediately
    before the event occurred.

    It contains a bitwise combination of:
    \list
    \li Qt.NoModifier - No modifier key is pressed.
    \li Qt.ShiftModifier - A Shift key on the keyboard is pressed.
    \li Qt.ControlModifier - A Ctrl key on the keyboard is pressed.
    \li Qt.AltModifier - An Alt key on the keyboard is pressed.
    \li Qt.MetaModifier - A Meta key on the keyboard is pressed.
    \li Qt.KeypadModifier - A keypad button is pressed.
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

/*!
    \qmlproperty bool QtLocation::MapMouseEvent::wasHeld

     This property is true if the mouse button has been held pressed longer the threshold (800ms).
*/

void QDeclarativeGeoMapMouseEvent::setWasHeld(bool wasHeld)
{
    wasHeld_ = wasHeld;
}

bool QDeclarativeGeoMapMouseEvent::wasHeld() const
{
    return wasHeld_;
}

/*!
    \qmlproperty int QtLocation::MapMouseEvent::x

    This property holds the x-screen coordinate of the position supplied
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

/*!
    \qmlproperty int QtLocation::MapMouseEvent::y

    This property holds the y-screen coordinate of the position supplied
    by the mouse event.
*/

void QDeclarativeGeoMapMouseEvent::setY(int y)
{
    y_ = y;
}

int QDeclarativeGeoMapMouseEvent::y() const
{
    return y_;
}

/*!
    \qmlproperty coordinate QtLocation::MapMouseEvent::coordinate

    This property holds the coordinate corresponding to the latitude
    and longitude of the position on the map at which the mouse event
    occurred.
*/

void QDeclarativeGeoMapMouseEvent::setCoordinate(const QGeoCoordinate &coordinate)
{
    coordinate_ = coordinate;
}

QGeoCoordinate QDeclarativeGeoMapMouseEvent::coordinate()
{
    return coordinate_;
}

#include "moc_qdeclarativegeomapmouseevent_p.cpp"

QT_END_NAMESPACE
