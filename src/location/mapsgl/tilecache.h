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
#ifndef TILECACHE_H
#define TILECACHE_H

#include <QtLocation/qlocationglobal.h>

#include <QObject>
#include <QCache>
#include <QSet>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE



class QGeoMappingManager;

class TileSpec;
class Tile;

class TileDisk;
class TileMemory;
class TileTexture;

class QGLSceneNode;

class QPixmap;
class QThread;

class Q_LOCATION_EXPORT TileCache : public QObject
{
    Q_OBJECT
public:
    TileCache(const QString &directory = QString(), QObject *parent = 0);
    ~TileCache();

    void setMappingManager(QGeoMappingManager *manager);

    void setMaxDiskUsage(int diskUsage);
    int maxDiskUsage() const;
    int diskUsage() const;

    void setMaxMemoryUsage(int memoryUsage);
    int maxMemoryUsage() const;
    int memoryUsage() const;

    void setMaxTextureUsage(int textureUsage);
    int maxTextureUsage() const;
    int textureUsage() const;

    void GLContextAvailable(QGLSceneNode *parentNode);

    bool contains(const TileSpec &spec) const;
    Tile get(const TileSpec &spec);

    void update(const TileSpec &spec, const Tile &tile);

    void prefetch(const QList<TileSpec> &tiles);

    void evictFromDiskCache(TileDisk *td);
    void evictFromMemoryCache(TileMemory *tm);
    void evictFromTextureCache(TileTexture *tt);

private Q_SLOTS:
    void insert(const TileSpec &spec, const QByteArray &bytes);
    void handleError(const TileSpec &spec, const QString &errorString);

Q_SIGNALS:
    void beginPrefetch(const QList<TileSpec> &tiles);
    void tileFetched(const TileSpec &spec);
    void prefetchingFinished();

private:
    void loadTiles();

    TileDisk* addToDiskCache(const TileSpec &spec, const QString &filename);
    TileMemory* addToMemoryCache(const TileSpec &spec, const QPixmap &pixmap);
    TileTexture* addToTextureCache(const TileSpec &spec, const QPixmap &pixmap);

    static QString tileSpecToFilename(const TileSpec &spec, const QString &directory);
    static TileSpec filenameToTileSpec(const QString &filename);

    QString directory_;

    QSet<TileSpec> keys_;
    QCache<TileSpec, TileDisk > diskCache_;
    QCache<TileSpec, TileMemory > memoryCache_;
    QCache<TileSpec, TileTexture > textureCache_;

    QList<Tile> cleanupList_;

    QGeoMappingManager *manager_;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // TILECACHE_H
