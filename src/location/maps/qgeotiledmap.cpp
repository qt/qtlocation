/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qgeotiledmap_p.h"
#include "qgeotiledmap_p_p.h"

#include "qgeotiledmappingmanagerengine_p.h"
#include "qgeotilecache_p.h"
#include "qgeotilespec_p.h"

#include "qgeocameratiles_p.h"
#include "qgeotilerequestmanager_p.h"
#include "qgeomapscene_p.h"
#include "qgeocameracapabilities_p.h"

#include <QMutex>
#include <QMap>

#include <qnumeric.h>

#include <QtPositioning/private/qgeoprojection_p.h>
#include <QtPositioning/private/qdoublevector2d_p.h>

#include <cmath>

QT_BEGIN_NAMESPACE


QGeoTiledMap::QGeoTiledMap(QGeoTiledMappingManagerEngine *engine, QObject *parent)
    : QGeoMap(engine, parent)
{
    d_ptr = new QGeoTiledMapPrivate(this, engine);
    engine->registerMap(this);

    connect(engine,
            SIGNAL(mapVersionChanged()),
            this,
            SLOT(updateMapVersion()));
    QMetaObject::invokeMethod(this, "updateMapVersion", Qt::QueuedConnection);
}

QGeoTiledMap::~QGeoTiledMap()
{
    if (d_ptr->engine()) // check if engine hasn't already been deleted
        d_ptr->engine().data()->deregisterMap(this);
    delete d_ptr;
}
QGeoTileRequestManager *QGeoTiledMap::getRequestManager()
{
    Q_D(QGeoTiledMap);
    return d->tileRequests_;
}

void QGeoTiledMap::newTileFetched(const QGeoTileSpec &spec)
{
    Q_D(QGeoTiledMap);
    d->newTileFetched(spec);
}

QGeoTileCache *QGeoTiledMap::tileCache()
{
    Q_D(QGeoTiledMap);
    return d->tileCache();
}

QSGNode *QGeoTiledMap::updateSceneGraph(QSGNode *oldNode, QQuickWindow *window)
{
    Q_D(QGeoTiledMap);
    return d->updateSceneGraph(oldNode, window);
}

void QGeoTiledMap::mapResized(int width, int height)
{
    Q_D(QGeoTiledMap);
    d->resized(width, height);
    evaluateCopyrights(d->visibleTiles());
}

void QGeoTiledMap::changeCameraData(const QGeoCameraData &oldCameraData)
{
    Q_D(QGeoTiledMap);
    d->changeCameraData(oldCameraData);
}

void QGeoTiledMap::prefetchData()
{
    Q_D(QGeoTiledMap);
    d->prefetchTiles();
}

void QGeoTiledMap::changeActiveMapType(const QGeoMapType mapType)
{
    Q_D(QGeoTiledMap);
    d->changeActiveMapType(mapType);
}

int QGeoTiledMap::mapVersion()
{
    return -1;
}

void QGeoTiledMap::updateMapVersion()
{
    Q_D(QGeoTiledMap);
    d->changeMapVersion(mapVersion());
}

void QGeoTiledMap::evaluateCopyrights(const QSet<QGeoTileSpec> &visibleTiles)
{
    Q_UNUSED(visibleTiles);
}

QGeoCoordinate QGeoTiledMap::itemPositionToCoordinate(const QDoubleVector2D &pos, bool clipToViewport) const
{
    Q_D(const QGeoTiledMap);
    if (clipToViewport) {
        int w = width();
        int h = height();

        if ((pos.x() < 0) || (w < pos.x()) || (pos.y() < 0) || (h < pos.y()))
            return QGeoCoordinate();
    }

    return d->itemPositionToCoordinate(pos);
}

QDoubleVector2D QGeoTiledMap::coordinateToItemPosition(const QGeoCoordinate &coordinate, bool clipToViewport) const
{
    Q_D(const QGeoTiledMap);
    QDoubleVector2D pos = d->coordinateToItemPosition(coordinate);

    if (clipToViewport) {
        int w = width();
        int h = height();

        if ((pos.x() < 0) || (w < pos.x()) || (pos.y() < 0) || (h < pos.y()))
            return QDoubleVector2D(qQNaN(), qQNaN());
    }

    return pos;
}

QGeoTiledMapPrivate::QGeoTiledMapPrivate(QGeoTiledMap *parent, QGeoTiledMappingManagerEngine *engine)
    : map_(parent),
      cache_(engine->tileCache()),
      engine_(engine),
      cameraTiles_(new QGeoCameraTiles()),
      mapScene_(new QGeoMapScene()),
      tileRequests_(new QGeoTileRequestManager(parent))
{
    cameraTiles_->setMaximumZoomLevel(static_cast<int>(std::ceil(engine->cameraCapabilities().maximumZoomLevel())));
    cameraTiles_->setTileSize(engine->tileSize().width());
    cameraTiles_->setPluginString(map_->pluginString());

    mapScene_->setTileSize(engine->tileSize().width());

    QObject::connect(mapScene_,
                     SIGNAL(newTilesVisible(QSet<QGeoTileSpec>)),
                     map_,
                     SLOT(evaluateCopyrights(QSet<QGeoTileSpec>)));
}

QGeoTiledMapPrivate::~QGeoTiledMapPrivate()
{
    // controller_ is a child of map_, don't need to delete it here

    delete tileRequests_;
    delete mapScene_;
    delete cameraTiles_;

    // TODO map items are not deallocated!
    // However: how to ensure this is done in rendering thread?
}

QGeoTileCache *QGeoTiledMapPrivate::tileCache()
{
    return cache_;
}

QPointer<QGeoTiledMappingManagerEngine> QGeoTiledMapPrivate::engine() const
{
    return engine_;
}

void QGeoTiledMapPrivate::prefetchTiles()
{
    cameraTiles_->findPrefetchTiles();

    if (tileRequests_)
        tileRequests_->requestTiles(cameraTiles_->tiles() - mapScene_->texturedTiles());
}

void QGeoTiledMapPrivate::changeCameraData(const QGeoCameraData &oldCameraData)
{
    double lat = oldCameraData.center().latitude();

    if (mapScene_->verticalLock()) {
        QGeoCoordinate coord = map_->cameraData().center();
        coord.setLatitude(lat);
        map_->cameraData().setCenter(coord);
    }

    // For zoomlevel, "snap" 0.05 either side of a whole number.
    // This is so that when we turn off bilinear scaling, we're
    // snapped to the exact pixel size of the tiles
    QGeoCameraData cam = map_->cameraData();
    int izl = static_cast<int>(std::floor(cam.zoomLevel()));
    float delta = cam.zoomLevel() - izl;
    if (delta > 0.5) {
        izl++;
        delta -= 1.0;
    }
    if (qAbs(delta) < 0.05) {
        cam.setZoomLevel(izl);
    }

    cameraTiles_->setCamera(cam);

    mapScene_->setCameraData(cam);
    mapScene_->setVisibleTiles(cameraTiles_->tiles());

    if (tileRequests_) {
        // don't request tiles that are already built and textured
        QList<QSharedPointer<QGeoTileTexture> > cachedTiles =
                tileRequests_->requestTiles(cameraTiles_->tiles() - mapScene_->texturedTiles());

        foreach (const QSharedPointer<QGeoTileTexture> &tex, cachedTiles) {
            mapScene_->addTile(tex->spec, tex);
        }

        if (!cachedTiles.isEmpty())
            map_->update();
    }
}

void QGeoTiledMapPrivate::changeActiveMapType(const QGeoMapType mapType)
{
    cameraTiles_->setMapType(mapType);
}

void QGeoTiledMapPrivate::changeMapVersion(int mapVersion)
{
    cameraTiles_->setMapVersion(mapVersion);
}

void QGeoTiledMapPrivate::resized(int width, int height)
{
    if (cameraTiles_)
        cameraTiles_->setScreenSize(QSize(width, height));
    if (mapScene_)
        mapScene_->setScreenSize(QSize(width, height));
    if (map_)
        map_->setCameraData(map_->cameraData());

    if (width > 0 && height > 0 && cache_ && cameraTiles_) {
        // absolute minimum size: one tile each side of display, 32-bit colour
        int texCacheSize = (width + cameraTiles_->tileSize() * 2) *
                (height + cameraTiles_->tileSize() * 2) * 4;

        // multiply by 3 so the 'recent' list in the cache is big enough for
        // an entire display of tiles
        texCacheSize *= 3;
        // TODO: move this reasoning into the tilecache

        int newSize = qMax(cache_->minTextureUsage(), texCacheSize);
        cache_->setMinTextureUsage(newSize);
    }
}

void QGeoTiledMapPrivate::newTileFetched(const QGeoTileSpec &spec)
{
    // Only promote the texture up to GPU if it is visible
    if (cameraTiles_->tiles().contains(spec)){
        QSharedPointer<QGeoTileTexture> tex = engine_.data()->getTileTexture(spec);
        if (tex) {
            mapScene_->addTile(spec, tex);
            map_->update();
        }
    }
}

QSet<QGeoTileSpec> QGeoTiledMapPrivate::visibleTiles()
{
    return cameraTiles_->tiles();
}

QSGNode *QGeoTiledMapPrivate::updateSceneGraph(QSGNode *oldNode, QQuickWindow *window)
{
    return mapScene_->updateSceneGraph(oldNode, window);
}

QGeoCoordinate QGeoTiledMapPrivate::itemPositionToCoordinate(const QDoubleVector2D &pos) const
{
    return QGeoProjection::mercatorToCoord(mapScene_->itemPositionToMercator(pos));
}

QDoubleVector2D QGeoTiledMapPrivate::coordinateToItemPosition(const QGeoCoordinate &coordinate) const
{
    return mapScene_->mercatorToItemPosition(QGeoProjection::coordToMercator(coordinate));
}

QT_END_NAMESPACE
