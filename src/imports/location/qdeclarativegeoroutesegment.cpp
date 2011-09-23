/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativegeoroutesegment_p.h"

QT_BEGIN_NAMESPACE

/*!
    \qmlclass RouteSegment

    \brief The RouteSegment element represents a segment of a Route.
    \ingroup qml-routing
    \since 5.0

    A RouteSegment instance has information about the physcial layout
    of the route segment, the length of the route and estimated time required
    to traverse the route segment and optional RouteManeuvers associated with
    the end of the route segment.

    RouteSegment instances can be thought of as edges on a routing
    graph, with RouteManeuver instances as optional labels attached to the
    vertices of the graph.

    The primary means of acquiring Route elements is via Routes via \l RouteModel.
*/

QDeclarativeGeoRouteSegment::QDeclarativeGeoRouteSegment(QObject *parent)
    : QObject(parent)
{
    maneuver_ = new QDeclarativeGeoManeuver(this);
}

QDeclarativeGeoRouteSegment::QDeclarativeGeoRouteSegment(const QGeoRouteSegment &segment,
                                                         QObject *parent)
    : QObject(parent),
      segment_(segment)
{
    maneuver_ = new QDeclarativeGeoManeuver(segment_.maneuver(), this);

    for (int i = 0; i < segment_.path().size(); ++i)
        path_.append(new QDeclarativeCoordinate(segment_.path().at(i), this));
}

QDeclarativeGeoRouteSegment::~QDeclarativeGeoRouteSegment() {}

/*!
    \qmlproperty int RouteSegment::travelTime

    Read-only property which holds the estimated amount of time it will take to
    traverse this segment, in seconds.

*/

int QDeclarativeGeoRouteSegment::travelTime() const
{
    return segment_.travelTime();
}

/*!
    \qmlproperty int RouteSegment::distance

    Read-only property which holds the distance covered by this segment of the route, in metres.

*/

qreal QDeclarativeGeoRouteSegment::distance() const
{
    return segment_.distance();
}

/*!
    \qmlproperty int RouteSegment::maneuver

    Read-only property which holds the manevuer for this route segment.

    Will return invalid maneuver if no information has been attached to the endpoint
    of this route segment.
*/

QDeclarativeGeoManeuver* QDeclarativeGeoRouteSegment::maneuver() const
{
    return maneuver_;
}

/*
    \qmlproperty QDeclarativeListProperty<Coordinate> RouteSegment::path

    Read-only property which holds the geographical coordinates of this segment.
    Coordinates are listed in the order in which they would be traversed by someone
    traveling along this segment of the route.

    To access individual segments you can use standard list accessors: 'path.length'
    indicates the number of elements and 'path[index starting from zero]' gives
    the actual element.

    \sa Coordinate
*/

QDeclarativeListProperty<QDeclarativeCoordinate> QDeclarativeGeoRouteSegment::path()
{
    return QDeclarativeListProperty<QDeclarativeCoordinate>(this,
                                                            0,
                                                            path_append,
                                                            path_count,
                                                            path_at,
                                                            path_clear);
}

void QDeclarativeGeoRouteSegment::path_append(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, QDeclarativeCoordinate *coordinate)
{
    static_cast<QDeclarativeGeoRouteSegment*>(prop->object)->path_.append(coordinate);
}

int QDeclarativeGeoRouteSegment::path_count(QDeclarativeListProperty<QDeclarativeCoordinate> *prop)
{
    return static_cast<QDeclarativeGeoRouteSegment*>(prop->object)->path_.count();
}

QDeclarativeCoordinate* QDeclarativeGeoRouteSegment::path_at(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, int index)
{
    return static_cast<QDeclarativeGeoRouteSegment*>(prop->object)->path_.at(index);
}

void QDeclarativeGeoRouteSegment::path_clear(QDeclarativeListProperty<QDeclarativeCoordinate> *prop)
{
    static_cast<QDeclarativeGeoRouteSegment*>(prop->object)->path_.clear();
}

#include "moc_qdeclarativegeoroutesegment_p.cpp"

QT_END_NAMESPACE
