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

#ifndef QGEOTILEFETCHER_H
#define QGEOTILEFETCHER_H

#include <QObject>
#include <qlocationglobal.h>
#include "qgeomaptype.h"
#include "qgeotiledmappingmanagerengine.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QGeoMapRequestOptions;

class QGeoTileFetcherPrivate;
class QGeoTiledMappingManagerEngine;
class QGeoTiledMapReply;
class QGeoTileSpec;

class Q_LOCATION_EXPORT QGeoTileFetcher : public QObject
{
    Q_OBJECT

public:
    QGeoTileFetcher(QGeoTiledMappingManagerEngine *engine, QObject *parent = 0);
    virtual ~QGeoTileFetcher();

public Q_SLOTS:
    void threadStarted();
    void threadFinished();
    void updateTileRequests(const QSet<QGeoTileSpec> &tilesAdded, const QSet<QGeoTileSpec> &tilesRemoved);

private Q_SLOTS:
    void cancelTileRequests(const QSet<QGeoTileSpec> &tiles);
    void requestNextTile();
    void finished();

Q_SIGNALS:
    void tileFinished(const QGeoTileSpec &spec, const QByteArray &bytes, const QString &format);
    void tileError(const QGeoTileSpec &spec, const QString &errorString);

protected:
    virtual bool init();
    QGeoTiledMappingManagerEngine::CacheAreas cacheHint() const;

private:
    QGeoTileFetcherPrivate *d_ptr;

    virtual QGeoTiledMapReply *getTileImage(const QGeoTileSpec &spec) = 0;
    void handleReply(QGeoTiledMapReply *reply, const QGeoTileSpec &spec);

    Q_DECLARE_PRIVATE(QGeoTileFetcher)
    Q_DISABLE_COPY(QGeoTileFetcher)

    friend class QGeoTiledMappingManagerEngine;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
