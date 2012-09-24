/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#include "qgeopositioninfosource_maemo_p.h"
#include <iostream>
#include <QDateTime>

using namespace std;

QT_BEGIN_NAMESPACE

QGeoPositionInfoSourceMaemo::QGeoPositionInfoSourceMaemo(QObject *parent): QGeoPositionInfoSource(parent)
{
    requestTimer = new QTimer(this);
    QObject::connect(requestTimer, SIGNAL(timeout()), this, SLOT(requestTimerExpired()));
    locationOngoing = false;
}


int QGeoPositionInfoSourceMaemo::init()
{
    dbusComm = new DBusComm(this);
    int status = dbusComm->init();

    if (status == 0) {
        QObject::connect(dbusComm, SIGNAL(receivedPositionUpdate(const QGeoPositionInfo &)),
                         this, SLOT(newPositionUpdate(const QGeoPositionInfo &)));
        QObject::connect(dbusComm, SIGNAL(serviceConnected()),
                         this, SLOT(onServiceConnect()));
        QObject::connect(dbusComm, SIGNAL(serviceDisconnected()),
                         this, SLOT(onServiceDisconnect()));
    }
    return status;
}

void QGeoPositionInfoSourceMaemo::onServiceDisconnect()
{
    //
}

void QGeoPositionInfoSourceMaemo::onServiceConnect()
{
    DBusComm::Command command =
        (DBusComm::Command)
        ( DBusComm::CommandSetInterval |
          DBusComm::CommandSetMethods );

    int interval = QGeoPositionInfoSource::updateInterval();
    QGeoPositionInfoSource::PositioningMethods method;
    method = QGeoPositionInfoSource::preferredPositioningMethods();

    if (locationOngoing) {
        command = (DBusComm::Command) ( command | DBusComm::CommandStart );
        dbusComm->sendConfigRequest(command, method, interval);
    } else {
        dbusComm->sendConfigRequest(command, method, interval);
    }
}

void QGeoPositionInfoSourceMaemo::newPositionUpdate(const QGeoPositionInfo &update)
{
    if(update.isValid()) {
        emit positionUpdated(update);
        if ( requestTimer->isActive() )
            shutdownRequestSession();
    } else {
        if ( !requestTimer->isActive() )
            emit updateTimeout();
    }
}


QGeoPositionInfo QGeoPositionInfoSourceMaemo::lastKnownPosition(bool fromSatellitePositioningMethodsOnly) const
{
    QGeoPositionInfo update = dbusComm->requestLastKnownPosition(fromSatellitePositioningMethodsOnly);

    return update;
}


QGeoPositionInfoSource::PositioningMethods QGeoPositionInfoSourceMaemo::supportedPositioningMethods() const
{
    return dbusComm->availableMethods();
}


void QGeoPositionInfoSourceMaemo::setUpdateInterval(int msec)
{
    qint32 min = dbusComm->minimumInterval();
    msec = (msec < min) ? min : msec;
    QGeoPositionInfoSource::setUpdateInterval(msec);

    dbusComm->sendConfigRequest(dbusComm->CommandSetInterval, 0, msec);
}


void QGeoPositionInfoSourceMaemo::setPreferredPositioningMethods(PositioningMethods sources)
{
    QGeoPositionInfoSource::setPreferredPositioningMethods(sources);
    dbusComm->sendConfigRequest(dbusComm->CommandSetMethods, sources, 0);
}


int QGeoPositionInfoSourceMaemo::minimumUpdateInterval() const
{
    return dbusComm->minimumInterval();
}

QGeoPositionInfoSource::Error QGeoPositionInfoSourceMaemo::error() const
{
    return UnknownSourceError;
}

void QGeoPositionInfoSourceMaemo::startUpdates()
{
    locationOngoing = true;
    if ( !requestTimer->isActive() )
        dbusComm->sendConfigRequest(DBusComm::CommandStart, 0, 0);
}


void QGeoPositionInfoSourceMaemo::stopUpdates()
{
    locationOngoing = false;
    if ( !requestTimer->isActive() )
        dbusComm->sendConfigRequest(dbusComm->CommandStop, 0, 0);
}


void QGeoPositionInfoSourceMaemo::requestUpdate(int timeout)
{
    if ( QGeoPositionInfoSource::updateInterval() !=
         dbusComm->minimumInterval() )
        dbusComm->sendConfigRequest(dbusComm->CommandSetInterval, 0,
                                    dbusComm->minimumInterval());

    if ( !QGeoPositionInfoSource::preferredPositioningMethods().testFlag(QGeoPositionInfoSource::AllPositioningMethods) )
        dbusComm->sendConfigRequest(dbusComm->CommandSetMethods,
                                    QGeoPositionInfoSource::AllPositioningMethods, 0);

    if ( !locationOngoing )
        dbusComm->sendConfigRequest(dbusComm->CommandStart, 0, 0);

    requestTimer->start(timeout);
}

void QGeoPositionInfoSourceMaemo::requestTimerExpired()
{
    emit updateTimeout();
    shutdownRequestSession();
}

void QGeoPositionInfoSourceMaemo::shutdownRequestSession()
{
    requestTimer->stop();

    if ( !locationOngoing )
        dbusComm->sendConfigRequest(dbusComm->CommandStop, 0, 0);

    if ( QGeoPositionInfoSource::updateInterval() !=
         dbusComm->minimumInterval() )
        dbusComm->sendConfigRequest(dbusComm->CommandSetInterval, 0,
                                    QGeoPositionInfoSource::updateInterval());

    if ( !QGeoPositionInfoSource::preferredPositioningMethods().testFlag(QGeoPositionInfoSource::AllPositioningMethods) )
        dbusComm->sendConfigRequest(dbusComm->CommandSetMethods,
                                    QGeoPositionInfoSource::preferredPositioningMethods(), 0);
}

#include "moc_qgeopositioninfosource_maemo_p.cpp"
QT_END_NAMESPACE

