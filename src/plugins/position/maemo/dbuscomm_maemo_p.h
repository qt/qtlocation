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

#ifndef DBUSCOMM_MAEMO_H
#define DBUSCOMM_MAEMO_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtDBus/QtDBus>

#include "qgeopositioninfo.h"
#include "qgeosatelliteinfo.h"
#include "qgeopositioninfosource.h"
#include "qgeosatelliteinfosource.h"
#include "dbusserver_maemo_p.h"

QT_BEGIN_NAMESPACE

class DBusComm: public QObject, DBusServerIF
{
    Q_OBJECT


public:
    enum Command {CommandStart = 1,      CommandStop = 2,     CommandOneShot = 3,
                  CommandSetMethods = 4, CommandSetInterval = 8,
                  CommandSatStart = 16,  CommandSatStop = 32, CommandSatOneShot = 48};

    DBusComm(QObject *parent = 0);
    int  init();
    bool sendDBusRegister();
    bool sendConfigRequest(Command command, QGeoPositionInfoSource::PositioningMethods method,
                           int interval) const;
    QGeoPositionInfo &requestLastKnownPosition(bool satelliteMethodOnly);
    QGeoPositionInfoSource::PositioningMethods availableMethods() const;
    int minimumInterval() const;

Q_SIGNALS:
    void receivedPositionUpdate(const QGeoPositionInfo &update);
    void receivedSatellitesInView(const QList<QGeoSatelliteInfo> &update);
    void receivedSatellitesInUse(const QList<QGeoSatelliteInfo> &update);

    void serviceDisconnected();
    void serviceConnected();

private:
    static const QString positioningdService;
    static const QString positioningdPath;
    static const QString positioningdInterface;

    // from DBusServerIF
    void receivePositionUpdate(const QGeoPositionInfo &update);
    void receiveSatellitesInView(const QList<QGeoSatelliteInfo> &info);
    void receiveSatellitesInUse(const QList<QGeoSatelliteInfo> &info);
    void receiveSettings(QGeoPositionInfoSource::PositioningMethod methods, qint32 interval);

    QDBusInterface *positioningdProxy;
    DBusServer *dbusServer;
    QObject serverObj;
    QString myService;
    QString myPath;
    int clientId;
    int minimumUpdateInterval;
    QGeoPositionInfoSource::PositioningMethods availablePositioningMethods;
    QDBusServiceWatcher *serviceDisconnectWatcher;
    QDBusServiceWatcher *serviceConnectWatcher;
    bool createUniqueName();

    Q_DISABLE_COPY(DBusComm)

private Q_SLOTS:
    void onServiceDisconnect(const QString &name);
    void onServiceConnect(const QString &name);

};

QT_END_NAMESPACE

#endif
