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

#ifndef QGEOPOSITIONINFOSOURCE_NPE_BACKEND_H
#define QGEOPOSITIONINFOSOURCE_NPE_BACKEND_H

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

#include "qgeopositioninfosource.h"
#include <qtimer.h>
#include <locationdaemonconnection.h>


class QGeoPositionInfoSourceNpeBackend : public QGeoPositionInfoSource
{
    Q_OBJECT

public:
    QGeoPositionInfoSourceNpeBackend(QObject *parent = 0);
    bool init();
    void setUpdateInterval(int interval);
    void setPreferredPositioningMethods(PositioningMethods sources);
    QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const;
    PositioningMethods supportedPositioningMethods() const;
    int minimumUpdateInterval() const;
    QGeoPositionInfoSource::Error error() const;

public Q_SLOTS:
    void startUpdates();
    void stopUpdates();
    void requestUpdate(int timeout = 5000);

private:
    void setError(QGeoPositionInfoSource::Error positionError);
    void shutdownRequestSession();
    LocationDaemonConnection* m_locationdConn;
    bool locationOngoing;
    bool timeoutSent;
    QTimer* requestTimer;
    QGeoPositionInfoSource::Error mPositionError;


private Q_SLOTS:
    void onPositionUpdate(const PositionData &position);
    void onConnectionError(LocationDaemonConnection::SocketError socketError);
    void requestTimerExpired();
};

#endif // QGEOPOSITIONINFOSOURCE_NPE_BACKEND_H
