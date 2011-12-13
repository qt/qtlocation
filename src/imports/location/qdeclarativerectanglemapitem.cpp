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

#include "qdeclarativerectanglemapitem_p.h"
#include <QPainter>

QT_BEGIN_NAMESPACE

struct Vertex
{
    QVector2D position;
};

QDeclarativeRectangleMapItem::QDeclarativeRectangleMapItem(QQuickItem *parent):
    QDeclarativeGeoMapItemBase(parent),
    mapRectangleNode_(0),
    topLeft_(0),
    bottomRight_(0),
    zoomLevel_(0.0),
    dragActive_(false)
{
    setFlag(ItemHasContents, true);
}

QDeclarativeRectangleMapItem::~QDeclarativeRectangleMapItem()
{
}

void QDeclarativeRectangleMapItem::setMap(QDeclarativeGeoMap* quickMap, Map *map)
{
    QDeclarativeGeoMapItemBase::setMap(quickMap,map);
    if (map) QObject::connect(map, SIGNAL(cameraDataChanged(CameraData)), this, SLOT(handleCameraDataChanged(CameraData)));
}

void QDeclarativeRectangleMapItem::setTopLeft(QDeclarativeCoordinate *topLeft)
{
    if (topLeft_ == topLeft)
        return;
    if (topLeft_)
        topLeft_->disconnect(this);
    topLeft_ = topLeft;

    if (topLeft_) {
        connect(topLeft_, SIGNAL(latitudeChanged(double)), this,
                SLOT(updateMapItem()));
        connect(topLeft_, SIGNAL(longitudeChanged(double)), this,
                SLOT(updateMapItem()));
        connect(topLeft_, SIGNAL(altitudeChanged(double)), this,
                SLOT(updateMapItem()));
    }

    updateMapItem(true);
    emit topLeftChanged(topLeft_);
}

QDeclarativeCoordinate* QDeclarativeRectangleMapItem::topLeft()
{
    return topLeft_;
}

void QDeclarativeRectangleMapItem::setBottomRight(QDeclarativeCoordinate *bottomRight)
{
    if (bottomRight_ == bottomRight)
        return;
    if (bottomRight_)
        bottomRight_->disconnect(this);
    bottomRight_ = bottomRight;
    if (bottomRight_) {
        connect(bottomRight_, SIGNAL(latitudeChanged(double)), this,
                SLOT(updateMapItem()));
        connect(bottomRight_, SIGNAL(longitudeChanged(double)), this,
                SLOT(updateMapItem()));
        connect(bottomRight_, SIGNAL(altitudeChanged(double)), this,
                SLOT(updateMapItem()));
    }
    updateMapItem(true);
    emit bottomRightChanged(bottomRight_);
}

QDeclarativeCoordinate* QDeclarativeRectangleMapItem::bottomRight()
{
    return bottomRight_;
}

QColor QDeclarativeRectangleMapItem::color() const
{
    return color_;
}

void QDeclarativeRectangleMapItem::setColor(const QColor &color)
{
    if (color_ == color)
        return;
    color_ = color;
    updateMapItem(false);
    emit colorChanged(color_);
}

QSGNode* QDeclarativeRectangleMapItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    Q_UNUSED(data);

    MapRectangleNode *node = static_cast<MapRectangleNode*>(oldNode);

    if (!node) {
        mapRectangleNode_ = new MapRectangleNode();
        updateMapItem(true);
    }

    mapRectangleNode_->update();
    return mapRectangleNode_;
}

void QDeclarativeRectangleMapItem::updateMapItem(bool dirtyGeometry)
{
    if (!map() || !topLeft() || !topLeft()->coordinate().isValid()
            || !bottomRight() || !bottomRight()->coordinate().isValid()
            || !mapRectangleNode_)
        return;

    mapRectangleNode_->setBrushColor(color_);

    if (dirtyGeometry) mapRectangleNode_->setGeometry(*map(), topLeft()->coordinate(), bottomRight()->coordinate());

    const QSizeF& size = mapRectangleNode_->size();

    setWidth(size.width());
    setHeight(size.height());

    setPositionOnMap(topLeft()->coordinate(), QPointF(0, 0));
    update();
}

void QDeclarativeRectangleMapItem::handleCameraDataChanged(const CameraData& cameraData)
{
    if (cameraData.zoomFactor() != zoomLevel_) {
        zoomLevel_ = cameraData.zoomFactor();
        updateMapItem(true);
    } else {
        updateMapItem(false);
    }
}

bool QDeclarativeRectangleMapItem::contains(QPointF point)
{
    return mapRectangleNode_->contains(point);
}

void QDeclarativeRectangleMapItem::dragEnded()
{
    if (!dragActive_)
        return;
    dragActive_ = false;
    QPointF newTopLeftPoint = QPointF(x(),y());
    // does not preserve exact projection geometry but that should be acceptable
    QGeoCoordinate newTopLeft = map()->screenPositionToCoordinate(newTopLeftPoint, false);
    QPointF newBottomRightPoint = QPointF(x() + width(), y() + height());
    QGeoCoordinate newBottomRight = map()->screenPositionToCoordinate(newBottomRightPoint, false);
    if (newTopLeft.isValid() && newBottomRight.isValid()) {
        internalTopLeft_.setCoordinate(newTopLeft);
        internalBottomRight_.setCoordinate(newBottomRight);
        setTopLeft(&internalTopLeft_);
        setBottomRight(&internalBottomRight_);
    }
}

void QDeclarativeRectangleMapItem::dragStarted()
{
    dragActive_ = true;
}
//////////////////////////////////////////////////////////////////////

MapRectangleNode::MapRectangleNode():
    fillColor_(Qt::black),
    borderColor_(Qt::black),
    geometry_(QSGGeometry::defaultAttributes_Point2D(),0)
{
    geometry_.setDrawingMode(GL_QUADS);
    QSGGeometryNode::setMaterial(&fill_material_);
    QSGGeometryNode::setGeometry(&geometry_);
}

MapRectangleNode::~MapRectangleNode()
{
}

void MapRectangleNode::update()
{
    QSGGeometry *fill = QSGGeometryNode::geometry();

    Q_ASSERT(fill->sizeOfVertex() == sizeof(Vertex));

    int fillVertexCount = 0;
    //note this will not allocate new buffer if the size has not changed
    fill->allocate(4);

    Vertex *vertices = (Vertex *)fill->vertexData();

    //set corners
    vertices[fillVertexCount++].position = QVector2D(rect_.left(),rect_.top());
    vertices[fillVertexCount++].position = QVector2D(rect_.right(),rect_.top());
    vertices[fillVertexCount++].position = QVector2D(rect_.right(),rect_.bottom());
    vertices[fillVertexCount++].position = QVector2D(rect_.left(),rect_.bottom());

    Q_ASSERT(fillVertexCount == fill->vertexCount());

    markDirty(DirtyGeometry);

    if (fillColor_ != fill_material_.color()) {
        fill_material_.setColor(fillColor_);
        setMaterial(&fill_material_);
    }
    //TODO: implement me : borders , gradient
}

void MapRectangleNode::setBrushColor(const QColor &color)
{
    fillColor_= color;
}

QColor MapRectangleNode::brushColor() const
{
    return fillColor_;
}

void MapRectangleNode::setPenColor(const QColor &color)
{
    borderColor_ = color;
}

QColor MapRectangleNode::penColor() const
{
    return borderColor_;
}

bool MapRectangleNode::contains(QPointF point)
{
    return rect_.contains(point);
}

void  MapRectangleNode::setGeometry(const Map& map, const QGeoCoordinate &topLeft, const QGeoCoordinate &bottomRight)
{
    QPointF p1 = map.coordinateToScreenPosition(topLeft, false);
    QPointF p2 = map.coordinateToScreenPosition(bottomRight, false);

    qreal minX = qMin(p1.x(), p2.x());
    qreal maxX = qMax(p1.x(), p2.x());
    qreal minY = qMin(p1.y(), p2.y());
    qreal maxY = qMax(p1.y(), p2.y());

    qreal w = maxX - minX;
    qreal h = maxY - minY;

    size_ = QSizeF(w, h);
    rect_.setTopLeft(QPointF(0, 0));
    rect_.setBottomRight(QPointF(w, h));
}

QT_END_NAMESPACE
