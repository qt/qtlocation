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

#include "qdeclarativegeoroute_p.h"

QT_BEGIN_NAMESPACE

/*!
    \qmltype Route
    \instantiates QDeclarativeGeoRoute
    \inqmlmodule QtLocation 5.0
    \ingroup qml-QtLocation5-routing
    \since Qt Location 5.0

    \brief The Route type represents one geographical route.

    A Route type contains high level information about a route, such
    as the length the route, the estimated travel time for the route,
    and enough information to render a basic image of the route on a map.

    The QGeoRoute object also contains a list of \l RouteSegment objects which
    describe subsections of the route in greater detail.

    The primary means of acquiring Route objects is \l RouteModel.

    \section1 Example

    This example shows how to display a route's maneuvers in a ListView:

    \snippet snippets/declarative/routing.qml QtQuick import
    \snippet snippets/declarative/routing.qml QtLocation import
    \codeline
    \snippet snippets/declarative/routing.qml Route Maneuver List

*/

QDeclarativeGeoRoute::QDeclarativeGeoRoute(QObject *parent)
    : QObject(parent)
{
    this->init();
}

QDeclarativeGeoRoute::QDeclarativeGeoRoute(const QGeoRoute &route, QObject *parent)
    : QObject(parent),
      route_(route)
{
    this->init();
}

QDeclarativeGeoRoute::~QDeclarativeGeoRoute() {}

void QDeclarativeGeoRoute::init()
{
    bounds_ = new QDeclarativeGeoRectangle(route_.bounds(), this);

    for (int i = 0; i < route_.path().size(); ++i)
        path_.append(new QDeclarativeCoordinate(route_.path().at(i), this));

    QGeoRouteSegment segment = route_.firstRouteSegment();
    while (segment.isValid()) {
        segments_.append(new QDeclarativeGeoRouteSegment(segment, this));
        segment = segment.nextRouteSegment();
    }
}

/*!
    \internal
*/
QList<QGeoCoordinate> QDeclarativeGeoRoute::routePath()
{
    if (!route_.path().isEmpty()) {
        return route_.path();
    } else if (!path_.isEmpty()) {
        // this is for testing. Route types are not meant to be
        // user instantiable but for testing it makes sense
        QList<QGeoCoordinate> list;
        for (int i = 0; i < path_.count(); ++i)
            list.append(path_.at(i)->coordinate());
        return list;
    }
    return QList<QGeoCoordinate>();
}

/*!
    \qmlproperty GeoRectangle QtLocation5::Route::bounds

    Read-only property which holds a bounding box which encompasses the entire route.

*/

QDeclarativeGeoRectangle *QDeclarativeGeoRoute::bounds() const
{
    return bounds_;
}

/*!
    \qmlproperty int QtLocation5::Route::travelTime

    Read-only property which holds the estimated amount of time it will take to
    traverse this route, in seconds.

*/

int QDeclarativeGeoRoute::travelTime() const
{
    return route_.travelTime();
}

/*!
    \qmlproperty real QtLocation5::Route::distance

    Read-only property which holds distance covered by this route, in meters.
*/

qreal QDeclarativeGeoRoute::distance() const
{
    return route_.distance();
}

/*!
    \qmlproperty QQmlListProperty<Coordinate> QtLocation5::Route::path

    Read-only property which holds the geographical coordinates of this route.
    Coordinates are listed in the order in which they would be traversed by someone
    traveling along this segment of the route.

    To access individual segments you can use standard list accessors: 'path.length'
    indicates the number of objects and 'path[index starting from zero]' gives
    the actual object.

    \sa Coordinate
*/

QQmlListProperty<QDeclarativeCoordinate> QDeclarativeGeoRoute::path()
{
    return QQmlListProperty<QDeclarativeCoordinate>(this, 0, path_append, path_count,
                                                    path_at, path_clear);
}

/*!
    \internal
*/
void QDeclarativeGeoRoute::path_append(QQmlListProperty<QDeclarativeCoordinate> *prop,
                                       QDeclarativeCoordinate *coordinate)
{
    static_cast<QDeclarativeGeoRoute *>(prop->object)->appendPath(coordinate);
}

/*!
    \internal
*/
int QDeclarativeGeoRoute::path_count(QQmlListProperty<QDeclarativeCoordinate> *prop)
{
    return static_cast<QDeclarativeGeoRoute *>(prop->object)->path_.count();
}

/*!
    \internal
*/
QDeclarativeCoordinate *QDeclarativeGeoRoute::path_at(QQmlListProperty<QDeclarativeCoordinate> *prop, int index)
{
    return static_cast<QDeclarativeGeoRoute *>(prop->object)->path_.at(index);
}

/*!
    \internal
*/
void QDeclarativeGeoRoute::path_clear(QQmlListProperty<QDeclarativeCoordinate> *prop)
{
    static_cast<QDeclarativeGeoRoute *>(prop->object)->clearPath();
}

/*!
    \internal
*/
void QDeclarativeGeoRoute::appendPath(QDeclarativeCoordinate *coordinate)
{
    path_.append(coordinate);
}

/*!
    \internal
*/
void QDeclarativeGeoRoute::clearPath()
{
    path_.clear();
}

/*!
    \qmlproperty QQmlListProperty<RouteSegment> QtLocation5::Route::segments

    Read-only property which holds the list of \l RouteSegment objects of this route.

    To access individual segments you can use standard list accessors: 'segments.length'
    indicates the number of objects and 'segments[index starting from zero]' gives
    the actual objects.

    \sa RouteSegment
*/

QQmlListProperty<QDeclarativeGeoRouteSegment> QDeclarativeGeoRoute::segments()
{
    return QQmlListProperty<QDeclarativeGeoRouteSegment>(this, 0, segments_append, segments_count,
                                                         segments_at, segments_clear);
}

/*!
    \internal
*/
void QDeclarativeGeoRoute::segments_append(QQmlListProperty<QDeclarativeGeoRouteSegment> *prop,
                                           QDeclarativeGeoRouteSegment *segment)
{
    static_cast<QDeclarativeGeoRoute *>(prop->object)->appendSegment(segment);
}

/*!
    \internal
*/
int QDeclarativeGeoRoute::segments_count(QQmlListProperty<QDeclarativeGeoRouteSegment> *prop)
{
    return static_cast<QDeclarativeGeoRoute *>(prop->object)->segments_.count();
}

/*!
    \internal
*/
QDeclarativeGeoRouteSegment *QDeclarativeGeoRoute::segments_at(QQmlListProperty<QDeclarativeGeoRouteSegment> *prop, int index)
{
    return static_cast<QDeclarativeGeoRoute *>(prop->object)->segments_.at(index);
}

/*!
    \internal
*/
void QDeclarativeGeoRoute::segments_clear(QQmlListProperty<QDeclarativeGeoRouteSegment> *prop)
{
    static_cast<QDeclarativeGeoRoute *>(prop->object)->clearSegments();
}

/*!
    \internal
*/
void QDeclarativeGeoRoute::appendSegment(QDeclarativeGeoRouteSegment *segment)
{
    segments_.append(segment);
}

/*!
    \internal
*/
void QDeclarativeGeoRoute::clearSegments()
{
    segments_.clear();
}

#include "moc_qdeclarativegeoroute_p.cpp"

QT_END_NAMESPACE
