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

#include "qdeclarativepolygonmapitem_p.h"
#include "qdeclarativegeomapquickitem_p.h"
#include <QDeclarativeInfo>
#include <QPainter>

static QPolygonF createPolygon(const Map& map, const QList<QGeoCoordinate> &path, qreal& w,
                               qreal& h)
{
    QPolygonF polygon;

    qreal minX, maxX, minY, maxY;
    //TODO: dateline handling

    for (int i = 0; i < path.size(); ++i) {

        const QGeoCoordinate &coord = path.at(i);

        if (!coord.isValid())
            continue;

        QPointF point = map.coordinateToScreenPosition(coord, false);

        if (i == 0) {
            minX = point.x();
            maxX = point.x();
            minY = point.y();
            maxY = point.y();
            polygon.append(point);
        } else {
            minX = qMin(point.x(), minX);
            maxX = qMax(point.x(), maxX);
            minY = qMin(point.y(), minY);
            maxY = qMax(point.y(), maxY);
        }

        polygon.append(point);
    }

    polygon.translate(-minX, -minY);

    w = maxX - minX;
    h = maxY - minY;

    return polygon;
}

QDeclarativePolygonMapItem::QDeclarativePolygonMapItem(QQuickItem *parent) :
        QDeclarativeGeoMapItemBase(parent), quickItem_(new QDeclarativeGeoMapQuickItem(this)), polygonMapPaintedItem_(
            new PolygonMapPaintedItem(quickItem_)), initialized_(false)
{
}

QDeclarativePolygonMapItem::~QDeclarativePolygonMapItem()
{
}

void QDeclarativePolygonMapItem::componentComplete()
{
    initialized_ = true;
}

QDeclarativeListProperty<QDeclarativeCoordinate> QDeclarativePolygonMapItem::declarativePath()
{
    return QDeclarativeListProperty<QDeclarativeCoordinate>(this, 0, path_append, path_count,
            path_at, path_clear);
}

void QDeclarativePolygonMapItem::path_append(
    QDeclarativeListProperty<QDeclarativeCoordinate> *property, QDeclarativeCoordinate *coordinate)
{
    QDeclarativePolygonMapItem* item = qobject_cast<QDeclarativePolygonMapItem*>(
                                            property->object);
    item->path_.append(coordinate);
    QList<QGeoCoordinate> p = item->polygonMapPaintedItem_->path();
    p.append(coordinate->coordinate());
    item->polygonMapPaintedItem_->setPath(p);
    if (item->initialized_)
        emit item->pathChanged();
}

int QDeclarativePolygonMapItem::path_count(
    QDeclarativeListProperty<QDeclarativeCoordinate> *property)
{
    return qobject_cast<QDeclarativePolygonMapItem*>(property->object)->path_.count();
}

QDeclarativeCoordinate* QDeclarativePolygonMapItem::path_at(
    QDeclarativeListProperty<QDeclarativeCoordinate> *property, int index)
{
    return qobject_cast<QDeclarativePolygonMapItem*>(property->object)->path_.at(index);
}

void QDeclarativePolygonMapItem::path_clear(
    QDeclarativeListProperty<QDeclarativeCoordinate> *property)
{
    QDeclarativePolygonMapItem* item = qobject_cast<QDeclarativePolygonMapItem*>(
                                            property->object);
    qDeleteAll(item->path_);
    item->path_.clear();
    item->polygonMapPaintedItem_->setPath(QList<QGeoCoordinate>());
    if (item->initialized_)
        emit item->pathChanged();
}

void QDeclarativePolygonMapItem::addCoordinate(QDeclarativeCoordinate* coordinate)
{
    path_.append(coordinate);
    QList<QGeoCoordinate> path = polygonMapPaintedItem_->path();
    path.append(coordinate->coordinate());
    polygonMapPaintedItem_->setPath(path);
    emit pathChanged();
}

void QDeclarativePolygonMapItem::removeCoordinate(QDeclarativeCoordinate* coordinate)
{
    int index = path_.lastIndexOf(coordinate);

    if (index == -1) {
        qmlInfo(this) << tr("Coordinate does not belong to PolygonMapItem.");
        return;
    }

    QList<QGeoCoordinate> path = polygonMapPaintedItem_->path();

    if (path.count() < index + 1) {
        qmlInfo(this) << tr("Coordinate does not belong to PolygonMapItem.");
        return;
    }
    path.removeAt(index);
    polygonMapPaintedItem_->setPath(path);
    path_.removeAt(index);
    emit pathChanged();
}

void QDeclarativePolygonMapItem::update()
{
    if (!map())
        return;

    quickItem_->setCoordinate(
        new QDeclarativeCoordinate(polygonMapPaintedItem_->quickItemCoordinate()));
    quickItem_->setAnchorPoint(polygonMapPaintedItem_->quickItemAnchorPoint());

    if (quickItem_->sourceItem() == 0) {
        QObject::connect(map(), SIGNAL(cameraDataChanged(CameraData)), this,
                         SLOT(handleCameraDataChanged(CameraData)));
        polygonMapPaintedItem_->setMap(map());
        quickItem_->setMap(quickMap(), map());
        quickItem_->setSourceItem(polygonMapPaintedItem_);
    }
}

void QDeclarativePolygonMapItem::handleCameraDataChanged(const CameraData& cameraData)
{
    polygonMapPaintedItem_->setZoomLevel(cameraData.zoomFactor());
    update();
}

QColor QDeclarativePolygonMapItem::color() const
{
    return color_;
}

void QDeclarativePolygonMapItem::setColor(const QColor &color)
{
    if (color_ == color)
        return;

    color_ = color;
    polygonMapPaintedItem_->setBrush(color);
    emit colorChanged(color_);
}

//////////////////////////////////////////////////////////////////////

PolygonMapPaintedItem::PolygonMapPaintedItem(QQuickItem *parent) :
        QQuickPaintedItem(parent), map_(0), zoomLevel_(-1), initialized_(false)
{
    setAntialiasing(true);
    connect(this, SIGNAL(xChanged()), this, SLOT(update()));
    connect(this, SIGNAL(yChanged()), this, SLOT(update()));
}

PolygonMapPaintedItem::~PolygonMapPaintedItem()
{
}

void PolygonMapPaintedItem::setMap(Map* map)
{
    map_ = map;
}

void PolygonMapPaintedItem::setZoomLevel(qreal zoomLevel)
{
    if (zoomLevel_ == zoomLevel)
        return;

    zoomLevel_ = zoomLevel;
    updateGeometry();
}

qreal PolygonMapPaintedItem::zoomLevel() const
{
    return zoomLevel_;
}

void PolygonMapPaintedItem::setPath(const QList<QGeoCoordinate>& path)
{
    coordPath_ = path;
    updateGeometry();
}

QList<QGeoCoordinate> PolygonMapPaintedItem::path() const
{
    return coordPath_;
}

QGeoCoordinate PolygonMapPaintedItem::quickItemCoordinate() const
{
    return quickItemCoordinate_;
}

QPointF PolygonMapPaintedItem::quickItemAnchorPoint() const
{
    return quickItemAnchorPoint_;
}

void PolygonMapPaintedItem::setBrush(const QBrush &brush)
{
    brush_ = brush;
}

QBrush PolygonMapPaintedItem::brush() const
{
    return brush_;
}

void PolygonMapPaintedItem::setPen(const QPen &pen)
{
    pen_ = pen;
}

QPen PolygonMapPaintedItem::pen() const
{
    return pen_;
}

void PolygonMapPaintedItem::paint(QPainter *painter)
{
    if (!initialized_)
        return;
    painter->setPen(pen_);
    painter->setBrush(brush_);
    painter->drawConvexPolygon(polygon_);
}

void PolygonMapPaintedItem::updateGeometry()
{
    initialized_ = false;

    if (!map_)
        return;

    if (coordPath_.size() == 0)
        return;

    if (zoomLevel_ == -1)
        return;

    QPointF point = map_->coordinateToScreenPosition(coordPath_.at(0), false);

    qreal w = 0;
    qreal h = 0;

    //TODO: optimize essential part
    polygon_ = createPolygon(*map_, coordPath_, w, h);

    setWidth(w);
    setHeight(h);
    setContentsSize(QSize(w, h));

    quickItemCoordinate_ = coordPath_.at(0);
    quickItemAnchorPoint_ = polygon_.first();
    initialized_ = true;

    update();
}
