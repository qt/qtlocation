// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/private/qohoscommon_p.h>
#include <QtCore/private/qohoslogger_p.h>
#include <QtLocation/private/qgeocameracapabilities_p.h>
#include <QtLocation/private/qgeofiletilecache_p.h>
#include <QtLocation/private/qgeomaptype_p.h>
#include <QtLocation/private/qgeotiledmap_p.h>
#include <qgeofiletilecacheohosmapkit.h>
#include <qgeotiledmappingmanagerengineohosmapkit.h>
#include <qgeotilefetcherohosmapkit.h>
#include <qohosmapkitcommon.h>

QT_BEGIN_NAMESPACE

namespace {

const QByteArray pluginName = "ohosmapkit";

constexpr double ohosMinimumZoomLevel = 1.0;
constexpr double ohosMaximumZoomLevel = 20.0;
constexpr QSize lowDpiTileSize = {256, 256};
constexpr int mapId = 0;

const QString highDpiParameterName = QStringLiteral("ohosmapkit.mapping.highdpi_tiles");
const QString cacheDirectoryParameterName = QStringLiteral("ohosmapkit.mapping.cache.directory");
const QString cacheDiskCostStrategyParameterName = QStringLiteral("ohosmapkit.mapping.cache.disk.cost_strategy");
const QString cacheDiskSizeParameterName = QStringLiteral("ohosmapkit.mapping.cache.disk.size");
const QString cacheMemoryCostStrategyParameterName = QStringLiteral("ohosmapkit.mapping.cache.memory.cost_strategy");
const QString cacheMemorySizeParameterName = QStringLiteral("ohosmapkit.mapping.cache.memory.size");
const QString cacheTextureCostStrategyParameterName = QStringLiteral("ohosmapkit.mapping.cache.texture.cost_strategy");
const QString cacheTextureSizeParameterName = QStringLiteral("ohosmapkit.mapping.cache.texture.size");
const QString prefetchingStyleParameterName = QStringLiteral("ohosmapkit.mapping.prefetching_style");

QAbstractGeoTileCache::CostStrategy getCacheCostStrategy(
    const QVariantMap &parameters, const QString &parameterName)
{
    return parameters.contains(parameterName)
        && parameters.value(parameterName).toString().toLower() == QLatin1String("bytesize")
            ? QGeoFileTileCache::ByteSize
            : QGeoFileTileCache::Unitary;
}

void tryExtractIntParameter(
    const QVariantMap &parameters, const QString &parameterName, const QOhosConsumer<int> &parameterConsumer)
{
    if (!parameters.contains(parameterName))
        return;

    bool ok = false;
    int cacheSize = parameters.value(parameterName).toInt(&ok);
    if (ok) {
        parameterConsumer(cacheSize);
    } else {
        qOhosPrintfWarning(
            "%s: Parameter %s value cannot be converted to int.",
            Q_FUNC_INFO, parameterName.toStdString().c_str());
    }
}

void configureFileTileCache(QGeoFileTileCache *tileCache, const QVariantMap &parameters)
{
    tileCache->setCostStrategyDisk(
        getCacheCostStrategy(parameters, cacheDiskCostStrategyParameterName));
    tryExtractIntParameter(
        parameters, cacheDiskSizeParameterName,
        [&](int cacheSize) {
            tileCache->setMaxDiskUsage(cacheSize);
        });

    tileCache->setCostStrategyMemory(
        getCacheCostStrategy(parameters, cacheMemoryCostStrategyParameterName));
    tryExtractIntParameter(
        parameters, cacheMemorySizeParameterName,
        [&](int cacheSize) {
            tileCache->setMaxMemoryUsage(cacheSize);
        });

    tileCache->setCostStrategyTexture(
        getCacheCostStrategy(parameters, cacheTextureCostStrategyParameterName));
    tryExtractIntParameter(
        parameters, cacheTextureSizeParameterName,
        [&](int cacheSize) {
            tileCache->setExtraTextureUsage(cacheSize);
        });
}

int getScaleFactor(const QVariantMap &parameters)
{
    constexpr int lowDpiScaleFactor = 1;
    constexpr int highDpiScaleFactor = 2;

    return parameters.contains(highDpiParameterName)
        && parameters.value(highDpiParameterName).toBool()
            ? highDpiScaleFactor
            : lowDpiScaleFactor;
}

class QGeoTiledMappingManagerEngineOhosMapKit : public QGeoTiledMappingManagerEngine
{
public:
    QGeoTiledMappingManagerEngineOhosMapKit(
        const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString);

private:
    QGeoMap *createMap() override;
};

QGeoTiledMappingManagerEngineOhosMapKit::QGeoTiledMappingManagerEngineOhosMapKit(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString)
    : QGeoTiledMappingManagerEngine()
{
    QGeoCameraCapabilities cameraCapabilities;
    cameraCapabilities.setMinimumZoomLevel(ohosMinimumZoomLevel);
    cameraCapabilities.setMaximumZoomLevel(ohosMaximumZoomLevel);
    cameraCapabilities.setSupportsBearing(true);
    cameraCapabilities.setSupportsTilting(true);
    cameraCapabilities.setOverzoomEnabled(true);
    setCameraCapabilities(cameraCapabilities);

    setSupportedMapTypes(
        {
            QGeoMapType(
                QGeoMapType::StreetMap, QStringLiteral("ohosmapkit.streets"), tr("Street"), false, false,
                mapId, pluginName, cameraCapabilities)
        });

    const auto scaleFactor = getScaleFactor(parameters);
    setTileSize(lowDpiTileSize * scaleFactor);

    setTileFetcher(
        makeGeoTileFetcherOhosMapKit(
            OhosMapKit::getAuthenticationKeyParameterOrEmpty(parameters),
            OhosMapKit::getUserAgentParameterOrDefault(parameters),
            scaleFactor, this));

    auto *tileCache =
        makeGeoFileTileCacheOhosMapKit(
            scaleFactor,
            parameters.contains(cacheDirectoryParameterName)
                ? parameters.value(cacheDirectoryParameterName).toString()
                : QAbstractGeoTileCache::baseLocationCacheDirectory() + QLatin1String(pluginName),
            this);
    configureFileTileCache(tileCache, parameters);
    setTileCache(tileCache);

    if (parameters.contains(prefetchingStyleParameterName)) {
        const QString prefetchingMode = parameters.value(prefetchingStyleParameterName).toString();
        if (prefetchingMode == QStringLiteral("TwoNeighbourLayers"))
            m_prefetchStyle = QGeoTiledMap::PrefetchTwoNeighbourLayers;
        else if (prefetchingMode == QStringLiteral("OneNeighbourLayer"))
            m_prefetchStyle = QGeoTiledMap::PrefetchNeighbourLayer;
        else if (prefetchingMode == QStringLiteral("NoPrefetching"))
            m_prefetchStyle = QGeoTiledMap::NoPrefetching;
    }

    if (error != nullptr)
        *error = QGeoServiceProvider::NoError;

    if (errorString != nullptr)
        errorString->clear();
}

QGeoMap *QGeoTiledMappingManagerEngineOhosMapKit::createMap()
{
    auto *map = new QGeoTiledMap(this, nullptr);
    map->setPrefetchStyle(m_prefetchStyle);
    return map;
}

}

QGeoTiledMappingManagerEngine *makeQGeoTiledMappingManagerEngineOhosMapKit(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString)
{
    return new QGeoTiledMappingManagerEngineOhosMapKit(parameters, error, errorString);
}

QT_END_NAMESPACE
