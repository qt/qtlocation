/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativegeomapmousearea_p.h"
#include <QtDeclarative/qdeclarativeinfo.h>
#include <QtDeclarative/QSGCanvas>

#ifndef QSGMOUSEAREA_AVAILABLE

#else

#include <qsgevents_p_p.h>
#include "qsgmousearea_p.h"

#include <QDebug>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass MapMouseArea

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

   MapMouseArea does not intend to be generic mouse area that supports every
   possible usage, rather it focuses on catering for the major use-cases. Hence there
   are some implementation limitations:
    \list
        \i The mouse event is guaranteed only to be valid for the
   duration of the signal handler (e.g. onPositionChanged, onClicked). Consequently
   the mouse event itself should not be stored. The main reason for this is to
   optimize memory usage; we do not want to allocate heap memory every time the mouse
   moves.
        \i Nested mouse areas are not supported (MapMouseArea { MapMouseArea {} }
        \i Using normal QML MouseArea in MapItem or Map has no effect
        \i If mouse areas of map overlap, the declaration order is significant (not e.g. 'z' value)
        \i Only one MapMouseArea per MapItem is supported, and it always fills the whole MapItem
    \endlist

    \sa MapMouseEvent
*/

QDeclarativeGeoMapMouseArea::QDeclarativeGeoMapMouseArea(QSGItem *parent)
    : QSGItem(parent),
      mouseEvent_(0),
      map_(0),
      mouseArea_(0),
      componentCompleted_(false)
{
    mouseEvent_ = new QDeclarativeGeoMapMouseEvent(this);
    mouseArea_ = new QSGMouseArea(this);
    // prevents mouseArea automagically intercepting mouse events:
    mouseArea_->setVisible(false);
    setVisible(false);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::MidButton | Qt::RightButton);
    setAcceptHoverEvents(false);
    // connect to signals so we can intercept them and convert to map
    // mouse events (and e.g. to amend with geo coordinate).
    // the signals that were present on mobility
    connect(mouseArea_, SIGNAL(pressed(QSGMouseEvent*)), this, SLOT(pressedHandler(QSGMouseEvent*)));
    connect(mouseArea_, SIGNAL(clicked(QSGMouseEvent*)), this, SLOT(clickedHandler(QSGMouseEvent*)));
    connect(mouseArea_, SIGNAL(doubleClicked(QSGMouseEvent*)), this, SLOT(doubleClickedHandler(QSGMouseEvent*)));
    connect(mouseArea_, SIGNAL(released(QSGMouseEvent*)), this, SLOT(releasedHandler(QSGMouseEvent*)));
    connect(mouseArea_, SIGNAL(pressedChanged()), this, SLOT(pressedChangedHandler()));
    connect(mouseArea_, SIGNAL(enabledChanged()), this, SLOT(enabledChangedHandler()));
    connect(mouseArea_, SIGNAL(acceptedButtonsChanged()), this, SLOT(acceptedButtonsChangedHandler()));
    //connect(mouseArea_, SIGNAL(hoverEnabledChanged()), this, SLOT(hoverEnabledChangedHandler()));
    // new signals from qt5 ->
    connect(mouseArea_, SIGNAL(hoveredChanged()), this, SLOT(hoveredChangedHandler()));
    connect(mouseArea_, SIGNAL(positionChanged(QSGMouseEvent*)), this, SLOT(positionChangedHandler(QSGMouseEvent*)));
    connect(mouseArea_, SIGNAL(entered()), this, SLOT(enteredHandler()));
    connect(mouseArea_, SIGNAL(exited()), this, SLOT(exitedHandler()));
    connect(mouseArea_, SIGNAL(pressAndHold(QSGMouseEvent*)), this, SLOT(pressAndHoldHandler(QSGMouseEvent*)));
}

QDeclarativeGeoMapMouseArea::~QDeclarativeGeoMapMouseArea()
{
    delete mouseEvent_;
    delete mouseArea_;
}

void QDeclarativeGeoMapMouseArea::componentComplete()
{
    mouseArea_->setWidth(this->width());
    mouseArea_->setHeight(this->height());
    mouseArea_->setX(this->x());
    mouseArea_->setY(this->y());
    componentCompleted_ = true;
    QSGItem::componentComplete();
}

void QDeclarativeGeoMapMouseArea::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    if (!componentCompleted_)
        return;
    mouseArea_->setWidth(this->width());
    mouseArea_->setHeight(this->height());
    mouseArea_->setX(this->x());
    mouseArea_->setY(this->y());
    QSGItem::geometryChanged(newGeometry, oldGeometry);
}

void QDeclarativeGeoMapMouseArea::hoveredChangedHandler()
{
    emit hoveredChanged(mouseArea_->property("containsMouse").toBool());
}

void QDeclarativeGeoMapMouseArea::pressAndHoldHandler(QSGMouseEvent* event)
{
    mapMouseEvent(event);
    emit pressAndHold(mouseEvent_);
}

/* hovering not supported at the moment
void QDeclarativeGeoMapMouseArea::hoverEnabledChangedHandler()
{
    emit hoverEnabledChanged(mouseArea_->hoverEnabled());
}
*/

void QDeclarativeGeoMapMouseArea::acceptedButtonsChangedHandler()
{
    emit acceptedButtonsChanged(static_cast<Qt::MouseButtons>(mouseArea_->property("acceptedButtons").toInt()));
}

void QDeclarativeGeoMapMouseArea::enabledChangedHandler()
{
    emit enabledChanged(mouseArea_->property("enabled").toBool());
}

void QDeclarativeGeoMapMouseArea::pressedHandler(QSGMouseEvent* event)
{
   mapMouseEvent(event);
   emit pressed(mouseEvent_);
}

void QDeclarativeGeoMapMouseArea::pressedChangedHandler()
{
    emit pressedChanged(mouseArea_->property("pressed").toBool());
}

void QDeclarativeGeoMapMouseArea::enteredHandler()
{
   emit entered();
}

void QDeclarativeGeoMapMouseArea::exitedHandler()
{
   emit exited();
}

void QDeclarativeGeoMapMouseArea::positionChangedHandler(QSGMouseEvent* event)
{
   mapMouseEvent(event);
   emit positionChanged(mouseEvent_);
}

void QDeclarativeGeoMapMouseArea::mapMouseEvent(QSGMouseEvent* event)
{
   // we could probably access these directly,
   // wouldn't worsen the private header dependency much
   mouseEvent_->setAccepted(event->property("accepted").toBool());
   mouseEvent_->setButton(event->property("button").toInt());
   mouseEvent_->setButtons(event->property("buttons").toInt());
   mouseEvent_->setModifiers(event->property("modifiers").toInt());
   mouseEvent_->setWasHeld(event->property("wasHeld").toBool());
   mouseEvent_->setX(event->property("x").toInt());
   mouseEvent_->setY(event->property("y").toInt());
   if (map_)
       mouseEvent_->setCoordinate(
                   map_->map()->screenPositionToCoordinate(QPointF(mouseEvent_->x(), mouseEvent_->y())));
}

void QDeclarativeGeoMapMouseArea::releasedHandler(QSGMouseEvent* event)
{
    mapMouseEvent(event);
    emit released(mouseEvent_);
}

void QDeclarativeGeoMapMouseArea::clickedHandler(QSGMouseEvent* event)
{
    mapMouseEvent(event);
    emit clicked(mouseEvent_);
}

void QDeclarativeGeoMapMouseArea::doubleClickedHandler(QSGMouseEvent* event)
{
    mapMouseEvent(event);
    emit doubleClicked(mouseEvent_);
}

void QDeclarativeGeoMapMouseArea::setMap(QDeclarative3DGraphicsGeoMap *map)
{
    Q_ASSERT(map);
    map_ = map;
}

/*!
    \qmlproperty real MapMouseArea::mouseX
    \qmlproperty real MapMouseArea::mouseY

    These properties hold the screen coordinates of the mouse cursor.

    These properties will only be valid while a button is pressed, and will remain
    valid as long as the button is held down even if the mouse is moved outside the area.

    The screen coordinates are relative to the MapMouseArea.
*/

qreal QDeclarativeGeoMapMouseArea::mouseX() const
{
    return mouseArea_->property("mouseX").toReal();
}

qreal QDeclarativeGeoMapMouseArea::mouseY() const
{
    return mouseArea_->property("mouseY").toReal();
}

/*!
    \qmlproperty bool MapMouseArea::containsMouse
    This property holds whether the mouse is currently inside the mouse area.

    \warning This property is not updated if the area moves under the mouse: \e containsMouse will not change.
    In addition, containsMouse will only be valid when the mouse is pressed.
*/

bool QDeclarativeGeoMapMouseArea::hovered() const
{
    return mouseArea_->property("containsMouse").toBool();
}

/*!
    \qmlproperty bool MapMouseArea::pressed
    This property holds whether the mouse area is currently pressed.
*/

bool QDeclarativeGeoMapMouseArea::pressed() const
{
    return mouseArea_->property("pressed").toBool();
}

/*!
    \qmlproperty bool MapMouseArea::enabled
    This property holds whether the item accepts mouse events.

    By default, this property is true.
*/

bool QDeclarativeGeoMapMouseArea::isEnabled() const
{
    return mouseArea_->property("enabled").toBool();
}

void QDeclarativeGeoMapMouseArea::setEnabled(bool enabled)
{
    mouseArea_->setProperty("enabled", QVariant(enabled));
}

/*!
    \qmlproperty MouseButton MapMouseArea::pressedButtons
    This property holds the mouse buttons currently pressed.

    It contains a bitwise combination of:
    \list
    \o Qt.LeftButton
    \o Qt.RightButton
    \o Qt.MiddleButton
    \endlist

    \sa acceptedButtons
*/

Qt::MouseButtons QDeclarativeGeoMapMouseArea::pressedButtons() const
{
    return static_cast<Qt::MouseButtons>(mouseEvent_->button());
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
    mouseArea_->setProperty("acceptedButtons", QVariant(acceptedButtons));
    setAcceptedMouseButtons(acceptedButtons);
}

Qt::MouseButtons QDeclarativeGeoMapMouseArea::acceptedButtons() const
{
    return static_cast<Qt::MouseButtons>(mouseArea_->property("acceptedButtons").toInt());
}

/* hover is currently not supported
    \qmlproperty bool MapMouseArea::hoverEnabled
    This property holds whether hover events are handled.

    By default, mouse events are only handled in response to a button event, or when a button is
    pressed. Hover enables handling of all mouse events even when no mouse button is
    pressed.

    This property affects the \l containsMouse property and the \l onEntered, \l onExited and
    \l onPositionChanged signals.


bool QDeclarativeGeoMapMouseArea::hoverEnabled() const
{
    return mouseArea_->hoverEnabled();
}

void QDeclarativeGeoMapMouseArea::setHoverEnabled(bool hoverEnabled)
{
    mouseArea_->setHoverEnabled(hoverEnabled);
}

bool QDeclarativeGeoMapMouseArea::hoverEvent(QHoverEvent *event)
{
    if (!map_ || !map_->canvas()) {
        qmlInfo(this)  << "Warning: no qsgcanvas available, cannot dispatch the mouse event";
        return false;
    }
    // we need to filter hover events to mimic regular MouseArea's behavior
    if (!mouseArea_->hoverEnabled()) {
        return false;
    }
    map_->canvas()->sendEvent(mouseArea_, event);
    return true;
}
*/

bool QDeclarativeGeoMapMouseArea::mouseEvent(QMouseEvent *event)
{
    if (!map_ || !map_->canvas()) {
        qmlInfo(this)  << "Warning: no qsgcanvas available, cannot dispatch the mouse event";
        return false;
    }
    if (!mouseArea_->property("enabled").toBool()) {
        QLOC_TRACE2("mouse area not enabled.", objectName());
        event->ignore();
        return false;
    }
    map_->canvas()->sendEvent(mouseArea_, event);
    return event->isAccepted(); // hmm..
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

/*!
    \qmlsignal MapMouseArea::onEntered()

    This handler is called when the mouse enters the mouse area.

    The onEntered handler is only called while a button is
    pressed.

    \sa onExited()
*/

/*!
    \qmlsignal MapMouseArea::onPositionChanged(MapMouseEvent mouse)

    This handler is called when the mouse position changes.

    The \l {MapMouseEvent}{mouse} parameter provides information about the mouse, including the x and y
    position, and any buttons currently pressed.

    The \e accepted property of the MapMouseEvent parameter is ignored in this handler.

    The onPositionChanged handler is only called while a button is pressed.
*/

/*!
    \qmlsignal MapMouseArea::onPressAndHold(MapMouseEvent mouse)

    This handler is called when there is a long press (currently 800ms).
    The \l {MapMouseEvent}{mouse} parameter provides information about the press, including the x and y
    position of the press, and which button is pressed.

    The \e accepted property of the MapMouseEvent parameter is ignored in this handler.
*/

/*!
    \qmlsignal MapMouseArea::onExited()

    This handler is called when the mouse exits the mouse area.

    The onExited handler is only called while a button is pressed.

    \sa onEntered()
*/

#endif // QSGMOUSEAREA_AVAILABLE

#include "moc_qdeclarativegeomapmousearea_p.cpp"

QT_END_NAMESPACE
