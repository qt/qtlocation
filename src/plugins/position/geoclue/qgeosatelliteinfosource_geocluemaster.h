/****************************************************************************
**
** Copyright (C) 2016 Jolla Ltd, author: Aaron McCarthy <aaron.mccarthy@jollamobile.com>
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

#ifndef QGEOSATELLITEINFOSOURCE_GEOCLUEMASTER_H
#define QGEOSATELLITEINFOSOURCE_GEOCLUEMASTER_H

#include "qgeocluemaster.h"

#include <QtCore/QTimer>
#include <QtPositioning/QGeoSatelliteInfoSource>

class OrgFreedesktopGeoclueInterface;
class OrgFreedesktopGeoclueSatelliteInterface;

QT_BEGIN_NAMESPACE

class QDBusMessage;
class QDBusPendingCallWatcher;

class QGeoSatelliteInfoSourceGeoclueMaster : public QGeoSatelliteInfoSource
{
    Q_OBJECT

public:
    explicit QGeoSatelliteInfoSourceGeoclueMaster(QObject *parent = 0);
    ~QGeoSatelliteInfoSourceGeoclueMaster();

    int minimumUpdateInterval() const override;
    void setUpdateInterval(int msec) override;

    Error error() const override;

    void startUpdates() override;
    void stopUpdates() override;
    void requestUpdate(int timeout = 0) override;

private slots:
    void positionProviderChanged(const QString &name, const QString &description,
                                 const QString &service, const QString &path);
    void requestUpdateTimeout();

    void getSatelliteFinished(QDBusPendingCallWatcher *watcher);
    void satelliteChanged(int timestamp, int satellitesUsed, int satellitesVisible,
                          const QList<int> &usedPrn, const QList<QGeoSatelliteInfo> &satInfos);
    void satelliteChanged(const QDBusMessage &message);

private:
    void configureSatelliteSource();
    void cleanupSatelliteSource();

    void updateSatelliteInfo(int timestamp, int satellitesUsed, int satellitesVisible,
                              const QList<int> &usedPrn, const QList<QGeoSatelliteInfo> &satInfos);

    QGeoclueMaster *m_master;

    OrgFreedesktopGeoclueInterface *m_provider;
    OrgFreedesktopGeoclueSatelliteInterface *m_sat;

    QTimer m_requestTimer;
    QList<QGeoSatelliteInfo> m_inView;
    QList<QGeoSatelliteInfo> m_inUse;
    Error m_error;
    bool m_satellitesChangedConnected;
    bool m_running;
};

QT_END_NAMESPACE

#endif // QGEOSATELLITEINFOSOURCE_GEOCLUEMASTER_H
