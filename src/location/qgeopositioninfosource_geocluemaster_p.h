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

#ifndef QGEOPOSITIONINFOSOURCE_GEOCLUEMASTER_H
#define QGEOPOSITIONINFOSOURCE_GEOCLUEMASTER_H

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

#include <qgeopositioninfosource.h>
#include <geoclue/geoclue-master.h>
#include <geoclue/geoclue-velocity.h>
#include <QTimer>

//#define Q_LOCATION_GEOCLUE_DEBUG

QTM_BEGIN_NAMESPACE

class QGeoPositionInfoSourceGeoclueMaster : public QGeoPositionInfoSource
{
    Q_OBJECT
public:
    void positionChanged(GeocluePosition      *position,
                         GeocluePositionFields fields,
                         int                   timestamp,
                         double                latitude,
                         double                longitude,
                         double                altitude,
                         GeoclueAccuracy      *accuracy);
    QGeoPositionInfoSourceGeoclueMaster(QObject *parent = 0);
    ~QGeoPositionInfoSourceGeoclueMaster();

    // From QGeoPositionInfoSource
    void setUpdateInterval(int msec);
    QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const;
    PositioningMethods supportedPositioningMethods() const;
    void setPreferredPositioningMethods(PositioningMethods methods);
    int minimumUpdateInterval() const;
    int init();

    void singleUpdateFailed();
    void singleUpdateSucceeded(GeocluePositionFields fields,
                               int                   timestamp,
                               double                latitude,
                               double                longitude,
                               double                altitude,
                               GeoclueAccuracy      *accuracy);
    void regularUpdateFailed();
    void regularUpdateSucceeded(GeocluePositionFields fields,
                                int                   timestamp,
                                double                latitude,
                                double                longitude,
                                double                altitude,
                                GeoclueAccuracy      *accuracy);
    void velocityUpdateFailed();
    void velocityUpdateSucceeded(double speed);

public slots:
    virtual void startUpdates();
    virtual void stopUpdates();
    virtual void requestUpdate(int timeout = 5000);

private slots:
    void requestUpdateTimeout();
    void startUpdatesTimeout();

private:
    bool tryGPS();
    int configurePositionSource();
    QGeoPositionInfo geoclueToPositionInfo(GeocluePositionFields fields,
                                           int                   timestamp,
                                           double                latitude,
                                           double                longitude,
                                           double                altitude,
                                           GeoclueAccuracy*      accuracy);
private:
    int m_updateInterval;
    GeoclueResourceFlags m_preferredResources;
    GeoclueAccuracyLevel m_preferredAccuracy;
    GeoclueMasterClient *m_client;
    GeocluePosition *m_pos;
    GeoclueVelocity* m_vel;
    QTimer m_updateTimer;
    QTimer m_requestTimer;
    bool m_lastPositionIsFresh;
    bool m_lastVelocityIsFresh;
    double m_lastVelocity;
    bool m_lastPositionFromSatellite;
    QGeoPositionInfo m_lastPosition;
    PositioningMethods m_methods;
};

QTM_END_NAMESPACE

#endif // QGEOPOSITIONINFOSOURCE_GEOCLUEMASTER_H
