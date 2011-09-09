/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include <QMutex>
#include <QMap>

#include <qglscenenode.h>
#include <qgeometrydata.h>
#include <qglbuilder.h>
#include <Qt3D/qglpainter.h>
#include <Qt3D/qglcamera.h>

#include <cmath>

Map::Map(TileCache *cache, QObject *parent)
    : QObject(parent),
      autoUpdate_(true)
{
//    d_ptr = new Map3DPrivate(this, cache, 20000.0);

    // edge is 2^max zoom * 4
    d_ptr = new Map2DPrivate(this, cache, 20, 256);
}

Map::~Map()
{
    delete d_ptr;
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

void Map::setAutoUpdate(bool autoUpdate)
{
    autoUpdate_ = autoUpdate;
}

bool Map::autoUpdate() const
{
    return autoUpdate_;
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

        if (autoUpdate_)
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

MapItem* Map::mapItem(int index) const
{
    return d_ptr->mapItem(index);
}

QList<MapItem*> Map::mapItems() const
{
    return d_ptr->mapItems();
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

QGeoCoordinate Map::screenPositionToCoordinate(const QPointF &pos) const
{
    return d_ptr->screenPositionToCoordinate(pos);
}

QPointF Map::coordinateToScreenPosition(const QGeoCoordinate &coordinate) const
{
    return d_ptr->coordinateToScreenPosition(coordinate);
}

//------------------------------------------------------------//

MapPrivate::MapPrivate(Map *parent, TileCache *cache)
    : manager_(0)
{
    sphere_ = new MapSphere(parent, this, cache);
    mapSceneNode_ = new QGLSceneNode();
    mapSceneNode_->addNode(sphere_->sphereSceneNode());
    objectSceneNode_ = new QGLSceneNode();
    glCamera_ = new QGLCamera();
}

MapPrivate::~MapPrivate()
{
    delete sphere_;
    delete mapSceneNode_;
    delete objectSceneNode_;
    delete glCamera_;
    // TODO map items are not deallocated!
    // However: how to ensure this is done in rendering thread?
}

void MapPrivate::setMappingManager(QGeoMappingManager *manager)
{
    manager_ = manager;
    sphere_->setMappingManager(manager_);
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
    if (!sphere_->updateMutex.tryLock()) {
        qDebug() << "map will miss a frame, no mutex acquired!------";
        return;
    }

    sphere_->GLContextAvailable();
    GLContextAvailable();

    mapSceneNode_->draw(painter);

    glDisable(GL_DEPTH_TEST);
    objectSceneNode_->draw(painter);
    glEnable(GL_DEPTH_TEST);

    sphere_->updateMutex.unlock();
}

int MapPrivate::numMapItems() const
{
    return mapItems_.length();
}

MapItem* MapPrivate::mapItem(int index) const
{
    return mapItems_.at(index);
}

QList<MapItem*> MapPrivate::mapItems() const
{
    return mapItems_;
}

void MapPrivate::addMapItem(MapItem *item)
{
    // TODO bit hard block - on declarative side the coordinate may well be invalid time to time
    if (!item->coordinate().isValid())
        return;
    sphere_->updateMutex.lock();
    updateMapItemSceneNode(item);
    if (item->sceneNode()) {
        objectSceneNode_->addNode(item->sceneNode());
    }
    mapItems_.append(item);
    sphere_->updateMutex.unlock();
}

// Function is guaranteed to be only called when executing in
// rendering thread with valid GL context. Furthermore it is
// safe to update any geometry/structures - mutex is locked.
void MapPrivate::GLContextAvailable()
{
    // remove obsolete items
    QSet<MapItem *>::const_iterator i = obsoleteItems_.constBegin();
    while (i != obsoleteItems_.constEnd()) {
        QGLSceneNode *node = (*i)->sceneNode();
        if (node) {
            //sceneNode_->removeNode(node); must be removed in GUI thread, not in rendering thread
            delete node;
        }
        mapItems_.removeAll(*i);
        ++i;
    }
    if (!obsoleteItems_.isEmpty())
        obsoleteItems_.clear();
}

// Must not be called from rendering thread
void MapPrivate::removeMapItem(MapItem *item)
{
    sphere_->updateMutex.lock();
    // nodes need to be removed from the scene node tree in GUI thread,
    // but the deletion needs to occur in rendering thread
    if (item->sceneNode())
        objectSceneNode_->removeNode(item->sceneNode());
    if (!obsoleteItems_.contains(item))
        obsoleteItems_.insert(item);
    sphere_->updateMutex.unlock();
}

void MapPrivate::clearMapItems()
{
    sphere_->updateMutex.lock();
    for (int i = 0; i < mapItems_.size(); ++i) {
        if (!obsoleteItems_.contains(mapItems_.at(i))) {
            if (mapItems_.at(i)->sceneNode())
                objectSceneNode_->removeNode(mapItems_.at(i)->sceneNode());
            obsoleteItems_.insert(mapItems_.at(i));
        }
    }
    sphere_->updateMutex.unlock();
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
