/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativerectanglemapitem_p.h"
#include <QPainter>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass MapRectangle QDeclarativeRectangleMapItem
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-maps
    \since QtLocation 5.0

    \brief The MapRectangle element displays a rectangle on a Map.

    The MapRectangle element displays a rectangle on a Map. Rectangles are a
    special case of Polygon with exactly 4 vertices and "straight" sides. In
    this case, "straight" means that the top-left and top-right points (the
    top side) have the same latitude, the points on the left side have the
    same longitude, and the points on the right side have another.

    To specify the rectangle, it requires a \l topLeft and \l bottomRight point,
    both given by \l Coordinate elements.

    By default, the rectangle is displayed with transparent fill and a 1-pixel
    thick black border. This can be changed using the \l color, \l border.color
    and \l border.width properties.

    \section2 Performance

    MapRectangles have a rendering cost identical to a MapPolygon with 4
    vertices.

    Like the other map objects, MapRectangle is normally drawn without a smooth
    appearance. Setting the \l opacity property will force the object to be
    blended, which decreases performance considerably depending on the hardware
    in use.

    \section2 Example Usage

    The following snippet shows a map containing a MapRectangle, spanning
    from (-27, 153) to (-28, 153.5), near Brisbane, Australia. The rectangle
    is filled in green, with a 2 pixel black border.

    \code
    Map {
        MapRectangle {
            color: 'green'
            border.width: 2
            topLeft: Coordinate { latitude: -27; longitude: 153 }
            bottomRight: Coordinate { latitude: -28; longitude: 153.5 }
        }
    }
    \endcode

    \image ../../../doc/src/images/api-maprectangle.png
*/

struct Vertex
{
    QVector2D position;
};

QDeclarativeRectangleMapItem::QDeclarativeRectangleMapItem(QQuickItem *parent):
    QDeclarativeGeoMapItemBase(parent),
    topLeft_(0),
    bottomRight_(0),
    color_(Qt::transparent),
    zoomLevel_(0.0),
    dirtyGeometry_(true),
    dirtyMaterial_(true)
{
    setFlag(ItemHasContents, true);
    QObject::connect(&border_, SIGNAL(colorChanged(QColor)),
                     this, SLOT(updateMapItemAssumeDirty()));
    QObject::connect(&border_, SIGNAL(widthChanged(qreal)),
                     this, SLOT(updateMapItemAssumeDirty()));
}

QDeclarativeRectangleMapItem::~QDeclarativeRectangleMapItem()
{
}

void QDeclarativeRectangleMapItem::setMap(QDeclarativeGeoMap* quickMap, QGeoMap *map)
{
    QDeclarativeGeoMapItemBase::setMap(quickMap,map);
    if (map) {
        QObject::connect(map, SIGNAL(cameraDataChanged(QGeoCameraData)),this, SLOT(handleCameraDataChanged(QGeoCameraData)));
        dirtyGeometry_ = true;
        updateMapItem();
    }
}

/*!
    \qmlproperty int MapRectangle::border.width
    \qmlproperty color MapRectangle::border.color

    These properties hold the width and color used to draw the border of the rectangle.
    The width is in pixels and is independent of the zoom level of the map.

    The default values correspond to a black border with a width of 1 pixel.
    For no line, use a width of 0 or a transparent color.
*/
QDeclarativeMapLineProperties *QDeclarativeRectangleMapItem::border()
{
    return &border_;
}

/*!
    \qmlproperty Coordinate MapRectangle::topLeft

    Holds the top-left coordinate of the MapRectangle.
*/
void QDeclarativeRectangleMapItem::setTopLeft(QDeclarativeCoordinate *topLeft)
{
    if (topLeft_ == topLeft)
        return;
    if (topLeft_)
        topLeft_->disconnect(this);
    topLeft_ = topLeft;

    if (topLeft_) {
        connect(topLeft_, SIGNAL(coordinateChanged(QGeoCoordinate)),
                this, SLOT(updateMapItemAssumeDirty()));
    }
    dirtyGeometry_ = true;
    updateMapItem();
    emit topLeftChanged(topLeft_);
}

QDeclarativeCoordinate* QDeclarativeRectangleMapItem::topLeft()
{
    return topLeft_;
}

void QDeclarativeRectangleMapItem::updateMapItemAssumeDirty()
{
    dirtyGeometry_ = true;
    updateMapItem();
}

/*!
    \qmlproperty Coordinate MapRectangle::bottomRight

    Holds the bottom-right coordinate of the MapRectangle.
*/
void QDeclarativeRectangleMapItem::setBottomRight(QDeclarativeCoordinate *bottomRight)
{
    if (bottomRight_ == bottomRight)
        return;
    if (bottomRight_)
        bottomRight_->disconnect(this);
    bottomRight_ = bottomRight;
    if (bottomRight_) {
        connect(bottomRight_, SIGNAL(coordinateChanged(QGeoCoordinate)),
                this, SLOT(updateMapItemAssumeDirty()));
    }
    dirtyGeometry_ = true;
    updateMapItem();
    emit bottomRightChanged(bottomRight_);
}

QDeclarativeCoordinate* QDeclarativeRectangleMapItem::bottomRight()
{
    return bottomRight_;
}

/*!
    \qmlproperty color MapRectangle::color

    This property holds the fill color of the rectangle. For no fill, use
    a transparent color.
*/
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

        setWidth(w);
        setHeight(h);

        QList<QGeoCoordinate> pathClosed;
        const double lonW = qAbs(bottomRight_->longitude() - topLeft_->longitude());
        const double latH = qAbs(bottomRight_->latitude() - topLeft_->latitude());
        pathClosed << topLeft_->coordinate();
        pathClosed << QGeoCoordinate(topLeft_->latitude(),
                                     topLeft_->longitude() + lonW);
        pathClosed << QGeoCoordinate(topLeft_->latitude() + latH,
                                     topLeft_->longitude() + lonW);
        pathClosed << QGeoCoordinate(topLeft_->latitude() + latH,
                                     topLeft_->longitude());
        pathClosed << pathClosed.first();

        QPolygonF newBorderPoly;
        QPainterPath outline;
        QPointF offset;
        QDeclarativePolylineMapItem::updatePolyline(newBorderPoly, *map(),
                                                    pathClosed, w, h,
                                                    border_.width(),
                                                    outline, offset);
        if (newBorderPoly.size() > 0) {
            newBorderPoly.translate(-1*offset);
            borderPoly_ = newBorderPoly;
        }
    }

    //TODO: add AnchorCoordiante
    setPositionOnMap(topLeft_->coordinate(), QPointF(0, 0));
    update();
}

void QDeclarativeRectangleMapItem::handleCameraDataChanged(const QGeoCameraData& cameraData)
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
    QPointF newTopLeftPoint = QPointF(x(),y());
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
