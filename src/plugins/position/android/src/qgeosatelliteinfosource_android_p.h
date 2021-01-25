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


#ifndef QGEOSATELLITEINFOSOURCEANDROID_H
#define QGEOSATELLITEINFOSOURCEANDROID_H

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

#include <QGeoSatelliteInfoSource>
#include <QTimer>

class QGeoSatelliteInfoSourceAndroid : public QGeoSatelliteInfoSource
{
    Q_OBJECT
public:
    explicit QGeoSatelliteInfoSourceAndroid(QObject *parent = 0);
    ~QGeoSatelliteInfoSourceAndroid();

    //From QGeoSatelliteInfoSource
    void setUpdateInterval(int msec);
    int minimumUpdateInterval() const;

    Error error() const;

public Q_SLOTS:
    void startUpdates();
    void stopUpdates();
    void requestUpdate(int timeout = 0);

    void processSatelliteUpdateInView(const QList<QGeoSatelliteInfo> &satsInView, bool isSingleUpdate);
    void processSatelliteUpdateInUse(const QList<QGeoSatelliteInfo> &satsInUse, bool isSingleUpdate);

    void locationProviderDisabled();
private Q_SLOTS:
    void requestTimeout();

private:
    void reconfigureRunningSystem();

    Error m_error;
    int androidClassKeyForUpdate;
    int androidClassKeyForSingleRequest;
    bool updatesRunning;

    QTimer requestTimer;
    QList<QGeoSatelliteInfo> m_satsInUse;
    QList<QGeoSatelliteInfo> m_satsInView;

};

#endif // QGEOSATELLITEINFOSOURCEANDROID_H
