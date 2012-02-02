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
#include "qgeotilecache_p.h"

#include "qgeotilespec.h"

#include "qgeomappingmanager.h"

#include <QDir>
#include <QMetaType>
#include <QPixmap>
#include <QDebug>

#include <Qt3D/qgltexture2d.h>

Q_DECLARE_METATYPE(QList<QGeoTileSpec>)
Q_DECLARE_METATYPE(QSet<QGeoTileSpec>)

QT_BEGIN_NAMESPACE

class TileDisk
{
public:
    ~TileDisk()
    {
//        qWarning() << "evicting (disk) " << spec;
//        cache->evictFromDiskCache(this);
    }

    QGeoTileSpec spec;
    QString filename;
    QGeoTileCache *cache;
};

class TileMemory
{
public:
    ~TileMemory()
    {
//        qWarning() << "evicting (memory) " << spec;
        cache->evictFromMemoryCache(this);
    }

    QGeoTileSpec spec;
    QPixmap pixmap;
    QGeoTileCache *cache;
};

class TileTexture {
public:
    ~TileTexture()
    {
//        qWarning() << "evicting (texture) " << spec;
        cache->evictFromTextureCache(this);
    }

    QGeoTileSpec spec;
    QGLTexture2D *texture;
    QGeoTileCache *cache;
};

QGeoTileCache::QGeoTileCache(const QString &directory, QObject *parent)
:   QObject(parent), directory_(directory)
{
    qRegisterMetaType<QGeoTileSpec>();
    qRegisterMetaType<QList<QGeoTileSpec> >();
    qRegisterMetaType<QSet<QGeoTileSpec> >();


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

QGeoTileCache::~QGeoTileCache() {}

void QGeoTileCache::setMaxDiskUsage(int diskUsage)
{
    diskCache_.setMaxCost(diskUsage);
}

int QGeoTileCache::maxDiskUsage() const
{
    return diskCache_.maxCost();
}

int QGeoTileCache::diskUsage() const
{
    return diskCache_.totalCost();
}

void QGeoTileCache::setMaxMemoryUsage(int memoryUsage)
{
    memoryCache_.setMaxCost(memoryUsage);
}

int QGeoTileCache::maxMemoryUsage() const
{
    return memoryCache_.maxCost();
}

int QGeoTileCache::memoryUsage() const
{
    return memoryCache_.totalCost();
}

void QGeoTileCache::setMaxTextureUsage(int textureUsage)
{
    textureCache_.setMaxCost(textureUsage);
}

int QGeoTileCache::maxTextureUsage() const
{
    return textureCache_.maxCost();
}

int QGeoTileCache::textureUsage() const
{
    return textureCache_.totalCost();
}

void QGeoTileCache::GLContextAvailable()
{
    int size = cleanupList_.size();
    for (int i = 0; i < size; ++i) {
        QGLTexture2D* texture = cleanupList_.at(i);
        if (texture) {
            texture->release();
            delete texture;
        }
    }
    cleanupList_.clear();
}

bool QGeoTileCache::contains(const QGeoTileSpec &spec) const
{
    return keys_.contains(spec);
}

QGLTexture2D* QGeoTileCache::get(const QGeoTileSpec &spec)
{
    if (textureCache_.contains(spec)) {
        TileTexture *tt = textureCache_.object(spec);
        return tt->texture;
    }
//    if (memoryCache_.contains(spec)) {
//        TileMemory *tm = memoryCache_.object(spec);
//        TileTexture *tt = addToTextureCache(tm->spec, tm->pixmap);
//        return tt->texture;
//    }
    if (diskCache_.contains(spec)) {
        TileDisk *td = diskCache_.object(spec);
//        TileMemory *tm = addToMemoryCache(td->spec, QPixmap(td->filename));
        TileTexture *tt = addToTextureCache(td->spec, QPixmap(td->filename));
        return tt->texture;
    }

    return 0;
}

void QGeoTileCache::insert(const QGeoTileSpec &spec,
                           const QByteArray &bytes,
                           const QString &format,
                           QGeoMappingManager::CacheAreas areas)
{
    keys_.insert(spec);

    QPixmap pixmap;
    // TODO use format string here to hint to the loading code?
    if (!pixmap.loadFromData(bytes)) {
        handleError(spec, QLatin1String("Problem with tile image"));
        return;
    }

    if (areas & QGeoMappingManager::DiskCache) {
        QString filename = tileSpecToFilename(spec, format, directory_);

        QFile file(filename);
        file.open(QIODevice::WriteOnly);
        file.write(bytes);
        file.close();

        addToDiskCache(spec, filename);
    }

    if (areas & QGeoMappingManager::MemoryCache) {
//        addToMemoryCache(spec, pixmap);
    }

    if (areas & QGeoMappingManager::TextureCache) {
        addToTextureCache(spec, pixmap);
    }
}

void QGeoTileCache::evictFromDiskCache(TileDisk *td)
{
    keys_.remove(td->spec);
    QFile::remove(td->filename);
}

void QGeoTileCache::evictFromMemoryCache(TileMemory * /* tm  */)
{
}

void QGeoTileCache::evictFromTextureCache(TileTexture *tt)
{
    cleanupList_ << tt->texture;
}

TileDisk* QGeoTileCache::addToDiskCache(const QGeoTileSpec &spec, const QString &filename)
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

TileMemory* QGeoTileCache::addToMemoryCache(const QGeoTileSpec &spec, const QPixmap &pixmap)
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

TileTexture* QGeoTileCache::addToTextureCache(const QGeoTileSpec &spec, const QPixmap &pixmap)
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

    tt->cache = this;

    int textureCost = pixmap.width() * pixmap.height() * pixmap.depth() / 8;;

    textureCache_.insert(spec,
                         tt,
                         textureCost);

    return tt;
}

void QGeoTileCache::handleError(const QGeoTileSpec &, const QString &error)
{
    qWarning() << "tile request error " << error;
}

void QGeoTileCache::loadTiles()
{
    QStringList formats;
    //formats << QLatin1String("*.png");
    formats << QLatin1String("*.*");

    QDir dir(directory_);
    //QStringList files = dir.entryList(formats, QDir::Files, QDir::Time | QDir::Reversed);
    QStringList files = dir.entryList(formats, QDir::Files);
    int tiles = 0;
    for (int i = 0; i < files.size(); ++i) {
        QGeoTileSpec spec = filenameToTileSpec(files.at(i));
        if (spec.zoom() == -1)
            continue;
        QString filename = dir.filePath(files.at(i));
        addToDiskCache(spec, filename);
        tiles++;
    }
    qDebug() << __FUNCTION__ << " loaded this many map tiles to cache: " << tiles;

}

QString QGeoTileCache::tileSpecToFilename(const QGeoTileSpec &spec, const QString &format, const QString &directory)
{
    QString filename = spec.plugin();
    filename += QLatin1String("-");
    filename += QString::number(spec.mapId());
    filename += QLatin1String("-");
    filename += QString::number(spec.zoom());
    filename += QLatin1String("-");
    filename += QString::number(spec.x());
    filename += QLatin1String("-");
    filename += QString::number(spec.y());
    filename += QLatin1String(".");
    filename += format;

    QDir dir = QDir(directory);

    return dir.filePath(filename);
}

QGeoTileSpec QGeoTileCache::filenameToTileSpec(const QString &filename)
{
    QGeoTileSpec emptySpec;

    QStringList parts = filename.split('.');

    if (parts.length() != 2)
        return emptySpec;

    QString name = parts.at(0);
    QStringList fields = name.split('-');

    if (fields.length() != 5)
        return emptySpec;

    QList<int> numbers;

    bool ok = false;
    for (int i = 1; i < 5; ++i) {
        ok = false;
        int value = fields.at(i).toInt(&ok);
        if (!ok)
            return emptySpec;
        numbers.append(value);
    }

    return QGeoTileSpec(fields.at(0),
                    numbers.at(0),
                    numbers.at(1),
                    numbers.at(2),
                    numbers.at(3));
}

QT_END_NAMESPACE
