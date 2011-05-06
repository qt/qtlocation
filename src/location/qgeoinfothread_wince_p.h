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

#ifndef QGEOINFOTHREAD_WINCE_P_H
#define QGEOINFOTHREAD_WINCE_P_H

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

#include "qmobilityglobal.h"

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QDateTime>

#include <windows.h>

// including requires <windows.h>
#include <gpsapi.h>

// including requires <windows.h>
// included here rather than in the cpp file to make sure it is included after <windows.h>
#include <service.h> // used for SERVICE_STATE_ constants

QTM_BEGIN_NAMESPACE

// Used to determine if the dataUpdated signal should be emitted by a QGeoInfoThreadWinCE instance.
// If valid() returns false the QGeoInfoThreadWinCE instance will ignore the data.
// This is subclassed in order to filter for valid position or satellite data in GPS_POSITION
// structs.
class QGeoInfoValidator
{
public:
    QGeoInfoValidator();
    virtual ~QGeoInfoValidator();
    virtual bool valid(const GPS_POSITION &data) const = 0;
};

class QGeoInfoThreadWinCE : public QThread
{
    Q_OBJECT

public:
    enum {
        // The default timeout for requests if none is specified.
        DefaultRequestTimeout = 10000,
        // The maximum time to spend waiting for GPS events in the main loop of the thread.
        MaximumMainLoopWaitTime = 5000
    };

    QGeoInfoThreadWinCE(QGeoInfoValidator *validator, bool timeoutsForPeriodicUpdates, QObject *parent = 0);
    ~QGeoInfoThreadWinCE();

//public slots:
    void requestUpdate(int timeout = 5000);
    void startUpdates();
    void stopUpdates();
    void setUpdateInterval(int updateInterval);

signals:
    void dataUpdated(GPS_POSITION data);
    void updateTimeout();

protected:
    void run();

private:
    void wakeUp();

    QDateTime currentDateTime();
    int msecsTo(QDateTime from, QDateTime to);

    QGeoInfoValidator *validator;
    bool timeoutsForPeriodicUpdates;

    bool requestScheduled;
    qint32 requestInterval;
    QDateTime requestNextTime;

    bool updatesScheduled;
    qint32 updatesInterval;
    QDateTime updatesNextTime;

    bool stopping;

    bool gpsReachedOnState;

    bool hasLastPosition;
    bool invalidDataReceived;
    bool updateTimeoutTriggered;
    GPS_POSITION m_lastPosition;

    QMutex mutex;
    QWaitCondition statusUpdated;

    HANDLE m_gps;
    HANDLE m_newDataEvent;
    HANDLE m_gpsStateChange;
    HANDLE m_wakeUpEvent;
};

QTM_END_NAMESPACE

#endif //#ifndef QGEOINFOTHREAD_WINCE_P_H
