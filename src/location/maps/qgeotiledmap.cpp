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
#include "qabstractgeotilecache_p.h"
#include "qgeotilespec_p.h"

#include "qgeocameratiles_p.h"
#include "qgeotilerequestmanager_p.h"
#include "qgeomapscene_p.h"
#include "qgeocameracapabilities_p.h"
#include <cmath>

QT_BEGIN_NAMESPACE
#define PREFETCH_FRUSTUM_SCALE 2.0

QGeoTiledMap::QGeoTiledMap(QGeoTiledMappingManagerEngine *engine, QObject *parent)
    : QGeoMap(*new QGeoTiledMapPrivate(engine), parent)
{
    Q_D(QGeoTiledMap);

    d->m_tileRequests = new QGeoTileRequestManager(this, engine);

    QObject::connect(engine,&QGeoTiledMappingManagerEngine::tileVersionChanged,
                     this,&QGeoTiledMap::handleTileVersionChanged);
}

QGeoTiledMap::~QGeoTiledMap()
{
    Q_D(QGeoTiledMap);
    delete d->m_tileRequests;
    d->m_tileRequests = 0;

    if (!d->m_engine.isNull()) {
        QGeoTiledMappingManagerEngine *engine = qobject_cast<QGeoTiledMappingManagerEngine*>(d->m_engine);
        Q_ASSERT(engine);
        engine->releaseMap(this);
    }
}

QGeoTileRequestManager *QGeoTiledMap::requestManager()
{
    Q_D(QGeoTiledMap);
    return d->m_tileRequests;
}

void QGeoTiledMap::updateTile(const QGeoTileSpec &spec)
{
    Q_D(QGeoTiledMap);
    d->updateTile(spec);
}

void QGeoTiledMap::setPrefetchStyle(QGeoTiledMap::PrefetchStyle style)
{
    Q_D(QGeoTiledMap);
    d->m_prefetchStyle = style;
}

QAbstractGeoTileCache *QGeoTiledMap::tileCache()
{
    Q_D(QGeoTiledMap);
    return d->m_cache;
}

QSGNode *QGeoTiledMap::updateSceneGraph(QSGNode *oldNode, QQuickWindow *window)
{
    Q_D(QGeoTiledMap);
    return d->updateSceneGraph(oldNode, window);
}

void QGeoTiledMap::prefetchData()
{
    Q_D(QGeoTiledMap);
    d->prefetchTiles();
}

void QGeoTiledMap::clearData()
{
    Q_D(QGeoTiledMap);
    d->m_cache->clearAll();
    d->m_mapScene->clearTexturedTiles();
}

void QGeoTiledMap::handleTileVersionChanged()
{
    Q_D(QGeoTiledMap);
    if (!d->m_engine.isNull()) {
        QGeoTiledMappingManagerEngine* engine = qobject_cast<QGeoTiledMappingManagerEngine*>(d->m_engine);
        Q_ASSERT(engine);
        d->changeTileVersion(engine->tileVersion());
    }
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

QGeoTiledMapPrivate::QGeoTiledMapPrivate(QGeoTiledMappingManagerEngine *engine)
    : QGeoMapPrivate(engine),
      m_cache(engine->tileCache()),
      m_visibleTiles(new QGeoCameraTiles()),
      m_prefetchTiles(new QGeoCameraTiles()),
      m_mapScene(new QGeoMapScene()),
      m_tileRequests(0),
      m_maxZoomLevel(static_cast<int>(std::ceil(engine->cameraCapabilities().maximumZoomLevel()))),
      m_minZoomLevel(static_cast<int>(std::ceil(engine->cameraCapabilities().minimumZoomLevel()))),
      m_prefetchStyle(QGeoTiledMap::PrefetchTwoNeighbourLayers)
{
    int tileSize = engine->tileSize().width();
    QString pluginString(engine->managerName() + QLatin1Char('_') + QString::number(engine->managerVersion()));
    m_visibleTiles->setTileSize(tileSize);
    m_prefetchTiles->setTileSize(tileSize);
    m_visibleTiles->setPluginString(pluginString);
    m_prefetchTiles->setPluginString(pluginString);
    m_mapScene->setTileSize(tileSize);
}

QGeoTiledMapPrivate::~QGeoTiledMapPrivate()
{
    // controller_ is a child of map_, don't need to delete it here

    delete m_mapScene;
    delete m_visibleTiles;
    delete m_prefetchTiles;

    // TODO map items are not deallocated!
    // However: how to ensure this is done in rendering thread?
}


void QGeoTiledMapPrivate::prefetchTiles()
{
    if (m_tileRequests) {

        QSet<QGeoTileSpec> tiles;
        QGeoCameraData camera = m_visibleTiles->cameraData();
        int currentIntZoom = static_cast<int>(std::floor(camera.zoomLevel()));

        m_prefetchTiles->setCameraData(camera);
        m_prefetchTiles->setViewExpansion(PREFETCH_FRUSTUM_SCALE);
        tiles = m_prefetchTiles->createTiles();

        switch (m_prefetchStyle) {

        case QGeoTiledMap::PrefetchNeighbourLayer: {
            double zoomFraction = camera.zoomLevel() - currentIntZoom;
            int nearestNeighbourLayer = zoomFraction > 0.5 ? currentIntZoom + 1 : currentIntZoom - 1;
            if (nearestNeighbourLayer <= m_maxZoomLevel && nearestNeighbourLayer >= m_minZoomLevel) {
                camera.setZoomLevel(nearestNeighbourLayer);
                // Approx heuristic, keeping total # prefetched tiles roughly independent of the
                // fractional zoom level.
                double neighbourScale = (1.0 + zoomFraction)/2.0;
                m_prefetchTiles->setCameraData(camera);
                m_prefetchTiles->setViewExpansion(PREFETCH_FRUSTUM_SCALE * neighbourScale);
                tiles += m_prefetchTiles->createTiles();
            }
        }
            break;

        case QGeoTiledMap::PrefetchTwoNeighbourLayers: {
            // This is a simpler strategy, we just prefetch from layer above and below
            // for the layer below we only use half the size as this fills the screen
            if (currentIntZoom > m_minZoomLevel) {
                camera.setZoomLevel(currentIntZoom - 1);
                m_prefetchTiles->setCameraData(camera);
                m_prefetchTiles->setViewExpansion(0.5);
                tiles += m_prefetchTiles->createTiles();
            }

            if (currentIntZoom < m_maxZoomLevel) {
                camera.setZoomLevel(currentIntZoom + 1);
                m_prefetchTiles->setCameraData(camera);
                m_prefetchTiles->setViewExpansion(1.0);
                tiles += m_prefetchTiles->createTiles();
            }

        }
        }

        m_tileRequests->requestTiles(tiles - m_mapScene->texturedTiles());
    }
}

void QGeoTiledMapPrivate::changeCameraData(const QGeoCameraData &oldCameraData)
{
    Q_Q(QGeoTiledMap);
    double lat = oldCameraData.center().latitude();

    if (m_mapScene->verticalLock()) {
        QGeoCoordinate coord = q->cameraData().center();
        coord.setLatitude(lat);
        q->cameraData().setCenter(coord);
    }

    // For zoomlevel, "snap" 0.05 either side of a whole number.
    // This is so that when we turn off bilinear scaling, we're
    // snapped to the exact pixel size of the tiles
    QGeoCameraData cam = q->cameraData();
    int izl = static_cast<int>(std::floor(cam.zoomLevel()));
    float delta = cam.zoomLevel() - izl;
    if (delta > 0.5) {
        izl++;
        delta -= 1.0;
    }
    if (qAbs(delta) < 0.05) {
        cam.setZoomLevel(izl);
    }

    m_visibleTiles->setCameraData(cam);
    m_mapScene->setCameraData(cam);
    updateScene();
}

void QGeoTiledMapPrivate::updateScene()
{
    Q_Q(QGeoTiledMap);
    // detect if new tiles introduced
    const QSet<QGeoTileSpec>& tiles = m_visibleTiles->createTiles();
    bool newTilesIntroduced = !m_mapScene->visibleTiles().contains(tiles);
    m_mapScene->setVisibleTiles(tiles);

    if (newTilesIntroduced)
        q->evaluateCopyrights(tiles);

    // don't request tiles that are already built and textured
    QList<QSharedPointer<QGeoTileTexture> > cachedTiles =
            m_tileRequests->requestTiles(m_visibleTiles->createTiles() - m_mapScene->texturedTiles());

    foreach (const QSharedPointer<QGeoTileTexture> &tex, cachedTiles) {
        m_mapScene->addTile(tex->spec, tex);
    }

    if (!cachedTiles.isEmpty())
        q->update();
}

void QGeoTiledMapPrivate::changeActiveMapType(const QGeoMapType mapType)
{
    m_visibleTiles->setMapType(mapType);
    m_prefetchTiles->setMapType(mapType);
}

void QGeoTiledMapPrivate::changeTileVersion(int version)
{
    m_visibleTiles->setMapVersion(version);
    m_prefetchTiles->setMapVersion(version);
    updateScene();
}

void QGeoTiledMapPrivate::mapResized(int width, int height)
{
    Q_Q(QGeoTiledMap);
    m_visibleTiles->setScreenSize(QSize(width, height));
    m_prefetchTiles->setScreenSize(QSize(width, height));
    m_mapScene->setScreenSize(QSize(width, height));
    if (q)
        q->setCameraData(q->cameraData());

    if (width > 0 && height > 0 && m_cache) {
        // absolute minimum size: one tile each side of display, 32-bit colour
        int texCacheSize = (width + m_visibleTiles->tileSize() * 2) *
                (height + m_visibleTiles->tileSize() * 2) * 4;

        // multiply by 3 so the 'recent' list in the cache is big enough for
        // an entire display of tiles
        texCacheSize *= 3;
        // TODO: move this reasoning into the tilecache

        int newSize = qMax(m_cache->minTextureUsage(), texCacheSize);
        m_cache->setMinTextureUsage(newSize);
    }
    q->evaluateCopyrights(m_visibleTiles->createTiles());
}

void QGeoTiledMapPrivate::updateTile(const QGeoTileSpec &spec)
{
     Q_Q(QGeoTiledMap);
    // Only promote the texture up to GPU if it is visible
    if (m_visibleTiles->createTiles().contains(spec)){
        QSharedPointer<QGeoTileTexture> tex = m_tileRequests->tileTexture(spec);
        if (!tex.isNull()) {
            m_mapScene->addTile(spec, tex);
            q->update();
        }
    }
}

QSGNode *QGeoTiledMapPrivate::updateSceneGraph(QSGNode *oldNode, QQuickWindow *window)
{
    return m_mapScene->updateSceneGraph(oldNode, window);
}

QGeoCoordinate QGeoTiledMapPrivate::itemPositionToCoordinate(const QDoubleVector2D &pos) const
{
    return QGeoProjection::mercatorToCoord(m_mapScene->itemPositionToMercator(pos));
}

QDoubleVector2D QGeoTiledMapPrivate::coordinateToItemPosition(const QGeoCoordinate &coordinate) const
{
    return m_mapScene->mercatorToItemPosition(QGeoProjection::coordToMercator(coordinate));
}

QT_END_NAMESPACE
