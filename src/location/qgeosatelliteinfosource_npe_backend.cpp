/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeosatelliteinfosource_npe_backend_p.h"
#include <sys/stat.h>

QT_USE_NAMESPACE_JSONSTREAM


// API for socket communication towards locationd
const QString ksatelliteStartUpdates = QLatin1String("satelliteStartUpdates");
const QString ksatelliteRequestUpdate = QLatin1String("satelliteRequestUpdate");
const QString ksatelliteStopUpdates = QLatin1String("satelliteStopUpdates");
const QString ksatelliteInfoUpdate = QLatin1String("satelliteInfoUpdate");
const QString ksetUpdateInterval = QLatin1String("setUpdateInterval");
const QString kgetMinimumUpdateInterval = QLatin1String("getMinimumUpdateInterval");
const QString kgetMinimumUpdateIntervalReply = QLatin1String("getMinimumUpdateIntervalReply");
// Attributes for socket communication towards locationd
const QString ksatId = QLatin1String("satId");
const QString kazimuth = QLatin1String("azimuth");
const QString kelevation = QLatin1String("elevation");
const QString ksignalStrength = QLatin1String("signalStrength");
const QString ksatList = QLatin1String("satList");
const QString ksystem = QLatin1String("system"); // {"GPS", "SBASS", "GLONASS", "QZSS", "IMES"}
const QString kGPS = QLatin1String("GPS");
const QString kGLONASS = QLatin1String("GLONASS");
const QString ksatStatus = QLatin1String("satStatus");
const QString kinterval = QLatin1String("interval");


// Bitmask Table for satStatus
// Satellite is above elevation mask
#define SATELLITES_STATUS_VISIBLE            0x0002  // --------------1-
// Satellite used in position calculation
#define SATELLITES_STATUS_USED_POS           0x0004  // -------------1--


QGeoSatelliteInfoSourceNpeBackend::QGeoSatelliteInfoSourceNpeBackend(QObject *parent): QGeoSatelliteInfoSource(parent),
    satOngoing(false), mSatelliteError(QGeoSatelliteInfoSource::UnknownSourceError)
{
    requestTimer = new QTimer(this);
    QObject::connect(requestTimer, SIGNAL(timeout()), this, SLOT(requestTimerExpired()));
    qRegisterMetaType<QJsonObject>("QJsonObject");
}


bool QGeoSatelliteInfoSourceNpeBackend::init()
{
    struct stat buf;
    if (stat("/var/run/locationd/locationd.socket", &buf) == 0) {
        mSocket = new QLocalSocket(this);
        if (mSocket) {
            connect(mSocket, SIGNAL(connected()), this, SLOT(onSocketConnected()));
            connect(mSocket, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()));
            connect(mSocket, SIGNAL(error(QLocalSocket::LocalSocketError)), this, SLOT(onSocketError(QLocalSocket::LocalSocketError)));
            mStream = new JsonStream(mSocket);
            if (mStream) {
                connect(mStream, SIGNAL(messageReceived(const QJsonObject&)), this, SLOT(onStreamReceived(const QJsonObject&)), Qt::QueuedConnection);
            }
            mSocket->connectToServer((QLatin1String)"/var/run/locationd/locationd.socket");
            return(mSocket->waitForConnected(500)); // wait up to 0.5 seconds to get connected, otherwise return false
        }
    }
    return(false);
}


void QGeoSatelliteInfoSourceNpeBackend::setUpdateInterval(int msec)
{
    QEventLoop loop; // loop to wait for response from locationd (asynchronous socket connection)
    connect( this, SIGNAL(minimumUpdateIntervalReceived()), &loop, SLOT(quit()));
    QJsonObject action;
    action[JsonDbString::kActionStr] = kgetMinimumUpdateInterval;
    mStream->send(action);
    loop.exec(); // wait for minimumUpdateIntervalReceived() signal sent by slot onStreamReceived
    if (msec < minInterval && msec != 0)
        msec = minInterval;
    QGeoSatelliteInfoSource::setUpdateInterval(msec);
    if (!requestTimer->isActive()) {
        QJsonObject actionUpdate;
        QJsonObject object;
        actionUpdate[JsonDbString::kActionStr] = ksetUpdateInterval;
        object[kinterval] = msec;
        actionUpdate[JsonDbString::kDataStr] = object;
        mStream->send(actionUpdate);
    }
}


int QGeoSatelliteInfoSourceNpeBackend::minimumUpdateInterval() const
{
    QEventLoop loop; // loop to wait for response from locationd (asynchronous socket connection)
    connect( this, SIGNAL(minimumUpdateIntervalReceived()), &loop, SLOT(quit()));
    QJsonObject action;
    action[JsonDbString::kActionStr] = kgetMinimumUpdateInterval;
    mStream->send(action);
    loop.exec(); // wait for minimumUpdateIntervalReceived() signal sent by slot onStreamReceived
    return(minInterval);
}


void QGeoSatelliteInfoSourceNpeBackend::startUpdates()
{
    if (!satOngoing) {
        satOngoing = true;
        QJsonObject action;
        action[JsonDbString::kActionStr] = ksatelliteStartUpdates;
        mStream->send(action);
    }
}


void QGeoSatelliteInfoSourceNpeBackend::stopUpdates()
{
    if (satOngoing && !requestTimer->isActive()) {
        satOngoing = false;
        QJsonObject action;
        action[JsonDbString::kActionStr] = ksatelliteStopUpdates;
        mStream->send(action);
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
            if ( QGeoSatelliteInfoSource::updateInterval() != minimumInterval) {
                QJsonObject actionUpdate;
                QJsonObject object;
                actionUpdate[JsonDbString::kActionStr] = ksetUpdateInterval;
                object[kinterval] = minimumInterval;
                actionUpdate[JsonDbString::kDataStr] = object;
                mStream->send(actionUpdate);
            }
        }
        // request the update only if no tracking session is active
        if ( !satOngoing) {
            QJsonObject action;
            action[JsonDbString::kActionStr] = ksatelliteRequestUpdate;
            mStream->send(action);
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
    }
}


void QGeoSatelliteInfoSourceNpeBackend::onStreamReceived(const QJsonObject& jsonObject)
{
    // this slot handles the communication received from locationd socket
    QVariantMap map = jsonObject.toVariantMap();
    if (map.contains(JsonDbString::kActionStr)) {
        QString action = map.value(JsonDbString::kActionStr).toString();

        if (action == kgetMinimumUpdateIntervalReply) {
            QVariantMap tmp = map.value(JsonDbString::kDataStr).toMap();
            minInterval = tmp.value(kinterval).toInt();
            emit minimumUpdateIntervalReceived();
        }

        if (action == ksatelliteInfoUpdate) {
            QVariantMap sat_info = map.value(JsonDbString::kDataStr).toMap();
            if (sat_info.contains(ksatList))
            {
                QVariantList sat_list = sat_info.value(ksatList).toList();
                QList<QGeoSatelliteInfo> inUse;
                QList<QGeoSatelliteInfo> inView;
                if (!sat_list.isEmpty()) {
                    while (!sat_list.isEmpty())
                    {
                        QVariantMap sat_elem = sat_list.takeFirst().toMap();
                        QGeoSatelliteInfo satinfo;
                        satinfo.setAttribute(QGeoSatelliteInfo::Elevation, sat_elem.value(kelevation).toReal());
                        satinfo.setAttribute(QGeoSatelliteInfo::Azimuth, sat_elem.value(kazimuth).toReal());
                        satinfo.setSatelliteIdentifier(sat_elem.value(ksatId).toInt());
                        satinfo.setSignalStrength(sat_elem.value(ksignalStrength).toInt());
                        if (sat_elem.value(ksystem).toString() == kGPS)
                            satinfo.setSatelliteSystem(QGeoSatelliteInfo::GPS);
                        else if (sat_elem.value(ksystem).toString() == kGLONASS)
                            satinfo.setSatelliteSystem(QGeoSatelliteInfo::GLONASS);
                        else
                            satinfo.setSatelliteSystem(QGeoSatelliteInfo::Undefined);
                        if (sat_elem.value(ksatStatus).toUInt() & SATELLITES_STATUS_VISIBLE) {
                            inView.append(satinfo);
                            if (sat_elem.value(ksatStatus).toUInt() & SATELLITES_STATUS_USED_POS)
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
        }
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


void QGeoSatelliteInfoSourceNpeBackend::onSocketError(QLocalSocket::LocalSocketError mError)
{
    switch (mError) {
    case QLocalSocket::PeerClosedError:
        setError(ClosedError);
        break;
    case QLocalSocket::SocketAccessError:
        setError(AccessError);
        break;
    default:
        setError(UnknownSourceError);
    }
}


void QGeoSatelliteInfoSourceNpeBackend::onSocketConnected()
{
}


void QGeoSatelliteInfoSourceNpeBackend::onSocketDisconnected()
{
}
