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
#include <QDeclarativeInfo>
#include <QPainter>

struct Vertex
{
    QVector2D position;
};

static void updatePolygon(QPolygonF& points,const Map& map, const QList<QGeoCoordinate> &path, qreal& w, qreal& h)
{

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
        } else {
            minX = qMin(point.x(), minX);
            maxX = qMax(point.x(), maxX);
            minY = qMin(point.y(), minY);
            maxY = qMax(point.y(), maxY);
        }
        points.append(point);
    }

    points.translate(-minX, -minY);

    w = maxX - minX;
    h = maxY - minY;
}

QDeclarativePolygonMapItem::QDeclarativePolygonMapItem(QQuickItem *parent) :
    QDeclarativeGeoMapItemBase(parent),
    mapPolygonNode_(0),
    zoomLevel_(0.0)
{
    setFlag(ItemHasContents, true);
    QObject::connect(&border_, SIGNAL(colorChanged(QColor)),
                     this, SLOT(handleBorderUpdated()));
    QObject::connect(&border_, SIGNAL(widthChanged(qreal)),
                     this, SLOT(handleBorderUpdated()));
}

void QDeclarativePolygonMapItem::handleBorderUpdated()
{
    updateMapItem(true);
}

QDeclarativePolygonMapItem::~QDeclarativePolygonMapItem()
{
}

QDeclarativeMapLineProperties *QDeclarativePolygonMapItem::border()
{
    return &border_;
}

void QDeclarativePolygonMapItem::setMap(QDeclarativeGeoMap* quickMap, Map *map)
{
    QDeclarativeGeoMapItemBase::setMap(quickMap, map);
    if (map)
        QObject::connect(map, SIGNAL(cameraDataChanged(CameraData)),
                         this, SLOT(handleCameraDataChanged(CameraData)));
}

QDeclarativeListProperty<QDeclarativeCoordinate> QDeclarativePolygonMapItem::declarativePath()
{
    return QDeclarativeListProperty<QDeclarativeCoordinate>(this, 0, path_append, path_count,
                                                            path_at, path_clear);
}

void QDeclarativePolygonMapItem::path_append(
        QDeclarativeListProperty<QDeclarativeCoordinate> *property, QDeclarativeCoordinate *coordinate)
{
    QDeclarativePolygonMapItem* item = qobject_cast<QDeclarativePolygonMapItem*>(property->object);
    item->coordPath_.append(coordinate);
    item->path_.append(coordinate->coordinate());
    item->updateMapItem(true);
    emit item->pathChanged();
}

int QDeclarativePolygonMapItem::path_count(
        QDeclarativeListProperty<QDeclarativeCoordinate> *property)
{
    return qobject_cast<QDeclarativePolygonMapItem*>(property->object)->coordPath_.count();
}

QDeclarativeCoordinate* QDeclarativePolygonMapItem::path_at(
        QDeclarativeListProperty<QDeclarativeCoordinate> *property, int index)
{
    return qobject_cast<QDeclarativePolygonMapItem*>(property->object)->coordPath_.at(index);
}

void QDeclarativePolygonMapItem::path_clear(
        QDeclarativeListProperty<QDeclarativeCoordinate> *property)
{
    QDeclarativePolygonMapItem* item = qobject_cast<QDeclarativePolygonMapItem*>(
                property->object);
    qDeleteAll(item->coordPath_);
    item->coordPath_.clear();
    item->path_.clear();
    item->updateMapItem(true);
    emit item->pathChanged();
}

void QDeclarativePolygonMapItem::addCoordinate(QDeclarativeCoordinate* coordinate)
{
    coordPath_.append(coordinate);
    path_.append(coordinate->coordinate());
    updateMapItem(true);
    emit pathChanged();
}

void QDeclarativePolygonMapItem::removeCoordinate(QDeclarativeCoordinate* coordinate)
{
    int index = coordPath_.lastIndexOf(coordinate);

    if (index == -1) {
        qmlInfo(this) << tr("Coordinate does not belong to PolygonMapItem.");
        return;
    }

    if (path_.count() < index + 1) {
        qmlInfo(this) << tr("Coordinate does not belong to PolygonMapItem.");
        return;
    }
    coordPath_.removeAt(index);
    path_.removeAt(index);
    updateMapItem(true);
    emit pathChanged();
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
    updateMapItem(false);
    emit colorChanged(color_);
}

QSGNode* QDeclarativePolygonMapItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    Q_UNUSED(data);

    MapPolygonNode *node = static_cast<MapPolygonNode*>(oldNode);

    if (!node) {
        mapPolygonNode_ = new MapPolygonNode();
        updateMapItem(true);
    }

    mapPolygonNode_->update();
    return mapPolygonNode_;
}

void
QDeclarativePolygonMapItem::updateMapItem(bool dirtyGeometry)
{
    if (!map() || path_.count() == 0 || !mapPolygonNode_)
        return;

    mapPolygonNode_->setBrushColor(color_);
    mapPolygonNode_->setPenColor(border_.color());
    mapPolygonNode_->setLineWidth(border_.width());

    if (dirtyGeometry)
        mapPolygonNode_->setGeometry(*map(), path_);

    const QSizeF& size = mapPolygonNode_->size();

    setWidth(size.width());
    setHeight(size.height());

    setPositionOnMap(path_.at(0), mapPolygonNode_->geometry().at(0));
    update();
}

void QDeclarativePolygonMapItem::handleCameraDataChanged(const CameraData& cameraData)
{
    if (cameraData.zoomFactor() != zoomLevel_) {
        zoomLevel_ = cameraData.zoomFactor();
        updateMapItem(true);
    } else {
        updateMapItem(false);
    }
}

void QDeclarativePolygonMapItem::dragStarted()
{
    qmlInfo(this) << "warning: mouse dragging is not currently supported with polygon.";
}

bool QDeclarativePolygonMapItem::contains(QPointF point)
{
    return mapPolygonNode_->contains(point);
}

//////////////////////////////////////////////////////////////////////

MapPolygonNode::MapPolygonNode() :
    fillColor_(Qt::black),
    borderColor_(Qt::black),
    borderWidth_(3.0),
    border_(new MapPolylineNode()),
    geometry_(QSGGeometry::defaultAttributes_Point2D(), 0)
{
    geometry_.setDrawingMode(GL_POLYGON);
    QSGGeometryNode::setMaterial(&fill_material_);
    QSGGeometryNode::setGeometry(&geometry_);

    appendChildNode(border_);
}

MapPolygonNode::~MapPolygonNode()
{
}

void MapPolygonNode::update()
{
    //TODO: optimize , perform calculation only if polygon has changed
    if (polygon_.size()==0) return;

    QSGGeometry *fill = QSGGeometryNode::geometry();

    Q_ASSERT(fill->sizeOfVertex() == sizeof(Vertex));

    int fillVertexCount = 0;
    //note this will not allocate new buffer if the size has not changed
    fill->allocate(polygon_.size());

    Vertex *vertices = (Vertex *)fill->vertexData();

    for (int i = 0; i < polygon_.size(); ++i) {
        vertices[fillVertexCount++].position = QVector2D(polygon_.at(i));
    }

    Q_ASSERT(fillVertexCount == fill->vertexCount());

    markDirty(DirtyGeometry);

    if (fillColor_ != fill_material_.color()) {
        fill_material_.setColor(fillColor_);
        setMaterial(&fill_material_);
    }

    border_->setPenColor(borderColor_);
    border_->setLineWidth(borderWidth_);

    border_->update();

    //TODO: implement me : gradient
}

qreal MapPolygonNode::lineWidth() const
{
    return borderWidth_;
}

void MapPolygonNode::setLineWidth(qreal width)
{
    borderWidth_ = width;
}

void MapPolygonNode::setBrushColor(const QColor &color)
{
    fillColor_= color;
}

QColor MapPolygonNode::brushColor() const
{
    return fillColor_;
}

void MapPolygonNode::setPenColor(const QColor &color)
{
    borderColor_ = color;
}

QColor MapPolygonNode::penColor() const
{
    return borderColor_;
}

bool MapPolygonNode::contains(QPointF point)
{
    return polygon_.containsPoint(point, Qt::OddEvenFill);
}

void MapPolygonNode::setGeometry(const Map& map, const QList<QGeoCoordinate> &path)
{
    qreal h = 0;
    qreal w = 0;
    polygon_.clear();
    updatePolygon(polygon_, map, path, w, h);
    size_ = QSizeF(w, h);

    QList<QGeoCoordinate> pathClosed = path;
    pathClosed.append(pathClosed.at(0));
    border_->setGeometry(map, pathClosed);
}
