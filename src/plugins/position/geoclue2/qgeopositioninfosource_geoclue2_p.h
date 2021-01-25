/****************************************************************************
**
** Copyright (C) 2018 Denis Shienkov <denis.shienkov@gmail.com>
** Contact: http://www.qt-project.org/legal
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

#ifndef QGEOPOSITIONINFOSOURCE_GEOCLUE2_P_H
#define QGEOPOSITIONINFOSOURCE_GEOCLUE2_P_H

#include <QtPositioning/QGeoPositionInfoSource>
#include <QtCore/QPointer>
#include <manager_interface.h>

class OrgFreedesktopGeoClue2ClientInterface;

QT_BEGIN_NAMESPACE
class QDBusObjectPath;
class QTimer;

class QGeoPositionInfoSourceGeoclue2 : public QGeoPositionInfoSource
{
    Q_OBJECT

public:
    explicit QGeoPositionInfoSourceGeoclue2(QObject *parent = nullptr);
    ~QGeoPositionInfoSourceGeoclue2();

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

private:
    void setError(QGeoPositionInfoSource::Error error);
    void restoreLastPosition();
    void saveLastPosition();
    void createClient();
    bool configureClient();
    void startClient();
    void stopClient();
    void requestUpdateTimeout();
    void handleNewLocation(const QDBusObjectPath &oldLocation,
                           const QDBusObjectPath &newLocation);

    QTimer *m_requestTimer = nullptr;
    OrgFreedesktopGeoClue2ManagerInterface m_manager;
    QPointer<OrgFreedesktopGeoClue2ClientInterface> m_client;
    bool m_running = false;
    bool m_lastPositionFromSatellite = false;
    QGeoPositionInfoSource::Error m_error = NoError;
    QGeoPositionInfo m_lastPosition;
};

QT_END_NAMESPACE

#endif // QGEOPOSITIONINFOSOURCE_GEOCLUE2_P_H
