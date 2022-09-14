/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
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

#ifndef QGEOTILEDMAPPINGMANAGERENGINE_H
#define QGEOTILEDMAPPINGMANAGERENGINE_H

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

#include <QObject>
#include <QtLocation/private/qlocationglobal_p.h>
#include <QtLocation/private/qabstractgeotilecache_p.h>
#include <QtLocation/private/qgeomappingmanagerengine_p.h>
#include <QtLocation/private/qgeotiledmap_p.h>


QT_BEGIN_NAMESPACE

class QGeoTiledMappingManagerEnginePrivate;
class QGeoTileFetcher;
struct QGeoTileTexture;
class QGeoTileSpec;
class QSize;

class Q_LOCATION_PRIVATE_EXPORT QGeoTiledMappingManagerEngine : public QGeoMappingManagerEngine
{
    Q_OBJECT

public:
    explicit QGeoTiledMappingManagerEngine(QObject *parent = nullptr);
    virtual ~QGeoTiledMappingManagerEngine();

    QGeoTileFetcher *tileFetcher();

    QGeoMap *createMap() override;
    void releaseMap(QGeoTiledMap *map);

    QSize tileSize() const;
    int tileVersion() const;

    virtual void updateTileRequests(QGeoTiledMap *map,
                            const QSet<QGeoTileSpec> &tilesAdded,
                            const QSet<QGeoTileSpec> &tilesRemoved);

    QAbstractGeoTileCache *tileCache();
    virtual QSharedPointer<QGeoTileTexture> getTileTexture(const QGeoTileSpec &spec);

    QAbstractGeoTileCache::CacheAreas cacheHint() const;

protected Q_SLOTS:
    virtual void engineTileFinished(const QGeoTileSpec &spec, const QByteArray &bytes, const QString &format);
    virtual void engineTileError(const QGeoTileSpec &spec, const QString &errorString);

Q_SIGNALS:
    void tileError(const QGeoTileSpec &spec, const QString &errorString);
    void tileVersionChanged();

protected:
    void setTileFetcher(QGeoTileFetcher *fetcher);
    void setTileSize(const QSize &tileSize);
    void setTileVersion(int version);
    void setCacheHint(QAbstractGeoTileCache::CacheAreas cacheHint);
    void setTileCache(QAbstractGeoTileCache *cache);

    QGeoTiledMap::PrefetchStyle m_prefetchStyle = QGeoTiledMap::PrefetchTwoNeighbourLayers;
    QGeoTiledMappingManagerEnginePrivate *d_ptr;

    Q_DECLARE_PRIVATE(QGeoTiledMappingManagerEngine)
    Q_DISABLE_COPY(QGeoTiledMappingManagerEngine)

    friend class QGeoTileFetcher;
};

QT_END_NAMESPACE

#endif
