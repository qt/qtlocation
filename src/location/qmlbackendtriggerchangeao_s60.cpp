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
#include "qmlbackendtriggerchangeao_s60_p.h"

#include <lbtsessiontrigger.h>
#include <lbttriggerentry.h>

QTM_BEGIN_NAMESPACE

//Design chnages

QMLBackendTriggerChangeAO* QMLBackendTriggerChangeAO::instance = NULL;
TInt QMLBackendTriggerChangeAO::refCount = 0;


//request for the notification of trigger change event with the
//LBT server
void QMLBackendTriggerChangeAO::NotifyChangeEvent()
{
    if (!IsActive()) {
        iStatus = KRequestPending;
        iLbt.NotifyTriggerChangeEvent(iTriggerChangeEvent, iStatus);
        SetActive();
    }
}


QMLBackendTriggerChangeAO::~QMLBackendTriggerChangeAO()
{
    Cancel();
    iLbt.Close();
}

void  QMLBackendTriggerChangeAO::DoCancel()
{
    if (IsActive()) { //if AO is active,cancel the trigger change event request
        iLbt.CancelNotifyTriggerChangeEvent();
    }
}

void QMLBackendTriggerChangeAO::RunL()
{
    switch (iStatus.Int()) {
        case KErrNone :
#if 0
            //implementation takes the updation, deletion into consideration
            //from the UI, which isn't possible with the current UI for LBT.
            //The same should be considered once made availbale as part of the UI
            //check for Multiple trigger change events
            if (iTriggerChangeEvent.iEventType == ELbtTriggerChangeEventMultiple) {
                CMonitorTriggerInfo* triggerInfo1 = NULL;

                CMonitorTriggerInfo* triggerInfo2 = NULL;

                CMonitorTriggerInfo* triggerInfo3 = NULL;

                TInt i = 0, count = 0;

                RPointerArray < CLbtTriggerInfo > trigInfoList;

                CLbtTriggerEntry* entry = NULL;

                //retrieve the list of triggers created by the client
                iLbt.GetTriggersL(trigInfoList);

                CleanupClosePushL(trigInfoList);

                count = trigInfoList.Count();

                //get the header of the linked list holding the information
                //of the trigger
                triggerInfo1 = iTriggerMonitorInfo->getMonitorTriggerInfo();

                while (triggerInfo1) {
                    for (i = 0; i < count; i++) {
                        //get the trigger entryinfo
                        entry = trigInfoList[i]->TriggerEntry();

                        //if the triggerinfo retrieved matches the trigInfoList
                        //id break
                        if (triggerInfo1->iTriggerID == entry->Id())
                            break;
                    }

                    triggerInfo2 = triggerInfo1->next;

                    if (i == count)   //if the triggerinfo1->triggerid not found
                        //in the trigger list of the LBT server,
                        //remove the triggerinfo from the linked list
                    {
                        iTriggerMonitorInfo->removeMonitorTriggerInfo(
                            triggerInfo1->iTriggerID);
                    } else {

                        //if triggerinfo1->triggerid is found in the trigger list,
                        //and if it is an entry trigger, search for the corresponding
                        //exit trigger in the linked list
                        if (triggerInfo1->iType == EntryTrigger)
                            triggerInfo3 = iTriggerMonitorInfo->getMonitorTriggerInfo(
                                               triggerInfo1->iParent , ExitTrigger);

                        //if triggerinfo1->triggerid is found in the trigger list,
                        //and if it is an entry trigger, search for the corresponding
                        //entry trigger in the linked list
                        else if (triggerInfo1->iType == ExitTrigger)
                            triggerInfo3 = iTriggerMonitorInfo->getMonitorTriggerInfo(
                                               triggerInfo1->iParent, EntryTrigger);

                        //callback called for the notification change event of the trigger
                        if (triggerInfo3) {

                            (triggerInfo1->iParent)->handleTriggerChangeEvent(iTriggerChangeEvent.iEventType, trigInfoList[i],
                                    triggerInfo3->iTriggerID);
                        } else {
                            (triggerInfo1->iParent)->handleTriggerChangeEvent(iTriggerChangeEvent.iEventType, trigInfoList[i],
                                    NULL);
                        }

                    }
                    triggerInfo3 = NULL;

                    triggerInfo1 = triggerInfo2;
                }

                CleanupStack::Pop(1);

                //cloae the trigInfoList, holding the list of trigger
                //info
                trigInfoList.Close();

            }
            //check for trigger delete event
            else if (iTriggerChangeEvent.iEventType == ELbtTriggerChangeEventDeleted) {
                //delete the entry from the linked list,corresponding to the
                //iTriggerChangeEvent.iTriggerId
                iTriggerMonitorInfo->removeMonitorTriggerInfo(
                    iTriggerChangeEvent.iTriggerId);
            }
            //check for trigger updation event
            else if (iTriggerChangeEvent.iEventType == ELbtTriggerChangeEventUpdated) {
                CLbtTriggerInfo *info = NULL;

                //get the triggerinfo from the linked list corresponding
                //to the iTriggerChangeEvent.iTriggerId
                CMonitorTriggerInfo* triggerInfo1 = iTriggerMonitorInfo->getMonitorTriggerInfo(
                                                        iTriggerChangeEvent.iTriggerId);

                if (!triggerInfo1)
                    break;

                //get the triggerinfo of trigger with id - iTriggerChangeEvent.iTriggerId
                //from the LBT server
                info = iLbt.GetTriggerLC(iTriggerChangeEvent.iTriggerId);

                if (info == NULL)
                    break;

                //change in one-trigger,must update the corresponding entry/exit
                //trigger
                if (triggerInfo1) {
                    CMonitorTriggerInfo* triggerInfo2 = NULL;

                    //if the trigger updated is EntryTrigger, get the information
                    //of the corresponding ExitTrigger if any,from the
                    //linked list
                    if (triggerInfo1->iType == EntryTrigger)
                        triggerInfo2 = iTriggerMonitorInfo->getMonitorTriggerInfo(
                                           triggerInfo1->iParent, ExitTrigger);

                    //if the trigger updated is ExitTrigger, get the information
                    //of the corresponding EntryTrigger if any,fro mthe
                    //linked list
                    else if (triggerInfo1->iType == ExitTrigger)
                        triggerInfo2 = iTriggerMonitorInfo->getMonitorTriggerInfo(
                                           triggerInfo1->iParent, EntryTrigger);

                    //callback called for the notification change event for the trigger
                    if (triggerInfo2) {
                        (triggerInfo1->iParent)->handleTriggerChangeEvent(iTriggerChangeEvent.iEventType, info,
                                triggerInfo2->iTriggerID);
                    } else {
                        (triggerInfo1->iParent)->handleTriggerChangeEvent(iTriggerChangeEvent.iEventType, info,
                                NULL);
                    }
                }

                //pop and destroy the triggerinfo retrived from GetTriggerLC()
                CleanupStack::PopAndDestroy(info);
            }
#endif
            if (iTriggerChangeEvent.iEventType == ELbtTriggerChangeEventMultiple) {

                CMonitorTriggerInfo* triggerInfo1 = NULL;

                CMonitorTriggerInfo* triggerInfo2 = NULL;

                TInt i = 0, count = 0;

                RArray < TLbtTriggerId > triggerIdList;


                //retrieve the list of trigger IDs created by the client
                TRAPD(ret, iLbt.ListTriggerIdsL(triggerIdList));

                if (ret != KErrNone) {
                    break;
                }

                CleanupClosePushL(triggerIdList);

                count = triggerIdList.Count();

                //get the header of the linked list holding the information
                //of the trigger
                triggerInfo1 = iTriggerMonitorInfo->getMonitorTriggerInfo();

                while (triggerInfo1) {
                    for (i = 0; i < count; i++) {
                        //if the triggerinfo retrieved matches the triggerIdList
                        //id break
                        if (triggerInfo1->iTriggerID == triggerIdList[i])
                            break;
                    }

                    triggerInfo2 = triggerInfo1->next;

                    if (i == count)   //if the triggerinfo1->triggerid not found
                        //in the trigger list of the LBT server,
                        //remove the triggerinfo from the linked list
                    {
                        iTriggerMonitorInfo->removeMonitorTriggerInfo(
                            triggerInfo1->iTriggerID);
                    }

                    triggerInfo1 = triggerInfo2;
                }

                CleanupStack::Pop(1);

                //close the trigInfoList, holding the list of trigger
                //info
                triggerIdList.Close();
            }

            //check for trigger delete event
            else if (iTriggerChangeEvent.iEventType == ELbtTriggerChangeEventDeleted) {
                //delete the entry from the linked list,corresponding to the
                //iTriggerChangeEvent.iTriggerId
                iTriggerMonitorInfo->removeMonitorTriggerInfo(
                    iTriggerChangeEvent.iTriggerId);
            }
            break;

        default :
            break;
    }

    //request for the notification of any notification change events
    NotifyChangeEvent();
}


//static function called prior to the destruction of the
//singleton QMLBackendTriggerChangeAO object
void QMLBackendTriggerChangeAO::DeleteAO()
{
    //decrement the referent count
    refCount--;

    if (!refCount) { //if reference count becomes 0 delete the instance
        delete instance;
        instance = NULL;
    }
}

QMLBackendTriggerChangeAO* QMLBackendTriggerChangeAO::NewL(RLbtServer& aLbtServ)
{

    if (!instance) {
        instance = new  QMLBackendTriggerChangeAO;
        instance->ConstructL(aLbtServ);
        if (!instance->isValid()) {
            delete instance;
            instance = NULL;
            return NULL;
        }
    }
    refCount++;
    return instance;

}

QMLBackendTriggerChangeAO::QMLBackendTriggerChangeAO() : CActive(EPriorityNormal), iTriggerMonitorInfo(NULL)
{
    CActiveScheduler::Add(this);        //add current AO, to the Schedular
}

void QMLBackendTriggerChangeAO::ConstructL(RLbtServer& aLbtServ)
{
    if (iLbt.Open(aLbtServ) == KErrNone) { //open the RLBT subsesion
        subsessionCreated = TRUE;
        //Get the pointer to the CBackendMonitorInfo singleton object
        iTriggerMonitorInfo = CBackendMonitorInfo::NewL();
    }
}

QTM_END_NAMESPACE
