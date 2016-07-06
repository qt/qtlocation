/****************************************************************************
**
** Copyright (C) 2014 Canonical Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
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
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeotiledmappingmanagerenginemapbox.h"
#include "qgeotilefetchermapbox.h"

#include <QtLocation/private/qgeocameracapabilities_p.h>
#include <QtLocation/private/qgeomaptype_p.h>
#include <QtLocation/private/qgeotiledmap_p.h>
#include "qgeofiletilecachemapbox.h"

QT_BEGIN_NAMESPACE

static QLatin1String mapboxPrefix = QLatin1String("mapbox.");

static QString mapboxIdToMapName(const QString &id)
{
    if ((id.indexOf(mapboxPrefix) == 0) && (id.size() > mapboxPrefix.size())) {
        // the names of the geomaptypes in this plugin reflect the actual mapbox mapIds, but are camel cased and with dashes replaced by spaces
        QString name = id.right(id.size() - mapboxPrefix.size());
        QStringList parts = name.split('-');
        for (int i=0; i < parts.size(); ++i)
            parts[i].replace(0, 1, parts[i][0].toUpper());
        name = parts.join(" ");
        return name;
    } else {
        return QString();
    }
}
static inline QString mapNameToMapboxId(const QString &name)
{
    return mapboxPrefix + name.toLower().replace(' ','-');
}

QGeoTiledMappingManagerEngineMapbox::QGeoTiledMappingManagerEngineMapbox(const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString)
:   QGeoTiledMappingManagerEngine()
{
    QGeoCameraCapabilities cameraCaps;
    cameraCaps.setMinimumZoomLevel(0.0);
    cameraCaps.setMaximumZoomLevel(19.0);
    setCameraCapabilities(cameraCaps);

    setTileSize(QSize(256, 256));

    QList<QGeoMapType> mapTypes;
    // as index 0 to retain compatibility with the current API, that expects the passed map_id to be on by default.
    if (parameters.contains(QStringLiteral("mapbox.map_id"))) {
        const QString name = mapboxIdToMapName(parameters.value(QStringLiteral("mapbox.map_id")).toString());
        mapTypes << QGeoMapType(QGeoMapType::CustomMap, name, QObject::tr(name.toLatin1().data()), false, false, mapTypes.size());
    }

    // As of 2016.06.15, valid mapbox map_ids are documented at https://www.mapbox.com/api-documentation/#maps
    mapTypes << QGeoMapType(QGeoMapType::StreetMap, QStringLiteral("Streets"), tr("Street"), false, false, mapTypes.size());
    mapTypes << QGeoMapType(QGeoMapType::StreetMap, QStringLiteral("Light"), tr("Light"), false, false, mapTypes.size());
    mapTypes << QGeoMapType(QGeoMapType::StreetMap, QStringLiteral("Dark"), tr("Dark"), false, true, mapTypes.size());
    mapTypes << QGeoMapType(QGeoMapType::SatelliteMapDay, QStringLiteral("Satellite"), tr("Satellite"), false, false, mapTypes.size());
    mapTypes << QGeoMapType(QGeoMapType::HybridMap, QStringLiteral("Streets Satellite"), tr("Streets Satellite"), false, false, mapTypes.size());
    mapTypes << QGeoMapType(QGeoMapType::CustomMap, QStringLiteral("Wheatpaste"), tr("Wheatpaste"), false, false, mapTypes.size());
    mapTypes << QGeoMapType(QGeoMapType::StreetMap, QStringLiteral("Streets Basic"), tr("Streets Basic"), false, false, mapTypes.size());
    mapTypes << QGeoMapType(QGeoMapType::CustomMap, QStringLiteral("Comic"), tr("Comic"), false, false, mapTypes.size());
    mapTypes << QGeoMapType(QGeoMapType::PedestrianMap, QStringLiteral("Outdoors"), tr("Outdoors"), false, false, mapTypes.size());
    mapTypes << QGeoMapType(QGeoMapType::CycleMap, QStringLiteral("Run Bike Hike"), tr("Run Bike Hike"), false, false, mapTypes.size());
    mapTypes << QGeoMapType(QGeoMapType::CustomMap, QStringLiteral("Pencil"), tr("Pencil"), false, false, mapTypes.size());
    mapTypes << QGeoMapType(QGeoMapType::CustomMap, QStringLiteral("Pirates"), tr("Pirates"), false, false, mapTypes.size());
    mapTypes << QGeoMapType(QGeoMapType::CustomMap, QStringLiteral("Emerald"), tr("Emerald"), false, false, mapTypes.size());
    mapTypes << QGeoMapType(QGeoMapType::CustomMap, QStringLiteral("High Contrast"), tr("High Contrast"), false, false, mapTypes.size());

    // New way to specify multiple customized map_ids via additional_map_ids
    if (parameters.contains(QStringLiteral("mapbox.additional_map_ids"))) {
        const QString ids = parameters.value(QStringLiteral("mapbox.additional_map_ids")).toString();
        const QStringList idList = ids.split(',', QString::SkipEmptyParts);

        for (const QString &str: idList) {
                const QString name = mapboxIdToMapName(str);
                if (!name.isEmpty())
                    mapTypes << QGeoMapType(QGeoMapType::CustomMap, name, QObject::tr(name.toLatin1().data()), false, false, mapTypes.size());
        }
    }

    QVector<QString> mapIds;
    for (int i=0; i < mapTypes.size(); ++i)
         mapIds.push_back(mapNameToMapboxId(mapTypes[i].name()));

    setSupportedMapTypes(mapTypes);

    bool doubleRes = true;
    if (parameters.contains(QStringLiteral("mapbox.highdpi_tiles"))) {
        const QString param = parameters.value(QStringLiteral("mapbox.highdpi_tiles")).toString().toLower();
        if (param == "false")
            doubleRes = false;
    }
    QGeoTileFetcherMapbox *tileFetcher = new QGeoTileFetcherMapbox(this, doubleRes);
    tileFetcher->setMapIds(mapIds);

    if (parameters.contains(QStringLiteral("useragent"))) {
        const QByteArray ua = parameters.value(QStringLiteral("useragent")).toString().toLatin1();
        tileFetcher->setUserAgent(ua);
    }
    if (parameters.contains(QStringLiteral("mapbox.format"))) {
        const QString format = parameters.value(QStringLiteral("mapbox.format")).toString();
        tileFetcher->setFormat(format);
    }
    if (parameters.contains(QStringLiteral("mapbox.access_token"))) {
        const QString token = parameters.value(QStringLiteral("mapbox.access_token")).toString();
        tileFetcher->setAccessToken(token);
    }

    setTileFetcher(tileFetcher);

    // TODO: do this in a plugin-neutral way so that other tiled map plugins
    //       don't need this boilerplate or hardcode plugin name

    if (parameters.contains(QStringLiteral("mapbox.cache.directory"))) {
        m_cacheDirectory = parameters.value(QStringLiteral("mapbox.cache.directory")).toString();
    } else {
        // managerName() is not yet set, we have to hardcode the plugin name below
        m_cacheDirectory = QAbstractGeoTileCache::baseCacheDirectory() + QLatin1String("mapbox");
    }

    // The Mapbox free plan allows for 6000 tiles to be stored for offline uses
    // As of 2016.06.15, according to https://www.mapbox.com/help/mobile-offline/ ,
    // this translates into 45-315 MiB, depending on the map and the area.
    // Setting a default limit of 300MiB, which can be overridden via parameters, if
    // the plan allows for more data to be stored offline.
    // NOTE:
    // It is illegal to violate Mapbox Terms of Service, setting a limit that exceeds
    // what the plan the token belongs to allows.

    QAbstractGeoTileCache *tileCache = new QGeoFileTileCacheMapbox(mapTypes, m_cacheDirectory);

    if (parameters.contains(QStringLiteral("mapbox.cache.disk.size"))) {
        bool ok = false;
        int cacheSize = parameters.value(QStringLiteral("mapbox.cache.disk.size")).toString().toInt(&ok);
        if (ok)
            tileCache->setMaxDiskUsage(cacheSize);
    } else {
        tileCache->setMaxDiskUsage(300 * 1024 * 1024);
    }

    if (parameters.contains(QStringLiteral("mapbox.cache.memory.size"))) {
        bool ok = false;
        int cacheSize = parameters.value(QStringLiteral("mapbox.cache.memory.size")).toString().toInt(&ok);
        if (ok)
            tileCache->setMaxMemoryUsage(cacheSize);
    }

    if (parameters.contains(QStringLiteral("mapbox.cache.texture.size"))) {
        bool ok = false;
        int cacheSize = parameters.value(QStringLiteral("mapbox.cache.texture.size")).toString().toInt(&ok);
        if (ok)
            tileCache->setExtraTextureUsage(cacheSize);
    }

    setTileCache(tileCache);

    *error = QGeoServiceProvider::NoError;
    errorString->clear();
}

QGeoTiledMappingManagerEngineMapbox::~QGeoTiledMappingManagerEngineMapbox()
{
}

QGeoMap *QGeoTiledMappingManagerEngineMapbox::createMap()
{
    return new QGeoTiledMap(this, 0);
}

QT_END_NAMESPACE
