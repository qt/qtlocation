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

QDeclarativeRouteMapItem::QDeclarativeRouteMapItem(QQuickItem *parent)
    : QDeclarativeGeoMapItemBase(parent),
      polylineItem_(new PolylineMapPaintedItem(this)),
      route_(0)
{
    polylineItem_->setParentItem(this);
}

QDeclarativeRouteMapItem::~QDeclarativeRouteMapItem()
{
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
    //QBrush m_brush(color);
    //polylineItem_->setBrush(m_brush);
    polylineItem_->setPen(color);
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
        polylineItem_->setPath(route_->routePath());
    } else {
        polylineItem_->setPath(QList<QGeoCoordinate>());
    }

    updateMapItem();
    emit routeChanged(route_);

}

void QDeclarativeRouteMapItem::updateContent()
{
    polylineItem_->updateGeometry();
    setWidth(polylineItem_->width());
    setHeight(polylineItem_->height());
}

QPointF QDeclarativeRouteMapItem::contentTopLeftPoint()
{
    return map_->coordinateToScreenPosition(
                polylineItem_->quickItemCoordinate(), false) - polylineItem_->quickItemAnchorPoint();
}

void QDeclarativeRouteMapItem::mapChanged()
{
    polylineItem_->setMap(map_);
    if (map_) {
        connect(map_, SIGNAL(cameraDataChanged(CameraData)), this, SLOT(handleCameraDataChanged(CameraData)));
        // initial update
        handleCameraDataChanged(map_->cameraData());
    }
}

void QDeclarativeRouteMapItem::dragStarted()
{
    qmlInfo(this) << "warning: mouse dragging is not currently supported with polylines/routes.";
}

bool QDeclarativeRouteMapItem::contains(QPointF point)
{
    return polylineItem_->contains(point);
}

void QDeclarativeRouteMapItem::handleCameraDataChanged(const CameraData& cameraData)
{
    polylineItem_->setZoomLevel(cameraData.zoomFactor());
    updateMapItem();
}
