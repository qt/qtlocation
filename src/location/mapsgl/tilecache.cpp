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
#include "tilecache.h"

#include "tile.h"
#include "tilespec.h"

#include "qgeomappingmanager.h"

#include <QDir>
#include <QRegExp>
#include <QThread>
#include <QMetaType>
#include <QDebug>

#include <Qt3D/qgltexture2d.h>
#include <Qt3D/qglscenenode.h>

Q_DECLARE_METATYPE(QList<TileSpec>)

struct TileDisk
{
    ~TileDisk()
    {
//        qWarning() << "evicting (disk) " << spec;
        cache->evictFromDiskCache(this);
    }

    TileSpec spec;
    QString filename;
    TileCache *cache;
};

struct TileMemory
{
    ~TileMemory()
    {
//        qWarning() << "evicting (memory) " << spec;
        cache->evictFromMemoryCache(this);
    }

    TileSpec spec;
    QPixmap pixmap;
    TileCache *cache;
};

struct TileTexture
{
    ~TileTexture()
    {
//        qWarning() << "evicting (texture) " << spec;
        cache->evictFromTextureCache(this);
    }

    TileSpec spec;
    bool bound;
    QGLTexture2D *texture;
    QGLSceneNode *node;
    TileCache *cache;
};

TileCache::TileCache(const QString &directory, QObject *parent)
:   QObject(parent), directory_(directory), manager_(0)
{
    qRegisterMetaType<TileSpec>();
    qRegisterMetaType<QList<TileSpec> >();


    if (directory_.isEmpty()) {
        QString dirname = QLatin1String(".tilecache");
        QDir home = QDir::home();
        if (!home.exists(dirname))
            home.mkdir(dirname);
        directory_ = home.filePath(dirname);
    }
    qDebug() << __FUNCTION__ << directory_;

    diskCache_.setMaxCost(100 * 1024 * 1024);
    memoryCache_.setMaxCost(50 * 1024 * 1024);
    textureCache_.setMaxCost(100 * 1024 * 1024);

    loadTiles();
}

TileCache::~TileCache()
{
    delete manager_;
}

void TileCache::setMappingManager(QGeoMappingManager *manager)
{
    manager_ = manager;
    connect(manager_,
            SIGNAL(tileFinished(TileSpec,QByteArray)),
            this,
            SLOT(insert(TileSpec,QByteArray)));
    connect(manager_,
            SIGNAL(tileError(TileSpec,QString)),
            this,
            SLOT(handleError(TileSpec,QString)));
    connect(manager_,
            SIGNAL(queueFinished()),
            this,
            SIGNAL(prefetchingFinished()));
}

void TileCache::setMaxDiskUsage(int diskUsage)
{
    diskCache_.setMaxCost(diskUsage);
}

int TileCache::maxDiskUsage() const
{
    return diskCache_.maxCost();
}

int TileCache::diskUsage() const
{
    return diskCache_.totalCost();
}

void TileCache::setMaxMemoryUsage(int memoryUsage)
{
    memoryCache_.setMaxCost(memoryUsage);
}

int TileCache::maxMemoryUsage() const
{
    return memoryCache_.maxCost();
}

int TileCache::memoryUsage() const
{
    return memoryCache_.totalCost();
}

void TileCache::setMaxTextureUsage(int textureUsage)
{
    textureCache_.setMaxCost(textureUsage);
}

int TileCache::maxTextureUsage() const
{
    return textureCache_.maxCost();
}

int TileCache::textureUsage() const
{
    return textureCache_.totalCost();
}

void TileCache::prefetch(const QList<TileSpec> &tiles)
{
    if (!manager_)
        return;

    manager_->requestTiles(tiles);
}

void TileCache::GLContextAvailable(QGLSceneNode *parentNode)
{
    int size = cleanupList_.size();
    for (int i = 0; i < size; ++i) {
        Tile t = cleanupList_.at(i);
        QGLSceneNode *node = t.sceneNode();
        if (node) {
            parentNode->removeNode(node);
            delete node;
        }
        QGLTexture2D *texture = t.texture();
        if (texture) {
            texture->release();
            delete texture;
        }
    }
    cleanupList_.clear();
}

bool TileCache::contains(const TileSpec &spec) const
{
    return keys_.contains(spec);
}

Tile TileCache::get(const TileSpec &spec)
{
    if (textureCache_.contains(spec)) {
        TileTexture *tt = textureCache_.object(spec);

        Tile t = Tile(tt->spec);
        t.setTexture(tt->texture);
        t.setSceneNode(tt->node);
        t.setBound(tt->bound);
        return t;
    }
//    if (memoryCache_.contains(spec)) {
//        TileMemory *tm = memoryCache_.object(spec);
//        TileTexture *tt = addToTextureCache(tm->spec, tm->pixmap);

//        Tile t = Tile(tt->spec);
//        t.setTexture(tt->texture);
//        t.setSceneNode(tt->node);
//        t.setBound(tt->bound);
//        return t;
//    }
    if (diskCache_.contains(spec)) {
        TileDisk *td = diskCache_.object(spec);
//        TileMemory *tm = addToMemoryCache(td->spec, QPixmap(td->filename));
//        TileTexture *tt = addToTextureCache(tm->spec, tm->pixmap);
        TileTexture *tt = addToTextureCache(td->spec, QPixmap(td->filename));

        Tile t = Tile(tt->spec);
        t.setTexture(tt->texture);
        t.setSceneNode(tt->node);
        t.setBound(tt->bound);
        return t;
    }

    return Tile();
}

// TODO rename so this is less strange
// OR do node creation in here somehow
void TileCache::update(const TileSpec &spec, const Tile &tile)
{
    TileTexture *tt = textureCache_.object(spec);
    if (tt) {
        tt->node = tile.sceneNode();
        tt->texture = tile.texture();
        tt->bound = tile.isBound();
    }
}

void TileCache::insert(const TileSpec &spec, const QByteArray &bytes)
{
    keys_.insert(spec);

    QString filename = tileSpecToFilename(spec, directory_);

    QPixmap pixmap;
    if (!pixmap.loadFromData(bytes)) {
        handleError(spec, QLatin1String("Problem with tile image"));
        return;
    }

    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    file.write(bytes);
    file.close();

    addToDiskCache(spec, filename);
//    addToMemoryCache(spec, pixmap);
    addToTextureCache(spec, pixmap);

    emit tileFetched(spec);
}

void TileCache::evictFromDiskCache(TileDisk *td)
{
    keys_.remove(td->spec);
    QFile::remove(td->filename);
}

void TileCache::evictFromMemoryCache(TileMemory */*tm*/)
{
}

void TileCache::evictFromTextureCache(TileTexture *tt)
{
    Tile t(tt->spec);
    t.setTexture(tt->texture);
    t.setSceneNode(tt->node);
    cleanupList_ << t;
}

TileDisk* TileCache::addToDiskCache(const TileSpec &spec, const QString &filename)
{
    keys_.insert(spec);

//    qWarning() << "adding (disk) " << spec;

    TileDisk *td = new TileDisk;
    td->spec = spec;
    td->filename = filename;
    td->cache = this;

    QFileInfo fi(filename);
    int diskCost = fi.size();

    diskCache_.insert(spec,
                      td,
                      diskCost);

    return td;
}

TileMemory* TileCache::addToMemoryCache(const TileSpec &spec, const QPixmap &pixmap)
{
    keys_.insert(spec);

//    qWarning() << "adding (memory) " << spec;

    TileMemory *tm = new TileMemory;
    tm->spec = spec;
    tm->pixmap = pixmap;
    tm->cache = this;

    int memoryCost = pixmap.width() * pixmap.height() * pixmap.depth() / 8;

    memoryCache_.insert(spec,
                        tm,
                        memoryCost);

    return tm;
}

TileTexture* TileCache::addToTextureCache(const TileSpec &spec, const QPixmap &pixmap)
{
    keys_.insert(spec);

//    qWarning() << "adding (texture) " << spec;

    TileTexture *tt = new TileTexture;
    tt->spec = spec;
    tt->texture = new QGLTexture2D();
    tt->texture->setPixmap(pixmap);
    tt->texture->setHorizontalWrap(QGL::ClampToEdge);
    tt->texture->setVerticalWrap(QGL::ClampToEdge);

//    tt->texture->bind();
//    tt->texture->clearImage();

    tt->node = 0;
    tt->cache = this;

    int textureCost = pixmap.width() * pixmap.height() * pixmap.depth() / 8;;

    textureCache_.insert(spec,
                         tt,
                         textureCost);

    return tt;
}

void TileCache::handleError(const TileSpec &, const QString &error)
{
    qWarning() << "tile request error " << error;
}

void TileCache::loadTiles()
{
    QStringList formats;
    formats << QLatin1String("*.png");

    QDir dir(directory_);
    //QStringList files = dir.entryList(formats, QDir::Files, QDir::Time | QDir::Reversed);
    QStringList files = dir.entryList(formats, QDir::Files);

    int tiles = 0;
    for (int i = 0; i < files.size(); ++i) {
        TileSpec spec = filenameToTileSpec(files.at(i));
        if (spec.zoom() == -1)
            continue;
        QString filename = dir.filePath(files.at(i));
        addToDiskCache(spec, filename);
        tiles++;
    }
    qDebug() << __FUNCTION__ << " loaded this many map tiles to cache: " << tiles;

}

QString TileCache::tileSpecToFilename(const TileSpec &spec, const QString &directory)
{
    QString filename = QString::number(spec.zoom());
    filename += QLatin1String("-");
    filename += QString::number(spec.x());
    filename += QLatin1String("-");
    filename += QString::number(spec.y());
    filename += QLatin1String(".png");

    QDir dir = QDir(directory);

    return dir.filePath(filename);
}

TileSpec TileCache::filenameToTileSpec(const QString &filename)
{
    TileSpec spec;
    QRegExp r(QLatin1String("(\\d+)-(\\d+)-(\\d+).png"));

    int index = r.indexIn(filename);
    if (index != -1) {
        bool ok = false;

        int zoom = r.cap(1).toInt(&ok);
        if (!ok)
            return spec;

        ok = false;
        int x = r.cap(2).toInt(&ok);
        if (!ok)
            return spec;

        ok = false;
        int y = r.cap(3).toInt(&ok);
        if (!ok)
            return spec;

        spec.setZoom(zoom);
        spec.setX(x);
        spec.setY(y);
    }

    return spec;
}
