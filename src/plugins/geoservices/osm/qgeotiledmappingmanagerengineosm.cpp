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

#include <QtLocation/private/qgeocameracapabilities_p.h>
#include <QtLocation/private/qgeomaptype_p.h>
#include <QtLocation/private/qgeotiledmap_p.h>

QT_BEGIN_NAMESPACE

QGeoTiledMappingManagerEngineOsm::QGeoTiledMappingManagerEngineOsm(const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString)
:   QGeoTiledMappingManagerEngine()
{
    QGeoCameraCapabilities cameraCaps;
    cameraCaps.setMinimumZoomLevel(0.0);
    cameraCaps.setMaximumZoomLevel(19.0);
    setCameraCapabilities(cameraCaps);

    setTileSize(QSize(256, 256));

    QList<QGeoMapType> mapTypes;
    // See map type implementations in QGeoTiledMapOsm and QGeoTileFetcherOsm.
    mapTypes << QGeoMapType(QGeoMapType::StreetMap, tr("Street Map"), tr("Street map view in daylight mode"), false, false, 1);
    mapTypes << QGeoMapType(QGeoMapType::SatelliteMapDay, tr("Satellite Map"), tr("Satellite map view in daylight mode"), false, false, 2);
    mapTypes << QGeoMapType(QGeoMapType::CycleMap, tr("Cycle Map"), tr("Cycle map view in daylight mode"), false, false, 3);
    mapTypes << QGeoMapType(QGeoMapType::TransitMap, tr("Transit Map"), tr("Public transit map view in daylight mode"), false, false, 4);
    mapTypes << QGeoMapType(QGeoMapType::TransitMap, tr("Night Transit Map"), tr("Public transit map view in night mode"), false, true, 5);
    mapTypes << QGeoMapType(QGeoMapType::TerrainMap, tr("Terrain Map"), tr("Terrain map view"), false, false, 6);
    mapTypes << QGeoMapType(QGeoMapType::PedestrianMap, tr("Hiking Map"), tr("Hiking map view"), false, false, 7);
    if (parameters.contains(QStringLiteral("osm.mapping.host")))
        mapTypes << QGeoMapType(QGeoMapType::CustomMap, tr("Custom URL Map"), tr("Custom url map view set via urlprefix parameter"), false, false, 8);
    setSupportedMapTypes(mapTypes);

    QGeoTileFetcherOsm *tileFetcher = new QGeoTileFetcherOsm(this);
    if (parameters.contains(QStringLiteral("osm.useragent"))) {
        const QByteArray ua = parameters.value(QStringLiteral("osm.useragent")).toString().toLatin1();
        tileFetcher->setUserAgent(ua);
    }
    if (parameters.contains(QStringLiteral("osm.mapping.host"))) {
        const QString up = parameters.value(QStringLiteral("osm.mapping.host")).toString().toLatin1();
        tileFetcher->setUrlPrefix(up);
    }
    if (parameters.contains(QStringLiteral("osm.mapping.copyright")))
        m_customCopyright = parameters.value(QStringLiteral("osm.mapping.copyright")).toString().toLatin1();

    setTileFetcher(tileFetcher);

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

QString QGeoTiledMappingManagerEngineOsm::customCopyright() const
{
    return m_customCopyright;
}

QT_END_NAMESPACE
