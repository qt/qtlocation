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

#include "qgeotiledmappingmanagerengineosm.h"
#include "qgeotilefetcherosm.h"
#include "qgeotiledmaposm.h"
#include "qgeotileproviderosm.h"

#include <QtLocation/private/qgeocameracapabilities_p.h>
#include <QtLocation/private/qgeomaptype_p.h>
#include <QtLocation/private/qgeotiledmap_p.h>
#include <QtLocation/private/qgeofiletilecache_p.h>

#include <QtNetwork/QNetworkAccessManager>

QT_BEGIN_NAMESPACE

QGeoTiledMappingManagerEngineOsm::QGeoTiledMappingManagerEngineOsm(const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString)
:   QGeoTiledMappingManagerEngine()
{
    QGeoCameraCapabilities cameraCaps;
    cameraCaps.setMinimumZoomLevel(0.0);
    cameraCaps.setMaximumZoomLevel(19.0);
    setCameraCapabilities(cameraCaps);

    setTileSize(QSize(256, 256));

    QNetworkAccessManager *nm = new QNetworkAccessManager();
    QString domain = QStringLiteral("http://maps-redirect.qt.io/osm/5.6/");
    if (parameters.contains(QStringLiteral("osm.mapping.providersrepository.address"))) {
        QString customAddress = parameters.value(QStringLiteral("osm.mapping.providersrepository.address")).toString();
        // Allowing some malformed addresses ( containing the suffix "/osm/5.6/"
        if (customAddress.indexOf(QStringLiteral(":")) < 0) // defaulting to http:// if no prefix is found
            customAddress = QStringLiteral("http://") + customAddress;
        if (customAddress[customAddress.length()-1] != QLatin1Char('/'))
            customAddress += QLatin1Char('/');
        domain = customAddress;
    }

    m_providers.push_back(
        new QGeoTileProviderOsm(domain + "street",
            nm,
            QGeoMapType(QGeoMapType::StreetMap, tr("Street Map"), tr("Street map view in daylight mode"), false, false, 1),
            QGeoTileProviderOsm::TileProvider(QStringLiteral("http://c.tile.openstreetmap.org/%z/%x/%y.png"),
                QStringLiteral("png"),
                QStringLiteral("<a href='http://www.openstreetmap.org/copyright'>OpenStreetMap.org</a>"),
                QStringLiteral("<a href='http://www.openstreetmap.org/copyright'>OpenStreetMap</a> contributors")
            )));
    m_providers.push_back(
        new QGeoTileProviderOsm(domain + "satellite",
            nm,
            QGeoMapType(QGeoMapType::SatelliteMapDay, tr("Satellite Map"), tr("Satellite map view in daylight mode"), false, false, 2),
            QGeoTileProviderOsm::TileProvider()
            ));
    m_providers.push_back(
        new QGeoTileProviderOsm(domain + "cycle",
            nm,
            QGeoMapType(QGeoMapType::CycleMap, tr("Cycle Map"), tr("Cycle map view in daylight mode"), false, false, 3),
            QGeoTileProviderOsm::TileProvider(QStringLiteral("http://c.tile.opencyclemap.org/cycle/%z/%x/%y.png"),
                QStringLiteral("png"),
                QStringLiteral("<a href='http://www.thunderforest.com/'>Thunderforest</a>"),
                QStringLiteral("<a href='http://www.openstreetmap.org/copyright'>OpenStreetMap</a> contributors")
            )));
    m_providers.push_back(
        new QGeoTileProviderOsm(domain + "transit",
            nm,
            QGeoMapType(QGeoMapType::TransitMap, tr("Transit Map"), tr("Public transit map view in daylight mode"), false, false, 4),
            QGeoTileProviderOsm::TileProvider(QStringLiteral("http://c.tile2.opencyclemap.org/transport/%z/%x/%y.png"),
                QStringLiteral("png"),
                QStringLiteral("<a href='http://www.thunderforest.com/'>Thunderforest</a>"),
                QStringLiteral("<a href='http://www.openstreetmap.org/copyright'>OpenStreetMap</a> contributors")
            )));
    m_providers.push_back(
        new QGeoTileProviderOsm(domain + "night-transit",
            nm,
            QGeoMapType(QGeoMapType::TransitMap, tr("Night Transit Map"), tr("Public transit map view in night mode"), false, true, 5),
            QGeoTileProviderOsm::TileProvider(QStringLiteral("http://a.tile.thunderforest.com/transport-dark/%z/%x/%y.png"),
                QStringLiteral("png"),
                QStringLiteral("<a href='http://www.thunderforest.com/'>Thunderforest</a>"),
                QStringLiteral("<a href='http://www.openstreetmap.org/copyright'>OpenStreetMap</a> contributors")
            )));
    m_providers.push_back(
        new QGeoTileProviderOsm(domain + "terrain",
            nm,
            QGeoMapType(QGeoMapType::TerrainMap, tr("Terrain Map"), tr("Terrain map view"), false, false, 6),
            QGeoTileProviderOsm::TileProvider(QStringLiteral("http://a.tile.thunderforest.com/landscape/%z/%x/%y.png"),
                QStringLiteral("png"),
                QStringLiteral("<a href='http://www.thunderforest.com/'>Thunderforest</a>"),
                QStringLiteral("<a href='http://www.openstreetmap.org/copyright'>OpenStreetMap</a> contributors")
            )));
    m_providers.push_back(
        new QGeoTileProviderOsm(domain + "hiking",
            nm,
            QGeoMapType(QGeoMapType::PedestrianMap, tr("Hiking Map"), tr("Hiking map view"), false, false, 7),
            QGeoTileProviderOsm::TileProvider(QStringLiteral("http://a.tile.thunderforest.com/outdoors/%z/%x/%y.png"),
                QStringLiteral("png"),
                QStringLiteral("<a href='http://www.thunderforest.com/'>Thunderforest</a>"),
                QStringLiteral("<a href='http://www.openstreetmap.org/copyright'>OpenStreetMap</a> contributors")
            )));

    if (parameters.contains(QStringLiteral("osm.mapping.custom.host"))
            || parameters.contains(QStringLiteral("osm.mapping.host"))) {
        // Adding a custom provider
        QString tmsServer;
        if (parameters.contains(QStringLiteral("osm.mapping.host")))
            tmsServer = parameters.value(QStringLiteral("osm.mapping.host")).toString();
        if (parameters.contains(QStringLiteral("osm.mapping.custom.host"))) // priority to the new one
            tmsServer = parameters.value(QStringLiteral("osm.mapping.custom.host")).toString();

        QString mapCopyright;
        QString dataCopyright;
        if (parameters.contains(QStringLiteral("osm.mapping.custom.mapcopyright")))
            mapCopyright = parameters.value(QStringLiteral("osm.mapping.custom.mapcopyright")).toString();
        if (parameters.contains(QStringLiteral("osm.mapping.custom.datacopyright")))
            dataCopyright = parameters.value(QStringLiteral("osm.mapping.custom.datacopyright")).toString();

        if (parameters.contains(QStringLiteral("osm.mapping.copyright")))
            m_customCopyright = parameters.value(QStringLiteral("osm.mapping.copyright")).toString();

        m_providers.push_back(
            new QGeoTileProviderOsm("",
                nm,
                QGeoMapType(QGeoMapType::CustomMap, tr("Custom URL Map"), tr("Custom url map view set via urlprefix parameter"), false, false, 8),
                QGeoTileProviderOsm::TileProvider(tmsServer + QStringLiteral("%z/%x/%y.png"),
                    QStringLiteral("png"),
                    mapCopyright,
                    dataCopyright
                )));

        m_providers.last()->disableRedirection();
   }

    bool disableRedirection = false;
    if (parameters.contains(QStringLiteral("osm.mapping.providersrepository.disabled")))
        disableRedirection = parameters.value(QStringLiteral("osm.mapping.providersrepository.disabled")).toBool();

    foreach (QGeoTileProviderOsm * provider, m_providers) {
        provider->setParent(this);
        if (disableRedirection)
            provider->disableRedirection();

        connect(provider, &QGeoTileProviderOsm::resolutionFinished,
                this, &QGeoTiledMappingManagerEngineOsm::onProviderResolutionFinished);
        connect(provider, &QGeoTileProviderOsm::resolutionError,
                this, &QGeoTiledMappingManagerEngineOsm::onProviderResolutionError);
    }
    updateMapTypes();

    QGeoTileFetcherOsm *tileFetcher = new QGeoTileFetcherOsm(m_providers, nm, this);
    if (parameters.contains(QStringLiteral("osm.useragent"))) {
        const QByteArray ua = parameters.value(QStringLiteral("osm.useragent")).toString().toLatin1();
        tileFetcher->setUserAgent(ua);
    }


    setTileFetcher(tileFetcher);

    QAbstractGeoTileCache *tileCache = new QGeoFileTileCache(QAbstractGeoTileCache::baseCacheDirectory() + QStringLiteral("osm"));
    // 50mb of disk cache by default to minimize n. of accesses to public OSM servers
    tileCache->setMaxDiskUsage(50 * 1024 * 1024);
    setTileCache(tileCache);

    *error = QGeoServiceProvider::NoError;
    errorString->clear();
}

QGeoTiledMappingManagerEngineOsm::~QGeoTiledMappingManagerEngineOsm()
{
}

QGeoMap *QGeoTiledMappingManagerEngineOsm::createMap()
{
    return new QGeoTiledMapOsm(this);
}

const QVector<QGeoTileProviderOsm *> &QGeoTiledMappingManagerEngineOsm::providers()
{
    return m_providers;
}

QString QGeoTiledMappingManagerEngineOsm::customCopyright() const
{
    return m_customCopyright;
}

void QGeoTiledMappingManagerEngineOsm::onProviderResolutionFinished(const QGeoTileProviderOsm *provider)
{
    if (!provider->isResolved())
        return;
    updateMapTypes();
}

void QGeoTiledMappingManagerEngineOsm::onProviderResolutionError(const QGeoTileProviderOsm *provider, QNetworkReply::NetworkError error)
{
    Q_UNUSED(error)
    if (!provider->isResolved())
        return;
    updateMapTypes();
}

void QGeoTiledMappingManagerEngineOsm::updateMapTypes()
{
    QList<QGeoMapType> mapTypes;
    foreach (QGeoTileProviderOsm * provider, m_providers) {
        // assume provider are ok until they have been resolved invalid
        if (!provider->isResolved() || provider->isValid())
            mapTypes << provider->mapType();
    }
    const QList<QGeoMapType> currentlySupportedMapTypes = supportedMapTypes();
    if (currentlySupportedMapTypes != mapTypes)
        // See map type implementations in QGeoTiledMapOsm and QGeoTileFetcherOsm.
        setSupportedMapTypes(mapTypes);
}

QT_END_NAMESPACE
