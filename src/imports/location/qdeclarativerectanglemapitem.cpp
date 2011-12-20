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
    topLeft_(0),
    bottomRight_(0),
    zoomLevel_(0.0),
    dirtyGeometry_(true),
    dirtyMaterial_(true),
    dragActive_(false)
{
    setFlag(ItemHasContents, true);
    QObject::connect(&border_, SIGNAL(colorChanged(QColor)),
                     this, SLOT(handleBorderUpdated()));
    QObject::connect(&border_, SIGNAL(widthChanged(qreal)),
                     this, SLOT(handleBorderUpdated()));
}

QDeclarativeRectangleMapItem::~QDeclarativeRectangleMapItem()
{
}

void QDeclarativeRectangleMapItem::setMap(QDeclarativeGeoMap* quickMap, Map *map)
{
    QDeclarativeGeoMapItemBase::setMap(quickMap,map);
    if (map) {
        QObject::connect(map, SIGNAL(cameraDataChanged(CameraData)), this, SLOT(handleCameraDataChanged(CameraData)));
        updateMapItem();
    }
}

QDeclarativeMapLineProperties *QDeclarativeRectangleMapItem::border()
{
    return &border_;
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
    dirtyGeometry_ = true;
    updateMapItem();
    emit topLeftChanged(topLeft_);
}

QDeclarativeCoordinate* QDeclarativeRectangleMapItem::topLeft()
{
    return topLeft_;
}

void QDeclarativeRectangleMapItem::handleBorderUpdated()
{
    dirtyGeometry_ = true;
    updateMapItem();
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
    dirtyGeometry_ = true;
    updateMapItem();
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
    dirtyMaterial_ = true;
    updateMapItem();
    emit colorChanged(color_);
}

QSGNode* QDeclarativeRectangleMapItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    Q_UNUSED(data);

    MapRectangleNode *node = static_cast<MapRectangleNode*>(oldNode);

    if (!node) {
        node = new MapRectangleNode();
    }

    //TODO: update only material
    if (dirtyGeometry_ || dirtyMaterial_) {
        node->update(color_, rectangle_, borderPoly_, border_.color(), border_.width());
        dirtyGeometry_ = false;
        dirtyMaterial_ = false;
    }
    return node;
}

void QDeclarativeRectangleMapItem::updateMapItem()
{
    if (!map() || !topLeft() || !topLeft()->coordinate().isValid()
            || !bottomRight() || !bottomRight()->coordinate().isValid())
        return;

    if (dirtyGeometry_) {
        QPointF p1 = map()->coordinateToScreenPosition(topLeft_->coordinate(), false);
        QPointF p2 = map()->coordinateToScreenPosition(bottomRight_->coordinate(), false);

        qreal minX = qMin(p1.x(), p2.x());
        qreal maxX = qMax(p1.x(), p2.x());
        qreal minY = qMin(p1.y(), p2.y());
        qreal maxY = qMax(p1.y(), p2.y());

        qreal w = maxX - minX;
        qreal h = maxY - minY;

        rectangle_.setTopLeft(QPointF(0, 0));
        rectangle_.setBottomRight(QPointF(w, h));

        borderPoly_.clear();
        borderPoly_.append(QPointF(0, 0));
        borderPoly_.append(QPointF(w, 0));
        borderPoly_.append(QPointF(w, h));
        borderPoly_.append(QPointF(0, h));
        borderPoly_.append(borderPoly_.at(0));

        setWidth(w);
        setHeight(h);
    }

    //TODO: add AnchorCoordiante
    setPositionOnMap(topLeft_->coordinate(), QPointF(0, 0));
    update();
}

void QDeclarativeRectangleMapItem::handleCameraDataChanged(const CameraData& cameraData)
{
    if (cameraData.zoomFactor() != zoomLevel_) {
        zoomLevel_ = cameraData.zoomFactor();
        dirtyGeometry_ = true;
    }
    updateMapItem();
}

bool QDeclarativeRectangleMapItem::contains(QPointF point)
{
    return rectangle_.contains(point);
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

//TODO: this is goingn to be removed by I466aa31d33a204b2f7c6c562025946371d3b6f46
void QDeclarativeRectangleMapItem::dragStarted()
{
    dragActive_ = true;
}
//////////////////////////////////////////////////////////////////////

MapRectangleNode::MapRectangleNode():
    geometry_(QSGGeometry::defaultAttributes_Point2D(),0),
    border_(new MapPolylineNode())
{
    geometry_.setDrawingMode(GL_TRIANGLE_STRIP);
    QSGGeometryNode::setMaterial(&fillMaterial_);
    QSGGeometryNode::setGeometry(&geometry_);

    appendChildNode(border_);
}

MapRectangleNode::~MapRectangleNode()
{
}

void MapRectangleNode::update(const QColor& fillColor, const QRectF& shape, const QPolygonF& borderPoly, const QColor& borderColor, qreal borderWidth)
{
    QSGGeometry *fill = QSGGeometryNode::geometry();

    Q_ASSERT(fill->sizeOfVertex() == sizeof(Vertex));

    int fillVertexCount = 0;
    //note this will not allocate new buffer if the size has not changed
    fill->allocate(4);

    Vertex *vertices = (Vertex *)fill->vertexData();

    //set corners
    vertices[fillVertexCount++].position = QVector2D(shape.left(), shape.top());
    vertices[fillVertexCount++].position = QVector2D(shape.right(),shape.top());
    vertices[fillVertexCount++].position = QVector2D(shape.left(),shape.bottom());
    vertices[fillVertexCount++].position = QVector2D(shape.right(), shape.bottom());

    Q_ASSERT(fillVertexCount == fill->vertexCount());

    markDirty(DirtyGeometry);

    if (fillColor != fillMaterial_.color()) {
        fillMaterial_.setColor(fillColor);
        setMaterial(&fillMaterial_);
    }

    border_->update(borderColor, borderPoly, borderWidth);
}

QT_END_NAMESPACE
