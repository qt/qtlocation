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
** This file is part of the Ovi services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file OVI_SERVICES_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Ovi services
** plugin source code.
**
****************************************************************************/

#include "qgeocameracapabilities_p.h"
#include "qgeotiledmappingmanagerengine_nokia.h"
#include "qgeotiledmapdata_nokia.h"
#include "qgeotilefetcher_nokia.h"

QT_BEGIN_NAMESPACE

QGeoTiledMappingManagerEngineNokia::QGeoTiledMappingManagerEngineNokia(const QMap<QString, QVariant> &parameters,
                                                                       QGeoServiceProvider::Error *error,
                                                                       QString *errorString)
        : QGeoTiledMappingManagerEngine()
{
    Q_UNUSED(error);
    Q_UNUSED(errorString);

    QGeoCameraCapabilities capabilities;

    capabilities.setMinimumZoomLevel(0.0);
    capabilities.setMaximumZoomLevel(20.0);

    setCameraCapabilities(capabilities);

    setTileSize(QSize(256, 256));

    QList<QGeoMapType> types;
    types << QGeoMapType(QGeoMapType::StreetMap,tr("Street Map"),tr("Nokia Street Map"), false, 1);
    types << QGeoMapType(QGeoMapType::SatelliteMapDay,tr("Satellite Map(day)"),tr("Nokia Satellite Map (day)"), false, 2);
    types << QGeoMapType(QGeoMapType::TerrainMap,tr("Terrain Map"),tr("Nokia Terrain Map"), false, 3);
    types << QGeoMapType(QGeoMapType::HybridMap,tr("Hybrid Map"),tr("Nokia Hybrid Map"), false, 4);
    types << QGeoMapType(QGeoMapType::TransitMap,tr("Transit Map"),tr("Nokia Transit Map"), false, 5);
    types << QGeoMapType(QGeoMapType::GrayStreetMap,tr("Gray Street Map"),tr("Nokia Gray Street Map"), false, 6);
    types << QGeoMapType(QGeoMapType::StreetMap,tr("Mobile Street Map"),tr("Nokia Mobile Street Map"), true, 7);
    types << QGeoMapType(QGeoMapType::TerrainMap,tr("Mobile Terrain Map"),tr("Nokia Mobile Terrain Map"), true, 8);
    types << QGeoMapType(QGeoMapType::HybridMap,tr("Mobile Hybrid Map"),tr("Nokia Mobile Hybrid Map"), true, 9);
    types << QGeoMapType(QGeoMapType::TransitMap,tr("Mobile Transit Map"),tr("Nokia Mobile Transit Map"), true, 10);
    types << QGeoMapType(QGeoMapType::GrayStreetMap,tr("Mobile Gray Street Map"),tr("Nokia Mobile Gray Street Map"), true, 11);
    setSupportedMapTypes(types);

    QGeoTileFetcherNokia *fetcher = new QGeoTileFetcherNokia(this);
    fetcher->setParams(&parameters);
    fetcher->setTileSize(tileSize());
    setTileFetcher(fetcher);
}

QGeoTiledMappingManagerEngineNokia::~QGeoTiledMappingManagerEngineNokia() {}

QGeoMapData *QGeoTiledMappingManagerEngineNokia::createMapData()
{
    return new QGeoTiledMapDataNokia(this);;
}

QT_END_NAMESPACE

