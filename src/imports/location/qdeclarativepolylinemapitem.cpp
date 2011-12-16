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

#include "qdeclarativepolylinemapitem_p.h"
#include <QDeclarativeInfo>
#include <QPainter>

QT_BEGIN_NAMESPACE

QDeclarativeMapLineProperties::QDeclarativeMapLineProperties(QObject *parent) :
    QObject(parent),
    width_(2.0),
    color_(Qt::black)
{
}

QColor QDeclarativeMapLineProperties::color() const
{
    return color_;
}

void QDeclarativeMapLineProperties::setColor(const QColor &color)
{
    if (color_ == color)
        return;

    color_ = color;
    emit colorChanged(color_);
}

qreal QDeclarativeMapLineProperties::width() const
{
    return width_;
}

void QDeclarativeMapLineProperties::setWidth(qreal width)
{
    if (width_ == width)
        return;

    width_ = width;
    emit widthChanged(width_);
}

struct Vertex
{
    QVector2D position;
};

static void updatePolyline(QPolygonF& points,const Map& map, const QList<QGeoCoordinate> &path, qreal& w, qreal& h)
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

QDeclarativePolylineMapItem::QDeclarativePolylineMapItem(QQuickItem *parent) :
    QDeclarativeGeoMapItemBase(parent),
    mapPolylineNode_(0),
    zoomLevel_(0.0)
{
    setFlag(ItemHasContents, true);
    QObject::connect(&line_, SIGNAL(colorChanged(QColor)),
                     this, SLOT(updateAfterLinePropertiesChanged()));
    QObject::connect(&line_, SIGNAL(widthChanged(qreal)),
                     this, SLOT(updateAfterLinePropertiesChanged()));
}

QDeclarativePolylineMapItem::~QDeclarativePolylineMapItem()
{
}

void QDeclarativePolylineMapItem::updateAfterLinePropertiesChanged()
{
    // pass true just in case we're a width change
    updateMapItem(true);
}

void QDeclarativePolylineMapItem::setMap(QDeclarativeGeoMap* quickMap, Map *map)
{
    QDeclarativeGeoMapItemBase::setMap(quickMap,map);
    if (map) QObject::connect(map, SIGNAL(cameraDataChanged(CameraData)), this, SLOT(handleCameraDataChanged(CameraData)));
}

QDeclarativeListProperty<QDeclarativeCoordinate> QDeclarativePolylineMapItem::declarativePath()
{
    return QDeclarativeListProperty<QDeclarativeCoordinate>(this, 0, path_append, path_count,
                                                            path_at, path_clear);
}

void QDeclarativePolylineMapItem::path_append(QDeclarativeListProperty<QDeclarativeCoordinate> *property, QDeclarativeCoordinate *coordinate)
{
    QDeclarativePolylineMapItem* item = qobject_cast<QDeclarativePolylineMapItem*>(
                property->object);
    item->coordPath_.append(coordinate);
    item->path_.append(coordinate->coordinate());
    item->updateMapItem(true);
    emit item->pathChanged();
}

int QDeclarativePolylineMapItem::path_count(
        QDeclarativeListProperty<QDeclarativeCoordinate> *property)
{
    return qobject_cast<QDeclarativePolylineMapItem*>(property->object)->coordPath_.count();
}

QDeclarativeCoordinate* QDeclarativePolylineMapItem::path_at(
        QDeclarativeListProperty<QDeclarativeCoordinate> *property, int index)
{
    return qobject_cast<QDeclarativePolylineMapItem*>(property->object)->coordPath_.at(index);
}

void QDeclarativePolylineMapItem::path_clear(
        QDeclarativeListProperty<QDeclarativeCoordinate> *property)
{
    QDeclarativePolylineMapItem* item = qobject_cast<QDeclarativePolylineMapItem*>(
                property->object);
    qDeleteAll(item->coordPath_);
    item->coordPath_.clear();
    item->path_.clear();
    item->updateMapItem(true);
    emit item->pathChanged();
}

void QDeclarativePolylineMapItem::addCoordinate(QDeclarativeCoordinate* coordinate)
{
    coordPath_.append(coordinate);
    path_.append(coordinate->coordinate());
    updateMapItem(true);
    emit pathChanged();
}

void QDeclarativePolylineMapItem::removeCoordinate(QDeclarativeCoordinate* coordinate)
{
    int index = coordPath_.lastIndexOf(coordinate);

    if (index == -1) {
        qmlInfo(this) << tr("Coordinate does not belong to PolylineMapItem.");
        return;
    }

    if (path_.count() < index + 1) {
        qmlInfo(this) << tr("Coordinate does not belong to PolylineMapItem.");
        return;
    }
    coordPath_.removeAt(index);
    path_.removeAt(index);
    updateMapItem(true);
    emit pathChanged();
}

QDeclarativeMapLineProperties *QDeclarativePolylineMapItem::line()
{
    return &line_;
}

QSGNode* QDeclarativePolylineMapItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
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

void QDeclarativePolylineMapItem::updateMapItem(bool dirtyGeometry) {

    if (!map() || path_.count()==0 || !mapPolylineNode_)
        return;

    mapPolylineNode_->setPenColor(line_.color());
    mapPolylineNode_->setLineWidth(line_.width());

    if (dirtyGeometry)
        mapPolylineNode_->setGeometry(*map(), path_);

    const QSizeF& size = mapPolylineNode_->size();

    setWidth(size.width());
    setHeight(size.height());

    setPositionOnMap(path_.at(0),mapPolylineNode_->geometry().at(0));
    update();
}

void QDeclarativePolylineMapItem::handleCameraDataChanged(const CameraData& cameraData)
{
    if (cameraData.zoomFactor() != zoomLevel_) {
        zoomLevel_ = cameraData.zoomFactor();
        updateMapItem(true);
    } else {
        updateMapItem(false);
    }
}

bool QDeclarativePolylineMapItem::contains(QPointF point)
{
    // TODO: this currently returns all points _inside_ of the implicitly closed
    // painterpath whereas we are only interested in the outlines of it
    return mapPolylineNode_->contains(point);
}

void QDeclarativePolylineMapItem::dragStarted()
{
    qmlInfo(this) << "warning: mouse dragging is not currently supported with polylines/routes.";
}

//////////////////////////////////////////////////////////////////////

MapPolylineNode::MapPolylineNode() :
    fillColor_(Qt::black),
    width_(2.0),
    geometry_(QSGGeometry::defaultAttributes_Point2D(),0)
{
    geometry_.setDrawingMode(GL_LINE_STRIP);
    QSGGeometryNode::setMaterial(&fill_material_);
    QSGGeometryNode::setGeometry(&geometry_);
}


MapPolylineNode::~MapPolylineNode()
{
}

void MapPolylineNode::update()
{
    //TODO: optimize , perform calculation only if polygon has changed
    if (polyline_.size() == 0)
        return;

    QSGGeometry *fill = QSGGeometryNode::geometry();

    fill->setLineWidth(width_);

    Q_ASSERT(fill->sizeOfVertex() == sizeof(Vertex));

    int fillVertexCount = 0;
    //note this will not allocate new buffer if the size has not changed
    fill->allocate(polyline_.size());

    Vertex *vertices = (Vertex *)fill->vertexData();

    for (int i = 0; i < polyline_.size(); ++i) {
        vertices[fillVertexCount++].position = QVector2D(polyline_.at(i));
    }

    Q_ASSERT(fillVertexCount == fill->vertexCount());

    markDirty(DirtyGeometry);

    if (fillColor_ != fill_material_.color()) {
        fill_material_.setColor(fillColor_);
        setMaterial(&fill_material_);
    }

    //TODO: implement me : borders , gradient
}

bool MapPolylineNode::contains(QPointF point)
{
    //TODO: implement me
    return polyline_.contains(point);
}

void MapPolylineNode::setPenColor(const QColor &color)
{
    fillColor_ = color;
}

QColor MapPolylineNode::penColor() const
{
    return fillColor_;
}

qreal MapPolylineNode::lineWidth() const
{
    return width_;
}

void MapPolylineNode::setLineWidth(qreal width)
{
    width_ = width;
}

void MapPolylineNode::setGeometry(const Map& map, const QList<QGeoCoordinate> &path)
{
    qreal h = 0;
    qreal w = 0;
    polyline_.clear();
    updatePolyline(polyline_, map, path, w, h);
    size_ = QSizeF(w, h);
}

QT_END_NAMESPACE
