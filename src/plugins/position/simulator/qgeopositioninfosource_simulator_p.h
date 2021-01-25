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

#ifndef QGEOPOSITIONINFOSOURCESIMULATOR_H
#define QGEOPOSITIONINFOSOURCESIMULATOR_H

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
#include "qgeopositioninfo.h"
#include "qlocationdata_simulator_p.h"

QT_BEGIN_NAMESPACE

class QTimer;

class QGeoPositionInfoSourceSimulator : public QGeoPositionInfoSource
{
    Q_OBJECT
public:
    QGeoPositionInfoSourceSimulator(QObject *parent = 0);
    ~QGeoPositionInfoSourceSimulator();

    QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const;
    PositioningMethods supportedPositioningMethods() const;

    void setUpdateInterval(int msec);
    int minimumUpdateInterval() const;
    Error error() const;

public Q_SLOTS:
    void startUpdates();
    void stopUpdates();

    void requestUpdate(int timeout = 0);

private slots:
    void updatePosition();
private:
    Q_DISABLE_COPY(QGeoPositionInfoSourceSimulator);
    QTimer *timer;
    QTimer *requestTimer;
    QGeoPositionInfo lastPosition;
    QGeoPositionInfoSource::Error m_positionError;
    void setError(QGeoPositionInfoSource::Error positionError);
};

QT_END_NAMESPACE

#endif // QGEOPOSITIONINFOSOURCESIMULATOR_H
