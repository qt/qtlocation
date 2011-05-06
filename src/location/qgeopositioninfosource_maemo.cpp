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

#include "qgeopositioninfosource_maemo_p.h"
#include <iostream>
#include <QDateTime>

using namespace std;

QTM_BEGIN_NAMESPACE

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
QTM_END_NAMESPACE

