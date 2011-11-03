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
#include "map.h"
#include "map_p.h"
#include "map2d_p.h"
#include "map3d_p.h"

#include "tilecache.h"
#include "mapsphere_p.h"
#include "projection_p.h"
#include "tile.h"
#include "mapitem.h"
#include "mapcontroller.h"

#include <QMutex>
#include <QMap>

#include <qglscenenode.h>
#include <qgeometrydata.h>
#include <qglbuilder.h>
#include <Qt3D/qglpainter.h>
#include <Qt3D/qglcamera.h>

#include <cmath>

Map::Map(TileCache *cache, QObject *parent)
    : QObject(parent)
{
//    d_ptr = new Map3DPrivate(this, cache, 20000.0);

    // edge is 2^max zoom * 4
    d_ptr = new Map2DPrivate(this, cache, 20, 256);
}

Map::~Map()
{
    delete d_ptr;
}

MapController* Map::mapController()
{
    return d_ptr->mapController();
}

void Map::setMappingManager(QGeoMappingManager *manager)
{
    d_ptr->setMappingManager(manager);
}

void Map::paintGL(QGLPainter *painter)
{
    d_ptr->paintGL(painter);
}

QGLCamera* Map::glCamera() const
{
    return d_ptr->glCamera();
}

void Map::resize(int width, int height)
{
    d_ptr->resize(width, height);
}

int Map::width() const
{
    return d_ptr->width();
}

int Map::height() const
{
    return d_ptr->height();
}

void Map::setAutoUpdate(bool autoUpdate)
{
    d_ptr->setAutoUpdate(autoUpdate);
}

bool Map::autoUpdate() const
{
    return d_ptr->autoUpdate();
}

void Map::clearCache()
{
//    sphere_->clearCache();
}

void Map::setCameraData(const CameraData &cameraData)
{
    if (cameraData == d_ptr->cameraData())
        return;

    d_ptr->setCameraData(cameraData);

    if (d_ptr->autoUpdate())
            update();

    emit cameraDataChanged(d_ptr->cameraData());
}

CameraData Map::cameraData() const
{
    return d_ptr->cameraData();
}

void Map::update()
{
    d_ptr->update();
    emit updateRequired();
}

int Map::numMapItems() const
{
    return d_ptr->numMapItems();
}

QList<MapItem*> Map::mapItems() const
{
    return d_ptr->mapItems();
}

QList<MapItem*> Map::mapItemsAt(const QPoint &point) const
{
    return d_ptr->mapItemsAt(point);
}

QList<MapItem*> Map::mapItemsWithin(const QRect &rect) const
{
    return d_ptr->mapItemsWithin(rect);
}

void Map::addMapItem(MapItem *item)
{
    d_ptr->addMapItem(item);
}

void Map::removeMapItem(MapItem *item)
{
    d_ptr->removeMapItem(item);
}

void Map::clearMapItems()
{
    d_ptr->clearMapItems();
}

QGeoCoordinate Map::screenPositionToCoordinate(const QPointF &pos, bool clipToViewport) const
{
    if (clipToViewport) {
        int w = d_ptr->width();
        int h = d_ptr->height();

        if ((pos.x() < 0) || (w < pos.x()) || (pos.y() < 0) || (h < pos.y()))
            return QGeoCoordinate();
    }

    return d_ptr->screenPositionToCoordinate(pos);
}

QPointF Map::coordinateToScreenPosition(const QGeoCoordinate &coordinate, bool clipToViewport) const
{
    QPointF pos = d_ptr->coordinateToScreenPosition(coordinate);

    if (clipToViewport) {
        int w = d_ptr->width();
        int h = d_ptr->height();

        if ((pos.x() < 0) || (w < pos.x()) || (pos.y() < 0) || (h < pos.y()))
            return QPointF(qQNaN(), qQNaN());
    }

    return pos;
}

void Map::setActiveMapType(const MapType type)
{
    d_ptr->setActiveMapType(type);
}

const MapType Map::activeMapType() const
{
    return d_ptr->activeMapType();
}

//------------------------------------------------------------//

MapPrivate::MapPrivate(Map *parent, TileCache *cache)
    : autoUpdate_(true),
      map_(parent),
      manager_(0),
      controller_(0),
      activeMapType_(MapType())
{
    sphere_ = new MapSphere(parent, this, cache);
    glCamera_ = new QGLCamera();
}

MapPrivate::~MapPrivate()
{
    // controller_ is a child of map_, don't need to delete it here
    delete sphere_;
    delete glCamera_;
    // TODO map items are not deallocated!
    // However: how to ensure this is done in rendering thread?
}

QGLSceneNode* MapPrivate::createTileNode(const Tile &tile)
{
    QGLSceneNode* node = createTileSpecNode(tile.tileSpec());

    QGLMaterial *mat = new QGLMaterial(node);
    mat->setTexture(tile.texture());
    node->setEffect(QGL::LitDecalTexture2D);
    node->setMaterial(mat);

    return node;
}

void MapPrivate::setMappingManager(QGeoMappingManager *manager)
{
    manager_ = manager;
    sphere_->setMappingManager(manager_);
}

MapController* MapPrivate::mapController()
{
    if (!controller_)
        controller_ = new MapController(map_, projection_);
    return controller_;
}

QGeoMappingManager* MapPrivate::manager() const
{
    return manager_;
}

QGLCamera* MapPrivate::glCamera() const
{
    return glCamera_;
}

void MapPrivate::setProjection(const QSharedPointer<Projection> &projection)
{
    projection_ = projection;
}

QSharedPointer<Projection> MapPrivate::projection() const
{
    return projection_;
}

void MapPrivate::setCameraData(const CameraData &cameraData)
{
    cameraData_ = cameraData;
    cameraData_.setAspectRatio(aspectRatio_);
    cameraData_.setProjection(projection_.toWeakRef());
    updateGlCamera(glCamera_);
    updateFrustum(frustum_);
    visibleTiles_ = updateVisibleTiles();
    if (autoUpdate_)
        sphere_->update(visibleTiles_);
}

CameraData MapPrivate::cameraData() const
{
    return cameraData_;
}

void MapPrivate::update()
{
    sphere_->update(visibleTiles_);
}

void MapPrivate::setAutoUpdate(bool autoUpdate)
{
    autoUpdate_ = autoUpdate;
}

bool MapPrivate::autoUpdate() const
{
    return autoUpdate_;
}

void MapPrivate::resize(int width, int height)
{
    width_ = width;
    height_ = height;
    aspectRatio_ = 1.0 * width_ / height_;
    setCameraData(cameraData_);
}

Frustum MapPrivate::frustum() const
{
    return frustum_;
}

void MapPrivate::paintGL(QGLPainter *painter)
{
    sphere_->paintGL(painter);
}

int MapPrivate::numMapItems() const
{
    return sphere_->numMapItems();
}

QList<MapItem*> MapPrivate::mapItems() const
{
    return sphere_->mapItems();
}

QList<MapItem*> MapPrivate::mapItemsAt(const QPoint &point) const
{
    return sphere_->mapItemsAt(point);
}

QList<MapItem*> MapPrivate::mapItemsWithin(const QRect &rect) const
{
    return sphere_->mapItemsWithin(rect);
}

void MapPrivate::addMapItem(MapItem *item)
{
    sphere_->addMapItem(item);
}

// Must not be called from rendering thread
void MapPrivate::removeMapItem(MapItem *item)
{
    sphere_->removeMapItem(item);
}

void MapPrivate::clearMapItems()
{
    sphere_->clearMapItems();
}

QVector2D MapPrivate::pointToTile(const QVector3D &point, int zoom, bool roundUp) const
{
    QVector2D p = projection_->pointToMercator(point);

    int z = 1 << zoom;
    int x = 0;
    int y = 0;

    if (p.y() == 1.0)
        y = z - 1;
    else
        y = static_cast<int>(z * p.y()) % z;

    if ((qAbs(p.x()) < 1e-6) || (qAbs(p.x() - 1) < 1e-6))
        if (roundUp)
            x = z - 1;
        else
            x = 0;
    else
        x = static_cast<int>(z * p.x()) % z;

    return QVector2D(x, y);
}

QVector3D MapPrivate::tileXIntersectToPoint(int zoomLevel, int x) const
{
    int zpow2 = 1 << zoomLevel;
    return projection_->mercatorToPoint(QVector2D(x * 1.0 / zpow2, zpow2 / 2.0));
}

QVector3D MapPrivate::tileYIntersectToPoint(int zoomLevel, int y) const
{
    int zpow2 = 1 << zoomLevel;
    return projection_->mercatorToPoint(QVector2D(zpow2 / 2.0, y * 1.0 / zpow2));
}

int MapPrivate::width() const
{
    return width_;
}

int MapPrivate::height() const
{
    return height_;
}

double MapPrivate::aspectRatio() const
{
    return aspectRatio_;
}

void
MapPrivate::setActiveMapType(const MapType type)
{
    activeMapType_ = type;
    //TODO: check if this shared
    //make it more optimal
    //rewrite current specs
    QList<TileSpec> temp = visibleTiles_;
    visibleTiles_.clear();
    foreach (TileSpec spec,temp) {
      spec.setMapId(type.mapId());
      visibleTiles_ << spec;
    }

    update();
}

const MapType MapPrivate::activeMapType() const
{
  return activeMapType_;
}
