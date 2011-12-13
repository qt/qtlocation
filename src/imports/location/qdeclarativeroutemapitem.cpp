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


#include "qdeclarativeroutemapitem_p.h"
#include "qdeclarativepolylinemapitem_p.h"
#include "qdeclarativegeoroute_p.h"
#include <QtDeclarative/QDeclarativeInfo>
#include <QtGui/QPainter>

QDeclarativeRouteMapItem::QDeclarativeRouteMapItem(QQuickItem *parent):
    QDeclarativeGeoMapItemBase(parent),
    mapPolylineNode_(0),
    route_(0),
    zoomLevel_(0.0)
{
    setFlag(ItemHasContents, true);
}

QDeclarativeRouteMapItem::~QDeclarativeRouteMapItem()
{
}

void QDeclarativeRouteMapItem::setMap(QDeclarativeGeoMap* quickMap, Map *map)
{
    QDeclarativeGeoMapItemBase::setMap(quickMap,map);
    if (map) QObject::connect(map, SIGNAL(cameraDataChanged(CameraData)), this, SLOT(handleCameraDataChanged(CameraData)));
}

QColor QDeclarativeRouteMapItem::color() const
{
    return color_;
}

void QDeclarativeRouteMapItem::setColor(const QColor &color)
{
    if (color_ == color)
        return;

    color_ = color;
    updateMapItem(false);
    emit colorChanged(color_);
}

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

    updateMapItem(true);
    emit routeChanged(route_);

}

QSGNode* QDeclarativeRouteMapItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    Q_UNUSED(data);

    MapPolylineNode *node = static_cast<MapPolylineNode*>(oldNode);

    if (!node) {
        mapPolylineNode_ = new MapPolylineNode();
        updateMapItem(true);
    }

    mapPolylineNode_->update();
    return mapPolylineNode_;
}


void QDeclarativeRouteMapItem::updateMapItem(bool dirtyGeometry) {

    if (!map() || path_.count() == 0 || !mapPolylineNode_)
        return;

    mapPolylineNode_->setPenColor(color_);

    if (dirtyGeometry) mapPolylineNode_->setGeometry(*map(), path_);

    const QSizeF& size = mapPolylineNode_->size();

    setWidth(size.width());
    setHeight(size.height());

    setPositionOnMap(path_.at(0), mapPolylineNode_->geometry().at(0));
    update();
}

void QDeclarativeRouteMapItem::handleCameraDataChanged(const CameraData& cameraData)
{
    if (cameraData.zoomFactor() != zoomLevel_) {
        zoomLevel_ = cameraData.zoomFactor();
        updateMapItem(true);
    } else {
        updateMapItem(false);
    }
}

bool QDeclarativeRouteMapItem::contains(QPointF point)
{
    return mapPolylineNode_->contains(point);
}

void QDeclarativeRouteMapItem::dragStarted()
{
    qmlInfo(this) << "warning: mouse dragging is not currently supported with polylines/routes.";
}
