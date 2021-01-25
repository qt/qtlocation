/****************************************************************************
**
** Copyright (C) 2016 Jolla Ltd.
** Contact: Aaron McCarthy <aaron.mccarthy@jollamobile.com>
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

#ifndef QGEOPOSITIONINFOSOURCE_GEOCLUEMASTER_H
#define QGEOPOSITIONINFOSOURCE_GEOCLUEMASTER_H

#include "qgeocluemaster.h"
#include "geocluetypes.h"

#include <QtCore/QTimer>
#include <QtPositioning/QGeoPositionInfoSource>

class OrgFreedesktopGeoclueInterface;
class OrgFreedesktopGeocluePositionInterface;
class OrgFreedesktopGeoclueVelocityInterface;

QT_BEGIN_NAMESPACE

class QDBusPendingCallWatcher;

class QGeoPositionInfoSourceGeoclueMaster : public QGeoPositionInfoSource
{
    Q_OBJECT

public:
    explicit QGeoPositionInfoSourceGeoclueMaster(QObject *parent = 0);
    ~QGeoPositionInfoSourceGeoclueMaster();

    // From QGeoPositionInfoSource
    void setUpdateInterval(int msec) override;
    QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const override;
    PositioningMethods supportedPositioningMethods() const override;
    void setPreferredPositioningMethods(PositioningMethods methods) override;
    int minimumUpdateInterval() const override;

    Error error() const override;

    void startUpdates() override;
    void stopUpdates() override;
    void requestUpdate(int timeout = 5000) override;

private slots:
    void positionProviderChanged(const QString &name, const QString &description,
                                 const QString &service, const QString &path);
    void requestUpdateTimeout();

    void getPositionFinished(QDBusPendingCallWatcher *watcher);
    void positionChanged(qint32 fields, qint32 timestamp, double latitude, double longitude,
                         double altitude, const Accuracy &accuracy);
    void velocityChanged(qint32 fields, qint32 timestamp, double speed, double direction,
                         double climb);

private:
    void configurePositionSource();
    void cleanupPositionSource();
    void setOptions();

    enum PositionField
    {
        NoPositionFields = 0,
        Latitude = 1 << 0,
        Longitude = 1 << 1,
        Altitude = 1 << 2
    };
    Q_DECLARE_FLAGS(PositionFields, PositionField)

    void updatePosition(PositionFields fields, int timestamp, double latitude,
                        double longitude, double altitude, Accuracy accuracy);
    void positionUpdateFailed();

    enum VelocityField
    {
        NoVelocityFields = 0,
        Speed = 1 << 0,
        Direction = 1 << 1,
        Climb = 1 << 2
    };
    Q_DECLARE_FLAGS(VelocityFields, VelocityField)

    void updateVelocity(VelocityFields fields, int timestamp, double speed, double direction,
                        double climb);
    void velocityUpdateFailed();

private:
    QGeoclueMaster *m_master;

    OrgFreedesktopGeoclueInterface *m_provider;
    OrgFreedesktopGeocluePositionInterface *m_pos;
    OrgFreedesktopGeoclueVelocityInterface *m_vel;

    QTimer m_requestTimer;
    bool m_lastVelocityIsFresh;
    bool m_regularUpdateTimedOut;
    double m_lastVelocity;
    double m_lastDirection;
    double m_lastClimb;
    bool m_lastPositionFromSatellite;
    QGeoPositionInfo m_lastPosition;
    bool m_running;
    QGeoPositionInfoSource::Error m_error;
};

QT_END_NAMESPACE

#endif // QGEOPOSITIONINFOSOURCE_GEOCLUEMASTER_H
