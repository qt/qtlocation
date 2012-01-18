/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qgeosatelliteinfosource_simulator_p.h"
#include "qlocationconnection_simulator_p.h"
#include "qlocationdata_simulator_p.h"

QT_BEGIN_NAMESPACE

QGeoSatelliteInfoSourceSimulator::QGeoSatelliteInfoSourceSimulator(QObject *parent)
    : QGeoSatelliteInfoSource(parent)
    , timer(new QTimer(this))
    , requestTimer(new QTimer(this))
{
    Simulator::LocationConnection::ensureSimulatorConnection();

    connect(timer, SIGNAL(timeout()), this, SLOT(updateData()));
    requestTimer->setSingleShot(true);
    connect(requestTimer, SIGNAL(timeout()), this, SLOT(updateData()));
}

void QGeoSatelliteInfoSourceSimulator::startUpdates()
{
    int interval = updateInterval();
    if (interval < minimumUpdateInterval())
        interval = minimumUpdateInterval();
    timer->setInterval(interval);
    timer->start();
}

void QGeoSatelliteInfoSourceSimulator::stopUpdates()
{
    timer->stop();
}

void QGeoSatelliteInfoSourceSimulator::requestUpdate(int timeout)
{
    if (!requestTimer->isActive()) {
        // Get a single update within timeframe
        if (timeout < 0)
            emit requestTimeout();
        else {
            requestTimer->start(timeout * qreal(0.75));
        }
    }
}

void QGeoSatelliteInfoSourceSimulator::setUpdateInterval(int msec)
{
    // msec should be equal to or larger than the minimum update interval; 0 is a special case
    // that currently behaves as if the interval is set to the minimum update interval
    if (msec != 0 && msec < minimumUpdateInterval())
        msec = minimumUpdateInterval();

    QGeoSatelliteInfoSource::setUpdateInterval(msec);
    if (timer->isActive()) {
        timer->setInterval(msec);
        timer->start();
    }
}

int QGeoSatelliteInfoSourceSimulator::minimumUpdateInterval() const
{
    return qtPositionInfo()->minimumInterval;
}

void QGeoSatelliteInfoSourceSimulator::updateData()
{
    QList<QGeoSatelliteInfo> satellitesInUse;
    QList<QGeoSatelliteInfo> satellitesInView;

    QGeoSatelliteInfoData *data = qtSatelliteInfo();
    for(int i = 0; i < data->satellites.count(); i++) {
        QGeoSatelliteInfoData::SatelliteInfo info = data->satellites.at(i);
        QGeoSatelliteInfo satInfo;
        satInfo.setAttribute(QGeoSatelliteInfo::Azimuth, info.azimuth);
        satInfo.setAttribute(QGeoSatelliteInfo::Elevation, info.elevation);
        satInfo.setSignalStrength(info.signalStrength);
        satInfo.setSatelliteSystem(static_cast<QGeoSatelliteInfo::SatelliteSystem>(info.satelliteSystem));
        satInfo.setSatelliteIdentifier(info.satelliteIdentifier);
        satellitesInView.append(satInfo);
        if (info.inUse)
            satellitesInUse.append(satInfo);
    }
    emit satellitesInViewUpdated(satellitesInView);
    emit satellitesInUseUpdated(satellitesInUse);
}

#include "moc_qgeosatelliteinfosource_simulator_p.cpp"
QT_END_NAMESPACE
