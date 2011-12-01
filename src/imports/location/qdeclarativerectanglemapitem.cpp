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

QDeclarativeRectangleMapItem::QDeclarativeRectangleMapItem(QQuickItem *parent)
:   QDeclarativeGeoMapItemBase(parent), screenItem_(new QDeclarativeGeoMapScreenItem(this)),
    rectangleMapPaintedItem_(new RectangleMapPaintedItem(screenItem_)), topLeft_(0),
    bottomRight_(0)
{
}

QDeclarativeRectangleMapItem::~QDeclarativeRectangleMapItem()
{
}

void QDeclarativeRectangleMapItem::setTopLeft(QDeclarativeCoordinate *topLeft)
{
    if (topLeft_ == topLeft)
        return;
    if (topLeft_)
        topLeft_->disconnect(this);
    topLeft_ = topLeft;
    rectangleMapPaintedItem_->setTopLeft(topLeft_->coordinate());
    if (topLeft_) {
        connect(topLeft_, SIGNAL(latitudeChanged(double)), this,
            SLOT(handleTopLeftCoordinateChanged()));
        connect(topLeft_, SIGNAL(longitudeChanged(double)), this,
            SLOT(handleTopLeftCoordinateChanged()));
        connect(topLeft_, SIGNAL(altitudeChanged(double)), this,
            SLOT(handleTopLeftCoordinateChanged()));
    }
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
    rectangleMapPaintedItem_->setBottomRight(bottomRight_->coordinate());
    if (bottomRight_) {
        connect(bottomRight_, SIGNAL(latitudeChanged(double)), this,
            SLOT(handleBottomRightCoordinateChanged()));
        connect(bottomRight_, SIGNAL(longitudeChanged(double)), this,
            SLOT(handleBottomRightCoordinateChanged()));
        connect(bottomRight_, SIGNAL(altitudeChanged(double)), this,
            SLOT(handleBottomRightCoordinateChanged()));
    }
    emit bottomRightChanged(bottomRight);
}

QDeclarativeCoordinate* QDeclarativeRectangleMapItem::bottomRight()
{
    return bottomRight_;
}

void QDeclarativeRectangleMapItem::update()
{
    if (!map())
        return;

    screenItem_->setCoordinate(
        new QDeclarativeCoordinate(rectangleMapPaintedItem_->screenItemCoordinate()));
    screenItem_->setAnchorPoint(rectangleMapPaintedItem_->screenItemAnchorPoint());

    if (screenItem_->sourceItem() == 0) {
        QObject::connect(map(), SIGNAL(cameraDataChanged(CameraData)), this,
            SLOT(handleCameraDataChanged(CameraData)));
        rectangleMapPaintedItem_->setMap(map());
        screenItem_->setMap(quickMap(), map());
        screenItem_->setSourceItem(rectangleMapPaintedItem_);
    }
}

void QDeclarativeRectangleMapItem::handleCameraDataChanged(const CameraData& cameraData)
{
    rectangleMapPaintedItem_->setZoomLevel(cameraData.zoomFactor());
    update();
}

void QDeclarativeRectangleMapItem::handleTopLeftCoordinateChanged()
{
    rectangleMapPaintedItem_->setTopLeft(topLeft_->coordinate());
    emit topLeftChanged(topLeft_);
}

void QDeclarativeRectangleMapItem::handleBottomRightCoordinateChanged()
{
    rectangleMapPaintedItem_->setBottomRight(bottomRight_->coordinate());
    emit bottomRightChanged(bottomRight_);
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
    QBrush m_brush(color);
    rectangleMapPaintedItem_->setBrush(m_brush);
    emit colorChanged(color_);
}

//////////////////////////////////////////////////////////////////////

RectangleMapPaintedItem::RectangleMapPaintedItem(QQuickItem *parent) :
    QQuickPaintedItem(parent), map_(0), zoomLevel_(-1), initialized_(false)
{
    setAntialiasing(true);
    connect(this, SIGNAL(xChanged()), this, SLOT(update()));
    connect(this, SIGNAL(yChanged()), this, SLOT(update()));
}

RectangleMapPaintedItem::~RectangleMapPaintedItem()
{
}

void RectangleMapPaintedItem::setMap(Map* map)
{
    map_ = map;
}

void RectangleMapPaintedItem::setZoomLevel(qreal zoomLevel)
{
    if (zoomLevel_ == zoomLevel)
        return;

    zoomLevel_ = zoomLevel;
    updateGeometry();
}

qreal RectangleMapPaintedItem::zoomLevel() const
{
    return zoomLevel_;
}

void RectangleMapPaintedItem::setTopLeft(const QGeoCoordinate &topLeftCoord)
{
    if (topLeftCoord_ == topLeftCoord)
        return;

    topLeftCoord_ = topLeftCoord;
    updateGeometry();
}

QGeoCoordinate RectangleMapPaintedItem::topLeft() const
{
    return topLeftCoord_;
}

void RectangleMapPaintedItem::setBottomRight(const QGeoCoordinate &bottomRightCoord)
{
    if (bottomRightCoord_ == bottomRightCoord)
        return;

    bottomRightCoord_ = bottomRightCoord;
    updateGeometry();
}

QGeoCoordinate RectangleMapPaintedItem::bottomRight() const
{
    return bottomRightCoord_;
}

void RectangleMapPaintedItem::paint(QPainter *painter)
{
    if (!initialized_)
        return;
    painter->setPen(pen_);
    painter->setBrush(brush_);
    painter->drawRect(rect_);
}

void RectangleMapPaintedItem::updateGeometry()
{
    initialized_ = false;

    if (!map_)
        return;

    if (!topLeftCoord_.isValid() || !bottomRightCoord_.isValid())
        return;

    if (zoomLevel_ == -1)
        return;

    QPointF p1 = map_->coordinateToScreenPosition(topLeftCoord_, false);
    QPointF p2 = map_->coordinateToScreenPosition(bottomRightCoord_, false);

    qreal minX = qMin(p1.x(), p2.x());
    qreal maxX = qMax(p1.x(), p2.x());
    qreal minY = qMin(p1.y(), p2.y());
    qreal maxY = qMax(p1.y(), p2.y());

    qreal w = maxX - minX;
    qreal h = maxY - minY;

    setWidth(w);
    setHeight(h);
    setContentsSize(QSize(w, h));

    rect_.setTopLeft(QPointF(0, 0));
    rect_.setBottomRight(QPointF(w, h));

    screenItemCoordinate_ = topLeftCoord_;
    screenItemAnchorPoint_ = rect_.topLeft();
    initialized_ = true;

    update();
}

QGeoCoordinate RectangleMapPaintedItem::screenItemCoordinate() const
{
    return screenItemCoordinate_;
}

QPointF RectangleMapPaintedItem::screenItemAnchorPoint() const
{
    return screenItemAnchorPoint_;
}

void RectangleMapPaintedItem::setBrush(const QBrush &brush)
{
    brush_ = brush;
}

QBrush RectangleMapPaintedItem::brush() const
{
    return brush_;
}

void RectangleMapPaintedItem::setPen(const QPen &pen)
{
    pen_ = pen;
}

QPen RectangleMapPaintedItem::pen() const
{
    return pen_;
}
