/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qgeotiledmapdata_p.h"
#include "qgeotiledmapdata_p_p.h"

#include "qgeotiledmappingmanagerengine_p.h"
#include "qgeotilecache_p.h"
#include "qgeotilespec_p.h"
#include "qgeoprojection_p.h"

#include "qgeocameratiles_p.h"
#include "qgeotilerequestmanager_p.h"
#include "qgeomapscene_p.h"
#include "qgeocoordinateinterpolator_p.h"
#include "qgeoprojection_p.h"
#include "qdoublevector2d_p.h"
#include "qgeocameracapabilities_p.h"

#include <QMutex>
#include <QMap>

#include <qnumeric.h>

#include <Qt3D/qglscenenode.h>
#include <Qt3D/qgeometrydata.h>
#include <Qt3D/qglbuilder.h>
#include <Qt3D/qglpainter.h>
#include <Qt3D/qgeometrydata.h>
#include <Qt3D/qglbuilder.h>
#include <Qt3D/qglcamera.h>
#include <Qt3D/qglsubsurface.h>

#include <cmath>

QT_BEGIN_NAMESPACE

class QGeoCoordinateInterpolator2D : public QGeoCoordinateInterpolator
{
public:
    QGeoCoordinateInterpolator2D();
    virtual ~QGeoCoordinateInterpolator2D();

    virtual QGeoCoordinate interpolate(const QGeoCoordinate &start, const QGeoCoordinate &end, qreal progress);
};

QGeoCoordinateInterpolator2D::QGeoCoordinateInterpolator2D() {}

QGeoCoordinateInterpolator2D::~QGeoCoordinateInterpolator2D() {}

QGeoCoordinate QGeoCoordinateInterpolator2D::interpolate(const QGeoCoordinate &start, const QGeoCoordinate &end, qreal progress)
{
    if (start == end) {
        if (progress < 0.5) {
            return start;
        } else {
            return end;
        }
    }

    QGeoCoordinate s2 = start;
    QGeoCoordinate e2 = end;
    QDoubleVector2D s = QGeoProjection::coordToMercator(s2);
    QDoubleVector2D e = QGeoProjection::coordToMercator(e2);

    double x = s.x();

    if (0.5 < qAbs(e.x() - s.x())) {
        // handle dateline crossing
        double ex = e.x();
        double sx = s.x();
        if (ex < sx)
            sx -= 1.0;
        else if (sx < ex)
            ex -= 1.0;

        x = (1.0 - progress) * sx + progress * ex;

        if (!qFuzzyIsNull(x) && (x < 0.0))
            x += 1.0;

    } else {
        x = (1.0 - progress) * s.x() + progress * e.x();
    }

    double y = (1.0 - progress) * s.y() + progress * e.y();

    QGeoCoordinate result = QGeoProjection::mercatorToCoord(QDoubleVector2D(x, y));
    result.setAltitude((1.0 - progress) * start.altitude() + progress * end.altitude());
    return result;
}

//------------------------
//------------------------

QGeoTiledMapData::QGeoTiledMapData(QGeoTiledMappingManagerEngine *engine, QObject *parent)
    : QGeoMapData(engine, parent)
{
    d_ptr = new QGeoTiledMapDataPrivate(this, engine);
    engine->registerMap(this);
    setCoordinateInterpolator(QSharedPointer<QGeoCoordinateInterpolator>(new QGeoCoordinateInterpolator2D()));
}

QGeoTiledMapData::~QGeoTiledMapData()
{
    if (d_ptr->engine()) // check if engine hasn't already been deleted
        d_ptr->engine().data()->deregisterMap(this);
    delete d_ptr;
}
QGeoTileRequestManager *QGeoTiledMapData::getRequestManager()
{
    Q_D(QGeoTiledMapData);
    return d->tileRequests_;
}

void QGeoTiledMapData::newTileFetched(const QGeoTileSpec &spec)
{
    Q_D(QGeoTiledMapData);
    d->newTileFetched(spec);
}

QGeoTileCache *QGeoTiledMapData::tileCache()
{
    Q_D(QGeoTiledMapData);
    return d->tileCache();
}

void QGeoTiledMapData::paintGL(QGLPainter *painter)
{
    Q_D(QGeoTiledMapData);
    d->paintGL(painter);
}

void QGeoTiledMapData::mapResized(int width, int height)
{
    Q_D(QGeoTiledMapData);
    d->resized(width, height);
    evaluateCopyrights(d->visibleTiles());
}

void QGeoTiledMapData::changeCameraData(const QGeoCameraData &oldCameraData)
{
    Q_D(QGeoTiledMapData);
    d->changeCameraData(oldCameraData);
}

void QGeoTiledMapData::prefetchData()
{
    Q_D(QGeoTiledMapData);
    d->prefetchTiles();
}

void QGeoTiledMapData::changeActiveMapType(const QGeoMapType mapType)
{
    Q_D(QGeoTiledMapData);
    d->changeActiveMapType(mapType);
}

void QGeoTiledMapData::evaluateCopyrights(const QSet<QGeoTileSpec> &visibleTiles)
{
    Q_UNUSED(visibleTiles);
}

QGeoCoordinate QGeoTiledMapData::screenPositionToCoordinate(const QPointF &pos, bool clipToViewport) const
{
    Q_D(const QGeoTiledMapData);
    if (clipToViewport) {
        int w = width();
        int h = height();

        if ((pos.x() < 0) || (w < pos.x()) || (pos.y() < 0) || (h < pos.y()))
            return QGeoCoordinate();
    }

    return d->screenPositionToCoordinate(pos);
}

QPointF QGeoTiledMapData::coordinateToScreenPosition(const QGeoCoordinate &coordinate, bool clipToViewport) const
{
    Q_D(const QGeoTiledMapData);
    QPointF pos = d->coordinateToScreenPosition(coordinate);

    if (clipToViewport) {
        int w = width();
        int h = height();

        if ((pos.x() < 0) || (w < pos.x()) || (pos.y() < 0) || (h < pos.y()))
            return QPointF(qQNaN(), qQNaN());
    }

    return pos;
}

QGeoTiledMapDataPrivate::QGeoTiledMapDataPrivate(QGeoTiledMapData *parent, QGeoTiledMappingManagerEngine *engine)
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
                     SIGNAL(newTilesVisible(const QSet<QGeoTileSpec>&)),
                     map_,
                     SLOT(evaluateCopyrights(const QSet<QGeoTileSpec>)));
}

QGeoTiledMapDataPrivate::~QGeoTiledMapDataPrivate()
{
    // controller_ is a child of map_, don't need to delete it here

    delete tileRequests_;
    delete mapScene_;
    delete cameraTiles_;

    // TODO map items are not deallocated!
    // However: how to ensure this is done in rendering thread?
}

QGeoTileCache *QGeoTiledMapDataPrivate::tileCache()
{
    return cache_;
}

QPointer<QGeoTiledMappingManagerEngine> QGeoTiledMapDataPrivate::engine() const
{
    return engine_;
}

void QGeoTiledMapDataPrivate::prefetchTiles()
{
    cameraTiles_->findPrefetchTiles();

    if (tileRequests_)
        tileRequests_->requestTiles(cameraTiles_->tiles() - mapScene_->texturedTiles());
}

void QGeoTiledMapDataPrivate::changeCameraData(const QGeoCameraData &oldCameraData)
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

void QGeoTiledMapDataPrivate::changeActiveMapType(const QGeoMapType mapType)
{
    cameraTiles_->setMapType(mapType);
}

void QGeoTiledMapDataPrivate::resized(int width, int height)
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

void QGeoTiledMapDataPrivate::newTileFetched(const QGeoTileSpec &spec)
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

QSet<QGeoTileSpec> QGeoTiledMapDataPrivate::visibleTiles()
{
    return cameraTiles_->tiles();
}

void QGeoTiledMapDataPrivate::paintGL(QGLPainter *painter)
{
    mapScene_->paintGL(painter);
    cache_->GLContextAvailable();
}

QGeoCoordinate QGeoTiledMapDataPrivate::screenPositionToCoordinate(const QPointF &pos) const
{
    return QGeoProjection::mercatorToCoord(mapScene_->screenPositionToMercator(pos));
}

QPointF QGeoTiledMapDataPrivate::coordinateToScreenPosition(const QGeoCoordinate &coordinate) const
{
    return mapScene_->mercatorToScreenPosition(QGeoProjection::coordToMercator(coordinate));
}

QT_END_NAMESPACE
