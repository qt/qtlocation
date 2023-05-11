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

#include "qgeopositioninfosource_cl_p.h"
#include "qgeopositioninfosourcefactory_cl.h"

QGeoPositionInfoSource *QGeoPositionInfoSourceFactoryCL::positionInfoSource(QObject *parent)
{
    return positionInfoSourceWithParameters(parent, QVariantMap());
}

QGeoSatelliteInfoSource *QGeoPositionInfoSourceFactoryCL::satelliteInfoSource(QObject *parent)
{
    return satelliteInfoSourceWithParameters(parent, QVariantMap());
}

QGeoAreaMonitorSource *QGeoPositionInfoSourceFactoryCL::areaMonitor(QObject *parent)
{
    return areaMonitorWithParameters(parent, QVariantMap());
}

QGeoPositionInfoSource *
QGeoPositionInfoSourceFactoryCL::positionInfoSourceWithParameters(QObject *parent,
                                                                  const QVariantMap &parameters)
{
    return new QGeoPositionInfoSourceCL(parameters, parent);
}

QGeoSatelliteInfoSource *
QGeoPositionInfoSourceFactoryCL::satelliteInfoSourceWithParameters(QObject *parent,
                                                                   const QVariantMap &parameters)
{
    Q_UNUSED(parent);
    Q_UNUSED(parameters);
    return 0;
}

QGeoAreaMonitorSource *
QGeoPositionInfoSourceFactoryCL::areaMonitorWithParameters(QObject *parent,
                                                           const QVariantMap &parameters)
{
    Q_UNUSED(parent);
    Q_UNUSED(parameters);
    return 0;
}
