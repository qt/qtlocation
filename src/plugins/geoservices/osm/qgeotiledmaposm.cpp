/****************************************************************************
**
** Copyright (C) 2016 Aaron McCarthy <mccarthy.aaron@gmail.com>
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

#include "qgeotiledmaposm.h"
#include "qgeotiledmappingmanagerengineosm.h"
#include "qgeotilefetcherosm.h"

#include <QtLocation/private/qgeotilespec_p.h>

QT_BEGIN_NAMESPACE

QGeoTiledMapOsm::QGeoTiledMapOsm(QGeoTiledMappingManagerEngineOsm *engine, QObject *parent)
:   Map(engine, parent), m_mapId(-1), m_engine(engine)
{
    // Needed because evaluateCopyrights() is only triggered if visible tiles change in the map.
    // It fails the first time it gets called if providers aren't resolved, and subsequent calls
    // to it will be skipped until visible tiles change.
    // This connection makes sure the copyrights are evaluated when copyright data are ready regardless
    // of what tiles are visible.
    connect(qobject_cast<QGeoTileFetcherOsm *>(engine->tileFetcher()), &QGeoTileFetcherOsm::providerDataUpdated,
            this, &QGeoTiledMapOsm::onProviderDataUpdated);
}

QGeoTiledMapOsm::~QGeoTiledMapOsm()
{
}

void QGeoTiledMapOsm::evaluateCopyrights(const QSet<QGeoTileSpec> &visibleTiles)
{
    if (visibleTiles.isEmpty())
        return;

    QGeoTileSpec tile = *visibleTiles.constBegin();
    if (tile.mapId() == m_mapId)
        return;

    int providerId = tile.mapId() - 1;
    if (providerId < 0 || providerId >= m_engine->providers().size())
        return;

    m_mapId = tile.mapId();
    if (!m_engine->providers().at(providerId)->isValid())
        return;

    onProviderDataUpdated(m_engine->providers().at(providerId));
}

void QGeoTiledMapOsm::onProviderDataUpdated(const QGeoTileProviderOsm *provider)
{
    if (!provider->isResolved() || provider->mapType().mapId() != m_mapId)
        return;
    QString copyRights;
    const QString mapCopy = provider->mapCopyRight();
    const QString dataCopy = provider->dataCopyRight();
    const QString styleCopy = provider->styleCopyRight();
    if (!mapCopy.isEmpty()) {
        copyRights += QStringLiteral("Map &copy; ");
        copyRights += mapCopy;
    }
    if (!dataCopy.isEmpty()) {
        if (!copyRights.isEmpty())
            copyRights += QStringLiteral(" | ");
        copyRights += QStringLiteral("Data &copy; ");
        copyRights += dataCopy;
    }
    if (!styleCopy.isEmpty()) {
        if (!copyRights.isEmpty())
            copyRights += QStringLiteral(" | ");
        copyRights += QStringLiteral("Style &copy; ");
        copyRights += styleCopy;
    }

    if (copyRights.isEmpty() && provider->mapType().style() == QGeoMapType::CustomMap)
        copyRights = m_engine->customCopyright();

    // Update CameraCapabilities
    setCameraCapabilities(provider->cameraCapabilities());

    emit copyrightsChanged(copyRights);
}

QT_END_NAMESPACE
