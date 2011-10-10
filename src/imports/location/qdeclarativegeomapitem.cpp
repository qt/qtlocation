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

#include "qdeclarativegeomapitem_p.h"
#include "qdeclarativecoordinate_p.h"
#include <QtDeclarative/qdeclarativeinfo.h>

#ifndef QSGSHADEREFFECTSOURCE_AVAILABLE

#else

#include <QDebug>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass MapItem

    The MapItem element is part of the \bold{QtLocation 5.0} module.
*/

class QDeclarativeGeoMapItemNode : public QSGNode
{
public:
    QDeclarativeGeoMapItemNode(QDeclarativeGeoMapItem* item) :
        item_(item)
    {
        Q_ASSERT(item);
        setFlag(UsePreprocess, true);
    }
    ~QDeclarativeGeoMapItemNode() {}
    virtual void preprocess() {
        QSGTextureProvider *provider = item_->shaderSource_->textureProvider();
        if (QSGShaderEffectTexture *texture = qobject_cast<QSGShaderEffectTexture *>(provider->texture()))
            texture->updateTexture();
    }
private:
    QDeclarativeGeoMapItem* item_;
};

QDeclarativeGeoMapItem::QDeclarativeGeoMapItem(QSGItem *parent)
    : QSGItem(parent),
      shaderSource_(0),
      sourceItem_(0),
      coordinate_(0),
      map_(0),
      componentCompleted_(false)
{
    setFlag(ItemHasContents);
    shaderSource_ = new QSGShaderEffectSource(this);
    shaderSource_->setParentItem(this);
}

QDeclarativeGeoMapItem::~QDeclarativeGeoMapItem()
{
    // Remove this item from map if it still exists (qpointer protection todo).
    if (map_)
        map_->removeMapItem(this);
}

QSGNode* QDeclarativeGeoMapItem::updatePaintNode(QSGNode* node, UpdatePaintNodeData* data)
{
    if (!node)
        node = new QDeclarativeGeoMapItemNode(this);
    return node;
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
    if (coordinate_)
        coordinate_->disconnect(this);
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

// note: call this only from render thread (e.g. paint() call) !
void QDeclarativeGeoMapItem::setMap(QDeclarative3DGraphicsGeoMap* map)
{
    if (map == map_)
        return;
    map_ = map;
    if (!map_) {
        // todo we may want to set parent item as zero too
        setParentItem(0);
        shaderSource_->setSourceItem(0);
        mapItem_.setTextureId(0); // invalidate
    } else {
        // QSG -parent ourselves - it glues us to QSG tree and
        // essentially enables rendering. This is particularly important to
        // do manually when instantiating items from models.
        setParent(map_);
        setParentItem(map_);
        if (sourceItem_)
            mapItem_.setSize(QSize(sourceItem_->width(), sourceItem_->height()));
    }
    if (map_ && map_->canvas_) {
        connect(shaderSource_, SIGNAL(sourceItemChanged()), this, SIGNAL(sourceItemChanged()));
        if (shaderSource_->textureProvider())
            connect(shaderSource_->textureProvider(), SIGNAL(textureChanged()), this, SLOT(textureChangedSlot()), Qt::DirectConnection);
    }
}

void QDeclarativeGeoMapItem::textureChangedSlot()
{
    if (!shaderSource_->textureProvider()->texture()) {
        mapItem_.setTextureId(0); // invalidate
        return;
    }
    QLOC_TRACE2("new texture ID: ", shaderSource_->textureProvider()->texture()->textureId());
    mapItem_.setTextureId(shaderSource_->textureProvider()->texture()->textureId());
    mapItem_.setSize(shaderSource_->textureProvider()->texture()->textureSize());
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
    if (!mapItem_.textureId() &&
            shaderSource_->textureProvider() &&
            shaderSource_->textureProvider()->texture()) {
        QLOC_TRACE2("changing/setting map item texture ID to: ", shaderSource_->textureProvider()->texture()->textureId());
        mapItem_.setTextureId(shaderSource_->textureProvider()->texture()->textureId());
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
    QSGItem::componentComplete();
}

void QDeclarativeGeoMapItem::setSourceItem(QSGItem* sourceItem)
{
    if (sourceItem == sourceItem_)
        return;
    sourceItem_ = sourceItem;
    shaderSource_->setSourceItem(sourceItem_);
    if (sourceItem_)
        mapItem_.setSize(QSize(sourceItem_->width(), sourceItem_->height()));
    else
        mapItem_.setSize(QSize(0,0)); // map item should be removed (fixme)
}

QSGItem* QDeclarativeGeoMapItem::sourceItem()
{
    if (!map_)
        return sourceItem_;
    return shaderSource_->sourceItem();
}
#endif

#include "moc_qdeclarativegeomapitem_p.cpp"

QT_END_NAMESPACE
