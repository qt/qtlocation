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

#include "qdeclarativegeomapscreenitem_p.h"
#include "qdeclarativecoordinate_p.h"
#include <QtDeclarative/qdeclarativeinfo.h>

#include <QDebug>

#include <cmath>

QT_BEGIN_NAMESPACE

QDeclarativeGeoMapItemBase::QDeclarativeGeoMapItemBase(QQuickItem *parent)
    : QQuickItem(parent),
      quickMap_(0),
      map_(0)
{
    setParentItem(parent);
    setFlag(ItemHasContents, false);
}

QDeclarativeGeoMapItemBase::~QDeclarativeGeoMapItemBase()
{
    if (quickMap_)
        quickMap_->removeMapScreenItem(this);
}

void QDeclarativeGeoMapItemBase::setMap(QDeclarativeGeoMap* quickMap, Map *map)
{
    QLOC_TRACE2(quickMap, quickMap_);
    if (quickMap == quickMap_)
        return;
    if (quickMap && quickMap_)
        return; // don't allow association to more than one map
    quickMap_ = quickMap;
    map_ = map;

    this->update();
}

void QDeclarativeGeoMapItemBase::update() {}

QDeclarativeGeoMap* QDeclarativeGeoMapItemBase::quickMap()
{
    return quickMap_;
}

Map* QDeclarativeGeoMapItemBase::map()
{
    return map_;
}

/*!
    \qmlclass MapScreenItem

    The MapScreenItem element is part of the \bold{QtLocation 5.0} module.
*/

QDeclarativeGeoMapScreenItem::QDeclarativeGeoMapScreenItem(QQuickItem *parent)
    : QDeclarativeGeoMapItemBase(parent),
      sourceItem_(0),
      coordinate_(0),
      zoomLevel_(0.0),
      inUpdate_(false),
      mapAndSourceItemSet_(false) {}

QDeclarativeGeoMapScreenItem::~QDeclarativeGeoMapScreenItem() {}

void QDeclarativeGeoMapScreenItem::setCoordinate(QDeclarativeCoordinate *coordinate)
{
    if (coordinate_ == coordinate)
        return;
    if (coordinate_)
        coordinate_->disconnect(this);
    coordinate_ = coordinate;
    update();
    if (coordinate_) {
        connect(coordinate_,
                SIGNAL(latitudeChanged(double)),
                this,
                SLOT(coordinateCoordinateChanged(double)));
        connect(coordinate_,
                SIGNAL(longitudeChanged(double)),
                this,
                SLOT(coordinateCoordinateChanged(double)));
        connect(coordinate_,
                SIGNAL(altitudeChanged(double)),
                this,
                SLOT(coordinateCoordinateChanged(double)));
    }
    emit coordinateChanged();
}

void QDeclarativeGeoMapScreenItem::coordinateCoordinateChanged(double)
{
    update();
    emit coordinateChanged();
}

QDeclarativeCoordinate* QDeclarativeGeoMapScreenItem::coordinate()
{
    return coordinate_;
}

void QDeclarativeGeoMapScreenItem::setSourceItem(QQuickItem* sourceItem)
{
    if (sourceItem == sourceItem_)
        return;
    sourceItem_ = sourceItem;
    update();
    emit sourceItemChanged();
}

QQuickItem* QDeclarativeGeoMapScreenItem::sourceItem()
{
    return sourceItem_;
}

void QDeclarativeGeoMapScreenItem::setAnchorPoint(const QPointF &anchorPoint)
{
    if (anchorPoint == anchorPoint_)
        return;
    anchorPoint_ = anchorPoint;
    update();
    emit anchorPointChanged();
}

QPointF QDeclarativeGeoMapScreenItem::anchorPoint() const
{
    return anchorPoint_;
}

void QDeclarativeGeoMapScreenItem::setZoomLevel(qreal zoomLevel)
{
    if (zoomLevel == zoomLevel_)
        return;
    zoomLevel_ = zoomLevel;
    update();
    emit zoomLevelChanged();
}

qreal QDeclarativeGeoMapScreenItem::zoomLevel() const
{
    return zoomLevel_;
}

void QDeclarativeGeoMapScreenItem::update()
{
    if (inUpdate_)
        return;

    if (!quickMap() && sourceItem_) {
        mapAndSourceItemSet_ = false;
        sourceItem_->setParentItem(0);
        return;
    }

    if (!quickMap() || !map() || !sourceItem_) {
        mapAndSourceItemSet_ = false;
        return;
    }

    inUpdate_ = true;

    if (!mapAndSourceItemSet_ && quickMap() && map() && sourceItem_) {
        mapAndSourceItemSet_ = true;

        sourceItem_->setParentItem(this);
        sourceItem_->setTransformOrigin(QQuickItem::TopLeft);
        connect(quickMap(), SIGNAL(heightChanged()), this, SLOT(update()));
        connect(quickMap(), SIGNAL(widthChanged()), this, SLOT(update()));
        connect(map(), SIGNAL(cameraDataChanged(CameraData)), this, SLOT(update()));
        connect(map(), SIGNAL(cameraDataChanged(CameraData)), this, SIGNAL(camerDataChanged(CameraData)));

        connect(sourceItem_, SIGNAL(xChanged()), this, SLOT(update()));
        connect(sourceItem_, SIGNAL(yChanged()), this, SLOT(update()));
        connect(sourceItem_, SIGNAL(widthChanged()), this, SLOT(update()));
        connect(sourceItem_, SIGNAL(heightChanged()), this, SLOT(update()));
    }

    qreal s = 1.0;
    if (zoomLevel_ != 0.0)
        s = pow(0.5, zoomLevel_ - map()->cameraData().zoomFactor());

    QPointF invalid = map()->coordinateToScreenPosition(QGeoCoordinate());
    QPointF topLeft = map()->coordinateToScreenPosition(coordinate()->coordinate(), false) - s * anchorPoint_;

    if ((topLeft.x() > quickMap()->width())
            || (topLeft.x() + s * sourceItem_->width() < 0)
            || (topLeft.y() + s * sourceItem_->height() < 0)
            || (topLeft.y() > quickMap()->height())) {
        sourceItem_->setPos(invalid);
    } else {
        sourceItem_->setPos(topLeft);
        sourceItem_->setScale(s);
    }

    inUpdate_ = false;
}

#include "moc_qdeclarativegeomapscreenitem_p.cpp"

QT_END_NAMESPACE
