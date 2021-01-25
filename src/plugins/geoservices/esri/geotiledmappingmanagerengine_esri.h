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

#ifndef GEOTILEDMAPPINGMANAGERENGINEESRI_H
#define GEOTILEDMAPPINGMANAGERENGINEESRI_H

#include <QGeoServiceProvider>

#include <QtLocation/private/qgeotiledmappingmanagerengine_p.h>

#include "geomapsource.h"

QT_BEGIN_NAMESPACE

class GeoTiledMappingManagerEngineEsri : public QGeoTiledMappingManagerEngine
{
    Q_OBJECT

public:
    GeoTiledMappingManagerEngineEsri(const QVariantMap &parameters,
                                     QGeoServiceProvider::Error *error, QString *errorString);
    virtual ~GeoTiledMappingManagerEngineEsri();

    QGeoMap *createMap() override;

    inline const QList<GeoMapSource *>& mapSources() const;
    GeoMapSource *mapSource(int mapId) const;

private:
    bool initializeMapSources(QGeoServiceProvider::Error *error, QString *errorString, const QGeoCameraCapabilities &cameraCaps);

    QList<GeoMapSource *> m_mapSources;
};

inline const QList<GeoMapSource *>& GeoTiledMappingManagerEngineEsri::mapSources() const
{
    return m_mapSources;
}

QT_END_NAMESPACE

#endif // GEOTILEDMAPPINGMANAGERENGINEESRI_H
