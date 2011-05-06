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

#include "qgeosatelliteinfosource_maemo5_p.h"
#include "liblocationwrapper_p.h"

QTM_BEGIN_NAMESPACE

QGeoSatelliteInfoSourceMaemo::QGeoSatelliteInfoSourceMaemo(QObject *parent)
        : QGeoSatelliteInfoSource(parent)
{
    client_id_ = -1;
    timerInterval = DEFAULT_UPDATE_INTERVAL;
    updateTimer = new QTimer(this);
    updateTimer->setSingleShot(true);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(satelliteStatus()));

    requestTimer = new QTimer(this);
    requestTimer->setSingleShot(true);
    connect(requestTimer, SIGNAL(timeout()), this, SLOT(requestTimeoutElapsed()));

    satelliteInfoState = QGeoSatelliteInfoSourceMaemo::Undefined;
}

int QGeoSatelliteInfoSourceMaemo::init()
{
    if (LiblocationWrapper::instance()->inited())
        return INIT_OK;
    else
        return INIT_FAILED;
}

void QGeoSatelliteInfoSourceMaemo::setUpdateInterval(int msec)
{
    bool updateTimerInterval = false;

    if (satelliteInfoState & QGeoSatelliteInfoSourceMaemo::PowersaveActive)
        if (satelliteInfoState & QGeoSatelliteInfoSourceMaemo::Stopped)
            updateTimerInterval = true;

    timerInterval = (msec < MINIMUM_UPDATE_INTERVAL) ? MINIMUM_UPDATE_INTERVAL : msec;

    if (timerInterval >= POWERSAVE_THRESHOLD)
        satelliteInfoState |= QGeoSatelliteInfoSourceMaemo::PowersaveActive;
    else
        satelliteInfoState &= ~QGeoSatelliteInfoSourceMaemo::PowersaveActive;

    // If powersave has been active when new update interval has been set,
    // ensure that timer is started.
    if (updateTimerInterval)
        startLocationDaemon();

    // Ensure that new timer interval is taken into use immediately.
    activateTimer();
}

void QGeoSatelliteInfoSourceMaemo::startUpdates()
{
    startLocationDaemon();

    // Ensure that powersave is selected, if stopUpdates() has been called,
    // but selected update interval is still greater than POWERSAVE_THRESHOLD.
    if (timerInterval >= POWERSAVE_THRESHOLD)
        satelliteInfoState |= QGeoSatelliteInfoSourceMaemo::PowersaveActive;

    activateTimer();
}

void QGeoSatelliteInfoSourceMaemo::stopUpdates()
{
    satelliteInfoState &= ~QGeoSatelliteInfoSourceMaemo::PowersaveActive;

    if (!(satelliteInfoState & QGeoSatelliteInfoSourceMaemo::RequestActive)) {
        updateTimer->stop();
        if (LiblocationWrapper::instance()->isActive())
            LiblocationWrapper::instance()->stop();
    }

    satelliteInfoState &= ~QGeoSatelliteInfoSourceMaemo::Started;
    satelliteInfoState |= QGeoSatelliteInfoSourceMaemo::Stopped;
}

void QGeoSatelliteInfoSourceMaemo::requestUpdate(int timeout)
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
        if (satelliteInfoState & QGeoSatelliteInfoSourceMaemo::RequestActive)
            return;

        emit requestTimeout();
        return;
    } else {
        timeoutForRequest = timeout;
    }

    satelliteInfoState |= QGeoSatelliteInfoSourceMaemo::RequestActive;

    if (!(LiblocationWrapper::instance()->isActive()))
        LiblocationWrapper::instance()->start();

    activateTimer();
    requestTimer->start(timeoutForRequest);
}

void QGeoSatelliteInfoSourceMaemo::satelliteStatus()
{
    QList<QGeoSatelliteInfo> satellitesInView =
        LiblocationWrapper::instance()->satellitesInView();
    QList<QGeoSatelliteInfo> satellitesInUse =
        LiblocationWrapper::instance()->satellitesInUse();

    if (satelliteInfoState & QGeoSatelliteInfoSourceMaemo::RequestActive) {
        satelliteInfoState &= ~QGeoSatelliteInfoSourceMaemo::RequestActive;

        requestTimer->stop();

        if (satelliteInfoState & QGeoSatelliteInfoSourceMaemo::Stopped) {
            if (LiblocationWrapper::instance()->isActive()) {
                LiblocationWrapper::instance()->stop();
            }
        }

        // Ensure that requested satellite info is emitted even though
        // powersave is active and GPS would normally be off.
        if ((satelliteInfoState & QGeoSatelliteInfoSourceMaemo::PowersaveActive) &&
                (satelliteInfoState & QGeoSatelliteInfoSourceMaemo::Stopped)) {
            if (satellitesInView.length()) {
                emit satellitesInViewUpdated(satellitesInView);
                emit satellitesInUseUpdated(satellitesInUse);
            }
        }
    }

    // Make sure that if update is triggered when waking up, there
    // is no false position update.
    if (!((satelliteInfoState & QGeoSatelliteInfoSourceMaemo::PowersaveActive) &&
            (satelliteInfoState & QGeoSatelliteInfoSourceMaemo::Stopped))) {
        if (satellitesInView.length()) {
            emit satellitesInViewUpdated(satellitesInView);
            emit satellitesInUseUpdated(satellitesInUse);
        }
    }

    activateTimer();
}

void QGeoSatelliteInfoSourceMaemo::requestTimeoutElapsed()
{
    updateTimer->stop();
    emit requestTimeout();

    satelliteInfoState &= ~QGeoSatelliteInfoSourceMaemo::RequestActive;

    if (satelliteInfoState & QGeoSatelliteInfoSourceMaemo::Stopped)
        if (LiblocationWrapper::instance()->isActive())
            LiblocationWrapper::instance()->stop();

    activateTimer();
}

void QGeoSatelliteInfoSourceMaemo::activateTimer()
{
    if (satelliteInfoState & QGeoSatelliteInfoSourceMaemo::RequestActive) {
        updateTimer->start(MINIMUM_UPDATE_INTERVAL);
        return;
    }

    if (satelliteInfoState & QGeoSatelliteInfoSourceMaemo::PowersaveActive) {
        if (satelliteInfoState & QGeoSatelliteInfoSourceMaemo::Started) {
            // Cannot call stopUpdates() here since we want to keep powersave
            // active.
            if (LiblocationWrapper::instance()->isActive())
                LiblocationWrapper::instance()->stop();
            updateTimer->start(timerInterval - POWERSAVE_POWERON_PERIOD);

            satelliteInfoState &= ~QGeoSatelliteInfoSourceMaemo::Started;
            satelliteInfoState |= QGeoSatelliteInfoSourceMaemo::Stopped;
        } else if (satelliteInfoState & QGeoSatelliteInfoSourceMaemo::Stopped) {
            startLocationDaemon();
            updateTimer->start(POWERSAVE_POWERON_PERIOD);
        }
        return;
    }

    if (satelliteInfoState & QGeoSatelliteInfoSourceMaemo::Started)
        updateTimer->start(timerInterval);
}

void QGeoSatelliteInfoSourceMaemo::startLocationDaemon()
{
    if (!(LiblocationWrapper::instance()->isActive()))
        LiblocationWrapper::instance()->start();
    satelliteInfoState |= QGeoSatelliteInfoSourceMaemo::Started;
    satelliteInfoState &= ~QGeoSatelliteInfoSourceMaemo::Stopped;
}

#include "moc_qgeosatelliteinfosource_maemo5_p.cpp"
QTM_END_NAMESPACE

