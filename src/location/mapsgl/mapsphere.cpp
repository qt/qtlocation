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
#include "mapsphere_p.h"

#include "tilecache.h"
#include "tilespec.h"
#include "tile.h"
#include "map.h"
#include "map_p.h"

#include <QOpenGLFramebufferObject>

#include <Qt3D/qglscenenode.h>
#include <Qt3D/qglframebufferobjectsurface.h>
#include <Qt3D/qglcamera.h>
#include <Qt3D/qglpainter.h>

#include <QVector>
#include <QTimer>

struct ItemTileResources {

    ItemTileResources() :
        node(0),
        fbo(0) {}

    bool operator == (const ItemTileResources &other) const {
        return ((fbo == other.fbo) && (node == other.node));
    }

    QGLSceneNode *node;
    QOpenGLFramebufferObject *fbo;
};

struct ItemTileCacheEntry {

    ItemTileCacheEntry(MapSphere *mapSphere) :
        mapSphere(mapSphere) {}

    ~ItemTileCacheEntry() {
        if (!mapSphere->itemTileEvictions_.contains(resources))
            mapSphere->itemTileEvictions_.append(resources);
    }

    MapSphere *mapSphere;

    QRect viewport;
    QList<MapItem*> items;

    ItemTileResources resources;
};

MapSphere::MapSphere(Map* map, MapPrivate *mapPrivate, TileCache *tileCache)
    : QObject(0),
      tileCache_(tileCache),
      map_(map),
      mapPrivate_(mapPrivate)
{

    sphereNode_ = new QGLSceneNode(this);

    itemNode_ = new QGLSceneNode(this);

    connect(tileCache_,
            SIGNAL(prefetchingFinished()),
            this,
            SLOT(prefetchingFinished()));
    connect(tileCache_,
            SIGNAL(tileFetched(TileSpec)),
            this,
            SLOT(tileFetched(TileSpec)));

    connect(this,
            SIGNAL(tileUpdated()),
            map,
            SIGNAL(updateRequired()));

    connect(this,
            SIGNAL(sphereUpdated()),
            map,
            SIGNAL(updatesFinished()));
}

MapSphere::~MapSphere()
{
    QList<QGLSceneNode*> nodes = built_.values();
    for (int i = 0; i < nodes.size(); ++i) {
        QGLSceneNode *node = nodes.at(i);
        //node->material()->texture()->release();
        node->geometry().clear();
        sphereNode_->removeNode(node);
        delete node;
    }
}

void MapSphere::setMappingManager(QGeoMappingManager *manager)
{
    tileCache_->setMappingManager(manager);
}

QGLSceneNode* MapSphere::sphereSceneNode() const
{
    return sphereNode_;
}

void MapSphere::clearCache()
{
    // clear cache
}

// Function to perform housekeeping that require access to GL context
// (access to GL context varies depending on if we are running as
// c++ app or QML app).
//
// Function is guaranteed to be only called when executing in
// rendering thread with valid GL context. Furthermore it is
// safe to update any geometry/structures - mutex is locked.
void MapSphere::GLContextAvailable()
{
    // need something like this in the cache for
    // releasing textures and freeing nodes that
    // have been evicted from the GPU
    tileCache_->GLContextAvailable(sphereNode_);
//    qDeleteAll(obsoleteNodes_);
//    obsoleteNodes_.clear();


    // remove obsolete scene nodes
    QSet<MapItemGLResources *>::const_iterator i = obsoleteGLResources_.constBegin();
    while (i != obsoleteGLResources_.constEnd()) {
        if (*i) {
            // todo do we have to delete all pointers or will scene node take down everything
            delete (*i)->sceneNode;
            //obsoleteGLResources_.removeAll(*i);
        }
        ++i;
    }
    if (!obsoleteGLResources_.isEmpty())
        obsoleteGLResources_.clear();
}

void MapSphere::update(const QList<TileSpec> &tiles)
{
    QHash<TileSpec, QGLSceneNode*> stillBuilt;

    QVector<TileSpec> req(tiles.size());
    QVector<TileSpec> draw(tiles.size());

    int reqSize = 0;
    int drawSize = 0;
    QList<TileSpec>::const_iterator i = tiles.constBegin();
    QList<TileSpec>::const_iterator tilesEnd = tiles.constEnd();
    while (i != tilesEnd) {
        /*
          If the tile is already built or has been requested then we
          shouldn't use it (where "use" means drawing a cached tile or
          requested an uncached tile).
        */
        if (built_.contains(*i)) {
            stillBuilt.insert(*i, built_.value(*i));
        } else if (!requested_.contains(*i)){
            /*
              Otherwise we add it to the correct list
            */
            if (tileCache_->contains(*i)) {
                draw[drawSize] = *i;
                ++drawSize;
            } else {
                req[reqSize] = *i;
                ++reqSize;
                requested_.insert(*i);
            }
        }

        ++i;
    }

    req.resize(reqSize);

    updateMutex.lock();

    QHash<TileSpec, QGLSceneNode*>::const_iterator j = built_.constBegin();
    QHash<TileSpec, QGLSceneNode*>::const_iterator end = built_.constEnd();
    while (j != end) {
        TileSpec spec = j.key();
        if (!stillBuilt.contains(spec)) {
            sphereNode_->removeNode(j.value());
        }
        ++j;
    }
    built_.swap(stillBuilt);

    updateMutex.unlock();

    for (int i = 0; i < drawSize; ++i)
            displayTile(draw.at(i));

    if (req.isEmpty()) {
        emit sphereUpdated();
    } else {
        tileCache_->prefetch(req.toList());
    }

    QSet<TileSpec> wereVisible = visibleTiles_.toSet() - tiles.toSet();
    QSet<TileSpec> stillVisible = visibleTiles_.toSet().intersect(tiles.toSet());
    QSet<TileSpec> nowVisible = tiles.toSet() - visibleTiles_.toSet();

    visibleTiles_ = tiles;

    QSet<TileSpec>::const_iterator a = wereVisible.constBegin();
    QSet<TileSpec>::const_iterator removeEnd = wereVisible.constEnd();

    while (a != removeEnd) {
        TileSpec spec = *a;
        removeSpec(spec);
        ++a;
    }

    QSet<TileSpec>::const_iterator c = nowVisible.constBegin();
    QSet<TileSpec>::const_iterator addEnd = nowVisible.constEnd();

    while (c != addEnd) {
        TileSpec spec = *c;
        addSpec(spec);
        ++c;
    }
}

void MapSphere::removeSpec(const TileSpec &spec)
{
    ItemTileCacheEntry *entry = itemTileCache_.object(spec);

    if (!entry) {
        return;
    }

    // update reverse map
    QList<MapItem*>::const_iterator i = entry->items.constBegin();
    QList<MapItem*>::const_iterator end = entry->items.constEnd();
    while (i != end) {
        MapItem *item = *i;
        QSet<TileSpec> v = itemTileReverseMap_.value(item, QSet<TileSpec>());
        v.remove(spec);
        itemTileReverseMap_.insert(item, v);
        ++i;
    }

    // update scene node
    QGLSceneNode *node = entry->resources.node;
    if (node && itemNode_->children().contains(node))
        itemNode_->removeNode(node);

    // update cache
    itemTileCache_.remove(spec);
}

void MapSphere::addSpec(const TileSpec &spec)
{
    QList<MapItem*> items = itemTree_.itemsWithin(mapPrivate_->specToRect(spec));

    if (items.isEmpty())
        return;

    // update reverse map
    QList<MapItem*>::const_iterator i = items.constBegin();
    QList<MapItem*>::const_iterator end = items.constEnd();
    while (i != end) {
        MapItem *item = *i;
        QSet<TileSpec> v = itemTileReverseMap_.value(item, QSet<TileSpec>());
        v.insert(spec);
        itemTileReverseMap_.insert(item, v);
        ++i;
    }

    bool created = false;
    ItemTileCacheEntry *entry = itemTileCache_.object(spec);

    if (entry == 0) {
        entry = new ItemTileCacheEntry(this);
        entry->viewport = mapPrivate_->specToRect(spec);
        created = true;
    }

    if (entry->items.toSet() != items.toSet()) {
        entry->items = items;
        itemTileUpdates_.insert(spec);
    }

    if (created)
        itemTileCache_.insert(spec, entry);
}

void MapSphere::prefetchingFinished()
{
    emit sphereUpdated();
}

void MapSphere::tileFetched(const TileSpec &spec)
{
    if (!requested_.contains(spec))
        return;

    if (itemTileCache_.contains(spec)) {
        ItemTileCacheEntry *entry = itemTileCache_.object(spec);
        if (entry && !entry->items.isEmpty())
            itemTileUpdates_.insert(spec);
    }

    displayTile(spec);

    requested_.remove(spec);

    emit tileUpdated();
}

void MapSphere::displayTile(const TileSpec &spec)
{
    if (built_.contains(spec))
        return;

    updateMutex.lock();
    Tile tile = tileCache_->get(spec);
    QGLSceneNode *node = tile.sceneNode();
    if (!node) {
        node = mapPrivate_->createTileNode(tile);
        tile.setSceneNode(node);
        tile.bind();
        tileCache_->update(spec, tile);
    }
    if (node) {
        sphereNode_->addNode(node);
        built_.insert(spec, node);
    }
    updateMutex.unlock();
}


// depth test should be disabled before this is called
void MapSphere::updateItems(QGLPainter *painter)
{
    // clear the eviction list
    QList<ItemTileResources>::iterator j = itemTileEvictions_.begin();
    QList<ItemTileResources>::iterator evictEnd = itemTileEvictions_.end();

    while (j != evictEnd) {
        ItemTileResources itr = *j;

        if (itr.node)
            delete itr.node;
        if (itr.fbo)
            delete itr.fbo;
        itr.node = 0;
        itr.fbo = 0;
        *j = itr;
        ++j;
    }

    itemTileEvictions_.clear();

    QGLCamera *camera = new QGLCamera(this);

    // loop through visible tiles
    // grab all the items within those tiles
    // if any are marked as needs update, add the spec to the update list

    for (int i = 0; i < visibleTiles_.size(); ++i) {
        ItemTileCacheEntry *entry = itemTileCache_.object(visibleTiles_.at(i));
        bool needsUpdate = false;
        if (entry) {
            for (int j = 0; j < entry->items.size(); ++j) {
                if (entry->items.at(j)->needsUpdate()) {
                    entry->items[j]->update();
                    needsUpdate = true;
                }
            }
        }
        if (needsUpdate) {
            itemTileUpdates_.insert(visibleTiles_.at(i));
        }
    }

    for (int i = 0; i < visibleTiles_.size(); ++i) {
        ItemTileCacheEntry *entry = itemTileCache_.object(visibleTiles_.at(i));
        if (entry) {
            for (int j = 0; j < entry->items.size(); ++j) {
                if (entry->items.at(j)->needsUpdate()) {
                    entry->items[j]->setNeedsUpdate(false);
                }
            }
        }
    }

    QSet<TileSpec>::const_iterator i = itemTileUpdates_.constBegin();
    QSet<TileSpec>::const_iterator updateEnd = itemTileUpdates_.constEnd();

    QSet<TileSpec> nextUpdates;

    // loop through updated entries
    while (i != updateEnd) {
        TileSpec spec = *i;
        ItemTileCacheEntry *entry = itemTileCache_.object(spec);

        if (!entry || entry->items.isEmpty()) {
            ++i;
            continue;
        }

        QOpenGLFramebufferObject *fbo = entry->resources.fbo;

        if (!fbo) {
            fbo = new QOpenGLFramebufferObject(512, 512);
            entry->resources.fbo = fbo;
        }

        QGLFramebufferObjectSurface *fboSurface = new QGLFramebufferObjectSurface(fbo, painter->context());

        double x = entry->viewport.x() + entry->viewport.width() / 2.0;
        double y = entry->viewport.y() + entry->viewport.height() / 2.0;

        camera->setCenter(QVector3D(x, y, 100));
        camera->setEye(QVector3D(x, y, 512));
        camera->setProjectionType(QGLCamera::Orthographic);
        camera->setViewSize(entry->viewport.size());

        painter->pushSurface(fboSurface);

        painter->setCamera(camera);

        if (built_.contains(spec))
            built_[spec]->draw(painter);

        for (int j = 0; j < entry->items.size(); ++j) {
            MapItem *item = entry->items.at(j);
            if (item->glResources() == 0)
                continue;

            if (item->textureId() == 0) {
                nextUpdates.insert(spec);
            } else if (item->sceneNode()) {
                item->sceneNode()->draw(painter);
            }
        }

        painter->popSurface();

        delete fboSurface;

        QGLSceneNode *node = entry->resources.node;

        bool hasNode = (node != 0);

        if (!hasNode) {
            node = mapPrivate_->createTileSpecNode(spec);
            node->setEffect(QGL::LitDecalTexture2D);
            QGLMaterial *mat = new QGLMaterial(node);
            node->setMaterial(mat);
            entry->resources.node = node;
        }

        QGLTexture2D *tex = QGLTexture2D::fromTextureId(fbo->texture(), fbo->size());
        // null check? free old texture?
        QGLMaterial *m = node->material();
        m->setTexture(tex);
        node->setMaterial(m);

        if (!hasNode)
            itemNode_->addNode(node);

        ++i;
    }

    delete camera;

    itemTileUpdates_.clear();
    itemTileUpdates_ = nextUpdates;
}

void MapSphere::paintGL(QGLPainter *painter)
{
    if (!updateMutex.tryLock()) {
        qWarning() << "----- map will miss a frame, no mutex acquired!------";
        return;
    }

    GLContextAvailable();

    sphereNode_->draw(painter);

    glDisable(GL_DEPTH_TEST);
    itemNode_->draw(painter);
    updateItems(painter);
    glEnable(GL_DEPTH_TEST);

    updateMutex.unlock();
}

int MapSphere::numMapItems() const
{
    return itemTree_.size();
}

QList<MapItem*> MapSphere::mapItems() const
{
    return itemTree_.items();
}

QList<MapItem*> MapSphere::mapItemsAt(const QPoint &point) const
{
    return itemTree_.itemsAt(point);
}

QList<MapItem*> MapSphere::mapItemsWithin(const QRect &rect) const
{
    return itemTree_.itemsWithin(rect);
}

void MapSphere::addMapItem(MapItem *item)
{
    // TODO bit hard block - on declarative side the coordinate may well be invalid time to time
    if (!item->coordinate().isValid())
        return;

    updateMutex.lock();
    if (!item->glResources())
        item->setGLResources(new MapItemGLResources);
    mapPrivate_->updateMapItemSceneNode(item);
    item->update();

    itemTree_.insert(item);

    QList<TileSpec>::const_iterator i = visibleTiles_.constBegin();
    QList<TileSpec>::const_iterator end = visibleTiles_.constEnd();

    QSet<TileSpec> specs = itemTileReverseMap_.value(item, QSet<TileSpec>());

    while (i != end) {
        TileSpec spec = *i;

        bool created = false;
        ItemTileCacheEntry *entry = itemTileCache_.object(spec);
        if (entry == 0) {
            entry = new ItemTileCacheEntry(this);
            entry->viewport = mapPrivate_->specToRect(spec);
            created = true;
        }

        if (itemTree_.itemsWithin(entry->viewport).contains(item)) {

            if (!entry->items.contains(item)) {
                entry->items.append(item);
                itemTileUpdates_.insert(spec);
            }

            specs.insert(spec);
            if (created)
                itemTileCache_.insert(spec, entry);
        }

        ++i;
    }

    itemTileReverseMap_.insert(item, specs);

    updateMutex.unlock();
}

// Must not be called from rendering thread
void MapSphere::removeMapItem(MapItem *item)
{
    if (!item)
        return;
    updateMutex.lock();
    // nodes need to be removed from the scene node tree in GUI thread
    // because the tree has been created in it,
    // but the actual scene node deletion needs to occur in rendering thread
    // because it has the gl context
    if (item->glResources() && !obsoleteGLResources_.contains(item->glResources())) {
        obsoleteGLResources_.insert(item->glResources());
    }
    item->setGLResources(0);
    itemTree_.remove(item);

    QSet<TileSpec> tiles = itemTileReverseMap_.value(item);
    itemTileReverseMap_.remove(item);

    QSet<TileSpec>::const_iterator i = tiles.constBegin();
    QSet<TileSpec>::const_iterator end = tiles.constEnd();

    while (i != end) {
        TileSpec spec = *i;
        if (itemTileCache_.contains(spec)) {
            ItemTileCacheEntry *entry = itemTileCache_.object(spec);
            entry->items.removeAll(item);
            if (entry->items.isEmpty()) {
                QGLSceneNode *node = entry->resources.node;
                if (node && itemNode_->children().contains(node))
                    itemNode_->removeNode(node);
                itemTileCache_.remove(spec);
            } else {
                itemTileUpdates_.insert(spec);
            }
        }

        ++i;
    }

    updateMutex.unlock();
}

void MapSphere::clearMapItems()
{
    updateMutex.lock();
    QList<MapItem*> items = itemTree_.items();
    for (int i = 0; i < items.size(); ++i) {
        MapItem *item = items.at(i);
        if (item->glResources() && !obsoleteGLResources_.contains(item->glResources())) {
            obsoleteGLResources_.insert(item->glResources());
        }
        itemTree_.remove(item);
    }

    itemTileReverseMap_.clear();

    QList<TileSpec> keys = itemTileCache_.keys();
    QList<TileSpec>::const_iterator i = keys.constBegin();
    QList<TileSpec>::const_iterator end = keys.constEnd();

    itemNode_->removeNodes(itemNode_->children());

    while (i != end) {
        itemTileCache_.remove(*i);
        ++i;
    }

    updateMutex.unlock();
}

