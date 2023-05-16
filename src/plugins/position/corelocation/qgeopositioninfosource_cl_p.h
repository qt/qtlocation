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

#ifndef QGEOPOSITIONINFOSOURCECL_H
#define QGEOPOSITIONINFOSOURCECL_H

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

#import <CoreLocation/CoreLocation.h>

#include "qgeopositioninfosource.h"
#include "qgeopositioninfo.h"

QT_BEGIN_NAMESPACE

class QGeoPositionInfoSourceCL : public QGeoPositionInfoSource
{
    Q_OBJECT
public:
    QGeoPositionInfoSourceCL(const QVariantMap &parameters, QObject *parent = 0);
    ~QGeoPositionInfoSourceCL();

    QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const override;
    PositioningMethods supportedPositioningMethods() const override;

    void setUpdateInterval(int msec) override;
    int minimumUpdateInterval() const override;
    Error error() const override;

    void locationDataAvailable(QGeoPositionInfo location);
    void setError(QGeoPositionInfoSource::Error positionError);
    void changeAuthorizationStatus(CLAuthorizationStatus status);

private:
    bool enableLocationManager();
    void setTimeoutInterval(int msec);

public Q_SLOTS:
    void startUpdates() override;
    void stopUpdates() override;

    void requestUpdate(int timeout = 0) override;

protected:
    void timerEvent(QTimerEvent *event) override;

private:
    Q_DISABLE_COPY(QGeoPositionInfoSourceCL);
    CLLocationManager *m_locationManager;
    bool m_updatesWanted;
    bool m_requestAlwaysPermission = false;

    QGeoPositionInfo m_lastUpdate;

    int m_updateTimer;
    int m_updateTimeout;

    QGeoPositionInfoSource::Error m_positionError;
};

QT_END_NAMESPACE

#endif // QGEOPOSITIONINFOSOURCECL_H
