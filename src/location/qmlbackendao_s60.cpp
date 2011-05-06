/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qgeopositioninfosource_s60_p.h"
#include "qgeosatelliteinfosource_s60_p.h"
#include "qmlbackendao_s60_p.h"

QTM_BEGIN_NAMESPACE

//The name of the requestor //Added by PM
//_LIT(KRequestor,"QTMobility Location");

// constructor
CQMLBackendAO::CQMLBackendAO() :
        CActive(EPriorityStandard), // Standard priority
        mPosInfo(NULL),
        mRequester(NULL),
        mRequesterSatellite(NULL),
        mRequestType(RequestType(0))
{
}

//
//
//
CQMLBackendAO* CQMLBackendAO::NewLC(QObject *aRequester,
                                    RequestType  aRequestType, TPositionModuleId  aModId)
{
    CQMLBackendAO* self = new(ELeave) CQMLBackendAO();
    CleanupStack::PushL(self);
    self->ConstructL(aRequester, aRequestType, aModId);
    return self;
}

//
//
//
CQMLBackendAO* CQMLBackendAO::NewL(QObject *aRequester,
                                   RequestType  aRequestType, TPositionModuleId  aModId)
{
    CQMLBackendAO* self = CQMLBackendAO::NewLC(aRequester, aRequestType, aModId);
    CleanupStack::Pop(); // self;
    return self;
}

//
//
//
TInt CQMLBackendAO::ConstructL(QObject *aRequester, RequestType  aRequestType,
                               TPositionModuleId  aModId)
{
    TInt error = KErrNone;
    RPositionServer PosServer;

    if (aRequester->inherits("QGeoSatelliteInfoSource"))
        mRequesterSatellite = static_cast<CQGeoSatelliteInfoSourceS60*>(aRequester);
    else
        mRequester = static_cast<CQGeoPositionInfoSourceS60*>(aRequester);

    mRequestType = aRequestType;

    if ((mRequestType == RegularUpdate) || (mRequestType == OnceUpdate)) {
        if (aRequester->inherits("QGeoSatelliteInfoSource"))
            PosServer = mRequesterSatellite->getPositionServer();
        else
            PosServer = mRequester->getPositionServer();

        error  =  mPositioner.Open(PosServer, aModId);

        if (error != KErrNone)
            return error;

        CleanupClosePushL(mPositioner);

        error = mPositioner.SetRequestor(CRequestor::ERequestorService ,
                                         CRequestor::EFormatApplication, _L("QTmobility_Location"));

        CleanupStack::Pop(1);

        if (error != KErrNone)
            return error;

    }

    CActiveScheduler::Add(this); // Add to scheduler

    return error;
}

//
CQMLBackendAO::~CQMLBackendAO()
{
    Cancel();

    if ((mRequestType == OnceUpdate) || (mRequestType == RegularUpdate)) {
        //close the subsession
        mPositioner.Close();

        if (mPosInfo) {
            mPosInfo->ClearRequestedFields();

            mPosInfo->ClearPositionData();
            //delete the HPositionGenericInfo
            delete mPosInfo;
        }
    } else if (mRequestType == DeviceStatus) {
        RPositionServer posServer;

        //done only by the position server Active Object
        if (mRequester)
            posServer = mRequester->getPositionServer();
        else
            posServer = mRequesterSatellite->getPositionServer();

        posServer.Close();
    }
}

//
void CQMLBackendAO::DoCancel()
{
    CancelAll();
}


//
void CQMLBackendAO::RunL()
{
    switch (mRequestType) {
        case DeviceStatus :
            handleDeviceNotification(iStatus.Int());
            break;
        case RegularUpdate :
        case OnceUpdate:
            handlePosUpdateNotification(iStatus.Int());
            break;
        default         :
            break;
    }
}

//
TInt CQMLBackendAO::RunError(TInt aError)
{
    return aError;
}


// checks any pending request in activeobject
bool CQMLBackendAO::isRequestPending()
{
    if (IsActive())
        return true;
    else
        return false;
}



// Async call to get notifications about device status.
void CQMLBackendAO::notifyDeviceStatus(TPositionModuleStatusEventBase &aStatusEvent)
{
    RPositionServer PosServ;

    if (mRequester)
        PosServ = mRequester->getPositionServer();
    else
        PosServ = mRequesterSatellite->getPositionServer();

    //register for device status events
    TPositionModuleStatusEventBase::TModuleEvent RequestedEvents(
        TPositionModuleStatusEventBase::EEventDeviceStatus);

    aStatusEvent.SetRequestedEvents(RequestedEvents);

    PosServ.NotifyModuleStatusEvent(aStatusEvent, iStatus);

    SetActive();

}

void CQMLBackendAO::CancelAll()
{

    RPositionServer PosServer;
    if (mRequestType == DeviceStatus) {
        if (mRequester)
            PosServer = mRequester->getPositionServer();
        else
            PosServer = mRequesterSatellite->getPositionServer();

        PosServer.CancelRequest(EPositionServerNotifyModuleStatusEvent);
    } else if ((mRequestType == OnceUpdate) || (mRequestType == RegularUpdate)) {
        mPositioner.CancelRequest(EPositionerNotifyPositionUpdate);
    }

}

//Initialize the posInfo with appropriate fields
bool CQMLBackendAO::initializePosInfo()
{
    if (!mPosInfo) {
        mPosInfo = HPositionGenericInfo::New();

        if (mPosInfo == NULL)
            return FALSE;
    } else {
        mPosInfo->ClearRequestedFields();

        mPosInfo->ClearPositionData();
    }
    RPositionServer posServer;
    TPositionModuleInfo moduleInfo;
    TInt error = KErrNone;

    //get the posiiton server
    posServer = mRequester->getPositionServer();

    //retrieve the module id used by the posiitoner
    if (mRequestType == RegularUpdate)
        error = posServer.GetModuleInfoById(mRequester->getCurrentPositonModuleID(), moduleInfo);
    else
        error = posServer.GetModuleInfoById(mRequester->getRequestUpdateModuleID(), moduleInfo);

    if (error == KErrNone) {

        //get capabilities of the module
        TPositionModuleInfo::TCapabilities caps = moduleInfo.Capabilities();

        if (caps & TPositionModuleInfo::ECapabilitySatellite) {
            mPosInfo->SetRequestedField(EPositionFieldSatelliteNumInView);
            mPosInfo->SetRequestedField(EPositionFieldSatelliteNumUsed);
        }

        if (caps & TPositionModuleInfo::ECapabilitySpeed) {
            mPosInfo->SetRequestedField(EPositionFieldHorizontalSpeed);
            mPosInfo->SetRequestedField(EPositionFieldVerticalSpeed);
        }
        if (caps & TPositionModuleInfo::ECapabilityCompass) {
            mPosInfo->SetRequestedField(EPositionFieldMagneticCourseError);
            mPosInfo->SetRequestedField(EPositionFieldHeading);
        }
        return TRUE;
    }
    return FALSE;
}

//requestUpdate : request for position update once
void CQMLBackendAO::requestUpdate(int aTimeout)
{
    TPositionUpdateOptions  aPosOption;

    mPositioner.GetUpdateOptions(aPosOption);

    aPosOption.SetUpdateInterval(TTimeIntervalMicroSeconds(0));

    aPosOption.SetUpdateTimeOut(TTimeIntervalMicroSeconds(aTimeout * 1000));

    mPositioner.SetUpdateOptions(aPosOption);

    startUpdates();
}


//
void CQMLBackendAO::cancelUpdate()
{
    Cancel();

}


//
void CQMLBackendAO::handleDeviceNotification(int aError)
{
    switch (aError) {
            //NotifyPositionModulestatusEvent successfully completed
        case KErrNone :

            //module not found
        case KErrNotFound :
            if (mRequester)
                mRequester->updateDeviceStatus();
            else
                mRequesterSatellite->updateDeviceStatus();
            break;

            //request has been successfully cancelled
        case KErrCancel :
            break;

            //unrecoverabe errors
        default :
            break;
    }
}


//
void CQMLBackendAO::handlePosUpdateNotification(int aError)
{

    HPositionGenericInfo *positionInfo = NULL;

    TPositionSatelliteInfo satInfo;
    switch (aError) {
            //NotifyPositionUpdate successfully completed
        case KErrNone :


            //requested information could not be retrieved within the maximum peroid
        case KErrTimedOut:



            if (mRequester) {
                positionInfo = HPositionGenericInfo::New();

                if (positionInfo == NULL)
                    return;

                //copy the buffer contents into a new HPositionGenericInfo buffer,to be used
                //for creating QGeoPositionInfo object later
                memcpy(positionInfo , mPosInfo , mPosInfo->BufferSize());
            } else {
                satInfo  = mPosSatInfo;
            }

            //if regUpdateAO, request for the next update
            if (mRequestType == RegularUpdate) {
                if (mRequester) {
                    initializePosInfo();
                    mPositioner.NotifyPositionUpdate(*mPosInfo, iStatus);
                } else {
					mPosSatInfo.ClearSatellitesInView();
                    mPositioner.NotifyPositionUpdate(mPosSatInfo, iStatus);
					}

                SetActive();
            }

            if (mRequester) {
                mRequester->updatePosition(positionInfo, aError);
                delete positionInfo;
            } else {
                if ((aError != KErrTimedOut) || (mRequestType != RegularUpdate)) {
                    mRequesterSatellite->updatePosition(satInfo, aError, (mRequestType == RegularUpdate));
                }
            }

            break;

        default :
            if (mRequester) {
                mRequester->updatePosition(positionInfo, aError); // positionInfo will be NULL
            } else {
                mRequesterSatellite->updatePosition(satInfo, aError, (mRequestType == RegularUpdate));
            }
            break;

    }
}

//////////////////////////////////////////////////////////////
// Sets the interval for getting the regular notification   //
// the time interval set is in milli seconds                      //
//////////////////////////////////////////////////////////////
int CQMLBackendAO::setUpdateInterval(int aMilliSec)
{
    int minimumUpdateInterval = 0;
    TInt64 mUpdateInterval = 0 ;


    if (mRequester)
        minimumUpdateInterval = mRequester->minimumUpdateInterval();
    else
        minimumUpdateInterval = mRequesterSatellite->minimumUpdateInterval();

    if (minimumUpdateInterval < 0)
        minimumUpdateInterval = 100;
    // if the current requesttype is  regular updates
    // then set the updateinterval otherwise ignore
    //if(mRequestType != REQ_REG_UPDATE)
    //    return;

    TPositionUpdateOptions  aPosOption;

    TInt error = mPositioner.GetUpdateOptions(aPosOption);

    // TTimeIntervalMicroSeconds is converted seconds
    TInt currentUpdateInterval  = aPosOption.UpdateInterval().Int64() / 1000;

    // If msec is not 0 and is less than the value returned by minimumUpdateInterval(),
    // the interval will be set to the minimum interval.
    // if (aMilliSec != 0 && aMilliSec <= minimumUpdateInterval) {
    // workaround, not accepting zero as value, see QTMOBILITY-995
    if (aMilliSec <= minimumUpdateInterval) {
        mUpdateInterval = minimumUpdateInterval;
    } else {
        mUpdateInterval = aMilliSec;
    }

    // if the same value is being set then just ignore it.
    if (currentUpdateInterval == mUpdateInterval) {
        return mUpdateInterval;
    }

    // will set Either zero, minimum or +ve value
    // seconds converted to TTimeIntervalMicroSeconds
    aPosOption.SetUpdateInterval(TTimeIntervalMicroSeconds(mUpdateInterval * 1000));
    // set the timeout to the smaller of 150% of interval or update interval + 10 seconds
    TInt64 mUpdateTimeout = (mUpdateInterval * 3) / 2;
    if (mUpdateTimeout > mUpdateInterval + 10000)
        mUpdateTimeout = mUpdateInterval + 10000;

    if (aMilliSec > 0)
        aPosOption.SetUpdateTimeOut(TTimeIntervalMicroSeconds(mUpdateTimeout * 1000));

    error = mPositioner.SetUpdateOptions(aPosOption);

    return mUpdateInterval;
}

void CQMLBackendAO::startUpdates()
{
    if (!IsActive()) {
        if (mRequester) {
            initializePosInfo();
            mPositioner.NotifyPositionUpdate(*mPosInfo , iStatus);
        } else {
			mPosSatInfo.ClearSatellitesInView();
            mPositioner.NotifyPositionUpdate(mPosSatInfo , iStatus);
		}
		
        SetActive();

    }
}

QTM_END_NAMESPACE
