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

#include "qmlbackendmonitorao_s60_p.h"
#include "qgeoareamonitor_s60_p.h"
#include "qmlbackendmonitorinfo_s60_p.h"
#include "qmlbackendmonitorcreatetriggerao_s60_p.h"

QTM_BEGIN_NAMESPACE

//Sets the radius of the monitoring area, to  aRadius.If the Radius is less than the MinimumTriggerAreaSize(),
//then aRadius will be set to MinimumTriggerAreaSize() supported by the LBT implementation.If the aRadius is
//greater than the MinimumTriggerAreaSize() it is unaltered.
TInt QMLBackendMonitorCreateTriggerAO::getRadius(qreal& aRadius)
{
    TInt ret = KErrNone;

    qreal minimumRadius;

    TLbtTriggeringSystemSettings triggerSettings;

    //gets the triggering System Setting
    TRAP(ret, iLbt.GetTriggeringSystemSettingsL(triggerSettings));

    if (ret != KErrNone)
        return ret;

    minimumRadius = triggerSettings.MinimumTriggerAreaSize();

    if (aRadius < minimumRadius)
        aRadius = minimumRadius;

    return ret;
}

//creates a trigger of type aType(EntryTrigger/ExitTrigger), with the coordinates and radius as
//supplied in the argument
bool  QMLBackendMonitorCreateTriggerAO::InitializeTrigger(QGeoAreaMonitorS60* aParent ,
        enTriggerType aType,
        TCoordinate& aCoordinate,
        qreal& aRadius)
{
    TInt ret = KErrGeneral;

    TLbtTriggerId triggerID = NULL;

    //try retrieving the trigger information from the linked list corresponding to the aParent and aType
    CMonitorTriggerInfo* triggerInfo = iTriggerMonitorInfo->getMonitorTriggerInfo(aParent, aType);

    //if no triggerinfo available in the linked list
    if (triggerInfo ==  NULL) {
        //Define the triggering area
        CLbtGeoCircle* trigArea = NULL;

        TRAP(ret, trigArea = CLbtGeoCircle::NewL(
                                 aCoordinate ,//center coordinate
                                 aRadius               //radius in meters. If
                                 //NaN is used, Location
                                 //Triggering Server will
                                 //use minimal size of trigger
                                 //area as the radius of the
                                 //trigger
                             ));

        if ((ret != KErrNone) || !trigArea)
            return FALSE;

        CleanupStack::PushL(trigArea);

        CLbtTriggerConditionArea* cond = NULL;

        if (aType == EntryTrigger) {
            //2: Construct a entry type of trigger condition
            TRAP(ret, cond = CLbtTriggerConditionArea::NewL(
                                 trigArea,
                                 CLbtTriggerConditionArea::EFireOnEnter
                             ));
        } else if (aType == ExitTrigger) {
            TRAP(ret, cond = CLbtTriggerConditionArea::NewL(
                                 trigArea,
                                 CLbtTriggerConditionArea::EFireOnExit
                             ));
        }


        if ((ret != KErrNone) || !cond) {
            CleanupStack::PopAndDestroy(trigArea);
            return FALSE;
        }

        CleanupStack::Pop(trigArea);   //ownership of trigArea is transferred.

        CleanupStack::PushL(cond);

        RRequestorStack reqStack;

        CleanupClosePushL(reqStack);

        //trigger name.
        _LIT(KMyTriggerName, "EntryTrigger");
        TDesC triggerName(KMyTriggerName);

        if (aType == ExitTrigger) {
            _LIT(KMyTriggerName, "ExitTrigger");
            triggerName = KMyTriggerName;
        }

        //Construct requestor
        _LIT(KMyRequestorName, "QTLBTBackend");   //Application name used as requestor identifier

        CRequestor *req = NULL;

        TRAP(ret, req = CRequestor::NewL(
                            CRequestorBase::ERequestorService,
                            CRequestorBase::EFormatApplication,
                            KMyRequestorName
                        ));

        if ((ret != KErrNone) || !req) {
            CleanupStack::PopAndDestroy(&reqStack);
            CleanupStack::PopAndDestroy(cond);
            return FALSE;
        }

        CleanupStack::PushL(req);

        TRAP(ret, reqStack.AppendL(req));

        CleanupStack::Pop(req);

        if (ret != KErrNone) {
            CleanupStack::PopAndDestroy(&reqStack);
            CleanupStack::PopAndDestroy(cond);
            return FALSE;
        }

        TUid managerUid = TUid::Uid(0);

        CLbtSessionTrigger* trig = NULL;

        TRAP(ret, trig =  CLbtSessionTrigger::NewL(
                              triggerName,
                              CLbtTriggerEntry::EStateDisabled,
                              reqStack,
                              managerUid,
                              cond
                          ));

        CleanupStack::PopAndDestroy(&reqStack);

        CleanupStack::Pop(cond);

        trig->SetTimeToRearm(0);

        if ((ret != KErrNone) || (!trig)) {
            CleanupStack::PopAndDestroy(cond);
            return FALSE;
        }

        CleanupStack::PushL(trig);

        //iLbt.CreateTrigger(*trig, triggerID, ETrue, iStatus);

        //CleanupStack::PopAndDestroy(trig );

        TRAP(ret, iActiveSchedulerwait = new(ELeave) CActiveSchedulerWait);

        if ((ret != KErrNone) || !iActiveSchedulerwait) {
            CleanupStack::PopAndDestroy(trig);
            return FALSE;
        }

        iTriggerCreation = FALSE;

        //create a trigger asynchronously
        iLbt.CreateTrigger(*trig, triggerID, ETrue, iStatus);

        SetActive();

        //wait till the iActiveSchedularwait->AsyncStop() is called in RunL
        iActiveSchedulerwait->Start();

        delete iActiveSchedulerwait;

        CleanupStack::PopAndDestroy(trig);

        //if the trigger creation is successful, add the triggerinfo to the linked list
        if (iTriggerCreation == TRUE)
            iTriggerMonitorInfo->addMonitorTriggerInfo(aParent, triggerID, aType);

        delete req;

        return iTriggerCreation;
    } else {     //triggerinfo available in the linked list

        CLbtSessionTrigger* trig = NULL;

        //Define the triggering area
        CLbtGeoCircle* trigArea = NULL;

        TRAP(ret, trigArea = CLbtGeoCircle::NewL(
                                 aCoordinate ,   //center coordinate
                                 aRadius          //radius in meters. If
                                 //NaN is used, Location
                                 //Triggering Server will
                                 //use minimal size of trigger
                                 //area as the radius of the
                                 //trigger
                             ));

        if ((ret != KErrNone) || (!trigArea)) {

            return FALSE;
        }

        CleanupStack::PushL(trigArea);

        //2: Construct a entry type of trigger condition
        CLbtTriggerConditionArea* cond = NULL;

        if (aType == EntryTrigger) {
            //2: Construct a entry type of trigger condition
            TRAP(ret, cond = CLbtTriggerConditionArea::NewL(
                                 trigArea,
                                 CLbtTriggerConditionArea::EFireOnEnter
                             ));
        } else if (aType == ExitTrigger) {
            TRAP(ret, cond = CLbtTriggerConditionArea::NewL(
                                 trigArea,
                                 CLbtTriggerConditionArea::EFireOnExit
                             ));
        }


        if ((ret != KErrNone) || !cond) {
            CleanupStack::PopAndDestroy(trigArea);
            return FALSE;
        }

        CleanupStack::Pop(trigArea);   //ownership of trigArea is transferred.

        CleanupStack::PushL(cond);

        //create a session trigger
        TRAP(ret, trig = CLbtSessionTrigger::NewL());

        if ((ret != KErrNone) || (!trig)) {
            CleanupStack::PopAndDestroy(cond);
            return FALSE;
        }

        //set the condition for the trigger
        trig->SetCondition(cond);

        CleanupStack::Pop(cond);

        CleanupStack::PushL(trig);

        //set the trigger ID
        trig->SetId(triggerInfo->iTriggerID);

        iLbt.SetTriggerStateL(triggerInfo->iTriggerID, CLbtTriggerEntry::EStateDisabled, ELbtTrue);

        //update the trigger with the new condition in LBT server
        TRAP(ret, iLbt.UpdateTriggerL(*trig, CLbtTriggerEntry::EAttributeCondition, ELbtTrue));

        CleanupStack::PopAndDestroy(trig);


        if (ret != KErrNone) {
            return FALSE;;
        }

        return TRUE;
    }
}


QMLBackendMonitorCreateTriggerAO::~QMLBackendMonitorCreateTriggerAO()
{
    Cancel();
    iLbt.Close();   //closes the subsession
}


void  QMLBackendMonitorCreateTriggerAO::DoCancel()
{
    if (!IsActive()) {
        iActiveSchedulerwait->AsyncStop();
    }
}

void QMLBackendMonitorCreateTriggerAO::RunL()
{
    switch (iStatus.Int()) {
        case KErrNone :
            iTriggerCreation = TRUE;
            break;
        default :
            break;
    }
    //stops the AO, waiting in the iActiveSchedulerwait->Start()
    iActiveSchedulerwait->AsyncStop();
}

QMLBackendMonitorCreateTriggerAO* QMLBackendMonitorCreateTriggerAO::NewL(QGeoAreaMonitorS60* aParent , RLbtServer &aLbt)
{

    QMLBackendMonitorCreateTriggerAO* self = QMLBackendMonitorCreateTriggerAO::
            NewLC(aParent, aLbt);
    CleanupStack::Pop();

    return self;
}

QMLBackendMonitorCreateTriggerAO* QMLBackendMonitorCreateTriggerAO::NewLC(QGeoAreaMonitorS60* aParent , RLbtServer &aLbtServer)
{
    QMLBackendMonitorCreateTriggerAO *self = new(ELeave) QMLBackendMonitorCreateTriggerAO;
    CleanupStack::PushL(self);
    self->ConstructL(aLbtServer);
    if (!self->isValid()) {
        delete self;
        self = NULL;
    }
    return self;
}

void QMLBackendMonitorCreateTriggerAO::ConstructL(RLbtServer &aLbtServ)
{
    if (iLbt.Open(aLbtServ) == KErrNone) {      //opens the subseesion
        subsessionCreated = TRUE;
        //get the singleton object of CBackendMonitorInfo class
        iTriggerMonitorInfo = CBackendMonitorInfo::NewL();
    }
}

QMLBackendMonitorCreateTriggerAO::QMLBackendMonitorCreateTriggerAO()
        : CActive(EPriorityStandard), // Standard priority
        subsessionCreated(FALSE), iTriggerCreation(FALSE)
{
    CActiveScheduler::Add(this);        //add AO to the Schedular
}

//Enables/Disables the trigger state depending on the aStatus
void QMLBackendMonitorCreateTriggerAO::SetTriggerState(QGeoAreaMonitorS60* aParent, enTriggerType aType, bool aStatus)
{
    //retrieve the triggerinfo from the linked list from the supplied aPrent and aType
    CMonitorTriggerInfo* triggerInfo = iTriggerMonitorInfo->getMonitorTriggerInfo(aParent, aType);

    if (aStatus == true) {
        TRAPD(err, iLbt.SetTriggerStateL(triggerInfo->iTriggerID, CLbtTriggerEntry::EStateEnabled, ELbtTrue));
    } else {
        TRAPD(err, iLbt.SetTriggerStateL(triggerInfo->iTriggerID, CLbtTriggerEntry::EStateDisabled, ELbtTrue));
    }
}

//checks whether trigger is Initialized. The trigger entry corresponding to the aParent and aType is
//searched in the linked list
bool QMLBackendMonitorCreateTriggerAO::isTriggerInitialized(QGeoAreaMonitorS60* aParent, enTriggerType aType)
{
    CMonitorTriggerInfo* triggerInfo = iTriggerMonitorInfo->getMonitorTriggerInfo(aParent, aType);

    return (triggerInfo != NULL) ? TRUE : FALSE;
}

QTM_END_NAMESPACE
