/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtPositioning module of the Qt Toolkit.
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

#include "qgeopositioninfosourcefactory_simulator.h"

QGeoPositionInfoSource *QGeoPositionInfoSourceFactorySimulator::positionInfoSource(QObject *parent)
{
    return new QGeoPositionInfoSourceSimulator(parent);
}

QGeoSatelliteInfoSource *QGeoPositionInfoSourceFactorySimulator::satelliteInfoSource(QObject *parent)
{
    QGeoSatelliteInfoSourceSimulator *src = new QGeoSatelliteInfoSourceSimulator(parent);
    if (!src->isConnected()) {
        delete src;
        src = 0;
    }
    return src;
}

QGeoAreaMonitorSource *QGeoPositionInfoSourceFactorySimulator::areaMonitor(QObject *parent)
{
    Q_UNUSED(parent);
    return 0;
}
