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

#include <QDebug>
#include "qgeoareamonitor_s60_p.h"
#include "qgeopositioninfo.h"
#include "qmlbackendmonitorinfo_s60_p.h"
#include "qmlbackendmonitorcreatetriggerao_s60_p.h"

QTM_BEGIN_NAMESPACE

TInt QGeoAreaMonitorS60::refCount = 0;

QGeoAreaMonitorS60* QGeoAreaMonitorS60::NewL(QObject *aParent)
{
    RProcess thisProcess;
    if (!thisProcess.HasCapability(ECapabilityLocation)) {
        qWarning() << "QGeoAreaMonitor::createDefaultMonitor() requires the Symbian Location capability to succeed on the Symbian platform.";
        return 0;
    }

    QGeoAreaMonitorS60 *self = QGeoAreaMonitorS60::NewLC(aParent);
    CleanupStack::Pop();
    if (!self->isValid()) {
        delete self;
        self = NULL;
    }
    return self;
}

//creates an entry and exit trigger based on the aCoordinate and
//aRadius values passed as argument
void QGeoAreaMonitorS60::setMonitoredArea(const QGeoCoordinate & aCoordinate, qreal aRadius)
{
    TCoordinate coord;

    TInt ret1 = QCoordinateToTCoordinate(aCoordinate, coord);

    TInt ret2 = iTriggerCreateAO->getRadius(aRadius);

    if (ret2 == KErrNone)
        QGeoAreaMonitor::setRadius(aRadius);

    if ((ret1 != KErrNone) || (ret2 != KErrNone))
        return;

    //Initialize the trigger and enable the trigger if atleast one slot is  connected to the areaEntered
    //signal
    if ((iTriggerCreateAO->InitializeTrigger(this, EntryTrigger, coord, aRadius)) &&
            (receivers(SIGNAL(areaEntered(const QGeoPositionInfo&))) > 0)) {
        iTriggerCreateAO->SetTriggerState(this, EntryTrigger, true);
        iTriggerAO->NotifyFiredEvent();
    }

    //Initialize the trigger and enable the trigger if atleast one slot is  connected to the areaExited
    //signal
    if ((iTriggerCreateAO->InitializeTrigger(this, ExitTrigger, coord, aRadius)) &&
            (receivers(SIGNAL(areaExited(const QGeoPositionInfo&))) > 0)) {
        iTriggerCreateAO->SetTriggerState(this, ExitTrigger, true);
        iTriggerAO->NotifyFiredEvent();
    }

    //request for the trigger change notification events
    iNotifyTriggerAO->NotifyChangeEvent();
}

//virtual function sets the Center of the monitoring area to coordinate
void QGeoAreaMonitorS60::setCenter(const QGeoCoordinate& coordinate)
{
    if (coordinate.isValid())
        QGeoAreaMonitor::setCenter(coordinate);
    else
        return;

    if (QGeoAreaMonitor::radius() != 0)     //if radius is not initialised
        setMonitoredArea(coordinate, QGeoAreaMonitor::radius());
}

//virtual function sets the radius of the monitorijng area to the radius
void QGeoAreaMonitorS60::setRadius(qreal radius)
{
    setMonitoredArea(QGeoAreaMonitor::center(), radius);
}

//callback from the QMLBackendMonitorAO object for the entry/exit event
void QGeoAreaMonitorS60::handleTriggerEvent(TPositionInfo aPosInfo, enTriggerType aStatus)
{
    QGeoPositionInfo posInfo;

    TPositionInfoToQGeoPositionInfo(aPosInfo, posInfo);

    if (!posInfo.isValid())
        return;

    switch (aStatus) {
        case  EntryTrigger :           //emit areaEntered trigger
            emit areaEntered(posInfo);
            break;
        case ExitTrigger :             //emit areaExited trigger
            emit areaExited(posInfo);
            break;
        case NotifyChangesTrigger:
        case InvalidTrigger:
            break;
    }

}



//destructor cleaning up the resources
QGeoAreaMonitorS60::~QGeoAreaMonitorS60()
{

    if (iTriggerAO || iNotifyTriggerAO || iTriggerCreateAO) {
        QMLBackendMonitorAO::DeleteAO(this);
        iTriggerAO = NULL;
        QMLBackendTriggerChangeAO::DeleteAO();
        iNotifyTriggerAO = NULL;
        delete iTriggerCreateAO;
        iTriggerCreateAO = NULL;
    }
    if (connectedLbt) {
        --refCount;
        if (refCount == 0) {
            lbtServ.Close();
        }
    }
}

QGeoAreaMonitorS60* QGeoAreaMonitorS60::NewLC(QObject* aParent)
{
    QGeoAreaMonitorS60 *self = new(ELeave) QGeoAreaMonitorS60(aParent);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

//second level construction : creating the QMLBackendMonitorAO : for
//monitoring the trigger fired event,QMLBackendMonitorCreateTriggerAO:
//instance object for creating the trigger and QMLBackendTriggerChangeAO :
//for monitoring the changes to the trigger properties
void QGeoAreaMonitorS60::ConstructL()
{
    if (lbtServ.Connect() == KErrNone) {
        CleanupClosePushL(lbtServ);

        connectedLbt = true;
        ++refCount;

        iTriggerAO = QMLBackendMonitorAO::NewL(lbtServ);

        if (!iTriggerAO)
            return;

        iTriggerCreateAO = QMLBackendMonitorCreateTriggerAO::NewL(this, lbtServ);

        if (!iTriggerCreateAO)
            return;

        iNotifyTriggerAO = QMLBackendTriggerChangeAO::NewL(lbtServ);

        CleanupStack::Pop(1);
    } else {
        connectedLbt = false;
    }
}

//constructor initializing the default values
QGeoAreaMonitorS60::QGeoAreaMonitorS60(QObject* aParent) : QGeoAreaMonitor(aParent),
        iTriggerAO(NULL), iNotifyTriggerAO(NULL)
{

}

//convert a symbian-TPositionInfo value to corresponding QT-QGeoPositionInfo
void QGeoAreaMonitorS60::TPositionInfoToQGeoPositionInfo(TPositionInfo& aPosInfo, QGeoPositionInfo& aQInfo)
{
    QGeoCoordinate coord;
    TPosition pos;
    aPosInfo.GetPosition(pos);

    coord.setLatitude(pos.Latitude());
    coord.setLongitude(pos.Longitude());
    coord.setAltitude(pos.Altitude());

    //store the QGeoCoordinate values
    aQInfo.setCoordinate(coord);

    TDateTime datetime = pos.Time().DateTime();
    QDateTime dt(QDate(datetime.Year(), datetime.Month() + 1, datetime.Day() + 1),
                 QTime(datetime.Hour(), datetime.Minute(), datetime.Second(),
                       datetime.MicroSecond() / 1000),
                Qt::UTC);

    //store the time stamp
    aQInfo.setTimestamp(dt);

    //store the horizontal accuracy
    aQInfo.setAttribute(QGeoPositionInfo::HorizontalAccuracy, pos.HorizontalAccuracy());

    //store the vertical accuracy
    aQInfo.setAttribute(QGeoPositionInfo::VerticalAccuracy, pos.VerticalAccuracy());

}


//convert QT-QCoordinate value to Symbian-TCoordinate value
int QGeoAreaMonitorS60::QCoordinateToTCoordinate(const QGeoCoordinate& aQCoord, TCoordinate& aTCoord)
{
    if (!aQCoord.isValid())
        return KErrGeneral;

    QGeoAreaMonitor::setCenter(aQCoord);
    aTCoord.SetCoordinate(aQCoord.latitude(), aQCoord.longitude(), aQCoord.altitude());
    return KErrNone;
}

//Notification called when a sot is connected to the areaEntered or
//areaExited signal
void QGeoAreaMonitorS60::connectNotify(const char* signal)
{
    if ((iTriggerCreateAO->isTriggerInitialized(this, EntryTrigger)) && (QLatin1String(signal) == SIGNAL(areaEntered(QGeoPositionInfo))) && (receivers(SIGNAL(areaEntered(const QGeoPositionInfo&))) <= 1)) {
        iTriggerCreateAO->SetTriggerState(this, EntryTrigger, true);
        iTriggerAO-> NotifyFiredEvent();
    }

    if ((iTriggerCreateAO->isTriggerInitialized(this, ExitTrigger)) && (QLatin1String(signal) == SIGNAL(areaExited(QGeoPositionInfo))) && (receivers(SIGNAL(areaExited(const QGeoPositionInfo&))) <= 1)) {
        iTriggerCreateAO->SetTriggerState(this, ExitTrigger, true);
        iTriggerAO-> NotifyFiredEvent();
    }
}

//Notification called when a sot is disconnected from the areaEntered or
//areaExited signal
void QGeoAreaMonitorS60::disconnectNotify(const char* signal)
{
    // Disable the trigger, if no slot connected to signal
    if ((iTriggerCreateAO->isTriggerInitialized(this, EntryTrigger)) && (QLatin1String(signal) == SIGNAL(areaEntered(QGeoPositionInfo)))  && (receivers(SIGNAL(areaEntered(const QGeoPositionInfo&))) == 0)) {
        // iEnterTrigger->NotifyFiredEvent (FALSE);
        iTriggerCreateAO->SetTriggerState(this, EntryTrigger, FALSE);
    }

    if ((iTriggerCreateAO->isTriggerInitialized(this, ExitTrigger)) && (QLatin1String(signal) == SIGNAL(areaExited(QGeoPositionInfo))) && (receivers(SIGNAL(areaExited(const QGeoPositionInfo&))) == 0)) {
        // iExitTrigger->NotifyFiredEvent (FALSE);
        iTriggerCreateAO->SetTriggerState(this, ExitTrigger, FALSE);
    }
}

QTM_END_NAMESPACE

