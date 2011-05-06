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
#ifndef QNMEAPOSITIONINFOSOURCE_P_H
#define QNMEAPOSITIONINFOSOURCE_P_H

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

#include "qnmeapositioninfosource.h"
#include "qgeopositioninfo.h"

#include <QObject>
#include <QQueue>
#include <QPointer>

QT_BEGIN_NAMESPACE
class QBasicTimer;
class QTimerEvent;
class QTimer;
QT_END_NAMESPACE

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QNmeaReader;
struct QPendingGeoPositionInfo {
    QGeoPositionInfo info;
    bool hasFix;
};


class QNmeaPositionInfoSourcePrivate : public QObject
{
    Q_OBJECT
public:
    explicit QNmeaPositionInfoSourcePrivate(QNmeaPositionInfoSource *parent);
    ~QNmeaPositionInfoSourcePrivate();

    void startUpdates();
    void stopUpdates();
    void requestUpdate(int msec);

    bool parsePosInfoFromNmeaData(const char *data,
                                  int size,
                                  QGeoPositionInfo *posInfo,
                                  bool *hasFix);

    void notifyNewUpdate(QGeoPositionInfo *update, bool fixStatus);

    QNmeaPositionInfoSource::UpdateMode m_updateMode;
    QPointer<QIODevice> m_device;
    QGeoPositionInfo m_lastUpdate;
    bool m_invokedStart;

public Q_SLOTS:
    void readyRead();

protected:
    void timerEvent(QTimerEvent *event);

private Q_SLOTS:
    void emitPendingUpdate();
    void sourceDataClosed();
    void updateRequestTimeout();

private:
    bool openSourceDevice();
    bool initialize();
    void prepareSourceDevice();
    void emitUpdated(const QGeoPositionInfo &update);

    QNmeaPositionInfoSource *m_source;
    QNmeaReader *m_nmeaReader;
    QBasicTimer *m_updateTimer;
    QGeoPositionInfo m_pendingUpdate;
    QDate m_currentDate;
    QTimer *m_requestTimer;
    bool m_noUpdateLastInterval;
    bool m_updateTimeoutSent;
    bool m_connectedReadyRead;
};


class QNmeaReader
{
public:
    explicit QNmeaReader(QNmeaPositionInfoSourcePrivate *sourcePrivate)
            : m_proxy(sourcePrivate) {}
    virtual ~QNmeaReader() {}

    virtual void readAvailableData() = 0;

protected:
    QNmeaPositionInfoSourcePrivate *m_proxy;
};


class QNmeaRealTimeReader : public QNmeaReader
{
public:
    explicit QNmeaRealTimeReader(QNmeaPositionInfoSourcePrivate *sourcePrivate);
    virtual void readAvailableData();
};


class QNmeaSimulatedReader : public QObject, public QNmeaReader
{
    Q_OBJECT
public:
    explicit QNmeaSimulatedReader(QNmeaPositionInfoSourcePrivate *sourcePrivate);
    ~QNmeaSimulatedReader();
    virtual void readAvailableData();

protected:
    virtual void timerEvent(QTimerEvent *event);

private Q_SLOTS:
    void simulatePendingUpdate();

private:
    bool setFirstDateTime();
    void processNextSentence();

    QQueue<QPendingGeoPositionInfo> m_pendingUpdates;
    int m_currTimerId;
    bool m_hasValidDateTime;
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif
