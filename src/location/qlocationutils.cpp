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
#include "qlocationutils_p.h"
#include "qgeopositioninfo.h"

#include <QTime>
#include <QList>
#include <QByteArray>
#include <QDebug>

#include <math.h>

QTM_BEGIN_NAMESPACE

// converts e.g. 15306.0235 from NMEA sentence to 153.100392
static double qlocationutils_nmeaDegreesToDecimal(double nmeaDegrees)
{
    double deg;
    double min = 100.0 * modf(nmeaDegrees / 100.0, &deg);
    return deg + (min / 60.0);
}

static void qlocationutils_readGga(const char *data, int size, QGeoPositionInfo *info, bool *hasFix)
{
    QByteArray sentence(data, size);
    QList<QByteArray> parts = sentence.split(',');
    QGeoCoordinate coord;

    if (hasFix && parts.count() > 6 && parts[6].count() > 0)
        *hasFix = parts[6].toInt() > 0;

    if (parts.count() > 1 && parts[1].count() > 0) {
        QTime time;
        if (QLocationUtils::getNmeaTime(parts[1], &time))
            info->setTimestamp(QDateTime(QDate(), time, Qt::UTC));
    }

    if (parts.count() > 5 && parts[3].count() == 1 && parts[5].count() == 1) {
        double lat;
        double lng;
        if (QLocationUtils::getNmeaLatLong(parts[2], parts[3][0], parts[4], parts[5][0], &lat, &lng)) {
            coord.setLatitude(lat);
            coord.setLongitude(lng);
        }
    }

    if (parts.count() > 9 && parts[9].count() > 0) {
        bool hasAlt = false;
        double alt = parts[9].toDouble(&hasAlt);
        if (hasAlt)
            coord.setAltitude(alt);
    }

    if (coord.type() != QGeoCoordinate::InvalidCoordinate)
        info->setCoordinate(coord);
}

static void qlocationutils_readGll(const char *data, int size, QGeoPositionInfo *info, bool *hasFix)
{
    QByteArray sentence(data, size);
    QList<QByteArray> parts = sentence.split(',');
    QGeoCoordinate coord;

    if (hasFix && parts.count() > 6 && parts[6].count() > 0)
        *hasFix = (parts[6][0] == 'A');

    if (parts.count() > 5 && parts[5].count() > 0) {
        QTime time;
        if (QLocationUtils::getNmeaTime(parts[5], &time))
            info->setTimestamp(QDateTime(QDate(), time, Qt::UTC));
    }

    if (parts.count() > 4 && parts[2].count() == 1 && parts[4].count() == 1) {
        double lat;
        double lng;
        if (QLocationUtils::getNmeaLatLong(parts[1], parts[2][0], parts[3], parts[4][0], &lat, &lng)) {
            coord.setLatitude(lat);
            coord.setLongitude(lng);
        }
    }

    if (coord.type() != QGeoCoordinate::InvalidCoordinate)
        info->setCoordinate(coord);
}

static void qlocationutils_readRmc(const char *data, int size, QGeoPositionInfo *info, bool *hasFix)
{
    QByteArray sentence(data, size);
    QList<QByteArray> parts = sentence.split(',');
    QGeoCoordinate coord;
    QDate date;
    QTime time;

    if (hasFix && parts.count() > 2 && parts[2].count() > 0)
        *hasFix = (parts[2][0] == 'A');

    if (parts.count() > 9 && parts[9].count() == 6) {
        date = QDate::fromString(parts[9], "ddMMyy");
        if (date.isValid())
            date = date.addYears(100);     // otherwise starts from 1900
        else
            date = QDate();
    }

    if (parts.count() > 1 && parts[1].count() > 0)
        QLocationUtils::getNmeaTime(parts[1], &time);

    if (parts.count() > 6 && parts[4].count() == 1 && parts[6].count() == 1) {
        double lat;
        double lng;
        if (QLocationUtils::getNmeaLatLong(parts[3], parts[4][0], parts[5], parts[6][0], &lat, &lng)) {
            coord.setLatitude(lat);
            coord.setLongitude(lng);
        }
    }

    bool parsed = false;
    double value = 0.0;
    if (parts.count() > 7 && parts[7].count() > 0) {
        value = parts[7].toDouble(&parsed);
        if (parsed)
            info->setAttribute(QGeoPositionInfo::GroundSpeed, qreal(value * 1.852 / 3.6));    // knots -> m/s
    }
    if (parts.count() > 8 && parts[8].count() > 0) {
        value = parts[8].toDouble(&parsed);
        if (parsed)
            info->setAttribute(QGeoPositionInfo::Direction, qreal(value));
    }
    if (parts.count() > 11 && parts[11].count() == 1
            && (parts[11][0] == 'E' || parts[11][0] == 'W')) {
        value = parts[10].toDouble(&parsed);
        if (parsed) {
            if (parts[11][0] == 'W')
                value *= -1;
            info->setAttribute(QGeoPositionInfo::MagneticVariation, qreal(value));
        }
    }

    if (coord.type() != QGeoCoordinate::InvalidCoordinate)
        info->setCoordinate(coord);

    info->setTimestamp(QDateTime(date, time, Qt::UTC));
}

static void qlocationutils_readVtg(const char *data, int size, QGeoPositionInfo *info, bool *hasFix)
{
    if (hasFix)
        *hasFix = false;

    QByteArray sentence(data, size);
    QList<QByteArray> parts = sentence.split(',');

    bool parsed = false;
    double value = 0.0;
    if (parts.count() > 1 && parts[1].count() > 0) {
        value = parts[1].toDouble(&parsed);
        if (parsed)
            info->setAttribute(QGeoPositionInfo::Direction, qreal(value));
    }
    if (parts.count() > 7 && parts[7].count() > 0) {
        value = parts[7].toDouble(&parsed);
        if (parsed)
            info->setAttribute(QGeoPositionInfo::GroundSpeed, qreal(value / 3.6));    // km/h -> m/s
    }
}

static void qlocationutils_readZda(const char *data, int size, QGeoPositionInfo *info, bool *hasFix)
{
    if (hasFix)
        *hasFix = false;

    QByteArray sentence(data, size);
    QList<QByteArray> parts = sentence.split(',');
    QDate date;
    QTime time;

    if (parts.count() > 1 && parts[1].count() > 0)
        QLocationUtils::getNmeaTime(parts[1], &time);

    if (parts.count() > 4 && parts[2].count() > 0 && parts[3].count() > 0
            && parts[4].count() == 4) {     // must be full 4-digit year
        int day = parts[2].toUInt();
        int month = parts[3].toUInt();
        int year = parts[4].toUInt();
        if (day > 0 && month > 0 && year > 0)
            date.setDate(year, month, day);
    }

    info->setTimestamp(QDateTime(date, time, Qt::UTC));
}

bool QLocationUtils::getPosInfoFromNmea(const char *data, int size, QGeoPositionInfo *info, bool *hasFix)
{
    if (!info)
        return false;

    if (hasFix)
        *hasFix = false;
    if (size < 6 || data[0] != '$' || !hasValidNmeaChecksum(data, size))
        return false;

    if (data[3] == 'G' && data[4] == 'G' && data[5] == 'A') {
        // "$--GGA" sentence.
        qlocationutils_readGga(data, size, info, hasFix);
        return true;
    }

    if (data[3] == 'G' && data[4] == 'L' && data[5] == 'L') {
        // "$--GLL" sentence.
        qlocationutils_readGll(data, size, info, hasFix);
        return true;
    }

    if (data[3] == 'R' && data[4] == 'M' && data[5] == 'C') {
        // "$--RMC" sentence.
        qlocationutils_readRmc(data, size, info, hasFix);
        return true;
    }

    if (data[3] == 'V' && data[4] == 'T' && data[5] == 'G') {
        // "$--VTG" sentence.
        qlocationutils_readVtg(data, size, info, hasFix);
        return true;
    }

    if (data[3] == 'Z' && data[4] == 'D' && data[5] == 'A') {
        // "$--ZDA" sentence.
        qlocationutils_readZda(data, size, info, hasFix);
        return true;
    }

    return false;
}

bool QLocationUtils::hasValidNmeaChecksum(const char *data, int size)
{
    int asteriskIndex = -1;
    for (int i = 0; i < size; i++) {
        if (data[i] == '*') {
            asteriskIndex = i;
            break;
        }
    }

    const int CSUM_LEN = 2;
    if (asteriskIndex < 0 || asteriskIndex + CSUM_LEN >= size)
        return false;

    // XOR byte value of all characters between '$' and '*'
    int result = 0;
    for (int i = 1; i < asteriskIndex; i++)
        result ^= data[i];
    /*
        char calc[CSUM_LEN + 1];
        ::snprintf(calc, CSUM_LEN + 1, "%02x", result);
        return ::strncmp(calc, &data[asteriskIndex+1], 2) == 0;
        */

    QString s;
    s.sprintf("%02x", result);
    return s.toAscii() == QByteArray(&data[asteriskIndex+1], 2);
}

bool QLocationUtils::getNmeaTime(const QByteArray &bytes, QTime *time)
{
    int dotIndex = bytes.indexOf('.');
    QTime tempTime;

    if (dotIndex < 0) {
        tempTime = QTime::fromString(bytes, "hhmmss");
    } else {
        tempTime = QTime::fromString(bytes.mid(0, dotIndex), "hhmmss");
        bool hasMsecs = false;
        int midLen = qMin(3, bytes.size() - dotIndex - 1);
        int msecs = bytes.mid(dotIndex + 1, midLen).toUInt(&hasMsecs);
        if (hasMsecs)
            tempTime = tempTime.addMSecs(msecs);
    }

    if (tempTime.isValid()) {
        *time = tempTime;
        return true;
    }
    return false;
}

bool QLocationUtils::getNmeaLatLong(const QByteArray &latString, char latDirection, const QByteArray &lngString, char lngDirection, double *lat, double *lng)
{
    if ((latDirection != 'N' && latDirection != 'S')
            || (lngDirection != 'E' && lngDirection != 'W')) {
        return false;
    }

    bool hasLat = false;
    bool hasLong = false;
    double tempLat = latString.toDouble(&hasLat);
    double tempLng = lngString.toDouble(&hasLong);
    if (hasLat && hasLong) {
        tempLat = qlocationutils_nmeaDegreesToDecimal(tempLat);
        if (latDirection == 'S')
            tempLat *= -1;
        tempLng = qlocationutils_nmeaDegreesToDecimal(tempLng);
        if (lngDirection == 'W')
            tempLng *= -1;

        if (isValidLat(tempLat) && isValidLong(tempLng)) {
            *lat = tempLat;
            *lng = tempLng;
            return true;
        }
    }
    return false;
}

QTM_END_NAMESPACE

