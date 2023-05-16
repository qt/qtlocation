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

#ifndef QGEOPOSITIONINFOSOURCEFACTORY_CL_H
#define QGEOPOSITIONINFOSOURCEFACTORY_CL_H

#include <QObject>
#include <QGeoPositionInfoSourceFactory>

class QGeoPositionInfoSourceFactoryCL : public QObject, public QGeoPositionInfoSourceFactoryV2
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.qt.position.sourcefactory/5.0"
                      FILE "plugin.json")
    Q_INTERFACES(QGeoPositionInfoSourceFactoryV2)
public:
    QGeoPositionInfoSource *positionInfoSource(QObject *parent);
    QGeoSatelliteInfoSource *satelliteInfoSource(QObject *parent);
    QGeoAreaMonitorSource *areaMonitor(QObject *parent);

    QGeoPositionInfoSource *positionInfoSourceWithParameters(QObject *parent, const QVariantMap &parameters);
    QGeoSatelliteInfoSource *satelliteInfoSourceWithParameters(QObject *parent, const QVariantMap &parameters);
    QGeoAreaMonitorSource *areaMonitorWithParameters(QObject *parent, const QVariantMap &parameters);
};

#endif // QGEOPOSITIONINFOSOURCEFACTORY_CL_H
