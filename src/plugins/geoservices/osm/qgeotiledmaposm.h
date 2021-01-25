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

#ifndef QGEOTILEDMAPOSM_H
#define QGEOTILEDMAPOSM_H

#include "qgeotileproviderosm.h"

#include <QtLocation/private/qgeotiledmap_p.h>
#ifdef LOCATIONLABS
#include <QtLocation/private/qgeotiledmaplabs_p.h>
typedef QGeoTiledMapLabs Map;
#else
typedef QGeoTiledMap Map;
#endif

QT_BEGIN_NAMESPACE

class QGeoTiledMappingManagerEngineOsm;
class QGeoTiledMapOsm: public Map
{
    Q_OBJECT

public:
    QGeoTiledMapOsm(QGeoTiledMappingManagerEngineOsm *engine, QObject *parent = 0);
    ~QGeoTiledMapOsm();

protected:
    void evaluateCopyrights(const QSet<QGeoTileSpec> &visibleTiles) override;

protected Q_SLOTS:
    void onProviderDataUpdated(const QGeoTileProviderOsm *provider);

private:
    int m_mapId;
    QGeoTiledMappingManagerEngineOsm *m_engine;
};

QT_END_NAMESPACE

#endif
