/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeopositioninfosource_npe_backend_p.h"
#include <sys/stat.h>


// API for socket communication towards nld
const QString kstartUpdates = QLatin1String("startUpdates");
const QString krequestUpdate = QLatin1String("requestUpdate");
const QString kstopUpdates = QLatin1String("stopUpdates");
const QString ksetUpdateInterval = QLatin1String("setUpdateInterval");
const QString ksetPreferredMethod = QLatin1String("setPreferredMethod");
const QString kgetMinimumUpdateInterval = QLatin1String("getMinimumUpdateInterval"); // Request
const QString kgetMinimumUpdateIntervalReply = QLatin1String("getMinimumUpdateIntervalReply"); // Response
const QString kgetSupportedMethods = QLatin1String("getSupportedMethods");
const QString kgetSupportedMethodsReply = QLatin1String("getSupportedMethodsReply");
const QString kgetLastKnownPosition = QLatin1String("getLastKnownPosition");
const QString kgetLastKnownPositionReply = QLatin1String("getLastKnownPositionReply");
const QString kpositionUpdate = QLatin1String("positionUpdate"); // Notification

// Attributes for socket communication towards nld
const QString kinterval = QLatin1String("interval");
const QString ksatelliteOnly = QLatin1String("satelliteOnly");
const QString klatitude = QLatin1String("latitude");
const QString klongitude = QLatin1String("longitude");
const QString khorizontalAccuracy = QLatin1String("horizontalAccuracy");
const QString kaltitude = QLatin1String("altitude");
const QString kverticalAccuracy = QLatin1String("verticalAccuracy");
const QString kgroundSpeed = QLatin1String("groundSpeed");
const QString kverticalSpeed = QLatin1String("verticalSpeed");
const QString kbearing = QLatin1String("bearing");
const QString kmagneticVariation = QLatin1String("magneticVariation");
const QString ktimestamp = QLatin1String("timestamp");
const QString kmethod = QLatin1String("method");
const QString kvalid = QLatin1String("valid");



QGeoPositionInfoSourceNpeBackend::QGeoPositionInfoSourceNpeBackend(QObject *parent):
    QGeoPositionInfoSource(parent), locationOngoing(false), timeoutSent(false)
{
    requestTimer = new QTimer(this);
    QObject::connect(requestTimer, SIGNAL(timeout()), this, SLOT(requestTimerExpired()));
}


bool QGeoPositionInfoSourceNpeBackend::init()
{
    struct stat buf;
    if (stat("/var/run/nld/nld.socket", &buf) == 0) {
        mSocket = new QLocalSocket(this);
        if (mSocket) {
            connect(mSocket, SIGNAL(connected()), this, SLOT(onSocketConnected()));
            connect(mSocket, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()));
            mStream = new VariantStream(mSocket);
            if (mStream) {
                connect(mStream, SIGNAL(receive(const QVariantMap&)), this, SLOT(onStreamReceived(const QVariantMap&)), Qt::QueuedConnection);
            }
            mSocket->connectToServer("/var/run/nld/nld.socket");
            return(mSocket->waitForConnected(500)); // wait up to 0.5 seconds to get connected, otherwise return false
        }
    }
    return(false);
}


QGeoPositionInfo QGeoPositionInfoSourceNpeBackend::lastKnownPosition(bool fromSatellitePositioningMethodsOnly) const
{
    QEventLoop loop; // loop to wait for response from nld (asynchronous socket connection)
    connect( this, SIGNAL(lastKnownPositionReceived()), &loop, SLOT(quit()));
    QVariantMap action;
    QVariantMap object;
    action.insert(JsonDbString::kActionStr, kgetLastKnownPosition);
    object.insert(ksatelliteOnly, fromSatellitePositioningMethodsOnly);
    action.insert(JsonDbString::kDataStr, object);
    mStream->send(action);
    loop.exec(); // wait for  signal lastKnownPositionReceived()sent by slot onStreamReceived
    return(lastPosition);
}


QGeoPositionInfoSource::PositioningMethods QGeoPositionInfoSourceNpeBackend::supportedPositioningMethods() const
{
    QEventLoop loop; // loop to wait for response from nld (asynchronous socket connection)
    connect( this, SIGNAL(supportedPositioningMethodsReceived()), &loop, SLOT(quit()));
    QVariantMap action;
    action.insert(JsonDbString::kActionStr, kgetSupportedMethods);
    mStream->send(action);
    loop.exec(); // wait for supportedPositioningMethodsReceived() signal sent by slot onStreamReceived
    switch (supportedMethods){
    case QGeoPositionInfoSource::SatellitePositioningMethods:
        return(QGeoPositionInfoSource::SatellitePositioningMethods);
    case QGeoPositionInfoSource::NonSatellitePositioningMethods:
        return(QGeoPositionInfoSource::NonSatellitePositioningMethods);
    default:
        return(QGeoPositionInfoSource::AllPositioningMethods);
    }
}


void QGeoPositionInfoSourceNpeBackend::setUpdateInterval(int msec)
{
    QVariantMap action;
    QEventLoop loop; // loop to wait for response from nld (asynchronous socket connection)
    connect( this, SIGNAL(minimumUpdateIntervalReceived()), &loop, SLOT(quit()));
    action.insert(JsonDbString::kActionStr, kgetMinimumUpdateInterval);
    mStream->send(action);
    loop.exec(); // wait for minimumUpdateIntervalReceived() signal sent by slot onStreamReceived
    if (msec < minInterval && msec != 0)
        msec = minInterval;
    QGeoPositionInfoSource::setUpdateInterval(msec);
    if (!requestTimer->isActive()) {
    QVariantMap actionUpdate;
    QVariantMap object;
    actionUpdate.insert(JsonDbString::kActionStr, ksetUpdateInterval);
    object.insert(kinterval, msec);
    actionUpdate.insert(JsonDbString::kDataStr, object);
    mStream->send(actionUpdate);
    }
}


void QGeoPositionInfoSourceNpeBackend::setPreferredPositioningMethods(PositioningMethods sources)
{
    QGeoPositionInfoSource::setPreferredPositioningMethods(sources);
    QVariantMap action;
    QVariantMap object;
    action.insert(JsonDbString::kActionStr, ksetPreferredMethod);
    object.insert(kmethod, (uint)sources);
    action.insert(JsonDbString::kDataStr, object);
    mStream->send(action);
}


int QGeoPositionInfoSourceNpeBackend::minimumUpdateInterval() const
{
    QVariantMap action;
    QEventLoop loop; // loop to wait for response from nld (asynchronous socket connection)
    connect( this, SIGNAL(minimumUpdateIntervalReceived()), &loop, SLOT(quit()));
    action.insert(JsonDbString::kActionStr, kgetMinimumUpdateInterval);
    mStream->send(action);
    loop.exec(); // wait for minimumUpdateIntervalReceived() signal sent by slot onStreamReceived
    return(minInterval);
}


void QGeoPositionInfoSourceNpeBackend::startUpdates()
{
    if (!locationOngoing) {
        locationOngoing = true;
        QVariantMap action;
        action.insert(JsonDbString::kActionStr, kstartUpdates);
        mStream->send(action);
    }
}


void QGeoPositionInfoSourceNpeBackend::stopUpdates()
{
    if ( locationOngoing && !requestTimer->isActive() ) {
        locationOngoing = false;
        QVariantMap action;
        action.insert(JsonDbString::kActionStr, kstopUpdates);
        mStream->send(action);
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
        // do not start request if timeout can not be fullfilled by the source
        if (timeout < minimumInterval) {
            emit updateTimeout();
            return;
        }
        // get position as fast as possible in case of ongoing satellite based session
        if ( locationOngoing ) {
            if ( QGeoPositionInfoSource::updateInterval() != minimumInterval) {
                QVariantMap actionUpdate;
                QVariantMap object;
                actionUpdate.insert(JsonDbString::kActionStr, ksetUpdateInterval);
                object.insert(kinterval, minimumInterval);
                actionUpdate.insert(JsonDbString::kDataStr, object);
                mStream->send(actionUpdate);
            }
        }
        // request the update only if no tracking session is active
        if ( !locationOngoing) {
            QVariantMap action;
            action.insert(JsonDbString::kActionStr, krequestUpdate);
            mStream->send(action);
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
    int minimumInterval = minimumUpdateInterval();
    if ( QGeoPositionInfoSource::updateInterval() != minimumInterval)
        setUpdateInterval(QGeoPositionInfoSource::updateInterval());
}


void QGeoPositionInfoSourceNpeBackend::onStreamReceived(const QVariantMap& map)
{
    // this slot handles the communication received from nld socket
    if (map.contains(JsonDbString::kActionStr)) {
        QString action = map.value(JsonDbString::kActionStr).toString();

        if (action == kgetMinimumUpdateIntervalReply) {
            QVariantMap tmp = map.value(JsonDbString::kDataStr).toMap();
            minInterval = tmp.value(kinterval).toInt();
            emit minimumUpdateIntervalReceived();
        }

        else if (action == kgetSupportedMethodsReply) {
            QVariantMap tmp = map.value(JsonDbString::kDataStr).toMap();
            supportedMethods = tmp.value(kmethod).toUInt();
            emit supportedPositioningMethodsReceived();
        }

        else if (action == kgetLastKnownPositionReply) {
            QVariantMap tmp = map.value(JsonDbString::kDataStr).toMap();
            QGeoCoordinate coordinate;
            coordinate.setLatitude(tmp.value(klatitude).toDouble());
            coordinate.setLongitude(tmp.value(klongitude).toDouble());
            coordinate.setAltitude(tmp.value(kaltitude).toDouble());
            if (coordinate.isValid()) {
                lastPosition.setCoordinate(coordinate);
                lastPosition.setAttribute(QGeoPositionInfo::HorizontalAccuracy, tmp.value(khorizontalAccuracy).toReal());
                lastPosition.setAttribute(QGeoPositionInfo::VerticalAccuracy, tmp.value(kverticalAccuracy).toReal());
                lastPosition.setAttribute(QGeoPositionInfo::GroundSpeed, tmp.value(kgroundSpeed).toReal());
                lastPosition.setAttribute(QGeoPositionInfo::VerticalSpeed, tmp.value(kverticalSpeed).toReal());
                lastPosition.setAttribute(QGeoPositionInfo::MagneticVariation, tmp.value(kmagneticVariation).toReal());
                lastPosition.setAttribute(QGeoPositionInfo::Direction, tmp.value(kbearing).toReal());
                QDateTime timestamp = tmp.value(ktimestamp).toDateTime();
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
            emit lastKnownPositionReceived();
        }

        else if (action == kpositionUpdate) {
            QVariantMap tmp = map.value(JsonDbString::kDataStr).toMap();
            QGeoPositionInfo update;
            QGeoCoordinate coordinate;
            coordinate.setLatitude(tmp.value(klatitude).toDouble());
            coordinate.setLongitude(tmp.value(klongitude).toDouble());
            coordinate.setAltitude(tmp.value(kaltitude).toDouble());
            if (coordinate.isValid()) {
                update.setCoordinate(coordinate);
                update.setAttribute(QGeoPositionInfo::HorizontalAccuracy, tmp.value(khorizontalAccuracy).toReal());
                update.setAttribute(QGeoPositionInfo::VerticalAccuracy, tmp.value(kverticalAccuracy).toReal());
                update.setAttribute(QGeoPositionInfo::GroundSpeed, tmp.value(kgroundSpeed).toReal());
                update.setAttribute(QGeoPositionInfo::VerticalSpeed, tmp.value(kverticalSpeed).toReal());
                update.setAttribute(QGeoPositionInfo::MagneticVariation, tmp.value(kmagneticVariation).toReal());
                update.setAttribute(QGeoPositionInfo::Direction, tmp.value(kbearing).toReal());
                QDateTime timestamp = tmp.value(ktimestamp).toDateTime();
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
}


void QGeoPositionInfoSourceNpeBackend::onSocketConnected()
{
}


void QGeoPositionInfoSourceNpeBackend::onSocketDisconnected()
{
}
