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

#include "qmobilityglobal.h"

QT_BEGIN_NAMESPACE
class QTime;
class QByteArray;
QT_END_NAMESPACE

QTM_BEGIN_NAMESPACE

class QGeoPositionInfo;
class QLocationUtils
{
public:
    inline static bool isValidLat(double lat) {
        return lat >= -90 && lat <= 90;
    }
    inline static bool isValidLong(double lng) {
        return lng >= -180 && lng <= 180;
    }

    /*
        Creates a QGeoPositionInfo from a GGA, GLL, RMC, VTG or ZDA sentence.

        Note:
        - GGA and GLL sentences have time but not date so the update's
          QDateTime object will have an invalid date.
        - RMC reports date with a two-digit year so in this case the year
          is assumed to be after the year 2000.
    */
    Q_AUTOTEST_EXPORT static bool getPosInfoFromNmea(const char *data, int size, QGeoPositionInfo *info, bool *hasFix = 0);

    /*
        Returns true if the given NMEA sentence has a valid checksum.
    */
    Q_AUTOTEST_EXPORT static bool hasValidNmeaChecksum(const char *data, int size);

    /*
        Returns time from a string in hhmmss or hhmmss.z+ format.
    */
    Q_AUTOTEST_EXPORT static bool getNmeaTime(const QByteArray &bytes, QTime *time);

    /*
        Accepts e.g. ("2734.7964", 'S', "15306.0124", 'E') and returns the
        lat-long values. Fails if lat or long fail isValidLat() or isValidLong().
    */
    Q_AUTOTEST_EXPORT static bool getNmeaLatLong(const QByteArray &latString, char latDirection, const QByteArray &lngString, char lngDirection, double *lat, double *lon);
};

QTM_END_NAMESPACE

#endif
