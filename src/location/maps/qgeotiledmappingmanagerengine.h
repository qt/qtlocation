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
** This file is part of the Ovi services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file OVI_SERVICES_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Ovi services
** plugin source code.
**
****************************************************************************/

#ifndef QGEOTILEDMAPPINGMANAGERENGINE_H
#define QGEOTILEDMAPPINGMANAGERENGINE_H

#include <QObject>
#include <QSize>
#include <QPair>
#include <QtLocation/qlocationglobal.h>
#include "qgeomaptype.h"
#include "qgeomappingmanagerengine.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QGeoTiledMappingManagerEnginePrivate;
class QGeoMapRequestOptions;
class QGeoTileFetcher;
class QGeoTileTexture;

class QGeoTileSpec;
class QGeoTiledMapData;
class QGeoTileCache;

class Q_LOCATION_EXPORT QGeoTiledMappingManagerEngine : public QGeoMappingManagerEngine
{
    Q_OBJECT

public:
    enum CacheArea {
        DiskCache = 0x01,
        MemoryCache = 0x02,
        AllCaches = 0xFF
    };
    Q_DECLARE_FLAGS(CacheAreas, CacheArea)

    QGeoTiledMappingManagerEngine(QObject *parent = 0);
    virtual ~QGeoTiledMappingManagerEngine();

    QGeoTileFetcher *tileFetcher();

    virtual QGeoMap* createMap(QObject *parent);

    void registerMap(QGeoTiledMapData *map);
    void deregisterMap(QGeoTiledMapData *map);

    QSize tileSize() const;

    void updateTileRequests(QGeoTiledMapData *map,
                            const QSet<QGeoTileSpec> &tilesAdded,
                            const QSet<QGeoTileSpec> &tilesRemoved);

    QGeoTileCache *tileCache(); // TODO: check this is still used
    QSharedPointer<QGeoTileTexture> getTileTexture(const QGeoTileSpec &spec);


    QGeoTiledMappingManagerEngine::CacheAreas cacheHint() const;

private Q_SLOTS:
    void engineTileFinished(const QGeoTileSpec &spec, const QByteArray &bytes, const QString &format);
    void engineTileError(const QGeoTileSpec &spec, const QString &errorString);

Q_SIGNALS:
    void tileError(const QGeoTileSpec &spec, const QString &errorString);

protected:
    void setTileFetcher(QGeoTileFetcher *fetcher);
    void setTileSize(const QSize &tileSize);
    void setCacheHint(QGeoTiledMappingManagerEngine::CacheAreas cacheHint);

    QGeoTileCache *createTileCacheWithDir(const QString& cacheDirectory);

private:
    QGeoTiledMappingManagerEnginePrivate* d_ptr;

    Q_DECLARE_PRIVATE(QGeoTiledMappingManagerEngine)
    Q_DISABLE_COPY(QGeoTiledMappingManagerEngine)

    friend class QGeoTileFetcher;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGeoTiledMappingManagerEngine::CacheAreas)

QT_END_NAMESPACE

QT_END_HEADER

#endif
