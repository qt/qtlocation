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
#include "qgeomap_p.h"
#include "qgeomap_p_p.h"

#include "qgeotilecache_p.h"
#include "qgeotilespec.h"
#include "qgeoprojection_p.h"
#include "qgeocameracapabilities_p.h"
#include "qgeomapcontroller_p.h"
#include "qdoublevector2d_p.h"
#include "qdoublevector3d_p.h"

#include "qgeocameratiles_p.h"
#include "qgeomapimages_p.h"
#include "qgeomapgeometry_p.h"

#include "qgeomappingmanager.h"

#include <QMutex>
#include <QMap>

#include <qnumeric.h>

#include <qglscenenode.h>
#include <qgeometrydata.h>
#include <qglbuilder.h>
#include <Qt3D/qglpainter.h>
#include <Qt3D/qgeometrydata.h>
#include <Qt3D/qglbuilder.h>
#include <Qt3D/qglcamera.h>
#include <Qt3D/qglsubsurface.h>

#include <cmath>

QT_BEGIN_NAMESPACE

QGeoMap::QGeoMap(QGeoTileCache *cache, QObject *parent)
    : QObject(parent),
      d_ptr(new QGeoMapPrivate(this, cache)) {}

QGeoMap::~QGeoMap()
{
    delete d_ptr;
}

QGeoTileCache* QGeoMap::tileCache()
{
    Q_D(QGeoMap);
    return d->tileCache();
}

QGeoMapController* QGeoMap::mapController()
{
    Q_D(QGeoMap);
    return d->mapController();
}

void QGeoMap::setMappingManager(QGeoMappingManager *manager)
{
    Q_D(QGeoMap);
    d->setMappingManager(manager);
}

void QGeoMap::paintGL(QGLPainter *painter)
{
    Q_D(QGeoMap);
    d->paintGL(painter);
}

QGLCamera* QGeoMap::glCamera() const
{
    Q_D(const QGeoMap);
    return d->glCamera();
}

void QGeoMap::resize(int width, int height)
{
    Q_D(QGeoMap);
    d->resize(width, height);

    // always emit this signal to trigger items to redraw
    emit cameraDataChanged(d->cameraData());
}

int QGeoMap::width() const
{
    Q_D(const QGeoMap);
    return d->width();
}

int QGeoMap::height() const
{
    Q_D(const QGeoMap);
    return d->height();
}

void QGeoMap::setCameraData(const QGeoCameraData &cameraData)
{
    Q_D(QGeoMap);

    if (cameraData == d->cameraData())
        return;

    d->setCameraData(cameraData);
    update();

    emit cameraDataChanged(d->cameraData());
}

QGeoCameraData QGeoMap::cameraData() const
{
    Q_D(const QGeoMap);
    return d->cameraData();
}

QGeoCameraCapabilities QGeoMap::cameraCapabilities() const
{
    Q_D(const QGeoMap);
    if (d->manager())
        return d->manager()->cameraCapabilities();
    else
        return QGeoCameraCapabilities();
}

void QGeoMap::update()
{
    emit updateRequired();
}

QGeoCoordinate QGeoMap::screenPositionToCoordinate(const QPointF &pos, bool clipToViewport) const
{
    Q_D(const QGeoMap);
    if (clipToViewport) {
        int w = d->width();
        int h = d->height();

        if ((pos.x() < 0) || (w < pos.x()) || (pos.y() < 0) || (h < pos.y()))
            return QGeoCoordinate();
    }

    return d->screenPositionToCoordinate(pos);
}

QPointF QGeoMap::coordinateToScreenPosition(const QGeoCoordinate &coordinate, bool clipToViewport) const
{
    Q_D(const QGeoMap);
    QPointF pos = d->coordinateToScreenPosition(coordinate);

    if (clipToViewport) {
        int w = d->width();
        int h = d->height();

        if ((pos.x() < 0) || (w < pos.x()) || (pos.y() < 0) || (h < pos.y()))
            return QPointF(qQNaN(), qQNaN());
    }

    return pos;
}

void QGeoMap::setActiveMapType(const QGeoMapType type)
{
    Q_D(QGeoMap);
    d->setActiveMapType(type);
}

const QGeoMapType QGeoMap::activeMapType() const
{
    Q_D(const QGeoMap);
    return d->activeMapType();
}

QGeoMapPrivate::QGeoMapPrivate(QGeoMap *parent, QGeoTileCache *cache)
    : width_(0),
      height_(0),
      aspectRatio_(0.0),
      map_(parent),
      cache_(cache),
      manager_(0),
      controller_(0),
      cameraTiles_(new QGeoCameraTiles()),
      mapGeometry_(new QGeoMapGeometry()),
      mapImages_(0),
      activeMapType_(QGeoMapType()) {}

QGeoMapPrivate::~QGeoMapPrivate()
{
    // controller_ is a child of map_, don't need to delete it here

    delete mapImages_;
    delete mapGeometry_;
    delete cameraTiles_;

    manager_->deregisterMap(map_);
    // TODO map items are not deallocated!
    // However: how to ensure this is done in rendering thread?
}

QGeoTileCache* QGeoMapPrivate::tileCache()
{
    return cache_;
}

void QGeoMapPrivate::setMappingManager(QGeoMappingManager *manager)
{
    if (manager) {
        manager->registerMap(map_);

        cameraTiles_->setMaximumZoomLevel(static_cast<int>(ceil(manager->cameraCapabilities().maximumZoomLevel())));

        cameraTiles_->setTileSize(manager->tileSize());
        mapGeometry_->setTileSize(manager->tileSize());

        pluginString_ = manager->managerName() + QLatin1String("_") + QString::number(manager->managerVersion());
        cameraTiles_->setPluginString(pluginString_);

        mapImages_ = new QGeoMapImages(map_);
        mapImages_->setMappingManager(manager);

    } else {
        manager->deregisterMap(map_);
    }
    manager_ = manager;
}

QGeoMapController* QGeoMapPrivate::mapController()
{
    if (!controller_)
        controller_ = new QGeoMapController(map_, mapGeometry_->coordinateInterpolator());
    return controller_;
}

QGLCamera* QGeoMapPrivate::glCamera() const
{
    return mapGeometry_->camera();
}

void QGeoMapPrivate::setCameraData(const QGeoCameraData &cameraData)
{
    double lat = cameraData_.center().latitude();

    cameraData_ = cameraData;

    if (mapGeometry_->verticalLock()) {
        QGeoCoordinate coord = cameraData_.center();
        coord.setLatitude(lat);
        cameraData_.setCenter(coord);
    }

    if (manager_) {
        QGeoCameraCapabilities capabilities = manager_->cameraCapabilities();
        if (cameraData_.zoomLevel() < capabilities.minimumZoomLevel())
            cameraData_.setZoomLevel(capabilities.minimumZoomLevel());

        if (cameraData_.zoomLevel() > capabilities.maximumZoomLevel())
            cameraData_.setZoomLevel(capabilities.maximumZoomLevel());

        if (!capabilities.supportsBearing())
            cameraData_.setBearing(0.0);

        if (capabilities.supportsTilting()) {
            if (cameraData_.tilt() < capabilities.minimumTilt())
                cameraData_.setTilt(capabilities.minimumTilt());

            if (cameraData_.tilt() > capabilities.maximumTilt())
                cameraData_.setTilt(capabilities.maximumTilt());
        } else {
            cameraData_.setTilt(0.0);
        }

        if (!capabilities.supportsRolling())
            cameraData_.setRoll(0.0);
    }

    cameraData_.setCoordinateInterpolator(mapGeometry_->coordinateInterpolator().toWeakRef());

    cameraTiles_->setCamera(cameraData_);
    visibleTiles_ = cameraTiles_->tiles();

    mapGeometry_->setCameraData(cameraData_);
    mapGeometry_->setVisibleTiles(visibleTiles_);

    if (mapImages_) {
        mapImages_->setVisibleTiles(visibleTiles_);

        //QSet<QGeoTileSpec> cachedTiles = mapImages_->cachedTiles();
        // TODO make this more efficient
        QSet<QGeoTileSpec> cachedTiles = visibleTiles_;

        typedef QSet<QGeoTileSpec>::const_iterator iter;
        iter i = cachedTiles.constBegin();
        iter end = cachedTiles.constEnd();
        for (; i != end; ++i) {
            QGeoTileSpec tile = *i;
            if (cache_->contains(tile))
                mapGeometry_->addTile(tile, cache_->get(tile));
        }

        if (!cachedTiles.isEmpty())
            map_->update();

    }
}

QGeoCameraData QGeoMapPrivate::cameraData() const
{
    return cameraData_;
}

QGeoMappingManager *QGeoMapPrivate::manager() const
{
    return manager_;
}

void QGeoMapPrivate::resize(int width, int height)
{
    width_ = width;
    height_ = height;
    aspectRatio_ = 1.0 * width_ / height_;
    cameraTiles_->setScreenSize(QSize(width, height));
    mapGeometry_->setScreenSize(QSize(width, height));
    setCameraData(cameraData_);
}

int QGeoMapPrivate::width() const
{
    return width_;
}

int QGeoMapPrivate::height() const
{
    return height_;
}

double QGeoMapPrivate::aspectRatio() const
{
    return aspectRatio_;
}

void QGeoMapPrivate::setActiveMapType(const QGeoMapType &type)
{
    activeMapType_ = type;
    cameraTiles_->setMapType(type);
    visibleTiles_ = cameraTiles_->tiles();

    setCameraData(cameraData_);

    map_->update();
}

const QGeoMapType QGeoMapPrivate::activeMapType() const
{
  return activeMapType_;
}

void QGeoMapPrivate::tileFetched(const QGeoTileSpec &spec)
{
    if (cache_->contains(spec))
        mapGeometry_->addTile(spec, cache_->get(spec));
    mapImages_->tileFetched(spec);
    map_->update();
}

void QGeoMapPrivate::paintGL(QGLPainter *painter)
{
    mapGeometry_->paintGL(painter);
}

QGeoCoordinate QGeoMapPrivate::screenPositionToCoordinate(const QPointF &pos) const
{
    return QGeoProjection::mercatorToCoord(mapGeometry_->screenPositionToMercator(pos));
}

QPointF QGeoMapPrivate::coordinateToScreenPosition(const QGeoCoordinate &coordinate) const
{
    return mapGeometry_->mercatorToScreenPosition(QGeoProjection::coordToMercator(coordinate));
}

QT_END_NAMESPACE
