/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtPositioning module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#ifndef QLOCATIONCONNECTION_H
#define QLOCATIONCONNECTION_H

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

#include "qlocationdata_simulator_p.h"

#include <QtCore/QObject>

QT_BEGIN_NAMESPACE

namespace Simulator
{
    class Connection;
    class ConnectionWorker;

    class LocationConnection : public QObject
    {
        Q_OBJECT

    public:
        static bool ensureSimulatorConnection();
        virtual ~LocationConnection();

    private:
        LocationConnection();
        Q_DISABLE_COPY(LocationConnection)

    private slots:
        // these will be called by the simulator
        void setLocationData(const QGeoPositionInfoData &);
        void setSatelliteData(const QGeoSatelliteInfoData &);
        void initialLocationDataSent();

    signals:
        void initialDataReceived();

    private:
        Connection *mConnection;
        ConnectionWorker *mWorker;
    };
} // end namespace Simulator

QGeoPositionInfoData *qtPositionInfo();
QGeoSatelliteInfoData *qtSatelliteInfo();

QT_END_NAMESPACE

#endif // QLOCATIONCONNECTION_H
