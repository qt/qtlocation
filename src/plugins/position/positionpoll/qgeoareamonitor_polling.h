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

#ifndef QGEOAREAMONITORPOLLING_H
#define QGEOAREAMONITORPOLLING_H

#include <QtPositioning/qgeoareamonitorsource.h>
#include <QtPositioning/qgeopositioninfosource.h>


/**
 *  QGeoAreaMonitorPolling
 *
 */

class QGeoAreaMonitorPollingPrivate;
class QGeoAreaMonitorPolling : public QGeoAreaMonitorSource
{
    Q_OBJECT
public :
    explicit QGeoAreaMonitorPolling(QObject *parent = 0);
    ~QGeoAreaMonitorPolling();

    void setPositionInfoSource(QGeoPositionInfoSource *source) override;
    QGeoPositionInfoSource* positionInfoSource() const override;

    Error error() const override;

    bool startMonitoring(const QGeoAreaMonitorInfo &monitor) override;
    bool requestUpdate(const QGeoAreaMonitorInfo &monitor,
                       const char *signal) override;
    bool stopMonitoring(const QGeoAreaMonitorInfo &monitor) override;

    QList<QGeoAreaMonitorInfo> activeMonitors() const override;
    QList<QGeoAreaMonitorInfo> activeMonitors(const QGeoShape &region) const override;

    QGeoAreaMonitorSource::AreaMonitorFeatures supportedAreaMonitorFeatures() const override;

    inline bool isValid() { return positionInfoSource(); }

    bool signalsAreConnected;

private Q_SLOTS:
    void positionError(QGeoPositionInfoSource::Error error);
    void timeout(const QGeoAreaMonitorInfo &monitor);
    void processAreaEvent(const QGeoAreaMonitorInfo &minfo, const QGeoPositionInfo &pinfo, bool isEnteredEvent);

private:
    QGeoAreaMonitorPollingPrivate* d;
    QGeoAreaMonitorSource::Error lastError;

    void connectNotify(const QMetaMethod &signal) override;
    void disconnectNotify(const QMetaMethod &signal) override;

    int idForSignal(const char *signal);
};

#endif // QGEOAREAMONITORPOLLING_H
