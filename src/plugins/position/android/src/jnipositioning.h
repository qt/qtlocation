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

#ifndef JNIPOSITIONING_H
#define JNIPOSITIONING_H

#include <jni.h>
#include <QGeoPositionInfoSource>
#include <QGeoSatelliteInfoSource>

namespace AndroidPositioning
{
    int registerPositionInfoSource(QObject *obj);
    void unregisterPositionInfoSource(int key);

    QGeoPositionInfoSource::PositioningMethods availableProviders();
    QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly);

    QGeoPositionInfoSource::Error startUpdates(int androidClassKey);
    void stopUpdates(int androidClassKey);
    QGeoPositionInfoSource::Error requestUpdate(int androidClassKey);

    QGeoSatelliteInfoSource::Error startSatelliteUpdates(int androidClassKey,
                                                         bool isSingleRequest,
                                                         int updateRequestTimeout);
    bool requestionPositioningPermissions(JNIEnv *env);
}

#endif // JNIPOSITIONING_H
