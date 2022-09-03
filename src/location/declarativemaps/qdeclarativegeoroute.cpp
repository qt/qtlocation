/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativegeoroute_p.h"
#include "locationvaluetypehelper_p.h"
#include <QtLocation/private/qgeomap_p.h>
#include <QtLocation/private/qgeoroute_p.h>
#include <QtLocation/private/qdeclarativegeoroutemodel_p.h>

#include <QtPositioning/QGeoRectangle>

QT_BEGIN_NAMESPACE

/*!
    \qmltype Route
    \instantiates QDeclarativeGeoRoute
    \inqmlmodule QtLocation
    \ingroup qml-QtLocation5-routing
    \since QtLocation 5.5

    \brief The Route type represents one geographical route.

    A Route type contains high level information about a route, such
    as the length the route, the estimated travel time for the route,
    and enough information to render a basic image of the route on a map.

    The QGeoRoute object also contains a list of \l routeSegment objects which
    describe subsections of the route in greater detail.

    The primary means of acquiring Route objects is \l RouteModel.

    \section1 Example

    This example shows how to display a route's maneuvers in a ListView:

    \snippet declarative/routing.qml QtQuick import
    \snippet declarative/maps.qml QtLocation import
    \codeline
    \snippet declarative/routing.qml Route Maneuver List1
    \snippet declarative/routing.qml Route Maneuver List2
    \snippet declarative/routing.qml Route Maneuver List3

*/

QDeclarativeGeoRoute::QDeclarativeGeoRoute(QObject *parent)
    : QObject(parent)
{
}

QDeclarativeGeoRoute::QDeclarativeGeoRoute(const QGeoRoute &route, QObject *parent)
    : QObject(parent), route_(route)
{
}

QDeclarativeGeoRoute::~QDeclarativeGeoRoute() {}

/*!
    \internal
*/
QList<QGeoCoordinate> QDeclarativeGeoRoute::routePath()
{
    return route_.path();
}

/*!
    \qmlproperty georectangle QtLocation::Route::bounds

    Read-only property which holds a bounding box which encompasses the entire route.

*/

QGeoRectangle QDeclarativeGeoRoute::bounds() const
{
    return route_.bounds();
}

/*!
    \qmlproperty int QtLocation::Route::travelTime

    Read-only property which holds the estimated amount of time it will take to
    traverse this route, in seconds.

*/

int QDeclarativeGeoRoute::travelTime() const
{
    return route_.travelTime();
}

/*!
    \qmlproperty real QtLocation::Route::distance

    Read-only property which holds distance covered by this route, in meters.
*/

qreal QDeclarativeGeoRoute::distance() const
{
    return route_.distance();
}

/*!
    \qmlproperty list<coordinate> QtLocation::Route::path

    Read-only property which holds the geographical coordinates of this route.
    Coordinates are listed in the order in which they would be traversed by someone
    traveling along this segment of the route.

    To access individual segments you can use standard list accessors: 'path.length'
    indicates the number of objects and 'path[index starting from zero]' gives
    the actual object.

    \sa QtPositioning::coordinate
*/

QJSValue QDeclarativeGeoRoute::path() const
{
    return fromList(parent(), route_.path());
}

void QDeclarativeGeoRoute::setPath(const QJSValue &value)
{
    if (!value.isArray())
        return;

    const QList<QGeoCoordinate> pathList = toList(this, value);
    if (route_.path() == pathList)
        return;

    route_.setPath(pathList);
    emit pathChanged();
}

/*!
    \qmlproperty list<routeSegment> QtLocation::Route::segments

    Read-only property which holds the list of \l routeSegment objects of this route.

    To access individual segments you can use standard list accessors: 'segments.length'
    indicates the number of objects and 'segments[index starting from zero]' gives
    the actual objects.

    \sa routeSegment
*/

QList<QGeoRouteSegment> QDeclarativeGeoRoute::segments()
{
    return segments_;
}

/*!
    \internal
*/
void QDeclarativeGeoRoute::appendSegment(const QGeoRouteSegment &segment)
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

/*!
    \qmlmethod int QtLocation::Route::segmentsCount()

    Returns the number of segments in the route

    \sa routeSegment

    \since 5.11
*/

int QDeclarativeGeoRoute::segmentsCount() const
{
    return qMax(route_.d_ptr->segmentsCount(), segments_.count());
}

const QGeoRoute &QDeclarativeGeoRoute::route() const
{
    return route_;
}

/*!
    \qmlproperty RouteQuery QtLocation::Route::routeQuery

    Returns the route query associated with this route.

    \since 5.11
*/
QDeclarativeGeoRouteQuery *QDeclarativeGeoRoute::routeQuery()
{
    if (!routeQuery_)
        routeQuery_ = new QDeclarativeGeoRouteQuery(route_.request(), this);
    return routeQuery_;
}

/*!
    \qmlproperty list<Route> QtLocation::Route::legs

    Returns the route legs associated with this route.
    Route legs are the sub-routes between each two adjacent waypoints.
    The result may be empty, if this level of detail is not supported by the
    backend.

    \since QtLocation 5.12
*/
QList<QDeclarativeGeoRouteLeg *> QDeclarativeGeoRoute::legs()
{
    // route_.routeLegs() is expected not to change.
    // The following if condition is expected to be run only once.
    if (route_.routeLegs().size() != legs_.size()) {
        legs_.clear();
        QList<QGeoRouteLeg> rlegs = route_.routeLegs();
        for (const QGeoRouteLeg &r: rlegs) {
            QDeclarativeGeoRouteLeg *dr = new QDeclarativeGeoRouteLeg(r, this);
            legs_.append(dr);
        }
    }
    return legs_;
}

/*!
    \qmlproperty Object Route::extendedAttributes

    This property holds the extended attributes of the route and is a map.
    These attributes are plugin specific, and can be empty.

    Consult the \l {Qt Location#Plugin References and Parameters}{plugin documentation}
    for what attributes are supported and how they should be used.

    Note, due to limitations of the QQmlPropertyMap, it is not possible
    to declaratively specify the attributes in QML, assignment of attributes keys
    and values can only be accomplished by JavaScript.

    \since QtLocation 5.13
*/
QQmlPropertyMap *QDeclarativeGeoRoute::extendedAttributes() const
{
    if (!m_extendedAttributes) {
        QDeclarativeGeoRoute *self = const_cast<QDeclarativeGeoRoute *>(this);
        self->m_extendedAttributes = new QQmlPropertyMap(self);
        // Fill it
        const QVariantMap &xAttrs = route_.extendedAttributes();
        const QStringList &keys = xAttrs.keys();
        for (const QString &key: keys)
            self->m_extendedAttributes->insert(key, xAttrs.value(key));
    }
    return m_extendedAttributes;
}

/*!
    \qmlmethod bool QtLocation::Route::equals(Route other)

    This method performs deep comparison if the present route
    is identical to the \a other route.
    Returns \c true if the routes are equal.

    \since 5.12
*/
bool QDeclarativeGeoRoute::equals(QDeclarativeGeoRoute *other) const
{
    return route_ == other->route_;
}

/*!
    \qmltype RouteLeg
    \instantiates QDeclarativeGeoRouteLeg
    \inqmlmodule QtLocation
    \ingroup qml-QtLocation5-routing
    \since QtLocation 5.12

    \brief The RouteLeg type represents a leg of a Route, that is the portion
    of a route between one waypoint and the next.

    \note Since RouteLeg is a subclass of Route, QtLocation::Route::legs will
    return an empty list if accessed on a route leg.
*/

/*!
    \qmlproperty int QtLocation::RouteLeg::legIndex

    Read-only property which holds the index of the leg within the containing Route's list of QtLocation::Route::legs .
*/

/*!
    \qmlproperty Route QtLocation::RouteLeg::overallRoute

    Read-only property which holds the Route that contains this leg.
*/


QDeclarativeGeoRouteLeg::QDeclarativeGeoRouteLeg(QObject *parent)
    : QDeclarativeGeoRoute(parent)
{

}

QDeclarativeGeoRouteLeg::QDeclarativeGeoRouteLeg(const QGeoRouteLeg &routeLeg, QObject *parent)
    : QDeclarativeGeoRoute(routeLeg, parent), m_routeLeg(routeLeg)
{

}

QDeclarativeGeoRouteLeg::~QDeclarativeGeoRouteLeg()
{

}

int QDeclarativeGeoRouteLeg::legIndex() const
{
    return m_routeLeg.legIndex();
}

QObject *QDeclarativeGeoRouteLeg::overallRoute() const
{
    QDeclarativeGeoRoute *containingRoute = qobject_cast<QDeclarativeGeoRoute *>(parent());
    if (Q_UNLIKELY(!containingRoute))
        return new QDeclarativeGeoRoute(m_routeLeg.overallRoute(), parent());
    return containingRoute;
}

QT_END_NAMESPACE
