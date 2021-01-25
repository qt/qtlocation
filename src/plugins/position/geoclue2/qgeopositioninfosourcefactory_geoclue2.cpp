/****************************************************************************
**
** Copyright (C) 2018 Denis Shienkov <denis.shienkov@gmail.com>
** Contact: http://www.qt-project.org/legal
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

#include "qgeopositioninfosource_geoclue2_p.h"
#include "qgeopositioninfosourcefactory_geoclue2.h"

#include <QtCore/QLoggingCategory>

Q_LOGGING_CATEGORY(lcPositioningGeoclue2, "qt.positioning.geoclue2")

QT_BEGIN_NAMESPACE

QGeoPositionInfoSource *QGeoPositionInfoSourceFactoryGeoclue2::positionInfoSource(QObject *parent)
{
    return new QGeoPositionInfoSourceGeoclue2(parent);
}

QGeoSatelliteInfoSource *QGeoPositionInfoSourceFactoryGeoclue2::satelliteInfoSource(QObject *parent)
{
    Q_UNUSED(parent);
    return nullptr;
}

QGeoAreaMonitorSource *QGeoPositionInfoSourceFactoryGeoclue2::areaMonitor(QObject *parent)
{
    Q_UNUSED(parent);
    return nullptr;
}

QT_END_NAMESPACE
