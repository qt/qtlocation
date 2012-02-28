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

#include "qdeclarativerectanglemapitem_p.h"
#include "qdeclarativepolygonmapitem_p.h"
#include "qgeocameracapabilities_p.h"
#include <QPainterPath>
#include <qnumeric.h>
#include <QRectF>
#include <QPointF>

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

QGeoMapRectangleGeometry::QGeoMapRectangleGeometry(QObject *parent) :
    QGeoMapItemGeometry(parent)
{
}

void QGeoMapRectangleGeometry::updatePoints(const QGeoMap &map,
                                            const QGeoCoordinate &topLeft,
                                            const QGeoCoordinate &bottomRight)
{
    if (!screenDirty_ && !sourceDirty_)
        return;

    QPointF tl = map.coordinateToScreenPosition(topLeft, false);
    QPointF br = map.coordinateToScreenPosition(bottomRight, false);

    // We can get NaN if the map isn't set up correctly, or the projection
    // is faulty -- probably best thing to do is abort
    if (!qIsFinite(tl.x()) || !qIsFinite(tl.y()))
        return;
    if (!qIsFinite(br.x()) || !qIsFinite(br.y()))
        return;

    QRectF re(tl, br);
    re.translate(-1 * tl);

    screenTriangles_.clear();
    screenTriangles_.reserve(6);

    screenTriangles_ << re.topLeft().x() << re.topLeft().y();
    screenTriangles_ << re.topRight().x() << re.topRight().y();
    screenTriangles_ << re.bottomLeft().x() << re.bottomLeft().y();

    screenTriangles_ << re.topRight().x() << re.topRight().y();
    screenTriangles_ << re.bottomLeft().x() << re.bottomLeft().y();
    screenTriangles_ << re.bottomRight().x() << re.bottomRight().y();

    firstPointOffset_ = QPointF(0,0);
    srcOrigin_ = topLeft;
    screenBounds_ = re;

    screenOutline_ = QPainterPath();
    screenOutline_.addRect(re);
}

QDeclarativeRectangleMapItem::QDeclarativeRectangleMapItem(QQuickItem *parent):
    QDeclarativeGeoMapItemBase(parent),
    topLeft_(0),
    bottomRight_(0),
    color_(Qt::transparent),
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
        geometry_.markSourceDirty();
        borderGeometry_.markSourceDirty();
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
    geometry_.markSourceDirty();
    borderGeometry_.markSourceDirty();
    updateMapItem();
    emit topLeftChanged(topLeft_);
}

QDeclarativeCoordinate* QDeclarativeRectangleMapItem::topLeft()
{
    return topLeft_;
}

void QDeclarativeRectangleMapItem::updateMapItemAssumeDirty()
{
    geometry_.markSourceDirty();
    borderGeometry_.markSourceDirty();
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
    geometry_.markSourceDirty();
    borderGeometry_.markSourceDirty();
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

/*!
  \qmlproperty real MapRectangle::opacity

  This property holds the opacity of the item.  Opacity is specified as a
  number between 0 (fully transparent) and 1 (fully opaque).  The default is 1.

  If an item's opacity is set to 0, the item will no longer receive mouse
  events. In contrast, setting the visible property to false stops mouse events.
*/
QSGNode* QDeclarativeRectangleMapItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    Q_UNUSED(data);

    MapPolygonNode *node = static_cast<MapPolygonNode*>(oldNode);

    if (!node) {
        node = new MapPolygonNode();
    }

    //TODO: update only material
    if (geometry_.isScreenDirty() || borderGeometry_.isScreenDirty() || dirtyMaterial_) {
        node->update(color_, border_.color(), &geometry_, &borderGeometry_);
        geometry_.markClean();
        borderGeometry_.markClean();
        dirtyMaterial_ = false;
    }
    return node;
}

void QDeclarativeRectangleMapItem::updateMapItem()
{
    if (!map() || !topLeft() || !topLeft()->coordinate().isValid()
            || !bottomRight() || !bottomRight()->coordinate().isValid())
        return;

    geometry_.updatePoints(*map(), topLeft_->coordinate(),
                           bottomRight_->coordinate());

    QList<QGeoCoordinate> pathClosed;
    const double lonW = qAbs(bottomRight_->longitude() - topLeft_->longitude());
    const double latH = qAbs(bottomRight_->latitude() - topLeft_->latitude());
    pathClosed << topLeft_->coordinate();
    pathClosed << QGeoCoordinate(topLeft_->latitude(),
                                 topLeft_->longitude() + lonW);
    pathClosed << QGeoCoordinate(topLeft_->latitude() - latH,
                                 topLeft_->longitude() + lonW);
    pathClosed << QGeoCoordinate(topLeft_->latitude() - latH,
                                 topLeft_->longitude());
    pathClosed << pathClosed.first();

    if (border_.color() != Qt::transparent && border_.width() > 0) {
        borderGeometry_.updateSourcePoints(*map(), pathClosed);
        borderGeometry_.updateScreenPoints(*map(), border_.width());

        QList<QGeoMapItemGeometry*> geoms;
        geoms << &geometry_ << &borderGeometry_;
        QRectF combined = QGeoMapItemGeometry::translateToCommonOrigin(geoms);

        setWidth(combined.width());
        setHeight(combined.height());
    } else {
        borderGeometry_.clear();

        setWidth(geometry_.screenBoundingBox().width());
        setHeight(geometry_.screenBoundingBox().height());
    }

    setPositionOnMap(pathClosed.at(0), geometry_.firstPointOffset());
    update();
}

void QDeclarativeRectangleMapItem::afterViewportChanged(const QGeoMapViewportChangeEvent &event)
{
    // if the scene is tilted, we must regenerate our geometry every frame
    if (map()->cameraCapabilities().supportsTilting()
            && (event.cameraData.tilt() > 0.1
                || event.cameraData.tilt() < -0.1)) {
        geometry_.markSourceDirty();
        borderGeometry_.markSourceDirty();
    }

    // if the scene is rolled, we must regen too
    if (map()->cameraCapabilities().supportsRolling()
            && (event.cameraData.roll() > 0.1
                || event.cameraData.roll() < -0.1)) {
        geometry_.markSourceDirty();
        borderGeometry_.markSourceDirty();
    }

    // otherwise, only regen on rotate, resize and zoom
    if (event.bearingChanged || event.mapSizeChanged || event.zoomLevelChanged) {
        geometry_.markSourceDirty();
        borderGeometry_.markSourceDirty();
    }

    geometry_.markScreenDirty();
    borderGeometry_.markScreenDirty();
    updateMapItem();
}

bool QDeclarativeRectangleMapItem::contains(QPointF point)
{
    return (geometry_.contains(point) || borderGeometry_.contains(point));
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

QT_END_NAMESPACE
