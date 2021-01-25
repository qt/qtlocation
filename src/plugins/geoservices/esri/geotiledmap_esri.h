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

#ifndef GEOTILEDMAPESRI_H
#define GEOTILEDMAPESRI_H

#include <QtLocation/private/qgeotiledmap_p.h>
#ifdef LOCATIONLABS
#include <QtLocation/private/qgeotiledmaplabs_p.h>
typedef QGeoTiledMapLabs Map;
#else
typedef QGeoTiledMap Map;
#endif

QT_BEGIN_NAMESPACE

class GeoTiledMappingManagerEngineEsri;

class GeoTiledMapEsri: public Map
{
    Q_OBJECT

public:
    explicit GeoTiledMapEsri(GeoTiledMappingManagerEngineEsri *engine, QObject *parent = nullptr);
    virtual ~GeoTiledMapEsri();

protected:
    void evaluateCopyrights(const QSet<QGeoTileSpec> &visibleTiles) override;

    inline GeoTiledMappingManagerEngineEsri *engine() const;

private:
    GeoTiledMappingManagerEngineEsri *m_engine;
    int m_mapId;
};

inline GeoTiledMappingManagerEngineEsri *GeoTiledMapEsri::engine() const
{
    return m_engine;
}

QT_END_NAMESPACE

#endif // GEOTILEDMAPESRI_H
