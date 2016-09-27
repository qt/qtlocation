/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtPositioning module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QLOCATIONUTILS_P_H
#define QLOCATIONUTILS_P_H

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

#include <QtCore/QtGlobal>
#include <math.h>

QT_BEGIN_NAMESPACE
class QTime;
class QByteArray;

class QGeoPositionInfo;
class QLocationUtils
{
public:
    enum CardinalDirection {
        CardinalN,
        CardinalE,
        CardinalS,
        CardinalW,
        CardinalNE,
        CardinalSE,
        CardinalSW,
        CardinalNW,
        CardinalNNE,
        CardinalENE,
        CardinalESE,
        CardinalSSE,
        CardinalSSW,
        CardinalWSW,
        CardinalWNW,
        CardinalNNW
    };

    inline static bool isValidLat(double lat) {
        return lat >= -90 && lat <= 90;
    }
    inline static bool isValidLong(double lng) {
        return lng >= -180 && lng <= 180;
    }

    inline static double clipLat(double lat) {
        if (lat > 90)
            lat = 90;
        else if (lat < -90)
            lat = -90;
        return lat;
    }

    inline static double wrapLong(double lng) {
        if (lng > 180)
            lng -= 360;
        else if (lng < -180)
            lng += 360;
        return lng;
    }

    inline static CardinalDirection azimuthToCardinalDirection4(double azimuth)
    {
        azimuth = fmod(azimuth, 360.0);
        if (azimuth < 45.0 || azimuth > 315.0 )
            return CardinalN;
        else if (azimuth < 135.0)
            return CardinalE;
        else if (azimuth < 225.0)
            return CardinalS;
        else
            return CardinalW;
    }

    inline static CardinalDirection azimuthToCardinalDirection8(double azimuth)
    {
        azimuth = fmod(azimuth, 360.0);
        if (azimuth < 22.5 || azimuth > 337.5 )
            return CardinalN;
        else if (azimuth < 67.5)
            return CardinalNE;
        else if (azimuth < 112.5)
            return CardinalE;
        else if (azimuth < 157.5)
            return CardinalSE;
        else if (azimuth < 202.5)
            return CardinalS;

        else if (azimuth < 247.5)
            return CardinalSW;
        else if (azimuth < 292.5)
            return CardinalW;
        else
            return CardinalNW;
    }

    inline static CardinalDirection azimuthToCardinalDirection16(double azimuth)
    {
        azimuth = fmod(azimuth, 360.0);
        if (azimuth < 11.5 || azimuth > 348.75 )
            return CardinalN;
        else if (azimuth < 33.75)
            return CardinalNNE;
        else if (azimuth < 56.25)
            return CardinalNE;
        else if (azimuth < 78.75)
            return CardinalENE;
        else if (azimuth < 101.25)
            return CardinalE;
        else if (azimuth < 123.75)
            return CardinalESE;
        else if (azimuth < 146.25)
            return CardinalSE;
        else if (azimuth < 168.75)
            return CardinalSSE;
        else if (azimuth < 191.25)
            return CardinalS;

        else if (azimuth < 213.75)
            return CardinalSSW;
        else if (azimuth < 236.25)
            return CardinalSW;
        else if (azimuth < 258.75)
            return CardinalWSW;
        else if (azimuth < 281.25)
            return CardinalW;
        else if (azimuth < 303.75)
            return CardinalWNW;
        else if (azimuth < 326.25)
            return CardinalNW;
        else
            return CardinalNNW;
    }

    /*
        Creates a QGeoPositionInfo from a GGA, GLL, RMC, VTG or ZDA sentence.

        Note:
        - GGA and GLL sentences have time but not date so the update's
          QDateTime object will have an invalid date.
        - RMC reports date with a two-digit year so in this case the year
          is assumed to be after the year 2000.
    */
    Q_AUTOTEST_EXPORT static bool getPosInfoFromNmea(const char *data, int size,
                                                     QGeoPositionInfo *info, double uere,
                                                     bool *hasFix = 0);

    /*
        Returns true if the given NMEA sentence has a valid checksum.
    */
    Q_AUTOTEST_EXPORT static bool hasValidNmeaChecksum(const char *data, int size);

    /*
        Returns time from a string in hhmmss or hhmmss.z+ format.
    */
    Q_AUTOTEST_EXPORT static bool getNmeaTime(const QByteArray &bytes, QTime *time);

    /*
        Accepts for example ("2734.7964", 'S', "15306.0124", 'E') and returns the
        lat-long values. Fails if lat or long fail isValidLat() or isValidLong().
    */
    Q_AUTOTEST_EXPORT static bool getNmeaLatLong(const QByteArray &latString, char latDirection, const QByteArray &lngString, char lngDirection, double *lat, double *lon);
};

QT_END_NAMESPACE

#endif
