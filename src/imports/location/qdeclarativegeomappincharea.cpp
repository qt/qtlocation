/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtLocation module of the Qt Toolkit.
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui/QGuiApplication>
#include "qdeclarativegeomappincharea_p.h"
#include "qdeclarativegeomap_p.h"
#include "qdeclarativecoordinate_p.h"
#include <QtGui/qevent.h>
#include <QtGui/QStyleHints>
#include <QtQml/qqmlinfo.h>
#include <QPropertyAnimation>
#include <QDebug>
#include "math.h"
#include "qgeomap_p.h"

QT_BEGIN_NAMESPACE

/*!
    \qmlclass MapPinchEvent QDeclarativeGeoMapPinchEvent
    \inqmlmodule QtLocation 5.0

    \brief MapPinchEvent element provides basic information about pinch event.

    MapPinchEvent element provides basic information about pinch event. They are
    present in handlers of MapPinch (for example pinchStarted/pinchUpdated). Events are only
    guaranteed to be valid for the duration of the handler.

    Except for the \l accepted property, all properties are read-only.

    \section2 Example Usage

    The following example enables the pinch gesture on a map and reacts to the
    finished event.

    \code
    Map {
        id: map
        pinch.enabled: true
        pinch.onPinchFinished:{
            var coordinate1 = map.toCoordinate(pinch.point1)
            var coordinate2 = map.toCoordinate(pinch.point2)
            console.log("Pinch startet at:")
            console.log("        Points (" + pinch.point1.x + ", " + pinch.point1.y + ") - (" + pinch.point2.x + ", " + pinch.point2.y + ")")
            console.log("   Coordinates (" + coordinate1.latitude + ", " + coordinate1.longitude + ") - (" + coordinate2.latitude + ", " + coordinate2.longitude + ")")
        }
    }
    \endcode

    \ingroup qml-QtLocation5-maps
    \since Qt Location 5.0
*/

/*!
    \qmlproperty QPoint QtLocation5::MapPinchEvent::center

    This read-only property holds the current center point.
*/

/*!
    \qmlproperty real QtLocation5::MapPinchEvent::angle

    This read-only property holds the current angle between the two points in
    the range -180 to 180. Positive values for the angles mean counter-clockwise
    while negative values mean the clockwise direction. Zero degrees is at the
    3 o'clock position.
*/

/*!
    \qmlproperty QPoint QtLocation5::MapPinchEvent::point1
    \qmlproperty QPoint QtLocation5::MapPinchEvent::point2

    These read-only properties hold the actual touch points generating the pinch.
    The points are not in any particular order.
*/

/*!
    \qmlproperty int QtLocation5::MapPinchEvent::pointCount

    This read-only property holds the number of points currently touched.
    The MapPinch will not react until two touch points have initiated a gesture,
    but will remain active until all touch points have been released.
*/

/*!
    \qmlproperty bool QtLocation5::MapPinchEvent::accepted

    Setting this property to false in the \c MapPinch::onPinchStarted handler
    will result in no further pinch events being generated, and the gesture
    ignored.
*/

/*!
    \qmlclass MapPinchArea QDeclarativeGeoMapPinchArea

    \inqmlmodule QtLocation 5.0

    \brief The MapPinchArea element provides basic Map pinch interaction.

    MapPinchArea elements are used as part of a Map, to provide for the
    pinch-to-zoom gesture used on touch displays. This is comparable to the
    activity of the \l{PinchArea} element.

    A MapPinchArea is automatically created with a new Map and available with
    the \l{QtLocation5::Map::pinch}{pinch} property. This is the only way
    to create a MapPinchArea, and once created this way cannot be destroyed
    without its parent Map.

    The two most commonly used properties of the MapPinchArea are the \l enabled
    and \l activeGestures properties. Both of these must be set before a
    MapPinchArea will have any effect upon interaction with the Map.

    \section2 Performance

    The MapPinchArea, when enabled, must process all incoming touch events in
    order to track the shape and size of the "pinch". The overhead added on
    touch events can be considered constant time.

    \section2 Example Usage

    The following example enables the pinch gesture on a map.

    \code
    Map {
        pinch.enabled: true
        pinch.activeGestures: MapPinchArea.ZoomGesture
    }
    \endcode

    \ingroup qml-QtLocation5-maps
    \since Qt Location 5.0
*/

/*!
    \qmlproperty bool QtLocation5::MapPinchArea::active

    This read-only property holds whether a pinch gesture is active.
*/

/*!
    \qmlproperty enumeration QtLocation5::MapPinchArea::activeGestures

    This property holds the gestures that the pinch should control. By default
    the ZoomGesture is enabled.

    \list
    \li PinchArea.NoGesture - Don't support any additional gestures (value: 0x0000).
    \li PinchArea.ZoomGesture - Support the map zoom gesture (value: 0x0001).
    \li PinchArea.RotationGesture - Support the map rotation gesture (value: 0x0002).
    \li PinchArea.TiltGesture - Support the map tilt gesture (value: 0x0004).
    \li PinchArea.PanGesture  - Support the map pan gesture while pinching (value: 0x0008).
    \endlist

    For the extremist, one may OR flag the RotationGesture or TiltGesture
    but these come with absolutely no warranty or guarantees at the moment
    (may be removed, changed, moved around)

    \note For the time being, only \l PinchArea.ZoomGesture is supported.
*/

/*!
    \qmlproperty real QtLocation5::MapPinchArea::maximumZoomLevelChange

    This property holds the maximum zoom level change per pinch, essentially
    meant to be used for setting the zoom sensitivity.

    It is an indicative measure calculated from the dimensions of the
    map area, roughly corresponding how much zoom level could change with
    maximum pinch zoom. Default value is 2.0, maximum value is 10.0
*/

/*!
    \qmlproperty real MapPinchArea::rotationFactor

    This property holds the rotation factor for zoom, essentially meant to be used for setting
    the rotation sensitivity.

    It is an indicative measure; the default value 1.0 means the map roughly follows the fingers,
    whereas 2.0 means rotating twice as fast. Maximum value is 5.0.
*/

/*!
    \qmlsignal void QtLocation5::MapPinchArea::pinchStarted(PinchEvent event)

    Raised when a pinch gesture is started.

    \sa pinchUpdated, pinchFinished
*/

/*!
    \qmlsignal void QtLocation5::MapPinchArea::pinchUpdated(PinchEvent event)

    Once a pinch has begun this event gets raised as the user moves her fingers
    across the map.

    \sa pinchStarted, pinchFinished
*/

/*!
    \qmlsignal void QtLocation5::MapPinchArea::pinchUpdated(PinchEvent event)

    The end of a pinch gesture is signaled by this event.

    \sa pinchUpdated, pinchFinished
*/

/*!
    \qmlproperty bool QtLocation5::MapPinchArea::enabled

    This property holds whether the pinch gestures are enabled.

    \note disabling pinch during active pinch does not have effect on
    the potentially active current pinch.
*/

QDeclarativeGeoMapPinchArea::QDeclarativeGeoMapPinchArea(QObject *parent,
                                                         QDeclarativeGeoMapGestureArea *gestureArea)
    : QObject(parent),
      gestureArea_(gestureArea)
{
    gestureArea_->registerPinchDeprecated(this);
}

QDeclarativeGeoMapPinchArea::~QDeclarativeGeoMapPinchArea()
{
}

#include "moc_qdeclarativegeomappincharea_p.cpp"

QT_END_NAMESPACE
