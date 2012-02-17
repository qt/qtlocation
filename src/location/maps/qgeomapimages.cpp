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
#include "qgeomapimages_p.h"

#include "qgeotilespec.h"
#include "qgeotiledmapdata_p.h"
#include "qgeotiledmappingmanagerengine.h"
#include "qgeotilecache_p.h"

QT_BEGIN_NAMESPACE

class QGeoMapImagesPrivate
{
public:
    QGeoMapImagesPrivate(QGeoTiledMapData *map, QGeoTileCache *cache);
    ~QGeoMapImagesPrivate();

    QGeoTiledMapData *map_;
    QGeoTileCache *cache_;

    void setVisibleTiles(const QSet<QGeoTileSpec> &tiles);
    void tileFetched(const QGeoTileSpec &tile);

    QSet<QGeoTileSpec> visible_;
    QSet<QGeoTileSpec> cached_;
    QSet<QGeoTileSpec> requested_;
};

QGeoMapImages::QGeoMapImages(QGeoTiledMapData *map, QGeoTileCache *cache)
    : d_ptr(new QGeoMapImagesPrivate(map, cache)) {}

QGeoMapImages::~QGeoMapImages()
{
    delete d_ptr;
}

void QGeoMapImages::setVisibleTiles(const QSet<QGeoTileSpec> &tiles)
{
    Q_D(QGeoMapImages);
    d->setVisibleTiles(tiles);
}

QSet<QGeoTileSpec> QGeoMapImages::cachedTiles() const
{
    Q_D(const QGeoMapImages);
    return d->cached_;
}

void QGeoMapImages::tileFetched(const QGeoTileSpec &tile)
{
    Q_D(QGeoMapImages);
    d->tileFetched(tile);
}

QGeoMapImagesPrivate::QGeoMapImagesPrivate(QGeoTiledMapData *map, QGeoTileCache *cache)
    : map_(map),
      cache_(cache) {}

QGeoMapImagesPrivate::~QGeoMapImagesPrivate()
{
}

void QGeoMapImagesPrivate::setVisibleTiles(const QSet<QGeoTileSpec> &tiles)
{
    QSet<QGeoTileSpec> cancelTiles = requested_ - tiles;
    QSet<QGeoTileSpec> requestTiles = tiles - visible_ - requested_;

    cached_.clear();

    // remove tiles in cache from request tiles
    if (cache_) {
        typedef QSet<QGeoTileSpec>::const_iterator iter;
        iter i = requestTiles.constBegin();
        iter end = requestTiles.constEnd();
        for (; i != end; ++i) {
            QGeoTileSpec tile = *i;
            if (cache_->contains(tile))
                cached_.insert(tile);
        }
    }

    requestTiles -= cached_;

    visible_ = tiles;

    requested_ -= cancelTiles;
    requested_ += requestTiles;

    if (!requestTiles.isEmpty() || !cancelTiles.isEmpty()) {
        if (map_) {
            map_->updateTileRequests(requestTiles, cancelTiles);
        }
    }
}

void QGeoMapImagesPrivate::tileFetched(const QGeoTileSpec &tile)
{
    requested_.remove(tile);
}

QT_END_NAMESPACE
