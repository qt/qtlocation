/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtPositioning module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QNMESATELLITEINFOSOURCE_P_H
#define QNMESATELLITEINFOSOURCE_P_H

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

#include "qnmeasatelliteinfosource.h"
#include <QtPositioning/qgeosatelliteinfo.h>

#include <QObject>
#include <QQueue>
#include <QPointer>
#include <QMap>
#include <QtCore/qiodevice.h>
#include <QtCore/qtimer.h>

QT_BEGIN_NAMESPACE

#define USE_NMEA_PIMPL 1

struct SatelliteInfo
{
    QList<QGeoSatelliteInfo> satellitesInView;
    QList<QGeoSatelliteInfo> satellitesInUse;
    QList<int> inUseIds; // temp buffer for GSA received before GSV
    bool satellitesInUseReceived = false;
    bool updatingGSV = false;
    bool validInView = false;
    bool validInUse = false;
};

struct QNmeaSatelliteInfoUpdate
{
    QMap<QGeoSatelliteInfo::SatelliteSystem, SatelliteInfo> m_satellites;
    QList<QGeoSatelliteInfo> m_satellitesInViewParsed;
    bool m_validInView = false; // global state for all satellite systems
    bool m_validInUse = false; // global state for all satellite systems
    bool m_fresh = false;
#if USE_NMEA_PIMPL
    QByteArray gsa;
    QList<QByteArray> gsv;
#endif
    QList<QGeoSatelliteInfo> allSatellitesInUse() const;
    QList<QGeoSatelliteInfo> allSatellitesInView() const;
    void setSatellitesInView(QGeoSatelliteInfo::SatelliteSystem system,
                             const QList<QGeoSatelliteInfo> &inView);
    bool setSatellitesInUse(QGeoSatelliteInfo::SatelliteSystem system, const QList<int> &inUse);
    void consume();
    bool isFresh() const;
    void clear();
    bool isValid() const;
    bool calculateValidInUse() const;
    bool calculateValidInView() const;
};

class QNmeaSatelliteReader;
class QNmeaSatelliteInfoSourcePrivate : public QObject
{
    Q_OBJECT
public:
    QNmeaSatelliteInfoSourcePrivate(QNmeaSatelliteInfoSource *parent, QNmeaSatelliteInfoSource::UpdateMode updateMode);
    ~QNmeaSatelliteInfoSourcePrivate();

    void startUpdates();
    void stopUpdates();
    void requestUpdate(int msec);
    void notifyNewUpdate();
    void processNmeaData(QNmeaSatelliteInfoUpdate &updateInfo);

public slots:
    void readyRead();
    void emitPendingUpdate();
    void sourceDataClosed();
    void updateRequestTimeout();

public:
    QNmeaSatelliteInfoSource *m_source = nullptr;
    QGeoSatelliteInfoSource::Error m_satelliteError = QGeoSatelliteInfoSource::NoError;
    QPointer<QIODevice> m_device;
    QNmeaSatelliteInfoUpdate m_pendingUpdate;
    QNmeaSatelliteInfoUpdate m_lastUpdate;
    bool m_invokedStart = false;
    bool m_noUpdateLastInterval = false;
    bool m_updateTimeoutSent = false;
    bool m_connectedReadyRead = false;
    QBasicTimer *m_updateTimer = nullptr; // the timer used in startUpdates()
    QTimer *m_requestTimer = nullptr; // the timer used in requestUpdate()
    QScopedPointer<QNmeaSatelliteReader> m_nmeaReader;
    QNmeaSatelliteInfoSource::UpdateMode m_updateMode;
    int m_simulationUpdateInterval = 100;

protected:
    bool openSourceDevice();
    bool initialize();
    void prepareSourceDevice();
    bool emitUpdated(QNmeaSatelliteInfoUpdate &update, bool fromRequestUpdate);
    void timerEvent(QTimerEvent *event) override;
};

class QNmeaSatelliteReader
{
public:
    QNmeaSatelliteReader(QNmeaSatelliteInfoSourcePrivate *sourcePrivate);
    virtual ~QNmeaSatelliteReader();

    virtual void readAvailableData() = 0;

protected:
    QNmeaSatelliteInfoSourcePrivate *m_proxy;
};

class QNmeaSatelliteRealTimeReader : public QNmeaSatelliteReader
{
public:
    QNmeaSatelliteRealTimeReader(QNmeaSatelliteInfoSourcePrivate *sourcePrivate);
    void readAvailableData() override;
};

class QNmeaSatelliteSimulationReader : public QNmeaSatelliteReader
{
public:
    QNmeaSatelliteSimulationReader(QNmeaSatelliteInfoSourcePrivate *sourcePrivate);
    void readAvailableData() override;
    void setUpdateInterval(int msec);
    int updateInterval() const;

private:
    QScopedPointer<QTimer> m_timer;
    int m_updateInterval;
};

QT_END_NAMESPACE

#endif
