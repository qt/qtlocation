/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QGEOFILETILECACHE_P_H
#define QGEOFILETILECACHE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtLocation/private/qlocationglobal_p.h>

#include <QObject>
#include "qcache3q_p.h"

#include "qabstractgeotilecache_p.h"

QT_BEGIN_NAMESPACE

class QGeoMappingManager;

class QGeoTile;
class QGeoCachedTileMemory;
class QGeoFileTileCache;

class QImage;

/* This would be internal to qgeofiletilecache.cpp except that the eviction
 * policy can't be defined without it being concrete here */
class QGeoCachedTileDisk
{
public:
    ~QGeoCachedTileDisk();

    QGeoTileSpec spec;
    QString filename;
    QString format;
    QGeoFileTileCache *cache = nullptr;
};

/* Custom eviction policy for the disk cache, to avoid deleting all the files
 * when the application closes */
class Q_LOCATION_PRIVATE_EXPORT QCache3QTileEvictionPolicy : public QCache3QDefaultEvictionPolicy<QGeoTileSpec,QGeoCachedTileDisk>
{
protected:
    void aboutToBeRemoved(const QGeoTileSpec &key, QSharedPointer<QGeoCachedTileDisk> obj);
    void aboutToBeEvicted(const QGeoTileSpec &key, QSharedPointer<QGeoCachedTileDisk> obj);
};

class Q_LOCATION_PRIVATE_EXPORT QGeoFileTileCache : public QAbstractGeoTileCache
{
    Q_OBJECT
public:
    QGeoFileTileCache(const QString &directory = QString(), QObject *parent = nullptr);
    ~QGeoFileTileCache();

    void setMaxDiskUsage(int diskUsage) override;
    int maxDiskUsage() const override;
    int diskUsage() const override;

    void setMaxMemoryUsage(int memoryUsage) override;
    int maxMemoryUsage() const override;
    int memoryUsage() const override;

    void setMinTextureUsage(int textureUsage) override;
    void setExtraTextureUsage(int textureUsage) override;
    int maxTextureUsage() const override;
    int minTextureUsage() const override;
    int textureUsage() const override;
    void clearAll() override;
    void clearMapId(int mapId);
    void setCostStrategyDisk(CostStrategy costStrategy) override;
    CostStrategy costStrategyDisk() const override;
    void setCostStrategyMemory(CostStrategy costStrategy) override;
    CostStrategy costStrategyMemory() const override;
    void setCostStrategyTexture(CostStrategy costStrategy) override;
    CostStrategy costStrategyTexture() const override;


    QSharedPointer<QGeoTileTexture> get(const QGeoTileSpec &spec) override;

    // can be called without a specific tileCache pointer
    static void evictFromDiskCache(QGeoCachedTileDisk *td);
    static void evictFromMemoryCache(QGeoCachedTileMemory *tm);

    void insert(const QGeoTileSpec &spec,
                const QByteArray &bytes,
                const QString &format,
                QAbstractGeoTileCache::CacheAreas areas = QAbstractGeoTileCache::AllCaches) override;

    static QString tileSpecToFilenameDefault(const QGeoTileSpec &spec, const QString &format, const QString &directory);
    static QGeoTileSpec filenameToTileSpecDefault(const QString &filename);

protected:
    void init() override;
    void printStats() override;
    void loadTiles();

    QString directory() const;

    QSharedPointer<QGeoCachedTileDisk> addToDiskCache(const QGeoTileSpec &spec, const QString &filename);
    bool addToDiskCache(const QGeoTileSpec &spec, const QString &filename, const QByteArray &bytes);
    void addToMemoryCache(const QGeoTileSpec &spec, const QByteArray &bytes, const QString &format);
    QSharedPointer<QGeoTileTexture> addToTextureCache(const QGeoTileSpec &spec, const QImage &image);
    QSharedPointer<QGeoTileTexture> getFromMemory(const QGeoTileSpec &spec);
    QSharedPointer<QGeoTileTexture> getFromDisk(const QGeoTileSpec &spec);

    virtual bool isTileBogus(const QByteArray &bytes) const;
    virtual QString tileSpecToFilename(const QGeoTileSpec &spec, const QString &format, const QString &directory) const;
    virtual QGeoTileSpec filenameToTileSpec(const QString &filename) const;

    QCache3Q<QGeoTileSpec, QGeoCachedTileDisk, QCache3QTileEvictionPolicy> diskCache_;
    QCache3Q<QGeoTileSpec, QGeoCachedTileMemory> memoryCache_;
    QCache3Q<QGeoTileSpec, QGeoTileTexture> textureCache_;

    QString directory_;

    int minTextureUsage_ = 0;
    int extraTextureUsage_ = 0;
    CostStrategy costStrategyDisk_ = ByteSize;
    CostStrategy costStrategyMemory_ = ByteSize;
    CostStrategy costStrategyTexture_ = ByteSize;
    bool isDiskCostSet_ = false;
    bool isMemoryCostSet_ = false;
    bool isTextureCostSet_ = false;
};

QT_END_NAMESPACE

#endif // QGEOFILETILECACHE_P_H
