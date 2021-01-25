/****************************************************************************
**
** Copyright (C) 2013-2018 Esri <contracts@esri.com>
** Contact: https://www.qt.io/licensing/
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
**
**
**
****************************************************************************/

#include "geotilefetcher_esri.h"
#include "geotiledmappingmanagerengine_esri.h"
#include "geotiledmapreply_esri.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>

#include <QtLocation/private/qgeotilespec_p.h>

QT_BEGIN_NAMESPACE

GeoTileFetcherEsri::GeoTileFetcherEsri(QGeoTiledMappingManagerEngine *parent) :
    QGeoTileFetcher(parent), m_networkManager(new QNetworkAccessManager(this)),
    m_userAgent(QByteArrayLiteral("Qt Location based application"))
{
}

QGeoTiledMapReply *GeoTileFetcherEsri::getTileImage(const QGeoTileSpec &spec)
{
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::UserAgentHeader, userAgent());

    GeoTiledMappingManagerEngineEsri *engine = qobject_cast<GeoTiledMappingManagerEngineEsri *>(
          parent());

    GeoMapSource *mapSource = engine->mapSource(spec.mapId());

    if (!mapSource)
        qWarning("Unknown mapId %d\n", spec.mapId());
    else
        request.setUrl(mapSource->url().arg(spec.zoom()).arg(spec.x()).arg(spec.y()));

    QNetworkReply *reply = m_networkManager->get(request);

    return new GeoTiledMapReplyEsri(reply, spec);
}

QT_END_NAMESPACE
