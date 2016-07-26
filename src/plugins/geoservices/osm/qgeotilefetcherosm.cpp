/****************************************************************************
**
** Copyright (C) 2013 Aaron McCarthy <mccarthy.aaron@gmail.com>
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeotilefetcherosm.h"
#include "qgeomapreplyosm.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtLocation/private/qgeotilespec_p.h>


QT_BEGIN_NAMESPACE

static bool providersResolved(const QVector<QGeoTileProviderOsm *> &providers)
{
    foreach (const QGeoTileProviderOsm *provider, providers)
        if (!provider->isResolved())
            return false;
    return true;
}

QGeoTileFetcherOsm::QGeoTileFetcherOsm(const QVector<QGeoTileProviderOsm *> &providers,
                                       QNetworkAccessManager *nm,
                                       QObject *parent)
:   QGeoTileFetcher(parent), m_userAgent("Qt Location based application"),
    m_providers(providers), m_nm(nm), m_ready(true)
{
    m_nm->setParent(this);
    foreach (QGeoTileProviderOsm *provider, m_providers) {
        if (!provider->isResolved()) {
            m_ready = false;
            connect(provider, &QGeoTileProviderOsm::resolutionFinished,
                    this, &QGeoTileFetcherOsm::onProviderResolutionFinished);
            connect(provider, &QGeoTileProviderOsm::resolutionError,
                    this, &QGeoTileFetcherOsm::onProviderResolutionError);
            provider->resolveProvider();
        }
    }
    if (m_ready)
        readyUpdated();
}

void QGeoTileFetcherOsm::setUserAgent(const QByteArray &userAgent)
{
    m_userAgent = userAgent;
}

bool QGeoTileFetcherOsm::initialized() const
{
    if (!m_ready) {
        foreach (QGeoTileProviderOsm *provider, m_providers)
            if (!provider->isResolved())
                provider->resolveProvider();
    }
    return m_ready;
}

void QGeoTileFetcherOsm::onProviderResolutionFinished(const QGeoTileProviderOsm *provider)
{
    if ((m_ready = providersResolved(m_providers))) {
        qWarning("QGeoTileFetcherOsm: all providers resolved");
        readyUpdated();
    }
    emit providerDataUpdated(provider);
}

void QGeoTileFetcherOsm::onProviderResolutionError(const QGeoTileProviderOsm *provider, QNetworkReply::NetworkError error)
{
    Q_UNUSED(error)
    if ((m_ready = providersResolved(m_providers))) {
        qWarning("QGeoTileFetcherOsm: all providers resolved");
        readyUpdated();
    }
    emit providerDataUpdated(provider);
}

QGeoTiledMapReply *QGeoTileFetcherOsm::getTileImage(const QGeoTileSpec &spec)
{
    int id = spec.mapId();
    if (id < 1 || id > m_providers.size()) {
        qWarning("Unknown map id %d\n", spec.mapId());
        if (m_providers.isEmpty())
            return Q_NULLPTR;
        else
            id = 1;
    }
    id -= 1; // TODO: make OSM map ids start from 0.

    const QUrl url = m_providers[id]->tileAddress(spec.x(), spec.y(), spec.zoom());
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::UserAgentHeader, m_userAgent);
    request.setUrl(url);
    QNetworkReply *reply = m_nm->get(request);
    return new QGeoMapReplyOsm(reply, spec, m_providers[id]->format());
}

void QGeoTileFetcherOsm::readyUpdated()
{
    updateTileRequests(QSet<QGeoTileSpec>(), QSet<QGeoTileSpec>());
}

QT_END_NAMESPACE
