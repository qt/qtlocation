/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qlocationdata_simulator_p.h"

#include <QtCore/QDataStream>

QTM_BEGIN_NAMESPACE

void qt_registerLocationTypes()
{
    qRegisterMetaTypeStreamOperators<QGeoPositionInfoData>("QtMobility::QGeoPositionInfoData");
    qRegisterMetaTypeStreamOperators<QGeoSatelliteInfoData>("QtMobility::QGeoSatelliteInfoData");
    qRegisterMetaTypeStreamOperators<QGeoSatelliteInfoData::SatelliteInfo>("QtMobility::QGeoSatelliteInfoData::SatelliteInfo");
}

QDataStream &operator<<(QDataStream &out, const QGeoPositionInfoData &s)
{
    out << s.latitude << s.longitude << s.altitude;
    out << s.direction << s.groundSpeed << s.verticalSpeed << s.magneticVariation << s.horizontalAccuracy << s.verticalAccuracy;
    out << s.dateTime;
    out << s.minimumInterval << s.enabled;
    return out;
}

QDataStream &operator>>(QDataStream &in, QGeoPositionInfoData &s)
{
    in >> s.latitude >> s.longitude >> s.altitude;
    in >> s.direction >> s.groundSpeed >> s.verticalSpeed >> s.magneticVariation >> s.horizontalAccuracy >> s.verticalAccuracy;
    in >> s.dateTime;
    in >> s.minimumInterval >> s.enabled;
    return in;
}

QDataStream &operator<<(QDataStream &out, const QGeoSatelliteInfoData &s)
{
    out << s.satellites;
    return out;
}

QDataStream &operator>>(QDataStream &in, QGeoSatelliteInfoData &s)
{
    in >> s.satellites;
    return in;
}

QDataStream &operator<<(QDataStream &out, const QGeoSatelliteInfoData::SatelliteInfo &s)
{
    out << s.prn << s.azimuth << s.elevation << s.signalStrength << s.inUse;
    return out;
}

QDataStream &operator>>(QDataStream &in, QGeoSatelliteInfoData::SatelliteInfo &s)
{
    in >> s.prn >> s.azimuth >> s.elevation >> s.signalStrength >> s.inUse;
    return in;
}

QTM_END_NAMESPACE
