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
#include "qdeclarativegeomapflickable_p.h"
#include "qdeclarativegeomapgesturearea_p.h"


QT_BEGIN_NAMESPACE

/*!
    \qmlclass MapFlickable QDeclarativeGeoMapFlickable

    \inqmlmodule QtLocation 5

    \brief The MapFlickable element provides basic flicking of the Map.

    MapFlickable elements are used as part of a Map, to provide for the common
    flick gesture that pans the map in an intuitive fashion. This is comparable
    to the activity of the \l Flickable element.

    A MapFlickable is automatically created with a new Map and available with
    the \l{QtLocation5::Map::flick}{flick} property. This is the only way
    to create a MapFlickable, and once created this way cannot be destroyed
    without its parent Map.

    The two most commonly used properties of the MapFlickable are \l enabled and
    \l deceleration. The former must be set in order for the MapFlickable to
    have any effect upon its parent Map. The latter property customizes the
    response of the MapFlickable to input from the user, by adjusting how much
    "friction" slows down the flick after the user has raised their finger
    or released the mouse button.

    \section2 Performance

    The MapFlickable, when enabled, must process all incoming touch or cursor
    movement events in order to build up the "flick" gesture, and then complete
    an animation for the "deceleration" phase. The MapFlickable builds up a
    buffer of samples during the motion capture phase: currently a maximum of
    3 past samples are processed at each new event. As a result, the overhead
    on movement events can be considered constant time, as there are many more
    than 3 events in a typical gesture. The cost of animation is identical to
    two parallel double float \l{QPropertyAnimation}{QPropertyAnimations} with
    OutQuad easing curves.

    \section2 Example Usage

    The following example enables the flick gesture on a map, and sets the
    deceleration/friction value to be double its standard level.

    \code
    Map {
        flick.enabled: true
        flick.deceleration: 5000
    }
    \endcode

    \ingroup qml-QtLocation5-maps
    \since Qt Location 5.0
*/

/*!
    \qmlproperty bool QtLocation5::MapFlickable::enabled

    This property holds whether the flicking is enabled.
    By default flicking is enabled. Disabling flicking will terminate
    any potentially active flicks (movement or actual flick).
*/

/*!
    \qmlproperty real QtLocation5::MapFlickable::deceleration

    This property holds the rate at which a flick will decelerate. The
    larger the value, the faster the flick decelerates.
    Default value is 2500, minimum value is 500 and maximum value is 10000.
*/

/*!
    \qmlsignal QtLocation5::MapFlickable::onMovementStarted()

    This handler is called when the view begins moving due to user
    interaction. Typically this means that the user is dragging a finger -
    or a mouse with one of more mouse buttons pressed - on the map.
*/

/*!
    \qmlsignal QtLocation5::MapFlickable::onMovementEnded()

    This handler is called when the view stops moving due to user
    interaction.  If a flick was generated, this handler will
    be triggered once the flick stops.  If a flick was not
    generated, the handler will be triggered when the
    user stops dragging - that is a mouse or touch release.

*/

/*!
    \qmlsignal QtLocation5::MapFlickable::onFlickStarted()

    This handler is called when the view is flicked.  A flick
    starts from the point that the mouse or touch is released,
    while still in motion.
*/

/*!
    \qmlsignal QtLocation5::MapFlickable::onFlickEnded()

    This handler is called when the view stops moving due to a flick.
    The order of onMovementEnded() and onFlickEnded() is not specified.
*/

QDeclarativeGeoMapFlickable::QDeclarativeGeoMapFlickable(QObject *parent,
                                                         QDeclarativeGeoMapGestureArea *gestureArea)
    : QObject(parent),
      gestureArea_(gestureArea)
{
    gestureArea_->registerFlickDeprecated(this);
}

QDeclarativeGeoMapFlickable::~QDeclarativeGeoMapFlickable()
{
}

#include "moc_qdeclarativegeomapflickable_p.cpp"

QT_END_NAMESPACE
