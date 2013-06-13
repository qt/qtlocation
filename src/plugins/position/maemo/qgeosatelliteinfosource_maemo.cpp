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

#include "qgeosatelliteinfosource_maemo_p.h"

QT_BEGIN_NAMESPACE

QGeoSatelliteInfoSourceMaemo::QGeoSatelliteInfoSourceMaemo(QObject *parent) : QGeoSatelliteInfoSource(parent),
                                                                              running(false), satInViewSeen(false)
{
    requestTimer = new QTimer(this);
    QObject::connect(requestTimer, SIGNAL(timeout()), this, SLOT(requestTimerExpired()));
}


int QGeoSatelliteInfoSourceMaemo::init()
{
    dbusComm = new DBusComm(this);
    int status = dbusComm->init();

    if (status == 0) {
        QObject::connect(dbusComm, SIGNAL(receivedSatellitesInView(const QList<QGeoSatelliteInfo> &)),
                         this, SLOT(newSatellitesInView(const QList<QGeoSatelliteInfo> &)));
        QObject::connect(dbusComm, SIGNAL(receivedSatellitesInUse(const QList<QGeoSatelliteInfo> &)),
                         this, SLOT(newSatellitesInUse(const QList<QGeoSatelliteInfo> &)));
        QObject::connect(dbusComm, SIGNAL(serviceConnected()),
                         this, SLOT(onServiceConnect()));
        QObject::connect(dbusComm, SIGNAL(serviceDisconnected()),
                         this, SLOT(onServiceDisconnect()));
    }

    return status;
}

int QGeoSatelliteInfoSourceMaemo::minimumUpdateInterval() const
{
    return 1;
}

QGeoSatelliteInfoSource::Error QGeoSatelliteInfoSourceMaemo::error() const
{
    return UnknownSourceError;
}

void QGeoSatelliteInfoSourceMaemo::startUpdates()
{
    if ( !requestTimer->isActive() )
        dbusComm->sendConfigRequest(DBusComm::CommandSatStart, 0, 0);
    running = true;
}


void QGeoSatelliteInfoSourceMaemo::stopUpdates()
{
    if ( !requestTimer->isActive() )
        dbusComm->sendConfigRequest(DBusComm::CommandSatStop, 0, 0);
    running = false;
}


void QGeoSatelliteInfoSourceMaemo::requestUpdate(int timeout)
{
    if ( !running )
        dbusComm->sendConfigRequest(DBusComm::CommandSatStart, 0, 0);

    requestTimer->start(timeout);
    satInViewSeen = false;
}


void QGeoSatelliteInfoSourceMaemo::newSatellitesInView(const QList<QGeoSatelliteInfo> &update)
{
    if ( requestTimer->isActive() && satInViewSeen ) {
        requestTimer->stop();
        if ( !running )
            dbusComm->sendConfigRequest(DBusComm::CommandSatStop, 0, 0);
        else
            emit satellitesInViewUpdated(update);
    } else {
        emit satellitesInViewUpdated(update);
    }

    satInViewSeen = true;
}


void QGeoSatelliteInfoSourceMaemo::newSatellitesInUse(const QList<QGeoSatelliteInfo> &update)
{
    if ( requestTimer->isActive() ) {
        requestTimer->stop();
        if ( !running )
            dbusComm->sendConfigRequest(DBusComm::CommandSatStop, 0, 0);
    }

    emit satellitesInUseUpdated(update);
}

void QGeoSatelliteInfoSourceMaemo::requestTimerExpired()
{
    requestTimer->stop();

    if ( !running )
        dbusComm->sendConfigRequest(DBusComm::CommandSatStop, 0, 0);

    emit requestTimeout();
}

void QGeoSatelliteInfoSourceMaemo::onServiceDisconnect()
{
    //
}


void QGeoSatelliteInfoSourceMaemo::onServiceConnect()
{
    if (running) {
        dbusComm->sendConfigRequest(DBusComm::CommandSatStart, 0, 0);
    }
}


#include "moc_qgeosatelliteinfosource_maemo_p.cpp"
QT_END_NAMESPACE

