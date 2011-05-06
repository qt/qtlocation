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

#ifndef QGEOPOSITIONINFOSOURCEMAEMO5_H
#define QGEOPOSITIONINFOSOURCEMAEMO5_H

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
#include "qgeopositioninfosource.h"

#define INIT_OK                     0
#define INIT_FAILED                 -1
#define MINIMUM_UPDATE_INTERVAL     1000
#define DEFAULT_UPDATE_INTERVAL     5000
#define POWERSAVE_THRESHOLD         180000
#define POWERSAVE_POWERON_PERIOD    120000

QTM_BEGIN_NAMESPACE

class LiblocationWrapper;

class QGeoPositionInfoSourceMaemo : public QGeoPositionInfoSource
{
    Q_OBJECT

public:

    QGeoPositionInfoSourceMaemo(QObject *parent = 0);

    int init();

    virtual void setUpdateInterval(int interval);
    virtual void setPreferredPositioningMethods(PositioningMethods methods);
    virtual QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const;
    virtual PositioningMethods supportedPositioningMethods() const;
    virtual int minimumUpdateInterval() const;

private:
    bool positionInited;
    QTimer *updateTimer;
    QTimer *requestTimer;
    int timerInterval;
    bool errorOccurred;
    bool errorSent;

    void activateTimer();
    void startLocationDaemon();

    enum PositionInfoState {
        Undefined = 0,
        Started = 1,
        Stopped = 2,
        RequestActive = 4,
        PowersaveActive = 8
    };
    int positionInfoState;
    
    QGeoPositionInfo lastUpdateFromSatellite;
    QGeoPositionInfo lastUpdateFromNetwork;

signals:
    void positionUpdated(const QGeoPositionInfo &update);

public slots:
    void startUpdates();
    void stopUpdates();
    void requestUpdate(int timeout = DEFAULT_UPDATE_INTERVAL);

private slots:
    void requestTimeoutElapsed();
    void error();
    void newPositionUpdate(const QGeoPositionInfo &position);
    void updateTimeoutElapsed();

private:
    Q_DISABLE_COPY(QGeoPositionInfoSourceMaemo)
};

QTM_END_NAMESPACE

#endif // QGEOPOSITIONINFOSOURCEMAEMO5_H
