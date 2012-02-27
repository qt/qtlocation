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


#include "qdeclarativeroutemapitem_p.h"
#include "qdeclarativepolylinemapitem_p.h"
#include "qgeocameracapabilities_p.h"
#include "qdeclarativegeoroute_p.h"
#include <QtDeclarative/QDeclarativeInfo>
#include <QtGui/QPainter>

/*!
    \qmlclass MapRoute QDeclarativeRouteMapItem
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-maps
    \since QtLocation 5.0

    \brief The MapRoute element displays a Route on a Map.

    The MapRoute element displays a Route obtained through a RouteModel or
    other means, on the Map as a Polyline following the path of the Route.

    MapRoute is really a \l MapPolyline, but with the path specified using the
    \l route property instead of directly in Coordinates.

    By default, the route is displayed as a 1-pixel thick black line. This can
    be changed using the \l line.width and \l line.color properties.

    \section2 Performance

    For notes about the performance on MapRoute, refer to the documentation for
    \l MapPolyline.

    \section2 Example Usage
*/
/* TODO: an example here. do we want to use a RouteModel? */

QDeclarativeRouteMapItem::QDeclarativeRouteMapItem(QQuickItem *parent):
    QDeclarativeGeoMapItemBase(parent),
    route_(0)
{
    setFlag(ItemHasContents, true);
    line_.setWidth(3.0);
    QObject::connect(&line_, SIGNAL(colorChanged(QColor)),
                     this, SLOT(updateAfterLinePropertiesChanged()));
    QObject::connect(&line_, SIGNAL(widthChanged(qreal)),
                     this, SLOT(updateAfterLinePropertiesChanged()));
}

QDeclarativeRouteMapItem::~QDeclarativeRouteMapItem()
{
}

void QDeclarativeRouteMapItem::updateAfterLinePropertiesChanged()
{
    // mark dirty just in case we're a width change
    geometry_.markSourceDirty();
    updateMapItem();
}

void QDeclarativeRouteMapItem::setMap(QDeclarativeGeoMap* quickMap, QGeoMap *map)
{
    QDeclarativeGeoMapItemBase::setMap(quickMap,map);
    if (map) {
        geometry_.markSourceDirty();
        updateMapItem();
    }
}

/*!
    \qmlproperty GeoRoute MapRoute::route

    Holds the route to be drawn.
*/
QDeclarativeGeoRoute* QDeclarativeRouteMapItem::route() const
{
    return route_;
}

void QDeclarativeRouteMapItem::setRoute(QDeclarativeGeoRoute *route)
{
    if (route_ == route)
        return;

    route_ = route;

    if (route_) {
        path_ = route_->routePath();
    } else {
        path_ = QList<QGeoCoordinate>();
    }

    geometry_.markSourceDirty();
    updateMapItem();
    emit routeChanged(route_);

}

QSGNode* QDeclarativeRouteMapItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    Q_UNUSED(data);

    MapPolylineNode *node = static_cast<MapPolylineNode*>(oldNode);

    if (!node) {
        node = new MapPolylineNode();
    }

    //TODO: update only material
    if (geometry_.isScreenDirty() || dirtyMaterial_) {
        node->update(line_.color(), &geometry_);
        geometry_.markClean();
        dirtyMaterial_ = false;
    }
    return node;
}

/*!
    \qmlproperty int MapRoute::line.width
    \qmlproperty color MapRoute::line.color

    These properties hold the width and color used to draw the line.

    The width is in pixels and is independent of the zoom level of the map.
    The default values correspond to a black border with a width of 1 pixel.

    For no line, use a width of 0 or a transparent color.
*/

QDeclarativeMapLineProperties *QDeclarativeRouteMapItem::line()
{
    return &line_;
}

void QDeclarativeRouteMapItem::updateMapItem()
{
    if (!map() || path_.isEmpty())
        return;

    geometry_.updateSourcePoints(*map(), path_);
    geometry_.updateScreenPoints(*map(), line_.width());
    setWidth(geometry_.screenBoundingBox().width());
    setHeight(geometry_.screenBoundingBox().height());

    setPositionOnMap(path_.at(0), geometry_.firstPointOffset());
    update();
}

void QDeclarativeRouteMapItem::afterViewportChanged(const QGeoMapViewportChangeEvent &event)
{
    // if the scene is tilted, we must regenerate our geometry every frame
    if (map()->cameraCapabilities().supportsTilting()
            && (event.cameraData.tilt() > 0.1
                || event.cameraData.tilt() < -0.1)) {
        geometry_.markSourceDirty();
    }

    // if the scene is rolled, we must regen too
    if (map()->cameraCapabilities().supportsRolling()
            && (event.cameraData.roll() > 0.1
                || event.cameraData.roll() < -0.1)) {
        geometry_.markSourceDirty();
    }

    // otherwise, only regen on rotate, resize and zoom
    if (event.bearingChanged || event.mapSizeChanged || event.zoomLevelChanged) {
        geometry_.markSourceDirty();
    }

    geometry_.markScreenDirty();
    updateMapItem();
}

bool QDeclarativeRouteMapItem::contains(QPointF point)
{
    return geometry_.contains(point);
}
