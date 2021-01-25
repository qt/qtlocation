/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include <QtCore/QTimerEvent>

#include "qgeomappingmanagerengine_p.h"
#include "qgeotilefetcher_p.h"
#include "qgeotilefetcher_p_p.h"
#include "qgeotiledmapreply_p.h"
#include "qgeotilespec_p.h"
#include "qgeotiledmap_p.h"

#include <algorithm>
#include <iterator>

QT_BEGIN_NAMESPACE

QGeoTileFetcher::QGeoTileFetcher(QGeoMappingManagerEngine *parent)
:   QObject(*new QGeoTileFetcherPrivate(), parent)
{
    Q_D(QGeoTileFetcher);

    d->enabled_ = true;
    d->engine_ = parent;
}

QGeoTileFetcher::QGeoTileFetcher(QGeoTileFetcherPrivate &dd, QGeoMappingManagerEngine *parent)
:   QObject(dd,parent)
{
    Q_D(QGeoTileFetcher);
    d->enabled_ = true;
    d->engine_ = parent;
}

QGeoTileFetcher::~QGeoTileFetcher()
{
}

void QGeoTileFetcher::updateTileRequests(const QSet<QGeoTileSpec> &tilesAdded,
                                                  const QSet<QGeoTileSpec> &tilesRemoved)
{
    Q_D(QGeoTileFetcher);

    QMutexLocker ml(&d->queueMutex_);

    cancelTileRequests(tilesRemoved);

    std::copy(tilesAdded.cbegin(), tilesAdded.cend(), std::back_inserter(d->queue_));

    if (d->enabled_ && initialized() && !d->queue_.isEmpty() && !d->timer_.isActive())
        d->timer_.start(0, this);
}

void QGeoTileFetcher::cancelTileRequests(const QSet<QGeoTileSpec> &tiles)
{
    Q_D(QGeoTileFetcher);

    typedef QSet<QGeoTileSpec>::const_iterator tile_iter;
    // No need to lock: called only in updateTileRequests
    tile_iter tile = tiles.constBegin();
    tile_iter end = tiles.constEnd();
    for (; tile != end; ++tile) {
        QGeoTiledMapReply *reply = d->invmap_.value(*tile, 0);
        if (reply) {
            d->invmap_.remove(*tile);
            reply->abort();
            if (reply->isFinished())
                reply->deleteLater();
        }
        d->queue_.removeAll(*tile);
    }
}

void QGeoTileFetcher::requestNextTile()
{
    Q_D(QGeoTileFetcher);

    QMutexLocker ml(&d->queueMutex_);

    if (!d->enabled_)
        return;

    if (d->queue_.isEmpty())
        return;

    QGeoTileSpec ts = d->queue_.takeFirst();
    if (d->queue_.isEmpty())
        d->timer_.stop();

    // Check against min/max zoom to prevent sending requests for not existing objects
    const QGeoCameraCapabilities & cameraCaps = d->engine_->cameraCapabilities(ts.mapId());
    // the ZL in QGeoTileSpec is relative to the native tile size of the provider.
    // It gets denormalized in QGeoTiledMap.
    if (ts.zoom() < cameraCaps.minimumZoomLevel() || ts.zoom() > cameraCaps.maximumZoomLevel() || !fetchingEnabled())
        return;

    QGeoTiledMapReply *reply = getTileImage(ts);
    if (!reply)
        return;

    if (reply->isFinished()) {
        handleReply(reply, ts);
    } else {
        connect(reply,
                SIGNAL(finished()),
                this,
                SLOT(finished()),
                Qt::QueuedConnection);

        d->invmap_.insert(ts, reply);
    }
}

void QGeoTileFetcher::finished()
{
    Q_D(QGeoTileFetcher);

    QMutexLocker ml(&d->queueMutex_);

    QGeoTiledMapReply *reply = qobject_cast<QGeoTiledMapReply *>(sender());
    if (!reply)
        return;

    QGeoTileSpec spec = reply->tileSpec();

    if (!d->invmap_.contains(spec)) {
        reply->deleteLater();
        return;
    }

    d->invmap_.remove(spec);

    handleReply(reply, spec);
}

void QGeoTileFetcher::timerEvent(QTimerEvent *event)
{
    Q_D(QGeoTileFetcher);
    if (event->timerId() != d->timer_.timerId()) {
        QObject::timerEvent(event);
        return;
    }

    QMutexLocker ml(&d->queueMutex_);
    if (d->queue_.isEmpty() || !initialized()) {
        d->timer_.stop();
        return;
    }
    ml.unlock();

    requestNextTile();
}

bool QGeoTileFetcher::initialized() const
{
    return true;
}

bool QGeoTileFetcher::fetchingEnabled() const
{
    return true;
}

void QGeoTileFetcher::handleReply(QGeoTiledMapReply *reply, const QGeoTileSpec &spec)
{
    Q_D(QGeoTileFetcher);

    if (!d->enabled_) {
        reply->deleteLater();
        return;
    }

    if (reply->error() == QGeoTiledMapReply::NoError) {
        emit tileFinished(spec, reply->mapImageData(), reply->mapImageFormat());
    } else {
        emit tileError(spec, reply->errorString());
    }

    reply->deleteLater();
}

/*******************************************************************************
*******************************************************************************/

QGeoTileFetcherPrivate::QGeoTileFetcherPrivate()
:   QObjectPrivate(), enabled_(false), engine_(0)
{
}

QGeoTileFetcherPrivate::~QGeoTileFetcherPrivate()
{
}

QT_END_NAMESPACE
