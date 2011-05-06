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

#include "qgeopositioninfosource_maemo5_p.h"
#include "liblocationwrapper_p.h"
#include <qnumeric.h>

using namespace std;

QTM_BEGIN_NAMESPACE

QGeoPositionInfoSourceMaemo::QGeoPositionInfoSourceMaemo(QObject *parent)
        : QGeoPositionInfoSource(parent)
{
    // default values
    timerInterval = DEFAULT_UPDATE_INTERVAL;
    updateTimer = new QTimer(this);
    updateTimer->setSingleShot(true);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateTimeoutElapsed()));

    requestTimer = new QTimer(this);
    requestTimer->setSingleShot(true);
    connect(requestTimer, SIGNAL(timeout()), this, SLOT(requestTimeoutElapsed()));

    connect(LiblocationWrapper::instance(), SIGNAL(positionUpdated(QGeoPositionInfo)), this, SLOT(newPositionUpdate(QGeoPositionInfo)));

    errorOccurred = false;
    errorSent = false;

    positionInfoState = QGeoPositionInfoSourceMaemo::Undefined;
}

int QGeoPositionInfoSourceMaemo::init()
{
    if (LiblocationWrapper::instance()->inited()) {
        connect(LiblocationWrapper::instance(), SIGNAL(error()), this, SLOT(error()));
        return INIT_OK;
    } else {
        return INIT_FAILED;
    }
}

QGeoPositionInfo QGeoPositionInfoSourceMaemo::lastKnownPosition(bool fromSatellitePositioningMethodsOnly) const
{
    return (LiblocationWrapper::instance()->lastKnownPosition(fromSatellitePositioningMethodsOnly));
}

QGeoPositionInfoSource::PositioningMethods QGeoPositionInfoSourceMaemo::supportedPositioningMethods() const
{
    QGeoPositionInfoSource::PositioningMethods methods;

    if (!GConfItem("/system/nokia/location/gps-disabled").value().toBool())
        methods |= SatellitePositioningMethods;
    if (!GConfItem("/system/nokia/location/network-disabled").value().toBool())
        methods |= NonSatellitePositioningMethods;
    if (methods.testFlag(SatellitePositioningMethods) && methods.testFlag(NonSatellitePositioningMethods))
        methods |= AllPositioningMethods;

    return methods;
}

void QGeoPositionInfoSourceMaemo::setUpdateInterval(int msec)
{
    bool updateTimerInterval = false;

    if (positionInfoState & QGeoPositionInfoSourceMaemo::PowersaveActive)
        if (positionInfoState & QGeoPositionInfoSourceMaemo::Stopped)
            updateTimerInterval = true;

    if (!msec) {
        timerInterval = MINIMUM_UPDATE_INTERVAL;
        QGeoPositionInfoSource::setUpdateInterval(0);
    } else {
        timerInterval = (msec < MINIMUM_UPDATE_INTERVAL) ? MINIMUM_UPDATE_INTERVAL : msec;
        QGeoPositionInfoSource::setUpdateInterval(timerInterval);
    }

    if (timerInterval >= POWERSAVE_THRESHOLD)
        positionInfoState |= QGeoPositionInfoSourceMaemo::PowersaveActive;
    else
        positionInfoState &= ~QGeoPositionInfoSourceMaemo::PowersaveActive;

    // If powersave has been active when new update interval has been set,
    // ensure that timer is started.
    if (updateTimerInterval)
        startLocationDaemon();

    // Ensure that new timer interval is taken into use immediately.
    activateTimer();
}

void QGeoPositionInfoSourceMaemo::setPreferredPositioningMethods(PositioningMethods methods)
{
    QGeoPositionInfoSource::setPreferredPositioningMethods(methods);
}

int QGeoPositionInfoSourceMaemo::minimumUpdateInterval() const
{
    return MINIMUM_UPDATE_INTERVAL;
}

// public slots:
void QGeoPositionInfoSourceMaemo::startUpdates()
{
    startLocationDaemon();

    // Ensure that powersave is selected, if stopUpdates() has been called,
    // but selected update interval is still greater than POWERSAVE_THRESHOLD.
    if (timerInterval >= POWERSAVE_THRESHOLD)
        positionInfoState |= QGeoPositionInfoSourceMaemo::PowersaveActive;

    activateTimer();
}

void QGeoPositionInfoSourceMaemo::stopUpdates()
{
    positionInfoState &= ~QGeoPositionInfoSourceMaemo::PowersaveActive;

    if (!(positionInfoState & QGeoPositionInfoSourceMaemo::RequestActive)) {
        updateTimer->stop();
        if (LiblocationWrapper::instance()->isActive())
            LiblocationWrapper::instance()->stop();
    }

    errorOccurred = false;
    errorSent = false;

    positionInfoState &= ~QGeoPositionInfoSourceMaemo::Started;
    positionInfoState |= QGeoPositionInfoSourceMaemo::Stopped;
}

void QGeoPositionInfoSourceMaemo::requestUpdate(int timeout)
{
    int timeoutForRequest = 0;

    if (!timeout) {
        if (LiblocationWrapper::instance()->isActive())
            // If GPS is active, assume quick fix.
            timeoutForRequest = DEFAULT_UPDATE_INTERVAL;
        else
            // Otherwise reserve longer time to get a fix.
            timeoutForRequest = POWERSAVE_POWERON_PERIOD;
    } else if (timeout < MINIMUM_UPDATE_INTERVAL) {
        if (positionInfoState & QGeoPositionInfoSourceMaemo::RequestActive)
            return;

        emit updateTimeout();
        return;
    } else {
        timeoutForRequest = timeout;
    }

    positionInfoState |= QGeoPositionInfoSourceMaemo::RequestActive;

    if (!(LiblocationWrapper::instance()->isActive()))
        LiblocationWrapper::instance()->start();

    activateTimer();
    requestTimer->start(timeoutForRequest);
}

void QGeoPositionInfoSourceMaemo::newPositionUpdate(const QGeoPositionInfo &position)
{    
    /*
        Invalid fixes have NaN for horizontal accuracy regardless of
        whether they come from satellite or non-satellite position methods.

        Satellite fixes always have LOCATION_GPS_DEVICE_TIME_SET.
        If this is not set and we have a numeric value for horizontal
        accuracy then we are dealing with a non-satellite based positioning
        method.

        Since QGeoPositionInfo instances are only considered valid if
        they have a valid coordinate and a valid timestamp, we use
        the current date and time as the timestamp for the network based
        positioning.  This will help in the case where someone wants to
        reply a journey from a log file.

        Based on some logging it looks like satellite and non-satellite
        methods can be distinguished (after the initial fix) by whether
        the time has been set and / or whether the horizontal accuracy
        is above or below around 500 metres.  Using the timestamp
        appears to be more definitive than using the accuracy.
    */

    const bool horizontalAccuracyDefined = !qIsNaN(position.attribute(QGeoPositionInfo::HorizontalAccuracy));
    const bool hasTimeStamp = !position.timestamp().isNull();

    if (horizontalAccuracyDefined) {
        if (hasTimeStamp) {
            //Valid satellite fix
            lastUpdateFromSatellite = position;
        } else {
            //Valid non-satellite fix
            QGeoPositionInfo networkPosition(position);
            networkPosition.setTimestamp(QDateTime::currentDateTime());
            lastUpdateFromNetwork = networkPosition;
        }
    } else {
        //Invalid position update
        if (hasTimeStamp) {
            lastUpdateFromSatellite = QGeoPositionInfo();
        } else {
            lastUpdateFromNetwork = QGeoPositionInfo();
        }
    }
}

void QGeoPositionInfoSourceMaemo::updateTimeoutElapsed()
{
    QGeoPositionInfo position;

    QGeoPositionInfoSource::PositioningMethods methods = preferredPositioningMethods();

    if (methods.testFlag(AllPositioningMethods)) {
        methods |= SatellitePositioningMethods;
        methods |= NonSatellitePositioningMethods;
    }

    if (methods.testFlag(SatellitePositioningMethods) && !methods.testFlag(NonSatellitePositioningMethods)) {
        //only SatellitePositioningMethods preferred
        position = lastUpdateFromSatellite;
    } else if (methods.testFlag(NonSatellitePositioningMethods) && !methods.testFlag(SatellitePositioningMethods)) {
        //only NonSatellitePositioningMethods preferred
        position = lastUpdateFromNetwork;
    } else {
        //AllPositioningMethods or none preferred
        if (lastUpdateFromSatellite.isValid())
            position = lastUpdateFromSatellite;
        else
            position = lastUpdateFromNetwork;
    }

    if (position.isValid()) {
        errorOccurred = false;
        errorSent = false;

        if (positionInfoState & QGeoPositionInfoSourceMaemo::RequestActive) {
            positionInfoState &= ~QGeoPositionInfoSourceMaemo::RequestActive;
            requestTimer->stop();

            if (positionInfoState & QGeoPositionInfoSourceMaemo::Stopped)
                if (LiblocationWrapper::instance()->isActive())
                    LiblocationWrapper::instance()->stop();

            // Ensure that requested position fix is emitted even though
            // powersave is active and GPS would normally be off.
            if ((positionInfoState & QGeoPositionInfoSourceMaemo::PowersaveActive) &&
               (positionInfoState & QGeoPositionInfoSourceMaemo::Stopped)) {
                emit positionUpdated(position);
            }
        }

        // Make sure that if update is triggered when waking up, there
        // is no false position update.
        if (!((positionInfoState & QGeoPositionInfoSourceMaemo::PowersaveActive) &&
             (positionInfoState & QGeoPositionInfoSourceMaemo::Stopped)))
            emit positionUpdated(position);
    } else {
        // if an error occurs when we are updating periodically and we haven't
        // sent an error since the last fix...
        if (!(positionInfoState & QGeoPositionInfoSourceMaemo::RequestActive) &&
                errorOccurred && !errorSent) {
            errorSent = true;
            // we need to emit the updateTimeout signal
            emit updateTimeout();
        }
    }
    activateTimer();
}

void QGeoPositionInfoSourceMaemo::requestTimeoutElapsed()
{
    updateTimer->stop();
    emit updateTimeout();

    positionInfoState &= ~QGeoPositionInfoSourceMaemo::RequestActive;

    if (positionInfoState & QGeoPositionInfoSourceMaemo::Stopped)
        if (LiblocationWrapper::instance()->isActive())
            LiblocationWrapper::instance()->stop();

    activateTimer();
}

void QGeoPositionInfoSourceMaemo::error()
{
    errorOccurred = true;
}

void QGeoPositionInfoSourceMaemo::activateTimer()
{
    if (positionInfoState & QGeoPositionInfoSourceMaemo::RequestActive) {
        updateTimer->start(MINIMUM_UPDATE_INTERVAL);
        return;
    }

    if (positionInfoState & QGeoPositionInfoSourceMaemo::PowersaveActive) {
        if (positionInfoState & QGeoPositionInfoSourceMaemo::Started) {
            // Cannot call stopUpdates() here since we want to keep powersave
            // active.
            if (LiblocationWrapper::instance()->isActive())
                LiblocationWrapper::instance()->stop();
            updateTimer->start(timerInterval - POWERSAVE_POWERON_PERIOD);
            errorOccurred = false;
            errorSent = false;

            positionInfoState &= ~QGeoPositionInfoSourceMaemo::Started;
            positionInfoState |= QGeoPositionInfoSourceMaemo::Stopped;
        } else if (positionInfoState & QGeoPositionInfoSourceMaemo::Stopped) {
            startLocationDaemon();
            updateTimer->start(POWERSAVE_POWERON_PERIOD);
        }
        return;
    }

    if (positionInfoState & QGeoPositionInfoSourceMaemo::Started)
        updateTimer->start(timerInterval);
}

void QGeoPositionInfoSourceMaemo::startLocationDaemon()
{
    if (!(LiblocationWrapper::instance()->isActive()))
        LiblocationWrapper::instance()->start();
    positionInfoState |= QGeoPositionInfoSourceMaemo::Started;
    positionInfoState &= ~QGeoPositionInfoSourceMaemo::Stopped;
}

#include "moc_qgeopositioninfosource_maemo5_p.cpp"
QTM_END_NAMESPACE

