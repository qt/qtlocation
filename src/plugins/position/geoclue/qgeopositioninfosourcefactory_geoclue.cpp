/****************************************************************************
**
** Copyright (C) 2016 Jolla Ltd.
** Contact: Aaron McCarthy <aaron.mccarthy@jollamobile.com>
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

#include "qgeopositioninfosourcefactory_geoclue.h"
#include "qgeopositioninfosource_geocluemaster.h"
#include "qgeosatelliteinfosource_geocluemaster.h"

#include <QtCore/QLoggingCategory>

Q_LOGGING_CATEGORY(lcPositioningGeoclue, "qt.positioning.geoclue")

QT_BEGIN_NAMESPACE

QGeoPositionInfoSource *QGeoPositionInfoSourceFactoryGeoclue::positionInfoSource(QObject *parent)
{
    return new QGeoPositionInfoSourceGeoclueMaster(parent);
}

QGeoSatelliteInfoSource *QGeoPositionInfoSourceFactoryGeoclue::satelliteInfoSource(QObject *parent)
{
    return new QGeoSatelliteInfoSourceGeoclueMaster(parent);
}

QGeoAreaMonitorSource *QGeoPositionInfoSourceFactoryGeoclue::areaMonitor(QObject *parent)
{
    Q_UNUSED(parent);
    return 0;
}

QT_END_NAMESPACE
