/****************************************************************************
**
** Copyright (C) 2016 Aaron McCarthy <mccarthy.aaron@gmail.com>
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

#include "qgeotiledmappingmanagerengineosm.h"
#include "qgeotilefetcherosm.h"
#include "qgeotiledmaposm.h"
#include "qgeofiletilecacheosm.h"

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
    QString domain = QStringLiteral("http://maps-redirect.qt.io/osm/5.8/");
    if (parameters.contains(QStringLiteral("osm.mapping.providersrepository.address"))) {
        QString customAddress = parameters.value(QStringLiteral("osm.mapping.providersrepository.address")).toString();
        // Allowing some malformed addresses
        if (customAddress.indexOf(QStringLiteral(":")) < 0) // defaulting to http:// if no prefix is found
            customAddress = QStringLiteral("http://") + customAddress;
        if (customAddress[customAddress.length()-1] != QLatin1Char('/'))
            customAddress += QLatin1Char('/');
        domain = customAddress;
    }

    bool highdpi = false;
    if (parameters.contains(QStringLiteral("osm.mapping.highdpi_tiles"))) {
        const QString param = parameters.value(QStringLiteral("osm.mapping.highdpi_tiles")).toString().toLower();
        if (param == "true")
            highdpi = true;
    }

    /* TileProviders setup */
    QVector<TileProvider *> providers_street;
    QVector<TileProvider *> providers_satellite;
    QVector<TileProvider *> providers_cycle;
    QVector<TileProvider *> providers_transit;
    QVector<TileProvider *> providers_nighttransit;
    QVector<TileProvider *> providers_terrain;
    QVector<TileProvider *> providers_hiking;
    if (highdpi) {
        providers_street.push_back(new TileProvider(domain + "street-hires"));
        providers_satellite.push_back(new TileProvider(domain + "satellite-hires"));
        providers_cycle.push_back(new TileProvider(domain + "cycle-hires"));
        providers_transit.push_back(new TileProvider(domain + "transit-hires"));
        providers_nighttransit.push_back(new TileProvider(domain + "night-transit-hires"));
        providers_terrain.push_back(new TileProvider(domain + "terrain-hires"));
        providers_hiking.push_back(new TileProvider(domain + "hiking-hires"));
    }
    providers_street.push_back(new TileProvider(domain + "street"));
    providers_satellite.push_back(new TileProvider(domain + "satellite"));
    providers_cycle.push_back(new TileProvider(domain + "cycle"));
    providers_transit.push_back(new TileProvider(domain + "transit"));
    providers_nighttransit.push_back(new TileProvider(domain + "night-transit"));
    providers_terrain.push_back(new TileProvider(domain + "terrain"));
    providers_hiking.push_back(new TileProvider(domain + "hiking"));
    // Backups
    providers_street.push_back(
        new TileProvider(QStringLiteral("http://c.tile.openstreetmap.org/%z/%x/%y.png"),
            QStringLiteral("png"),
            QStringLiteral("<a href='http://www.openstreetmap.org/copyright'>OpenStreetMap.org</a>"),
            QStringLiteral("<a href='http://www.openstreetmap.org/copyright'>OpenStreetMap</a> contributors")));
    // No available satellite backup
    providers_cycle.push_back(
        new TileProvider(QStringLiteral("http://c.tile.opencyclemap.org/cycle/%z/%x/%y.png"),
            QStringLiteral("png"),
            QStringLiteral("<a href='http://www.thunderforest.com/'>Thunderforest</a>"),
            QStringLiteral("<a href='http://www.openstreetmap.org/copyright'>OpenStreetMap</a> contributors")));
    providers_transit.push_back(
        new TileProvider(QStringLiteral("http://c.tile2.opencyclemap.org/transport/%z/%x/%y.png"),
            QStringLiteral("png"),
            QStringLiteral("<a href='http://www.thunderforest.com/'>Thunderforest</a>"),
            QStringLiteral("<a href='http://www.openstreetmap.org/copyright'>OpenStreetMap</a> contributors")));
    providers_nighttransit.push_back(
        new TileProvider(QStringLiteral("http://a.tile.thunderforest.com/transport-dark/%z/%x/%y.png"),
            QStringLiteral("png"),
            QStringLiteral("<a href='http://www.thunderforest.com/'>Thunderforest</a>"),
            QStringLiteral("<a href='http://www.openstreetmap.org/copyright'>OpenStreetMap</a> contributors")) );
    providers_terrain.push_back(
        new TileProvider(QStringLiteral("http://a.tile.thunderforest.com/landscape/%z/%x/%y.png"),
            QStringLiteral("png"),
            QStringLiteral("<a href='http://www.thunderforest.com/'>Thunderforest</a>"),
            QStringLiteral("<a href='http://www.openstreetmap.org/copyright'>OpenStreetMap</a> contributors")));
    providers_hiking.push_back(
        new TileProvider(QStringLiteral("http://a.tile.thunderforest.com/outdoors/%z/%x/%y.png"),
            QStringLiteral("png"),
            QStringLiteral("<a href='http://www.thunderforest.com/'>Thunderforest</a>"),
            QStringLiteral("<a href='http://www.openstreetmap.org/copyright'>OpenStreetMap</a> contributors")));


    /* QGeoTileProviderOsms setup */
    m_providers.push_back( new QGeoTileProviderOsm( nm,
            QGeoMapType(QGeoMapType::StreetMap, tr("Street Map"), tr("Street map view in daylight mode"), false, false, 1),
            providers_street ));
    m_providers.push_back( new QGeoTileProviderOsm( nm,
            QGeoMapType(QGeoMapType::SatelliteMapDay, tr("Satellite Map"), tr("Satellite map view in daylight mode"), false, false, 2),
            providers_satellite ));
    m_providers.push_back( new QGeoTileProviderOsm( nm,
            QGeoMapType(QGeoMapType::CycleMap, tr("Cycle Map"), tr("Cycle map view in daylight mode"), false, false, 3),
            providers_cycle ));
    m_providers.push_back( new QGeoTileProviderOsm( nm,
            QGeoMapType(QGeoMapType::TransitMap, tr("Transit Map"), tr("Public transit map view in daylight mode"), false, false, 4),
            providers_transit ));
    m_providers.push_back( new QGeoTileProviderOsm( nm,
            QGeoMapType(QGeoMapType::TransitMap, tr("Night Transit Map"), tr("Public transit map view in night mode"), false, true, 5),
            providers_nighttransit ));
    m_providers.push_back( new QGeoTileProviderOsm( nm,
            QGeoMapType(QGeoMapType::TerrainMap, tr("Terrain Map"), tr("Terrain map view"), false, false, 6),
            providers_terrain ));
    m_providers.push_back( new QGeoTileProviderOsm( nm,
            QGeoMapType(QGeoMapType::PedestrianMap, tr("Hiking Map"), tr("Hiking map view"), false, false, 7),
            providers_hiking ));

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
            new QGeoTileProviderOsm( nm,
                QGeoMapType(QGeoMapType::CustomMap, tr("Custom URL Map"), tr("Custom url map view set via urlprefix parameter"), false, false, 8),
                { new TileProvider(tmsServer + QStringLiteral("%z/%x/%y.png"),
                    QStringLiteral("png"),
                    mapCopyright,
                    dataCopyright) }
                ));

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

    if (parameters.contains(QStringLiteral("osm.mapping.cache.directory"))) {
        m_cacheDirectory = parameters.value(QStringLiteral("osm.mapping.cache.directory")).toString();
    } else {
        // managerName() is not yet set, we have to hardcode the plugin name below
        m_cacheDirectory = QAbstractGeoTileCache::baseLocationCacheDirectory() + QLatin1String("osm");
    }
    if (parameters.contains(QStringLiteral("osm.mapping.offline.directory")))
        m_offlineDirectory = parameters.value(QStringLiteral("osm.mapping.offline.directory")).toString();

    QAbstractGeoTileCache *tileCache = new QGeoFileTileCacheOsm(m_offlineDirectory, m_cacheDirectory);

    // 50mb of disk cache by default to minimize n. of accesses to public OSM servers
    tileCache->setMaxDiskUsage(50 * 1024 * 1024);

    if (parameters.contains(QStringLiteral("osm.mapping.cache.disk.size"))) {
        bool ok = false;
        int cacheSize = parameters.value(QStringLiteral("osm.mapping.cache.disk.size")).toString().toInt(&ok);
        if (ok)
            tileCache->setMaxDiskUsage(cacheSize);
    } else {
        tileCache->setMaxDiskUsage(100 * 1024 * 1024);
    }
    if (parameters.contains(QStringLiteral("osm.mapping.cache.memory.size"))) {
        bool ok = false;
        int cacheSize = parameters.value(QStringLiteral("osm.mapping.cache.memory.size")).toString().toInt(&ok);
        if (ok)
            tileCache->setMaxMemoryUsage(cacheSize);
    }
    if (parameters.contains(QStringLiteral("osm.mapping.cache.texture.size"))) {
        bool ok = false;
        int cacheSize = parameters.value(QStringLiteral("osm.mapping.cache.texture.size")).toString().toInt(&ok);
        if (ok)
            tileCache->setExtraTextureUsage(cacheSize);
    }
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

void QGeoTiledMappingManagerEngineOsm::onProviderResolutionError(const QGeoTileProviderOsm *provider)
{
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
