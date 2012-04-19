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

#include "qdeclarativegeomaneuver_p.h"

QT_BEGIN_NAMESPACE

/*!
    \qmlclass RouteManeuver QDeclarativeGeoManeuver
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-routing
    \since Qt Location 5.0

    \brief The RouteManeuver element represents the information relevant to the
    point at which two RouteSegments meet.

    RouteSegment instances can be thought of as edges on a routing
    graph, with RouteManeuver instances as optional labels attached to the
    vertices of the graph.

    The most interesting information held in a RouteManeuver instance is
    normally the textual navigation to provide and the position at which to
    provide it, accessible by \l instructionText and \l position respectively.

    \section1 Example

    The following QML snippet demonstrates how to print information about a
    route maneuver:

    \snippet snippets/declarative/routing.qml QtQuick import
    \snippet snippets/declarative/routing.qml QtLocation import
    \codeline
    \snippet snippets/declarative/routing.qml RouteManeuver
*/

QDeclarativeGeoManeuver::QDeclarativeGeoManeuver(QObject *parent)
    : QObject(parent)
{
    position_ = new QDeclarativeCoordinate(this);
    waypoint_ = new QDeclarativeCoordinate(this);
}

QDeclarativeGeoManeuver::QDeclarativeGeoManeuver(const QGeoManeuver &maneuver, QObject *parent)
    : QObject(parent),
      maneuver_(maneuver)
{
    position_ = new QDeclarativeCoordinate(maneuver_.position(), this);
    waypoint_ = new QDeclarativeCoordinate(maneuver_.waypoint(), this);
}

QDeclarativeGeoManeuver::~QDeclarativeGeoManeuver() {}

/*!
    \qmlproperty bool RouteManeuver::valid

    This read-only property holds whether this maneuver is valid or not.

    Invalid maneuvers are used when there is no information
    that needs to be attached to the endpoint of a QGeoRouteSegment instance.
*/

bool QDeclarativeGeoManeuver::valid() const
{
    return maneuver_.isValid();
}

/*!
    \qmlproperty Coordinate RouteManeuver::position

    This read-only property holds where the \l instructionText should be displayed.

*/

QDeclarativeCoordinate* QDeclarativeGeoManeuver::position() const
{
    return position_;
}

/*!
    \qmlproperty string RouteManeuver::instructionText

    This read-only property holds textual navigation instruction.
*/

QString QDeclarativeGeoManeuver::instructionText() const
{
    return maneuver_.instructionText();
}

/*!
    \qmlproperty enumeration RouteManeuver::direction

    Describes the change in direction associated with the instruction text
    that is associated with a RouteManeuver.

    \list
    \li RouteManeuver.NoDirection - There is no direction associated with the instruction text
    \li RouteManeuver.DirectionForward - The instruction indicates that the direction of travel does not need to change
    \li RouteManeuver.DirectionBearRight - The instruction indicates that the direction of travel should bear to the right
    \li RouteManeuver.DirectionLightRight - The instruction indicates that a light turn to the right is required
    \li RouteManeuver.DirectionRight - The instruction indicates that a turn to the right is required
    \li RouteManeuver.DirectionHardRight - The instruction indicates that a hard turn to the right is required
    \li RouteManeuver.DirectionUTurnRight - The instruction indicates that a u-turn to the right is required
    \li RouteManeuver.DirectionUTurnLeft - The instruction indicates that a u-turn to the left is required
    \li RouteManeuver.DirectionHardLeft - The instruction indicates that a hard turn to the left is required
    \li RouteManeuver.DirectionLeft - The instruction indicates that a turn to the left is required
    \li RouteManeuver.DirectionLightLeft - The instruction indicates that a light turn to the left is required
    \li RouteManeuver.DirectionBearLeft - The instruction indicates that the direction of travel should bear to the left
    \endlist
*/

QDeclarativeGeoManeuver::Direction QDeclarativeGeoManeuver::direction() const
{
    return QDeclarativeGeoManeuver::Direction(maneuver_.direction());
}

/*!
    \qmlproperty int RouteManeuver::timeToNextInstruction

    This read-only property holds the estimated time it will take to travel
    from the point at which the associated instruction was issued and the
    point that the next instruction should be issued, in seconds.
*/

int QDeclarativeGeoManeuver::timeToNextInstruction() const
{
    return maneuver_.timeToNextInstruction();
}

/*!
    \qmlproperty real RouteManeuver::distanceToNextInstruction

    This read-only property holds the distance, in meters, between the point at which
    the associated instruction was issued and the point that the next instruction should
    be issued.
*/

qreal QDeclarativeGeoManeuver::distanceToNextInstruction() const
{
    return maneuver_.distanceToNextInstruction();
}

/*!
    \qmlproperty Coordinate RouteManeuver::waypoint

    This property holds the waypoint associated with this maneuver.
    All maneuvers do not have a waypoint associated with them, this
    can be checked with \l waypointValid.

*/

QDeclarativeCoordinate* QDeclarativeGeoManeuver::waypoint() const
{
    return waypoint_;
}

/*!
    \qmlproperty bool RouteManeuver::waypointValid

    This read-only property holds whether this \l waypoint, associated with this
    maneuver, is valid or not.
*/

bool QDeclarativeGeoManeuver::waypointValid() const
{
    return waypoint_->coordinate().isValid();
}

#include "moc_qdeclarativegeomaneuver_p.cpp"

QT_END_NAMESPACE
