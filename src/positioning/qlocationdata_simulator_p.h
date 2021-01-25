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

#ifndef QGEOPOSITIONINFODATA_SIMULATOR_P_H
#define QGEOPOSITIONINFODATA_SIMULATOR_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

//
//  DO NOT REMOVE
//  -------------
//
// This header file contains structures used to serialize communication between
// simulator's client and server implementations, it is included by simulator
// positioning plugin.

#include "qpositioningglobal_p.h"
#include <QtCore/QMetaType>
#include <QtCore/QDateTime>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

struct Q_POSITIONING_PRIVATE_EXPORT QGeoPositionInfoData
{
    QGeoPositionInfoData();

    // Coordinate information
    double latitude;
    double longitude;
    double altitude;

    // Attributes
    // ### transmit whether attributes are set or not
    qreal direction;
    qreal groundSpeed;
    qreal verticalSpeed;
    qreal magneticVariation;
    qreal horizontalAccuracy;
    qreal verticalAccuracy;

    // DateTime info
    QDateTime dateTime;

    int minimumInterval;
    bool enabled;
};

struct Q_POSITIONING_PRIVATE_EXPORT QGeoSatelliteInfoData
{
    struct SatelliteInfo
    {
        SatelliteInfo();

        // This enum duplicates the SatelliteSystem enum defined in qgeosatelliteinfo.h, which cannot be
        // included as this file must compile with Qt4 (it is used by Qt Simulator)
        enum SatelliteSystem
        {
            Undefined = 0x00,
            GPS = 0x01,
            GLONASS = 0x02
        };

        qreal azimuth;
        qreal elevation;
        int signalStrength;
        bool inUse;
        SatelliteSystem satelliteSystem;
        int satelliteIdentifier;
    };

    QList<SatelliteInfo> satellites;
};

Q_POSITIONING_PRIVATE_EXPORT void qt_registerLocationTypes();
Q_POSITIONING_PRIVATE_EXPORT QDataStream &operator<<(QDataStream &out, const QGeoPositionInfoData &s);
Q_POSITIONING_PRIVATE_EXPORT QDataStream &operator>>(QDataStream &in, QGeoPositionInfoData &s);
Q_POSITIONING_PRIVATE_EXPORT QDataStream &operator<<(QDataStream &out, const QGeoSatelliteInfoData &s);
Q_POSITIONING_PRIVATE_EXPORT QDataStream &operator>>(QDataStream &in, QGeoSatelliteInfoData &s);
Q_POSITIONING_PRIVATE_EXPORT QDataStream &operator<<(QDataStream &out, const QGeoSatelliteInfoData::SatelliteInfo &s);
Q_POSITIONING_PRIVATE_EXPORT QDataStream &operator>>(QDataStream &in, QGeoSatelliteInfoData::SatelliteInfo &s);

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QGeoPositionInfoData)
Q_DECLARE_METATYPE(QGeoSatelliteInfoData)
Q_DECLARE_METATYPE(QGeoSatelliteInfoData::SatelliteInfo)

#endif // QGEOPOSITIONINFODATA_SIMULATOR_P_H
