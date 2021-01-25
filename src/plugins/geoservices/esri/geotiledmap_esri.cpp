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

#include "geotiledmap_esri.h"
#include "geotiledmappingmanagerengine_esri.h"

#include <QtLocation/private/qgeotilespec_p.h>

QT_BEGIN_NAMESPACE

GeoTiledMapEsri::GeoTiledMapEsri(GeoTiledMappingManagerEngineEsri *engine, QObject *parent) :
    Map(engine, parent), m_engine(engine), m_mapId(-1)
{
}

GeoTiledMapEsri::~GeoTiledMapEsri()
{
}

void GeoTiledMapEsri::evaluateCopyrights(const QSet<QGeoTileSpec> &visibleTiles)
{
    if (visibleTiles.isEmpty())
        return;

    QGeoTileSpec tile = *(visibleTiles.constBegin());
    if (tile.mapId() == m_mapId)
        return;

    m_mapId = tile.mapId();

    GeoMapSource *mapSource = engine()->mapSource(m_mapId);

    if (mapSource)
        emit copyrightsChanged(mapSource->copyright());
}

QT_END_NAMESPACE
