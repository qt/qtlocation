/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeosatelliteinfosource_npe_backend_p.h"
#include <qdebug.h>


// Bitmask Table for satStatus
// Satellite is above elevation mask
#define SATELLITES_STATUS_VISIBLE            0x0002  // --------------1-
// Satellite used in position calculation
#define SATELLITES_STATUS_USED_POS           0x0004  // -------------1--


QGeoSatelliteInfoSourceNpeBackend::QGeoSatelliteInfoSourceNpeBackend(QObject *parent):
    QGeoSatelliteInfoSource(parent),m_locationdConn(0), satOngoing(false),
    mSatelliteError(QGeoSatelliteInfoSource::UnknownSourceError)
{
    requestTimer = new QTimer(this);
    QObject::connect(requestTimer, SIGNAL(timeout()), this, SLOT(requestTimerExpired()));
}


bool QGeoSatelliteInfoSourceNpeBackend::init()
{
    m_locationdConn = new LocationDaemonConnection(this);
    //check for Error: Could not connect to socket (locationd not started?)
    if (!m_locationdConn->connected())
        return(false);
    connect(m_locationdConn, SIGNAL(satelliteUpdate(QList<SatelliteData>)), this, SLOT(onSatelliteUpdate(QList<SatelliteData>)));
    connect(m_locationdConn, SIGNAL(connectionError(LocationDaemonConnection::SocketError)), this, SLOT(onConnectionError(LocationDaemonConnection::SocketError)));
    return(true);
}


void QGeoSatelliteInfoSourceNpeBackend::setUpdateInterval(int msec)
{
    int minInterval = m_locationdConn->minimumUpdateInterval();
    if (msec < minInterval && msec != 0)
        msec = minInterval;
    QGeoSatelliteInfoSource::setUpdateInterval(msec);
    if (!requestTimer->isActive())
        m_locationdConn->setUpdateInterval(msec);
}


int QGeoSatelliteInfoSourceNpeBackend::minimumUpdateInterval() const
{
    return(m_locationdConn->minimumUpdateInterval());
}


void QGeoSatelliteInfoSourceNpeBackend::startUpdates()
{
    if (!satOngoing) {
        satOngoing = true;
        m_locationdConn->startSatelliteUpdates();
    }
}


void QGeoSatelliteInfoSourceNpeBackend::stopUpdates()
{
    if (satOngoing) {
        satOngoing = false;

        // if there's still a pending sat update request, wait for it to complete before stopping satellite updates
        if (!requestTimer->isActive())
            m_locationdConn->stopSatelliteUpdates();
    }
}


void QGeoSatelliteInfoSourceNpeBackend::requestUpdate(int timeout)
{
    // ignore if another requestUpdate is still pending
    if (!requestTimer->isActive()) {
        int minimumInterval = minimumUpdateInterval();
        if (timeout == 0)
            timeout = 5*minimumInterval; // set reasonable timeout if provided timeout is 0
        // do not start request if timeout can not be fulfilled by the source
        if (timeout < minimumInterval) {
            emit requestTimeout();
            return;
        }
        // get satellite update as fast as possible in case of ongoing tracking session
        if ( satOngoing ) {
            if ( QGeoSatelliteInfoSource::updateInterval() != minimumInterval)
                m_locationdConn->setUpdateInterval(minimumInterval);
        } else { // request the update only if no tracking session is active
            m_locationdConn->requestSatelliteUpdate();
        }
        requestTimer->start(timeout);
    }
}


void QGeoSatelliteInfoSourceNpeBackend::requestTimerExpired()
{
    emit requestTimeout();
    shutdownRequestSession();
}


void QGeoSatelliteInfoSourceNpeBackend::shutdownRequestSession()
{
    requestTimer->stop();
    // Restore updateInterval from before Request Session in case of ongoing tracking session
    if ( satOngoing ) {
        int minimumInterval = minimumUpdateInterval();
        if ( QGeoSatelliteInfoSource::updateInterval() != minimumInterval)
            setUpdateInterval(QGeoSatelliteInfoSource::updateInterval());
    } else {
        // If our timer expired before we could get a sat update, make sure we stop the locationd request
        // (which would otherwise go on until a valid satellite update is obtained).
        m_locationdConn->stopSatelliteUpdates();
    }
}


void QGeoSatelliteInfoSourceNpeBackend::onSatelliteUpdate(const QList<SatelliteData> &satellites)
{
    QList<QGeoSatelliteInfo> inUse;
    QList<QGeoSatelliteInfo> inView;
    QListIterator<SatelliteData> it(satellites);

    if (!satellites.empty()) {
        while (it.hasNext())
        {
            QGeoSatelliteInfo satinfo;
            const SatelliteData &satellite = it.next();
            satinfo.setAttribute(QGeoSatelliteInfo::Elevation, satellite.elevation);
            satinfo.setAttribute(QGeoSatelliteInfo::Azimuth, satellite.azimuth);
            satinfo.setSatelliteIdentifier(satellite.id);
            satinfo.setSignalStrength(satellite.signalStrength);
            if (satellite.satSystem == LocationdStrings::PositionInfo::kGPS)
                satinfo.setSatelliteSystem(QGeoSatelliteInfo::GPS);
            else if (satellite.satSystem == LocationdStrings::PositionInfo::kGLONASS)
                satinfo.setSatelliteSystem(QGeoSatelliteInfo::GLONASS);
            else
                satinfo.setSatelliteSystem(QGeoSatelliteInfo::Undefined);
            if (satellite.status & SATELLITES_STATUS_VISIBLE) {
                inView.append(satinfo);
                if (satellite.status & SATELLITES_STATUS_USED_POS)
                    inUse.append(satinfo);
            }
        }
        if (inUse.count() > 0) // emit updated signal if satellite list is not empty
            emit satellitesInUseUpdated(inUse);
        qDebug() << "emit satelliteUpdated signals: in use count: " << inUse.count() << ", in view count: " <<inView.count();
        if (inView.count() > 0)
            emit satellitesInViewUpdated(inView);
        if ( requestTimer->isActive() )
            shutdownRequestSession();
    }
}


QGeoSatelliteInfoSource::Error QGeoSatelliteInfoSourceNpeBackend::error() const
{
    return mSatelliteError;
}


void QGeoSatelliteInfoSourceNpeBackend::setError(QGeoSatelliteInfoSource::Error satelliteError)
{
    mSatelliteError = satelliteError;
    emit QGeoSatelliteInfoSource::error(satelliteError);
}


void QGeoSatelliteInfoSourceNpeBackend::onConnectionError(LocationDaemonConnection::SocketError socketError)
{
    switch (socketError) {
    case LocationDaemonConnection::ClosedError:
        setError(ClosedError);
        break;
    case LocationDaemonConnection::AccessError:
        setError(AccessError);
        break;
    default:
        setError(UnknownSourceError);
    }
}
