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

#include "qlocationconnection_simulator_p.h"
#include "qgeopositioninfosource_simulator_p.h"
#include "qlocationdata_simulator_p.h"

#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtCore/QDataStream>
#include <QtCore/QEventLoop>

#include <QtNetwork/QLocalSocket>

#include <QtSimulator/connection.h>
#include <QtSimulator/version.h>
#include <QtSimulator/connectionworker.h>

QT_BEGIN_NAMESPACE

const QString simulatorName(QString::fromLatin1("QtSimulator_Mobility_ServerName1.3.0.0"));
const quint16 simulatorPort = 0xbeef + 1;

namespace Simulator
{
    LocationConnection::LocationConnection()
        : mConnection(new Connection(Connection::Client, simulatorName, simulatorPort, Version(1,3,0,0)))
    {
        qt_registerLocationTypes();
        mWorker = mConnection->connectToServer(Connection::simulatorHostName(true), simulatorPort);
        if (!mWorker)
            qFatal("Could not connect to server");
        mWorker->addReceiver(this);

        // register for location notifications
        mWorker->call("setRequestsLocationInfo");

        // wait until initial data is received
        QEventLoop loop;
        connect(this, SIGNAL(initialDataReceived()), &loop, SLOT(quit()));
        loop.exec();
    }

    LocationConnection::~LocationConnection()
    {
        delete mWorker;
        delete mConnection;
    }

    void LocationConnection::ensureSimulatorConnection()
    {
        static LocationConnection locationConnection;
    }

    void LocationConnection::initialLocationDataSent()
    {
        emit initialDataReceived();
    }

    void LocationConnection::setLocationData(const QGeoPositionInfoData &data)
    {
        *qtPositionInfo() = data;
    }

    void LocationConnection::setSatelliteData(const QGeoSatelliteInfoData &data)
    {
        *qtSatelliteInfo() = data;
    }

#include "moc_qlocationconnection_simulator_p.cpp"

} // namespace

QGeoPositionInfoData *qtPositionInfo()
{
    static QGeoPositionInfoData *positionInfo = 0;
    if (!positionInfo) {
        positionInfo = new QGeoPositionInfoData;
    }

    return positionInfo;
}

QGeoSatelliteInfoData *qtSatelliteInfo()
{
    static QGeoSatelliteInfoData *satelliteInfo = 0;
    if (!satelliteInfo) {
        satelliteInfo = new QGeoSatelliteInfoData;
    }

    return satelliteInfo;
}


QT_END_NAMESPACE
