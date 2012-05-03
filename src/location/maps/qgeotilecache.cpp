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

class QGeoCachedTileMemory
{
public:
    ~QGeoCachedTileMemory()
    {
        if (cache)
            cache->evictFromMemoryCache(this);
    }

    QGeoTileSpec spec;
    QGeoTileCache *cache;
    QByteArray bytes;
    QString format;
};

QGeoTileTexture::QGeoTileTexture()
    : texture(0),
      cache(0),
      textureBound(false) {}

void QCache3QTileEvictionPolicy::aboutToBeRemoved(const QGeoTileSpec &key, QSharedPointer<QGeoCachedTileDisk> obj)
{
    Q_UNUSED(key);
    // set the cache pointer to zero so we can't call evictFromDiskCache
    obj->cache = 0;
}

void QCache3QTileEvictionPolicy::aboutToBeEvicted(const QGeoTileSpec &key, QSharedPointer<QGeoCachedTileDisk> obj)
{
    Q_UNUSED(key);
    Q_UNUSED(obj);
    // leave the pointer set if it's a real eviction
}

QGeoCachedTileDisk::~QGeoCachedTileDisk()
{
    if (cache)
        cache->evictFromDiskCache(this);
}

QGeoTileTexture::~QGeoTileTexture()
{
    if (cache)
        cache->evictFromTextureCache(this);
}

QGeoTileCache::QGeoTileCache(const QString &directory, QObject *parent)
    : QObject(parent), directory_(directory),
      minTextureUsage_(0), extraTextureUsage_(0)
{
    qRegisterMetaType<QGeoTileSpec>();
    qRegisterMetaType<QList<QGeoTileSpec> >();
    qRegisterMetaType<QSet<QGeoTileSpec> >();

    // We keep default values here so that they are in one place
    // rather than in each individual plugin (the plugins can
    // of course override them)

    if (directory_.isEmpty()) {
        QString dirname = QLatin1String(".tilecache");
        QDir home = QDir::home();
        if (!home.exists(dirname))
            home.mkdir(dirname);
        directory_ = home.filePath(dirname);
    }

    // default values
    setMaxDiskUsage(20 * 1024 * 1024);
    setMaxMemoryUsage(3 * 1024 * 1024);
    setExtraTextureUsage(6 * 1024 * 1024);

    loadTiles();
}

QGeoTileCache::~QGeoTileCache() {}

void QGeoTileCache::printStats()
{
    textureCache_.printStats();
    memoryCache_.printStats();
    diskCache_.printStats();
}

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

void QGeoTileCache::setExtraTextureUsage(int textureUsage)
{
    extraTextureUsage_ = textureUsage;
    textureCache_.setMaxCost(minTextureUsage_ + extraTextureUsage_);
}

void QGeoTileCache::setMinTextureUsage(int textureUsage)
{
    minTextureUsage_ = textureUsage;
    textureCache_.setMaxCost(minTextureUsage_ + extraTextureUsage_);
}

int QGeoTileCache::maxTextureUsage() const
{
    return textureCache_.maxCost();
}

int QGeoTileCache::minTextureUsage() const
{
    return minTextureUsage_;
}


int QGeoTileCache::textureUsage() const
{
    return textureCache_.totalCost();
}

void QGeoTileCache::GLContextAvailable()
{
    QMutexLocker ml(&cleanupMutex_);

    /* Throttle the cleanup to 10 items/frame to avoid blocking the render
     * for too long. Normally only 6-20 tiles are on screen at a time so
     * eviction rates shouldn't be much higher than this. */
    int todo = qMin(cleanupList_.size(), 10);
    for (int i = 0; i < todo; ++i) {
        QGLTexture2D *texture = cleanupList_.front();
        if (texture) {
            texture->release();
            texture->cleanupResources();
            delete texture;
        }
        cleanupList_.pop_front();
    }
}

QSharedPointer<QGeoTileTexture> QGeoTileCache::get(const QGeoTileSpec &spec)
{
    QSharedPointer<QGeoTileTexture> tt = textureCache_.object(spec);
    if (tt)
        return tt;

    QSharedPointer<QGeoCachedTileMemory> tm = memoryCache_.object(spec);
    if (tm) {
        QPixmap pixmap;
        if (!pixmap.loadFromData(tm->bytes)) {
            handleError(spec, QLatin1String("Problem with tile image"));
            return QSharedPointer<QGeoTileTexture>(0);
        }
        QSharedPointer<QGeoTileTexture> tt = addToTextureCache(spec, pixmap);
        if (tt)
            return tt;
    }

    QSharedPointer<QGeoCachedTileDisk> td = diskCache_.object(spec);
    if (td) {
        QStringList parts = td->filename.split('.');
        QFile file(td->filename);
        file.open(QIODevice::ReadOnly);
        QByteArray bytes = file.readAll();
        file.close();

        QPixmap pixmap;
        const char *format = (parts.size() == 2 ? parts.at(1).toLocal8Bit().constData() : 0);
        if (!pixmap.loadFromData(bytes, format)) {
            handleError(spec, QLatin1String("Problem with tile image"));
            return QSharedPointer<QGeoTileTexture>(0);
        }

        addToMemoryCache(spec, bytes, (parts.size() == 2 ? parts.at(1) : QLatin1String("")));
        QSharedPointer<QGeoTileTexture> tt = addToTextureCache(td->spec, pixmap);
        if (tt)
            return tt;
    }

    return QSharedPointer<QGeoTileTexture>();
}

void QGeoTileCache::insert(const QGeoTileSpec &spec,
                           const QByteArray &bytes,
                           const QString &format,
                           QGeoTiledMappingManagerEngine::CacheAreas areas)
{
    if (areas & QGeoTiledMappingManagerEngine::DiskCache) {
        QString filename = tileSpecToFilename(spec, format, directory_);

        QFile file(filename);
        file.open(QIODevice::WriteOnly);
        file.write(bytes);
        file.close();

        addToDiskCache(spec, filename);
    }

    if (areas & QGeoTiledMappingManagerEngine::MemoryCache) {
        addToMemoryCache(spec, bytes, format);
    }

    /* inserts do not hit the texture cache -- this actually reduces overall
     * cache hit rates because many tiles come too late to be useful
     * and act as a poison */
}

void QGeoTileCache::evictFromDiskCache(QGeoCachedTileDisk *td)
{
    QFile::remove(td->filename);
}

void QGeoTileCache::evictFromMemoryCache(QGeoCachedTileMemory * /* tm  */)
{
}

void QGeoTileCache::evictFromTextureCache(QGeoTileTexture *tt)
{
    QMutexLocker ml(&cleanupMutex_);
    cleanupList_ << tt->texture;
}

QSharedPointer<QGeoCachedTileDisk> QGeoTileCache::addToDiskCache(const QGeoTileSpec &spec, const QString &filename)
{
    QSharedPointer<QGeoCachedTileDisk> td(new QGeoCachedTileDisk);
    td->spec = spec;
    td->filename = filename;
    td->cache = this;

    QFileInfo fi(filename);
    int diskCost = fi.size();
    diskCache_.insert(spec, td, diskCost);

    return td;
}

QSharedPointer<QGeoCachedTileMemory> QGeoTileCache::addToMemoryCache(const QGeoTileSpec &spec, const QByteArray &bytes, const QString &format)
{
    QSharedPointer<QGeoCachedTileMemory> tm(new QGeoCachedTileMemory);
    tm->spec = spec;
    tm->cache = this;
    tm->bytes = bytes;
    tm->format = format;

    int cost = bytes.size();
    memoryCache_.insert(spec, tm, cost);

    return tm;
}

QSharedPointer<QGeoTileTexture> QGeoTileCache::addToTextureCache(const QGeoTileSpec &spec, const QPixmap &pixmap)
{
    QSharedPointer<QGeoTileTexture> tt(new QGeoTileTexture);
    tt->spec = spec;
    tt->texture = new QGLTexture2D();
    tt->texture->setPixmap(pixmap);
    tt->texture->setHorizontalWrap(QGL::ClampToEdge);
    tt->texture->setVerticalWrap(QGL::ClampToEdge);
    tt->cache = this;

    /* Do not bind/cleanImage on the texture here -- it needs to be done
     * in the render thread (by qgeomapscene) */

    int textureCost = pixmap.width() * pixmap.height() * pixmap.depth() / 8;
    textureCache_.insert(spec, tt, textureCost);

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
