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
#include "qdeclarativegeomapquickitem_p.h"
#include "qdeclarativegeoroute_p.h"
#include <QPainter>

QDeclarativeRouteMapItem::QDeclarativeRouteMapItem(QQuickItem *parent)
        : QDeclarativeGeoMapItemBase(parent),
        quickItem_(new QDeclarativeGeoMapQuickItem(this)),
        polylineMapPaintedItem_(new PolylineMapPaintedItem(quickItem_)),
        route_(0)
{
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
    QBrush m_brush(color);
    polylineMapPaintedItem_->setBrush(m_brush);
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
        polylineMapPaintedItem_->setPath(route_->route_.path());
    } else {
        polylineMapPaintedItem_->setPath(QList<QGeoCoordinate>());
    }

    emit routeChanged(route_);

}

void QDeclarativeRouteMapItem::update()
{
    if (!map())
        return;

    quickItem_->setCoordinate(new QDeclarativeCoordinate(polylineMapPaintedItem_->screenItemCoordinate()));
    quickItem_->setAnchorPoint(polylineMapPaintedItem_->screenItemAnchorPoint());

    if (quickItem_->sourceItem() == 0) {
        QObject::connect(map(), SIGNAL(cameraDataChanged(CameraData)), this, SLOT(handleCameraDataChanged(CameraData)));
        polylineMapPaintedItem_->setMap(map());
        quickItem_->setMap(quickMap(), map());
        quickItem_->setSourceItem(polylineMapPaintedItem_);
    }
}

void QDeclarativeRouteMapItem::handleCameraDataChanged(const CameraData& cameraData)
{
    polylineMapPaintedItem_->setZoomLevel(cameraData.zoomFactor());
    update();
}
