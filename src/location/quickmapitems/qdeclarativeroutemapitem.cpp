/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
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


#include "qdeclarativeroutemapitem_p.h"
#include "qdeclarativepolylinemapitem_p.h"

#include <QtQml/QQmlInfo>
#include <QtGui/QPainter>

QT_BEGIN_NAMESPACE

/*!
    \qmltype MapRoute
    \instantiates QDeclarativeRouteMapItem
    \inqmlmodule QtLocation
    \ingroup qml-QtLocation5-maps
    \since QtLocation 5.0

    \brief The MapRoute type displays a Route on a Map.

    The MapRoute type displays a Route obtained through a RouteModel or
    other means, on the Map as a Polyline following the path of the Route.

    MapRoute is really a \l MapPolyline, but with the path specified using the
    \l route property instead of directly in \l {coordinate}{coordinates}.

    By default, the route is displayed as a 1-pixel thick black line. This can
    be changed using the \l line.width and \l line.color properties.

    \section2 Performance

    For notes about the performance on MapRoute, refer to the documentation for
    \l MapPolyline.

    \section2 Example Usage

    Here is how to draw a \l{Route}{route} on a \l{Map}{map}:

    \snippet declarative/maps.qml QtQuick import
    \snippet declarative/maps.qml QtLocation import
    \codeline
    \snippet declarative/maps.qml MapRoute
*/

/*!
    \qmlpropertygroup Location::MapRoute::line
    \qmlproperty int MapRoute::line.width
    \qmlproperty color MapRoute::line.color

    This property is part of the line property group. The line
    property group holds the width and color used to draw the line.

    The width is in pixels and is independent of the zoom level of the map.
    The default values correspond to a black border with a width of 1 pixel.

    For no line, use a width of 0 or a transparent color.
*/


QDeclarativeRouteMapItem::QDeclarativeRouteMapItem(QQuickItem *parent)
:   QDeclarativePolylineMapItem(parent)
{
    setFlag(ItemHasContents, true);
}

QDeclarativeRouteMapItem::~QDeclarativeRouteMapItem()
{
}

/*!
    \qmlproperty Route MapRoute::route

    This property holds the route to be drawn which can be used
    to represent one geographical route.
*/
QGeoRoute QDeclarativeRouteMapItem::route() const
{
    return route_;
}

void QDeclarativeRouteMapItem::setRoute(const QGeoRoute &route)
{
    if (route_ == route)
        return;

    route_ = route;

    setPathFromGeoList(route_.path());

    emit routeChanged(route_);
}

void QDeclarativeRouteMapItem::updateRoutePath()
{
    setPathFromGeoList(route_.path());
}

/*!
   \internal void QDeclarativeRouteMapItem::setPath(const QList<QGeoCoordinate> &value)

   Used to disable path property on the RouteMapItem
 */
void QDeclarativeRouteMapItem::setPath(const QList<QGeoCoordinate> &value)
{
    Q_UNUSED(value);
    qWarning() << "Can not set the path on QDeclarativeRouteMapItem."
               << "Please use the route property instead.";
}

QT_END_NAMESPACE
