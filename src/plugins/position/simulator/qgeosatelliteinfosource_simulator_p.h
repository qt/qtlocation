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

#ifndef QGEOSATELLITEINFOSOURCE_SIMULATOR_H
#define QGEOSATELLITEINFOSOURCE_SIMULATOR_H

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

#include <QTimer>
#include "qgeosatelliteinfosource.h"
#include "qgeosatelliteinfo.h"

#define MINIMUM_UPDATE_INTERVAL     1000
#define DEFAULT_UPDATE_INTERVAL     5000

QT_BEGIN_NAMESPACE

class QGeoSatelliteInfoSourceSimulator : public QGeoSatelliteInfoSource
{
    Q_OBJECT

public:
    explicit QGeoSatelliteInfoSourceSimulator(QObject *parent = 0);

    bool isConnected() const;

    virtual void setUpdateInterval(int msec);
    virtual int minimumUpdateInterval() const;

    // Default implementation for error()
    Error error() const { return QGeoSatelliteInfoSource::NoError; }
public slots:
    virtual void startUpdates();
    virtual void stopUpdates();
    virtual void requestUpdate(int timeout = 5000);

private slots:
    void updateData();

private:
    Q_DISABLE_COPY(QGeoSatelliteInfoSourceSimulator)
    QTimer *timer;
    QTimer *requestTimer;
};

QT_END_NAMESPACE

#endif // QGEOSATELLITEINFOSOURCE_SIMULATOR_H

