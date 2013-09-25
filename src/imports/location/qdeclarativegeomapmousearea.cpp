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

#include "qdeclarativegeomapmousearea_p.h"
#include "qdeclarativegeomapitembase_p.h"
#include <QtQml/qqmlinfo.h>
#include <QtQuick/QQuickWindow>

#include <QtQuick/private/qquickevents_p_p.h>
#include <QtQuick/private/qquickdrag_p.h>

#include <QDebug>

QT_BEGIN_NAMESPACE

/*!
    \qmltype MapMouseArea
    \instantiates QDeclarativeGeoMapMouseArea

    \brief The MapMouseArea item enables simple mouse handling.

    \inqmlmodule QtLocation
    \ingroup qml-QtLocation5-maps

    Like a standard Qt Quick \l{MouseArea}, the MapMouseArea provides mouse
    handling for an item. Creating a normal Qt Quick MouseArea inside a
    map object type (for example MapPolygon) will result in undefined behavior
    due to the way in which Map objects are rendered, so MapMouseArea exists to
    provide this functionality instead.

    The only exception to this is the MapQuickItem, which can have normal
    MouseArea objects defined within the sourceItem. See the documentation
    for \l{MapQuickItem} for more details.

    MapMouseArea objects should only ever be used within a map object, such as
    a MapPolygon or MapQuickItem, and never within another Qt Quick component.

    The \l enabled property of the MapMouseArea is used to enable and disable
    mouse handling for the proxied item. When disabled, the mouse area becomes
    transparent to mouse events.

    The \l pressed read-only property indicates whether or not the user is
    holding down a mouse button over the mouse area. This property is often
    used in bindings between properties in a user interface.

    Information about the mouse position and button clicks are provided via
    signals for which event handler properties are defined. The most commonly
    used handlers involve handling mouse presses and clicks: onClicked,
    onDoubleClicked, onPressed and onReleased.

    \section2 Example Usage

    The following example shows a map containing a MapCircle object. When
    the user clicks the MapCircle, a message is printed to the console.

    \code
    Map {
        MapCircle {
            center {
                latitude: -27.5
                longitude: 153
            }
            radius: 100

            MapMouseArea {
                anchors.fill: parent

                onClicked: {
                    console.log("You clicked the circle!");
                }
            }
        }
    }
    \endcode

    \section2 Limitations

    Some important limitations apply to the use of a MapMouseArea:
    \list
        \li The mouse event is guaranteed only to be valid for the
   duration of the signal handler (for example onPositionChanged, onClicked). Consequently
   the mouse event itself should not be stored. The main reason for this is to
   optimize memory usage; we do not want to allocate heap memory every time the mouse
   moves.
        \li Nested mouse areas are not supported (MapMouseArea { MapMouseArea {} }.
        \li If two or more MapMouseAreas overlap, the declaration order is significant (not for example 'z' value).
        \li Only one MapMouseArea per MapItem is supported, and it always fills the whole MapItem.
    \endlist

    \sa MapMouseEvent
*/

QDeclarativeGeoMapMouseArea::QDeclarativeGeoMapMouseArea(QQuickItem *parent)
    : QQuickMouseArea(parent),
      componentCompleted_(false),
      dragActive_(false)
{
}

QDeclarativeGeoMapMouseArea::~QDeclarativeGeoMapMouseArea()
{
}

/*!
    \qmlmethod coordinate QtLocation::MapMouseArea::mouseToCoordinate(MouseEvent event)

    Converts the \a event's coordinates to geo coordinates.
 */
QGeoCoordinate QDeclarativeGeoMapMouseArea::mouseToCoordinate(QQuickMouseEvent *event)
{
    // figure out the map association for this mouse area and use it to resolve geocoordinate
    QDeclarativeGeoMap *quickmap = map();
    if (quickmap)
        return quickmap->toCoordinate(quickmap->mapFromItem(this, QPointF(event->x(), event->y())));
    return QGeoCoordinate(); // return invalid coordinate
}

/*!
    \internal
*/
// TODO: cache the map association and hook up to parent change -signals
QDeclarativeGeoMap *QDeclarativeGeoMapMouseArea::map()
{
    QQuickItem *pmi = parentMapItem();
    QDeclarativeGeoMap *map = 0;
    if (pmi) {
        map = qobject_cast<QDeclarativeGeoMap *>(pmi);
        if (!map) {
            QDeclarativeGeoMapItemBase *item = qobject_cast<QDeclarativeGeoMapItemBase *>(pmi);
            if (item)
                map = item->quickMap();
        }
    }
    return map;
}

/*!
    \internal
*/
void QDeclarativeGeoMapMouseArea::dragActiveChanged()
{
    if (drag() && drag()->property("active").toBool()) {
        dragActive_ = true;
        QQuickItem *pmi = parentMapItem();
        if (pmi && qobject_cast<QDeclarativeGeoMapItemBase *>(pmi))
            qobject_cast<QDeclarativeGeoMapItemBase *>(pmi)->dragStarted();
    }
}

/*!
    \internal
*/
void QDeclarativeGeoMapMouseArea::componentComplete()
{
    componentCompleted_ = true;
    connect(drag(), SIGNAL(activeChanged()), this, SLOT(dragActiveChanged()));
    QQuickMouseArea::componentComplete();
}

/*!
    \internal
*/
void QDeclarativeGeoMapMouseArea::mousePressEvent(QMouseEvent *event)
{
    // ignore event if it misses non-rectangular geometry (e.g. circle, route)
    QQuickItem *pmi = parentMapItem();
    bool contains = true;
    if (pmi && qobject_cast<QDeclarativeGeoMapItemBase *>(pmi))
        contains = pmi->contains(event->pos());

    if (contains)
        QQuickMouseArea::mousePressEvent(event);
    else
        event->ignore();
}

/*!
    \internal
*/
void QDeclarativeGeoMapMouseArea::mouseReleaseEvent(QMouseEvent *event)
{
    QQuickItem *pmi = parentMapItem();

    if (dragActive_ && pmi && qobject_cast<QDeclarativeGeoMapItemBase *>(pmi)) {
        // position of the item may have changed by the time the activeChanged
        // is received, hence update already on mouse release
        qobject_cast<QDeclarativeGeoMapItemBase *>(pmi)->dragEnded();
        dragActive_ = false;
    }

    QQuickMouseArea::mouseReleaseEvent(event);
}

/*!
    \internal
*/
void QDeclarativeGeoMapMouseArea::mouseDoubleClickEvent(QMouseEvent *event)
{
    QQuickMouseArea::mouseDoubleClickEvent(event);
}

/*!
    \internal
*/
void QDeclarativeGeoMapMouseArea::mouseMoveEvent(QMouseEvent *event)
{
    QQuickMouseArea::mouseMoveEvent(event);
}

/*!
    \internal
*/
void QDeclarativeGeoMapMouseArea::hoverEnterEvent(QHoverEvent *event)
{
    QQuickItem *pmi = parentMapItem();
    bool contains = true;
    if (pmi && qobject_cast<QDeclarativeGeoMapItemBase *>(pmi))
         contains = pmi->contains(event->pos());

    /* for many objects it's doubtful this will ever be true */
    if (!hovered() && contains)
        QQuickMouseArea::hoverEnterEvent(event);
    else
        event->ignore();
}

/*!
    \internal
*/
void QDeclarativeGeoMapMouseArea::hoverMoveEvent(QHoverEvent *event)
{
    QQuickItem *pmi = parentMapItem();
    bool contains = true;
    if (pmi && qobject_cast<QDeclarativeGeoMapItemBase *>(pmi))
         contains = pmi->contains(event->pos());

    /* we may have already entered the object from scenegraph's perspective
     * (ie, the hoverEnterEvent fired and we ignored it), so make sure our
     * state is set correctly here */
    if (hovered() && !contains)
        QQuickMouseArea::hoverLeaveEvent(event);
    else if (!hovered() && contains)
        QQuickMouseArea::hoverEnterEvent(event);
    else if (hovered() && contains)
        QQuickMouseArea::hoverMoveEvent(event);
    else
        event->ignore();
}

/*!
    \internal
*/
void QDeclarativeGeoMapMouseArea::hoverLeaveEvent(QHoverEvent *event)
{
    QQuickItem *pmi = parentMapItem();
    bool contains = true;
    if (pmi && qobject_cast<QDeclarativeGeoMapItemBase *>(pmi))
         contains = pmi->contains(event->pos());

    if (!hovered() && contains)
        QQuickMouseArea::hoverEnterEvent(event);

    if (hovered())
        QQuickMouseArea::hoverLeaveEvent(event);
    else
        event->ignore();
}

/*!
    \internal
*/
QQuickItem *QDeclarativeGeoMapMouseArea::parentMapItem()
{
    QQuickItem *item = this;
    while (item->parentItem()) {
        item = item->parentItem();
        if (qobject_cast<QDeclarativeGeoMap *>(item) ||
                qobject_cast<QDeclarativeGeoMapItemBase *>(item))
            return item;
    }
    return 0;
}

/*!
    \qmlproperty real QtLocation::MapMouseArea::mouseX
    \qmlproperty real QtLocation::MapMouseArea::mouseY

    These properties hold the screen coordinates of the mouse cursor.

    These properties will only be valid while a button is pressed, and will remain
    valid as long as the button is held down even if the mouse is moved outside the area.

    The screen coordinates are relative to the MapMouseArea.
*/

/*!
    \qmlproperty bool QtLocation::MapMouseArea::containsMouse
    This property holds whether the mouse is currently inside the mouse area.

    \warning This property is not updated if the area moves under the mouse: \e containsMouse will not change.
    In addition, containsMouse will only be valid when the mouse is pressed.
*/

/*!
    \qmlproperty bool QtLocation::MapMouseArea::pressed
    This property holds whether the mouse area is currently pressed.
*/

/*!
    \qmlproperty bool QtLocation::MapMouseArea::enabled
    This property holds whether the item accepts mouse events.

    By default, this property is true.
*/

/*!
    \qmlproperty MouseButton QtLocation::MapMouseArea::pressedButtons
    This property holds the mouse buttons currently pressed.

    It contains a bitwise combination of:
    \list
    \li Qt.LeftButton
    \li Qt.RightButton
    \li Qt.MiddleButton
    \endlist

    \sa acceptedButtons
*/

/*!
    \qmlproperty Qt::MouseButtons QtLocation::MapMouseArea::acceptedButtons
    This property holds the mouse buttons that the mouse area reacts to.

    The available buttons are:
    \list
    \li Qt.LeftButton
    \li Qt.RightButton
    \li Qt.MiddleButton
    \endlist

    To accept more than one button the flags can be combined with the
    "|" (or) operator:

    \code
    MapMouseArea { acceptedButtons: Qt.LeftButton | Qt.RightButton }
    \endcode

    The default value is \c Qt.LeftButton.
*/

/*!
    \qmlsignal QtLocation::MapMouseArea::onPressed(MapMouseEvent mouse)

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
    \qmlsignal QtLocation::MapMouseArea::onReleased(MapMouseEvent mouse)

    This handler is called when there is a release.
    The \l {MapMouseEvent}{mouse} parameter provides information about the
    click, including the x and y position of the release of the click.

    The \l {MapMouseEvent}{accepted} property of the MapMouseEvent parameter is ignored
    in this handler.
*/

/*!
    \qmlsignal QtLocation::MapMouseArea::onClicked(MapMouseEvent mouse)

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
    \qmlsignal QtLocation::MapMouseArea::onDoubleClicked(MapMouseEvent mouse)

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
    \qmlsignal QtLocation::MapMouseArea::onEntered()

    This handler is called when the mouse enters the mouse area.

    The onEntered handler is only called while a button is
    pressed.

    \sa onExited
*/

/*!
    \qmlsignal QtLocation::MapMouseArea::onPositionChanged(MapMouseEvent mouse)

    This handler is called when the mouse position changes.

    The \l {MapMouseEvent}{mouse} parameter provides information about the mouse, including the x and y
    position, and any buttons currently pressed.

    The \e accepted property of the MapMouseEvent parameter is ignored in this handler.

    The onPositionChanged handler is only called while a button is pressed.
*/

/*!
    \qmlsignal QtLocation::MapMouseArea::onPressAndHold(MapMouseEvent mouse)

    This handler is called when there is a long press (currently 800ms).
    The \l {MapMouseEvent}{mouse} parameter provides information about the press, including the x and y
    position of the press, and which button is pressed.

    The \e accepted property of the MapMouseEvent parameter is ignored in this handler.
*/

/*!
    \qmlsignal QtLocation::MapMouseArea::onExited()

    This handler is called when the mouse exits the mouse area.

    The onExited handler is only called while a button is pressed.

    \sa onEntered
*/

#include "moc_qdeclarativegeomapmousearea_p.cpp"

QT_END_NAMESPACE
