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

#include "qgeopositioninfosource_npe_backend_p.h"


QGeoPositionInfoSourceNpeBackend::QGeoPositionInfoSourceNpeBackend(QObject *parent):
    QGeoPositionInfoSource(parent), m_locationdConn(0), trackingOngoing(false), timeoutSent(false),
    mPositionError(QGeoPositionInfoSource::UnknownSourceError)
{
    requestTimer = new QTimer(this);
    QObject::connect(requestTimer, SIGNAL(timeout()), this, SLOT(requestTimerExpired()));
}


bool QGeoPositionInfoSourceNpeBackend::init()
{
    m_locationdConn = new LocationDaemonConnection(this);
    //check for Error: Could not connect to socket (locationd not started?)
    if (!m_locationdConn->connected())
        return(false);
    connect(m_locationdConn, SIGNAL(positionUpdate(PositionData)), this, SLOT(onPositionUpdate(PositionData)));
    connect(m_locationdConn, SIGNAL(connectionError(LocationDaemonConnection::SocketError)), this, SLOT(onConnectionError(LocationDaemonConnection::SocketError)));
    return(true);
}


QGeoPositionInfo QGeoPositionInfoSourceNpeBackend::lastKnownPosition(bool fromSatellitePositioningMethodsOnly) const
{
    const PositionData &position = m_locationdConn->lastKnownPosition(fromSatellitePositioningMethodsOnly);
    QGeoCoordinate coordinate;
    QGeoPositionInfo lastPosition;
    coordinate.setLatitude(position.latitude);
    coordinate.setLongitude(position.longitude);
    coordinate.setAltitude(position.altitude);
    if (coordinate.isValid()) {
        lastPosition.setCoordinate(coordinate);
        lastPosition.setAttribute(QGeoPositionInfo::HorizontalAccuracy, position.horizontalAccuracy);
        lastPosition.setAttribute(QGeoPositionInfo::VerticalAccuracy, position.verticalAccuracy);
        lastPosition.setAttribute(QGeoPositionInfo::GroundSpeed,  position.groundSpeed);
        lastPosition.setAttribute(QGeoPositionInfo::VerticalSpeed, position.verticalSpeed);
        lastPosition.setAttribute(QGeoPositionInfo::MagneticVariation, position.magneticVariation);
        lastPosition.setAttribute(QGeoPositionInfo::Direction, position.bearing);
        QDateTime timestamp = QDateTime::fromString(position.dateTime, "yyyy-MM-ddThh:mm:ss");
        lastPosition.setTimestamp(timestamp);
    } else {
        // return null update as no valid lastKnown Position is available
        coordinate.setLatitude(0);
        coordinate.setLongitude(0);
        coordinate.setAltitude(0);
        lastPosition.setCoordinate(coordinate);
        lastPosition.setAttribute(QGeoPositionInfo::HorizontalAccuracy, 0);
        lastPosition.setAttribute(QGeoPositionInfo::VerticalAccuracy, 0);
        lastPosition.setAttribute(QGeoPositionInfo::GroundSpeed, 0);
        lastPosition.setAttribute(QGeoPositionInfo::VerticalSpeed, 0);
        lastPosition.setAttribute(QGeoPositionInfo::MagneticVariation, 0);
        lastPosition.setAttribute(QGeoPositionInfo::Direction, 0);
    }
    return(lastPosition);
}


QGeoPositionInfoSource::PositioningMethods QGeoPositionInfoSourceNpeBackend::supportedPositioningMethods() const
{
    switch (m_locationdConn->supportedMethods()) {
    case LocationdStrings::PositionInfo::Satellite:
        return(QGeoPositionInfoSource::SatellitePositioningMethods);
    case LocationdStrings::PositionInfo::NonSatellite:
        return(QGeoPositionInfoSource::NonSatellitePositioningMethods);
    default:
        return(QGeoPositionInfoSource::AllPositioningMethods);
    }
}


void QGeoPositionInfoSourceNpeBackend::setPreferredPositioningMethods(PositioningMethods sources)
{
    QGeoPositionInfoSource::setPreferredPositioningMethods(sources);
    LocationdStrings::PositionInfo::PositioningMethods positioningMethod;
    switch (sources){
    case QGeoPositionInfoSource::SatellitePositioningMethods:
        positioningMethod = LocationdStrings::PositionInfo::Satellite;
        break;
    case QGeoPositionInfoSource::NonSatellitePositioningMethods:
        positioningMethod = LocationdStrings::PositionInfo::NonSatellite;
        break;
    default:
        positioningMethod = LocationdStrings::PositionInfo::AllMethods;
    }
    m_locationdConn->setPreferredMethod(positioningMethod);
}


void QGeoPositionInfoSourceNpeBackend::setUpdateInterval(int msec)
{
    int minInterval = m_locationdConn->minimumUpdateInterval();
    if (msec < minInterval && msec != 0)
        msec = minInterval;
    QGeoPositionInfoSource::setUpdateInterval(msec);
    if (!requestTimer->isActive())
        m_locationdConn->setUpdateInterval(msec);
}


int QGeoPositionInfoSourceNpeBackend::minimumUpdateInterval() const
{
    return(m_locationdConn->minimumUpdateInterval());
}


void QGeoPositionInfoSourceNpeBackend::startUpdates()
{
    if (!trackingOngoing) {
        trackingOngoing = true;
        m_locationdConn->startPositionUpdates();
    }
}


void QGeoPositionInfoSourceNpeBackend::stopUpdates()
{
    if (trackingOngoing) {
        trackingOngoing = false;

        // if there's still a pending position request, wait for it to complete before stopping position updates
        if (!requestTimer->isActive())
            m_locationdConn->stopPositionUpdates();
    }
}


void QGeoPositionInfoSourceNpeBackend::requestUpdate(int timeout)
{
    // ignore if another requestUpdate is still pending
    if (!requestTimer->isActive()) {
        int minimumInterval = minimumUpdateInterval();
        // set reasonable timeout for the source if timeout is 0
        if (timeout == 0)
            timeout = 5*minimumInterval;
        // do not start request if timeout can not be fulfilled by the source
        if (timeout < minimumInterval) {
            emit updateTimeout();
            return;
        }
        // get position as fast as possible in case of ongoing satellite based session
        if ( trackingOngoing ) {
            if ( QGeoPositionInfoSource::updateInterval() != minimumInterval)
                m_locationdConn->setUpdateInterval(minimumInterval);
        } else {         // request the update only if no tracking session is active
            m_locationdConn->requestPositionUpdate();
        }
        requestTimer->start(timeout);
    }
}


void QGeoPositionInfoSourceNpeBackend::requestTimerExpired()
{
    emit updateTimeout();
    shutdownRequestSession();
}



void QGeoPositionInfoSourceNpeBackend::shutdownRequestSession()
{
    requestTimer->stop();
    // Restore updateInterval from before Request Session
    if (trackingOngoing) {
        int minimumInterval = minimumUpdateInterval();
        if ( QGeoPositionInfoSource::updateInterval() != minimumInterval)
            setUpdateInterval(QGeoPositionInfoSource::updateInterval());
    } else {
        // If our timer expired before we could get a fix, make sure we stop the location daemon position request
        // (which would otherwise go on until a fix is obtained).
        m_locationdConn->stopPositionUpdates();
    }
}


void QGeoPositionInfoSourceNpeBackend::onPositionUpdate(const PositionData &position)
{
    if (position.valid) {
        QGeoPositionInfo update;
        QGeoCoordinate coordinate;
        coordinate.setLatitude(position.latitude);
        coordinate.setLongitude(position.longitude);
        coordinate.setAltitude(position.altitude);
        if (coordinate.isValid()) {
            update.setCoordinate(coordinate);
            update.setAttribute(QGeoPositionInfo::HorizontalAccuracy, position.horizontalAccuracy);
            update.setAttribute(QGeoPositionInfo::VerticalAccuracy, position.verticalAccuracy);
            update.setAttribute(QGeoPositionInfo::GroundSpeed, position.groundSpeed);
            update.setAttribute(QGeoPositionInfo::VerticalSpeed, position.verticalSpeed);
            update.setAttribute(QGeoPositionInfo::MagneticVariation, position.magneticVariation);
            update.setAttribute(QGeoPositionInfo::Direction, position.bearing);
            QDateTime timestamp = QDateTime::fromString(position.dateTime, "yyyy-MM-ddThh:mm:ss");
            update.setTimestamp(timestamp);
            emit positionUpdated(update);
            timeoutSent = false;
            if ( requestTimer->isActive() )
                shutdownRequestSession();
        } else {
            if (!timeoutSent) {
                emit updateTimeout();
                timeoutSent = true;
            }
        }
    }
}



QGeoPositionInfoSource::Error QGeoPositionInfoSourceNpeBackend::error() const
{
    return mPositionError;
}


void QGeoPositionInfoSourceNpeBackend::setError(QGeoPositionInfoSource::Error positionError)
{
    mPositionError = positionError;
    emit QGeoPositionInfoSource::error(positionError);
}


void QGeoPositionInfoSourceNpeBackend::onConnectionError(LocationDaemonConnection::SocketError socketError)
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
