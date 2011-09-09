/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativegeomapitem_p.h"
#include "qdeclarativecoordinate_p.h"
#include <QtDeclarative/qdeclarativeinfo.h>

#ifndef QSGSHADEREFFECTSOURCE_AVAILABLE

#else

#include <QDebug>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass MapItem

    The MapItem element is part of the \bold{Qt.location 5.0} module.
*/

QDeclarativeGeoMapItem::QDeclarativeGeoMapItem(QSGItem *parent)
    : QSGShaderEffectSource(parent),
      sourceItem_(0),
      coordinate_(0),
      map_(0),
      componentCompleted_(false)
{
    connect(this, SIGNAL(sourceItemChanged()), this, SIGNAL(sourceChanged()));
    connect(this->texture(), SIGNAL(textureChanged()), this, SLOT(textureChangedSlot()));
}

QDeclarativeGeoMapItem::~QDeclarativeGeoMapItem()
{
    // Remove this item from map if it still exists (qpointer protection todo).
    if (map_)
        map_->removeMapItem(this);
}

void QDeclarativeGeoMapItem::textureChangedSlot()
{
    if (!texture()) {
        mapItem_.setTextureId(0); // invalidate
        return;
    }
    mapItem_.setTextureId(texture()->textureId());
    mapItem_.setSize(texture()->textureSize());
}

MapItem* QDeclarativeGeoMapItem::mapItem()
{
    return &mapItem_;
}

void QDeclarativeGeoMapItem::setZoomLevel(double zoom)
{
    if (zoom == mapItem_.zoom())
        return;
    mapItem_.setZoom(zoom);
    emit zoomLevelChanged();
}

double QDeclarativeGeoMapItem::zoomLevel()
{
    return mapItem_.zoom();
}

bool QDeclarativeGeoMapItem::hasValidTexture()
{
    if (mapItem_.textureId())
        return true;
    return false;
}

void QDeclarativeGeoMapItem::setCoordinate(QDeclarativeCoordinate *coordinate)
{
    if (coordinate_ == coordinate)
        return;
    if (coordinate_) {
        coordinate_->disconnect(this);
    }
    coordinate_ = coordinate;
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
        mapItem_.setCoordinate(coordinate->coordinate());
    } else {
        mapItem_.setCoordinate(QGeoCoordinate());
    }
    emit coordinateChanged();
}

void QDeclarativeGeoMapItem::setMap(QDeclarative3DGraphicsGeoMap* map)
{
    if (map == map_) {
        return;
    }
    map_ = map;
    if (!map_) {
        // todo we may want to set parent item as zero too
        setSourceItem(0);
        mapItem_.setTextureId(0); // invalidate
    } else {
        // Essential to QSG -parent ourselves - it glues us to QSG tree and
        // essentially enables rendering. This is particularly important to
        // do manually when instantiating items from models.

        setParent(map_);
        setParentItem(map_);
        setSourceItem(sourceItem_);
    }
}

void QDeclarativeGeoMapItem::coordinateCoordinateChanged(double)
{
    mapItem_.setCoordinate(coordinate_->coordinate());
    emit coordinateChanged();
}

void QDeclarativeGeoMapItem::updateItem()
{
    // we seem to not always get texture changed -signals when
    // adding static map items with add
    if (!mapItem_.textureId() && texture()) {
        mapItem_.setTextureId(texture()->textureId());
    }
    mapItem_.update();
}

QDeclarativeCoordinate* QDeclarativeGeoMapItem::coordinate()
{
    return coordinate_;
}

void QDeclarativeGeoMapItem::componentComplete()
{
    componentCompleted_ = true;
    QSGShaderEffectSource::componentComplete();
}

// This will enable us to intercept the setting of the source item
// if we need to intercept our own Map graphical elements like route.
// Also this way we can handle addition and removal of this item to map.
void QDeclarativeGeoMapItem::setSource(QSGItem* source)
{
    if (source == sourceItem_)
        return;
    sourceItem_ = source;
    if (map_)
        setSourceItem(sourceItem_);
    if (sourceItem_) {
        mapItem_.setSize(QSize(sourceItem_->width(), sourceItem_->height()));
    }
    emit sourceChanged();
}

QSGItem* QDeclarativeGeoMapItem::source()
{
    if (!map_)
        return sourceItem_;
    return sourceItem();
}
#endif

#include "moc_qdeclarativegeomapitem_p.cpp"

QT_END_NAMESPACE
