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

#include "qdeclarativegeomapmousearea_p.h"

#include <QDebug>

#include "qdeclarativegeomapobject_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass MapMouseArea QDeclarativeGeoMapMouseArea

    \brief The MapMouseArea item enables simple mouse handling.

    \ingroup qml-location-maps

   A MapMouseArea is an invisible item that is typically used in conjunction
   with a visible map object or map item in order to provide mouse handling.
   By effectively acting as a proxy, the logic for mouse handling can be
   contained within a MapMouseArea item.

   The \l enabled property is used to enable and disable mouse handling for
   the proxied item. When disabled, the mouse area becomes transparent to
   mouse events.

   The \l pressed read-only property indicates whether or not the user is
   holding down a mouse button over the mouse area. This property is often
   used in bindings between properties in a user interface.

   Information about the mouse position and button clicks are provided via
   signals for which event handler properties are defined. The most commonly
   used involved handling mouse presses and clicks: onClicked, onDoubleClicked,
   onPressed and onReleased.

   MapMouseArea items only report mouse clicks and not changes to the
   position of the mouse cursor.

    \sa MapMouseEvent
*/

QDeclarativeGeoMapMouseArea::QDeclarativeGeoMapMouseArea(QDeclarativeItem *parent)
    : QDeclarativeItem(parent),
      enabled_(true),
      hoverEnabled_(false) {}

QDeclarativeGeoMapMouseArea::~QDeclarativeGeoMapMouseArea()
{
}

void QDeclarativeGeoMapMouseArea::setMap(QDeclarativeGraphicsGeoMap *map)
{
    map_ = map;
}

QDeclarativeGraphicsGeoMap* QDeclarativeGeoMapMouseArea::map() const
{
    return map_;
}

/*!
    \qmlproperty qreal MapMouseArea::mouseX
    \qmlproperty qreal MapMouseArea::mouseY

    These properties hold the screen coordinates of the mouse cursor.

    These properties will only be valid while a button is pressed, and will
    remain valid as long as the button is held down even if the mouse is moved
    outside the area.

    The screen coordinates are relative to the MouseArea.
*/

qreal QDeclarativeGeoMapMouseArea::mouseX() const
{
    return mouseX_;
}

qreal QDeclarativeGeoMapMouseArea::mouseY() const
{
    return mouseY_;
}

bool QDeclarativeGeoMapMouseArea::hovered() const
{
    return hovered_;
}

void QDeclarativeGeoMapMouseArea::setHovered(bool hovered)
{
    if (hovered_ == hovered)
        return;

    hovered_ = hovered;

    emit hoveredChanged(hovered_);

    if (hovered_)
        emit entered();
    else
        emit exited();
}

/*!
    \qmlproperty bool MapMouseArea::pressed
    This property holds whether the mouse area is currently pressed.
*/

bool QDeclarativeGeoMapMouseArea::pressed() const
{
    return pressed_;
}

bool QDeclarativeGeoMapMouseArea::setPressed(bool pressed, QDeclarativeGeoMapMouseEvent *event)
{
    if (pressed_ == pressed)
        return false;

    bool isClick = pressed_ && !pressed;// && hovered_;

    pressed_ = pressed;

    if (pressed_) {
        if (!doubleClick_)
            emit QDeclarativeGeoMapMouseArea::pressed(event);
    } else {
        emit released(event);
        // TODO set saved position in event?
        if (isClick && !longPress_ && !doubleClick_) {
            emit clicked(event);

        }
    }

    emit pressedChanged(pressed_);

    return event->accepted();
}

/*!
    \qmlproperty bool MapMouseArea::enabled
    This property holds whether the item accepts mouse events.

    By default, this property is true.
*/

bool QDeclarativeGeoMapMouseArea::isEnabled() const
{
    return enabled_;
}

void QDeclarativeGeoMapMouseArea::setEnabled(bool enabled)
{
    if (enabled_ == enabled)
        return;

    enabled_ = enabled;

    emit enabledChanged(enabled_);
}

/*!
    \qmlproperty MouseButton MapMouseArea::pressedButton
    This property holds the mouse button currently pressed.

    It is one of:
    \list
    \o Qt.LeftButton
    \o Qt.RightButton
    \o Qt.MiddleButton
    \endlist

    \sa acceptedButtons
*/

Qt::MouseButton QDeclarativeGeoMapMouseArea::pressedButton() const
{
    return pressedButton_;
}


/*!
    \qmlproperty Qt::MouseButtons MapMouseArea::acceptedButtons
    This property holds the mouse buttons that the mouse area reacts to.

    The available buttons are:
    \list
    \o Qt.LeftButton
    \o Qt.RightButton
    \o Qt.MiddleButton
    \endlist

    To accept more than one button the flags can be combined with the
    "|" (or) operator:

    \code
    MapMouseArea { acceptedButtons: Qt.LeftButton | Qt.RightButton }
    \endcode

    The default value is \c Qt.LeftButton.
*/

void QDeclarativeGeoMapMouseArea::setAcceptedButtons(Qt::MouseButtons acceptedButtons)
{
    if (acceptedButtons_ == acceptedButtons)
        return;

    acceptedButtons_ = acceptedButtons;

    emit acceptedButtonsChanged(acceptedButtons_);
}

Qt::MouseButtons QDeclarativeGeoMapMouseArea::acceptedButtons() const
{
    return acceptedButtons_;
}

bool QDeclarativeGeoMapMouseArea::hoverEnabled() const
{
    return hoverEnabled_;
}

void QDeclarativeGeoMapMouseArea::setHoverEnabled(bool hoverEnabled)
{
    if (hoverEnabled == hoverEnabled_)
        return;

    hoverEnabled_ = hoverEnabled;
    setAcceptsHoverEvents(hoverEnabled_);
    setAcceptHoverEvents(hoverEnabled_);
    setAcceptedMouseButtons(Qt::LeftButton);
    emit hoverEnabledChanged(hoverEnabled_);
    // TODO update hovered property
}







void QDeclarativeGeoMapMouseArea::doubleClickEvent(QDeclarativeGeoMapMouseEvent *event)
{
    if (!enabled_) {
        //TODO QDeclarativeItem::mouseDoubleClickEvent(convert event to regular event here)
        return;
    }
    // TODO check this properly
    bool doubleClickConnected = true;

    if (doubleClickConnected)
        doubleClick_ = true;
    // TODO save event
    event->setAccepted(doubleClickConnected);
    emit doubleClicked(event);
    // TODO QDeclarativeItem::mouseDoubleClickEvent(convert event to regular event here)

    map_->setActiveMouseArea(0);
}

void QDeclarativeGeoMapMouseArea::pressEvent(QDeclarativeGeoMapMouseEvent *event)
{
    if (!enabled_) {
        //TODO QDeclarativeItem::mousePressEvent(convert event to regular event here)
        return;
    }

    event->setAccepted(true);

    longPress_ = false;
    // TODO save event
    mouseX_ = event->x();
    mouseY_ = event->y();
    pressedButton_ = Qt::MouseButton(event->button());
    modifiers_ = Qt::KeyboardModifiers(event->modifiers());

    //setHovered(true);
    // TODO setup long press timer
    event->setAccepted(setPressed(true, event));

    if (event->accepted())
        map_->setActiveMouseArea(this);
}

void QDeclarativeGeoMapMouseArea::releaseEvent(QDeclarativeGeoMapMouseEvent *event)
{
    if (!enabled_) {
        //TODO QDeclarativeItem::mouseReleaseEvent(convert event to regular event here)
        return;
    }

    // save event
    setPressed(false, event);
    pressedButton_ = Qt::NoButton;
    modifiers_ = Qt::NoModifier;
    doubleClick_ = false;
}

void QDeclarativeGeoMapMouseArea::enterEvent()
{
    if (!enabled_ || !hoverEnabled())
        return;

    setHovered(true);

    emit entered();
}

void QDeclarativeGeoMapMouseArea::exitEvent()
{
    if (!enabled_ || !hoverEnabled())
        return;

    setHovered(false);

    emit exited();
}

void QDeclarativeGeoMapMouseArea::moveEvent(QDeclarativeGeoMapMouseEvent *event)
{
    if (!enabled_)
        return;

    event->setButton(pressedButton_);
    event->setModifiers(modifiers_);

    emit positionChanged(event);
}

/*!
  \qmlsignal MapMouseArea::onPressed(MapMouseEvent mouse)

    This handler is called when there is a press.

    The \l {MapMouseEvent}{mouse} parameter provides information about the
    press, including the x and y position and which button was pressed.

    The \l {MapMouseEvent}{accepted} property of the MapMouseEvent parameter determines whether
    this MapMouseArea will handle the press and all future mouse events until
    release.  The default is to accept the event and not allow other
    MapMouseArea beneath this one to handle the event.  If \l {MapMouseEvent}{accepted}
    is set to false, no further events will be sent to this MapMouseArea until
    the button is next pressed.
*/

/*!
  \qmlsignal MapMouseArea::onReleased(MapMouseEvent mouse)

    This handler is called when there is a release.
    The \l {MapMouseEvent}{mouse} parameter provides information about the
    click, including the x and y position of the release of the click.

    The \l {MapMouseEvent}{accepted} property of the MapMouseEvent parameter is ignored
    in this handler.
*/

/*!
  \qmlsignal MapMouseArea::onClicked(MapMouseEvent mouse)

    This handler is called when there is a click. A click is defined as a
    press followed by a release, both inside the MapMouseArea (pressing,
    moving outside the MapMouseArea, and then moving back inside and
    releasing is also considered a click).

    The \l {MapMouseEvent}{mouse} parameter provides information about the
    click, including the x and y position of the release of the click.

    The \l {MapMouseEvent}{accepted} property of the MapMouseEvent parameter is ignored in
    this handler.
*/

/*!
  \qmlsignal MapMouseArea::onDoubleClicked(MapMouseEvent mouse)

    This handler is called when there is a double-click (a press followed
    by a release followed by a press).

    The \l {MapMouseEvent}{mouse} parameter provides information about the
    click, including the x and y position of the release of the click.

    If the \l {MapMouseEvent}{accepted} property of the \l {MapMouseEvent}{mouse} parameter is
    set to false in the handler, the onPressed/onReleased/onClicked handlers
    will be called for the second click; otherwise they are suppressed.
    The accepted property defaults to true.
*/

#include "moc_qdeclarativegeomapmousearea_p.cpp"

QTM_END_NAMESPACE
