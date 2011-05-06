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

#include <windows.h>
#include <gpsapi.h> // including requires <windows.h>

#include "qgeopositioninfosource_wince_p.h"
#include "qgeoinfothread_wince_p.h"

QTM_BEGIN_NAMESPACE

// ========== QGeoPositionInfoValidator ==========

QGeoPositionInfoValidator::QGeoPositionInfoValidator() : QGeoInfoValidator() {}

QGeoPositionInfoValidator::~QGeoPositionInfoValidator() {}

// Returns true if data contains at least the minimal amount of data we need to produce the
// QGeoPositionInfoSource positionUpdated signal, otherwise returns false.
bool QGeoPositionInfoValidator::valid(const GPS_POSITION &data) const
{
    if (((data.dwValidFields & GPS_VALID_LATITUDE) == 0)
            || ((data.dwValidFields & GPS_VALID_LONGITUDE) == 0)
            || ((data.dwValidFields & GPS_VALID_UTC_TIME) == 0)) {
        return false;
    }
    return true;
}

// ========== QGeoPositionInfoSourceWinCE ==========

QGeoPositionInfoSourceWinCE::QGeoPositionInfoSourceWinCE(QObject *parent)
        : QGeoPositionInfoSource(parent)
{
    QGeoInfoValidator *validator = new QGeoPositionInfoValidator();

    // The QGeoInfoThreadWinCE instance takes ownership of the validator.
    infoThread = new QGeoInfoThreadWinCE(validator, true, this);
    infoThread->start();
    // QGeoInfoThreadWinCE takes care of registering GPS_POSITION as a metatype.
    connect(infoThread, SIGNAL(dataUpdated(GPS_POSITION)), this, SLOT(dataUpdated(GPS_POSITION)));
    connect(infoThread, SIGNAL(updateTimeout()), this, SIGNAL(updateTimeout()));
}

QGeoPositionInfoSourceWinCE::~QGeoPositionInfoSourceWinCE()
{
    delete infoThread;
}

QGeoPositionInfoSource::PositioningMethods QGeoPositionInfoSourceWinCE::supportedPositioningMethods() const
{
    return QGeoPositionInfoSource::SatellitePositioningMethods;
}

QGeoPositionInfo QGeoPositionInfoSourceWinCE::lastKnownPosition(bool) const
{
    return lastPosition;
}

void QGeoPositionInfoSourceWinCE::setUpdateInterval(int msec)
{
    // If msec is 0 we send updates as data becomes available, otherwise we force msec to be equal
    // to or larger than the minimum update interval.
    if (msec != 0 && msec < MinimumUpdateInterval)
        msec = MinimumUpdateInterval;

    infoThread->setUpdateInterval(msec);
    QGeoPositionInfoSource::setUpdateInterval(msec);
}

int QGeoPositionInfoSourceWinCE::minimumUpdateInterval() const
{
    return MinimumUpdateInterval;
}

void QGeoPositionInfoSourceWinCE::startUpdates()
{
    infoThread->startUpdates();
}

void QGeoPositionInfoSourceWinCE::stopUpdates()
{
    infoThread->stopUpdates();
}

void QGeoPositionInfoSourceWinCE::requestUpdate(int timeout)
{
    // A timeout of 0 means to use the default timeout, which is handled by the QGeoInfoThreadWinCE
    // instance, otherwise if timeout is less than the minimum update interval we emit a
    // updateTimeout signal
    if (timeout < minimumUpdateInterval() && timeout != 0)
        emit updateTimeout();
    else
        infoThread->requestUpdate(timeout);
}

/*
 This is _only_ called when QGeoPositionInfoValidator::valid() returns true for the position.
 This means that it is implied that:
 - (data.dwValidFields & GPS_VALID_LATITUDE) != 0
 - (data.dwValidFields & GPS_VALID_LONGITUDE) != 0
 - (data.dwValidFields & GPS_VALID_UTC_TIME) != 0

 This guarantees that the newly created position will be valid.
 If the code is changed such that this is no longer guaranteed then this method will need to be
 updated to test for those conditions.
*/
void QGeoPositionInfoSourceWinCE::dataUpdated(GPS_POSITION data)
{
    QGeoCoordinate coordinate(data.dblLatitude, data.dblLongitude);

    // The altitude is optional in QGeoCoordinate, so we do not strictly require that the
    // GPS_POSITION structure has valid altitude data in order to trigger an update.
    if ((data.dwValidFields & GPS_VALID_ALTITUDE_WRT_SEA_LEVEL) != 0)
        coordinate.setAltitude(data.flAltitudeWRTSeaLevel);

    QDate date(data.stUTCTime.wYear, data.stUTCTime.wMonth, data.stUTCTime.wDay);
    QTime time(data.stUTCTime.wHour, data.stUTCTime.wMinute, data.stUTCTime.wSecond,
               data.stUTCTime.wMilliseconds);

    QDateTime dateTime(date, time, Qt::UTC);

    QGeoPositionInfo pos(coordinate, dateTime);

    // The following properties are optional, and so are set if the data is present and valid in
    // the GPS_POSITION structure.
    if ((data.dwValidFields & GPS_VALID_SPEED) != 0)
        pos.setAttribute(QGeoPositionInfo::GroundSpeed, data.flSpeed);

    if ((data.dwValidFields & GPS_VALID_HEADING) != 0)
        pos.setAttribute(QGeoPositionInfo::Direction, data.flHeading);

    if ((data.dwValidFields & GPS_VALID_MAGNETIC_VARIATION) != 0)
        pos.setAttribute(QGeoPositionInfo::MagneticVariation, data.dblMagneticVariation);

    lastPosition = pos;
    emit positionUpdated(pos);
}

#include "moc_qgeopositioninfosource_wince_p.cpp"
QTM_END_NAMESPACE
