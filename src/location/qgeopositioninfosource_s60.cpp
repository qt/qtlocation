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

#include <QObject>
#include <QDateTime>
#include <QDebug>
#include <limits.h>
#include "qgeopositioninfosource_s60_p.h"
#include "qgeopositioninfosource.h"
#include "qmlbackendao_s60_p.h"



QTM_BEGIN_NAMESPACE

// constructor
CQGeoPositionInfoSourceS60::CQGeoPositionInfoSourceS60(QObject* aParent) : QGeoPositionInfoSource(aParent),
        mCurrentModuleId(TUid::Null()),
        mReqModuleId(TUid::Null()),
        mDevStatusUpdateAO(NULL),
        mReqUpdateAO(NULL),
        mRegUpdateAO(NULL),
        mSupportedMethods(PositioningMethod(0)),
        mCurrentMethod(PositioningMethod(0)),
        mListSize(0),
        mMinUpdateInterval(100),
        mStartUpdates(FALSE),
        mRegularUpdateTimedOut(FALSE),
        mModuleFlags(0)
{
    memset(mList, 0 , MAX_SIZE * sizeof(CPosMethodInfo));
}

// destructor
CQGeoPositionInfoSourceS60::~CQGeoPositionInfoSourceS60()
{
    if (mReqUpdateAO)
        delete mReqUpdateAO;

    if (mRegUpdateAO)
        delete mRegUpdateAO;

    if (mDevStatusUpdateAO)
        delete mDevStatusUpdateAO;


}

// static function NewLC
CQGeoPositionInfoSourceS60* CQGeoPositionInfoSourceS60::NewLC(QObject* aParent)
{
    CQGeoPositionInfoSourceS60* self =
        new(ELeave) CQGeoPositionInfoSourceS60(aParent);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// static function NewL
CQGeoPositionInfoSourceS60* CQGeoPositionInfoSourceS60::NewL(QObject * aParent)
{
    RProcess thisProcess;
    if (!thisProcess.HasCapability(ECapabilityLocation)) {
        qWarning() << "QGeoPositionInfoSource::createDefaultSource() requires the Symbian Location capability to succeed on the Symbian platform.";
        return 0;
    }

    CQGeoPositionInfoSourceS60* self = CQGeoPositionInfoSourceS60::NewLC(aParent);
    CleanupStack::Pop();

    //check if the second phase construction is successful
    if (!self->isValid()) {
        delete self;
        self = NULL;
    }

    return self;
}

// 2nd phase constructor
void CQGeoPositionInfoSourceS60::ConstructL()
{
    TInt error = mPositionServer.Connect();

    if (error == KErrNone) {
        CleanupClosePushL(mPositionServer);

        mDevStatusUpdateAO = CQMLBackendAO::NewL(this, DeviceStatus);

        if (mDevStatusUpdateAO == NULL) {
            CleanupStack::Pop(1);

            return;
        }

        //update the list array with the available method initially
        updateDeviceStatus();

        // Set the PreferredPositioningMethods based on Supported methods
        QGeoPositionInfoSource::setPreferredPositioningMethods(mSupportedMethods);

        //devStatusUpdateAO->NotifyDeviceStatus(mStatusEvent);

        CleanupStack::PushL(mDevStatusUpdateAO);

        if (mCurrentModuleId != TUid::Null()) {
            mRegUpdateAO = CQMLBackendAO::NewL(this, RegularUpdate, mCurrentModuleId);
            mRegUpdateAO->setUpdateInterval(updateInterval());
        }
        CleanupStack::Pop(2);
    }

}

QGeoPositionInfo CQGeoPositionInfoSourceS60::lastKnownPosition(bool aFromSatellitePositioningMethodsOnly) const
{

    QGeoPositionInfo posUpdate;
    TPosition pos;
    TInt error = KErrNone;
    RPositioner iLastKnownpositioner;
    TRequestStatus status;

    error = iLastKnownpositioner.Open(*const_cast<RPositionServer*>(&mPositionServer));
    //request for lastknown position update and wait for the request to complete
    if (error == KErrNone) {

        TPositionInfo posInfo;

        iLastKnownpositioner.SetRequestor(CRequestor::ERequestorService ,
                                          CRequestor::EFormatApplication, _L("QTmobility_Location"));


        iLastKnownpositioner.GetLastKnownPosition(posInfo, status);

        User::WaitForRequest(status);

        error = status.Int();

        iLastKnownpositioner.Close();

        if ((error == KErrNone) || (error == KPositionPartialUpdate)) {

            TPositionModuleInfo modInfo;
            mPositionServer.GetModuleInfoById(posInfo.ModuleId(), modInfo);

            if (!aFromSatellitePositioningMethodsOnly || (aFromSatellitePositioningMethodsOnly && (modInfo.Capabilities() & TPositionModuleInfo::ECapabilitySatellite))) {
                QGeoCoordinate  coord;

                posInfo.GetPosition(pos);

                coord.setLatitude(pos.Latitude());
                coord.setLongitude(pos.Longitude());
                coord.setAltitude(pos.Altitude());

                //store the QGeoCoordinate values
                posUpdate.setCoordinate(coord);

                TDateTime datetime = pos.Time().DateTime();
                QDateTime dt(QDate(datetime.Year(), datetime.Month() + 1, datetime.Day() + 1),
                             QTime(datetime.Hour(), datetime.Minute(), datetime.Second(),
                                   datetime.MicroSecond() / 1000),
                             Qt::UTC);

                //store the time stamp
                posUpdate.setTimestamp(dt);

                //store the horizontal accuracy
                posUpdate.setAttribute(QGeoPositionInfo::HorizontalAccuracy, pos.HorizontalAccuracy());

                //store the vertical accuracy
                posUpdate.setAttribute(QGeoPositionInfo::VerticalAccuracy, pos.VerticalAccuracy());

            }
        }
    }

    return posUpdate;
}

//
int CQGeoPositionInfoSourceS60::minimumUpdateInterval() const
{
    if (mCurrentModuleId == TUid::Null())
        return 0;

    return mMinUpdateInterval;
}



//private function : to derive the supported positioning methods
void CQGeoPositionInfoSourceS60::updateAvailableTypes()
{
    PositioningMethods types;

    for (TInt i = 0; i < mListSize ; i++) {
        //check only for the available module without any device errors
        if (mList[i].mIsAvailable &&
                (mList[i].mStatus != TPositionModuleStatus::EDeviceUnknown) &&
                (mList[i].mStatus != TPositionModuleStatus::EDeviceError)) {
            types |= mList[i].mPosMethod;

        }

        //if both SatellitePositioningMethods and NonSatellitePositioningMethods return
        if (types.testFlag(AllPositioningMethods))
            break;
    }

    //update the supported methods based on types value, derived from the above steps
    mSupportedMethods = types;
}

//private function : to retrieve the index of the supplied module id from the mList array
TInt CQGeoPositionInfoSourceS60::checkModule(TPositionModuleId aId)//const
{

    TInt i;
    for (i = 0; i < mListSize; i++)
        if (mList[i].mUid == aId) {
            return i;
        }

    return -1;
}

//private function : get the index of the mList that supports the preferred method if
//available,else returns the index of the default module
TInt CQGeoPositionInfoSourceS60::getIndexPositionModule(TUint8 aBits, PositioningMethods aPosMethods) const
{
    TInt index, error;

    TPositionModuleId modID;

    //get the default module Id, whose index is returned if the preferred methods are not
    //available
    error = mPositionServer.GetDefaultModuleId(modID);

    if (error != KErrNone)
        modID = TUid::Null();

    //index = -1 : no methods available in the mList that supports preferred methods
    index = -1;


    for (TInt i = 0; i < mListSize ; i++) {
        //check the module properties to select the preferred method,search should not
        //not select an unavailable method,error device or index matching aLastIndex
        if (mList[i].mIsAvailable && aPosMethods.testFlag(mList[i].mPosMethod)
                && (mList[i].mStatus != TPositionModuleStatus::EDeviceUnknown)
                && (mList[i].mStatus != TPositionModuleStatus::EDeviceError)
                && (((aBits >> i) & 1))) {
            return i;
        }

        //store the index of the default module
        if ((modID == mList[i].mUid) && ((aBits >> i) & 1))
            index = i;
    }


    return index;
}

//private function : to get the index of the positioning method with time to first fix
//lesser than timeout
TInt CQGeoPositionInfoSourceS60::getMoreAccurateMethod(TInt aTimeout, TUint8 aBits)
{
    TInt index = -1;
    double temp = -1.0;
    PositioningMethods posMethods;
    TTimeIntervalMicroSeconds microSeconds;

    posMethods = preferredPositioningMethods();

    //convert the timeout --> micro seconds
    microSeconds = aTimeout * 1000;

    for (TInt i = 0 ; i < mListSize; i++) {
        if (mList[i].mIsAvailable
                && posMethods.testFlag(mList[i].mPosMethod)
                && (mList[i].mStatus != TPositionModuleStatus::EDeviceUnknown)
                && (mList[i].mStatus != TPositionModuleStatus::EDeviceError)
                && (((aBits >> i) & 1))
                && (mList[i].mTimeToFirstFix < microSeconds)) {
            if ((temp == -1.0) || (mList[i].mHorizontalAccuracy < temp)) {
                index = i;
                temp = mList[i].mHorizontalAccuracy;
            }
        }
    }

    if (index != -1) {
        return index;
    }

    bool minSet = false;
    microSeconds = 0;

    for (TInt i = 0 ; i < mListSize; i++) {
        if (mList[i].mIsAvailable
                && posMethods.testFlag(mList[i].mPosMethod)
                && (mList[i].mStatus != TPositionModuleStatus::EDeviceUnknown)
                && (mList[i].mStatus != TPositionModuleStatus::EDeviceError)
                && (((aBits >> i) & 1))) {
            if (!minSet || (mList[i].mTimeToFirstFix < microSeconds)) {
                index = i;
                minSet = true;
                microSeconds = mList[i].mTimeToFirstFix;
            }
        }
    }

    return index;
}

//private function : to update the mList array
void CQGeoPositionInfoSourceS60::updateStatus(TPositionModuleInfo &aModInfo, TInt aStatus)
{

    TInt i, index;
    TPositionModuleId id;
    PositioningMethod method;
    TBool available;
    TReal32 accuracy;
    TTimeIntervalMicroSeconds time_to_first_fix, time_to_next_fix;
    TPositionQuality quality;
    CQMLBackendAO *temp = NULL;

    //query for the following parameters
    id = aModInfo.ModuleId();

    //module with a satellite capability is assumed to be provided satellite based positioning
    //method
    method = (aModInfo.Capabilities() & TPositionModuleInfo::ECapabilitySatellite) ?
             SatellitePositioningMethods : NonSatellitePositioningMethods;

    //gets the device availability based on the user settings
    available = aModInfo.IsAvailable();

    //quality : holds the required metrics
    aModInfo.GetPositionQuality(quality);

    //Accuracy
    accuracy = quality.HorizontalAccuracy();

    //time taken for the first fix
    time_to_first_fix = quality.TimeToFirstFix();

    //time taken for the subsequent fix
    time_to_next_fix = quality.TimeToNextFix();



    if ((i = checkModule(id)) == -1) {
        //update the properties of the module
        QMutexLocker lLocker(&m_mutex);

        //TPositionModuleId of the module
        mList[mListSize].mUid = id;

        //positioning method method type
        mList[mListSize].mPosMethod = method;

        //status of the device
        mList[mListSize].mStatus = aStatus;

        //availablility of the module
        mList[mListSize].mIsAvailable = available;

        //horizontal accuracy of the module
        mList[mListSize].mHorizontalAccuracy = accuracy;

        //time required to get the first fix
        mList[mListSize].mTimeToFirstFix = time_to_first_fix;

        //time required for subsequent fix
        mList[mListSize].mTimeToNextFix = time_to_next_fix;

        //count on the mList array size
        mListSize++;

        //update the supported source types based on the device status
        updateAvailableTypes();

        //store the correct method in use from the mCurrentModuleId retireved earlier
        if (id == mCurrentModuleId) {
            mCurrentMethod = method;
            mMinUpdateInterval = mList[mListSize-1].mTimeToNextFix.Int64() / 1000;
        }
        lLocker.unlock();
    } else {

        QMutexLocker lLocker(&m_mutex);
        //module's status has changed
        if (mList[i].mStatus != aStatus)
            mList[i].mStatus = aStatus;

        //module's availability has changed
        if (mList[i].mIsAvailable != available)
            mList[i].mIsAvailable = available;

        //module's horizontal accuracy has changed
        if (mList[i].mHorizontalAccuracy != accuracy)
            mList[i].mHorizontalAccuracy = accuracy;

        //module's time to first fix has changed
        if (mList[i].mTimeToFirstFix != time_to_first_fix)
            mList[i].mTimeToFirstFix = time_to_first_fix;

        //module's time to subsequent fix has changed
        if (mList[i].mTimeToNextFix != time_to_next_fix)
            mList[i].mTimeToFirstFix = time_to_next_fix;

        //update the supported source types based on the device status
        updateAvailableTypes();

        lLocker.unlock();

        //if the mCurrentModuleId is NULL, try updating the reg update with the available
        //positioning method
        if (mCurrentModuleId == TUid::Null() && (available == TRUE) &&
                (aStatus != TPositionModuleStatus::EDeviceUnknown) &&
                (aStatus != TPositionModuleStatus::EDeviceError)) {
            TInt interval;

            interval = QGeoPositionInfoSource::updateInterval();

            TRAPD(ret, QT_TRYCATCH_LEAVING(temp = CQMLBackendAO::NewL(this, RegularUpdate,
                                                  mList[i].mUid)));

            QMutexLocker lRegLocker_interval(&m_mutex_interval);
            mMinUpdateInterval = interval;
            lRegLocker_interval.unlock();

            if ((ret == KErrNone) && (temp != NULL)) {
                temp->setUpdateInterval(interval);

                QMutexLocker lRegLocker(&m_mutex_RegUpAO);
                if (mRegUpdateAO)
                    delete mRegUpdateAO;
                mRegUpdateAO = temp;
                lRegLocker.unlock();

                //to be uncommented when startUpdates are done

                if (mStartUpdates)
                    mRegUpdateAO->startUpdates();

                QMutexLocker lRegLocker_interval1(&m_mutex_interval);
                mCurrentModuleId = mList[i].mUid;
                mMinUpdateInterval = mList[i].mTimeToNextFix.Int64() / 1000;

                mCurrentMethod = mList[i].mPosMethod;
                lRegLocker_interval1.unlock();

            }
        }

        //check if the status of the currently used modules for regular update or
        //request update has changed
        if (((id == mCurrentModuleId) || (id == mReqModuleId)) &&
                ((aStatus == TPositionModuleStatus::EDeviceUnknown) ||
                 (aStatus == TPositionModuleStatus::EDeviceError) ||
                 (available == FALSE))) {
            //if the change happened for regular update
            if (id == mCurrentModuleId) {
                TInt interval;

                TUint8 bits;

                interval = QGeoPositionInfoSource::updateInterval();

                QMutexLocker lRegLocker1(&m_mutex_RegUpAO);
                if (mRegUpdateAO)
                    delete  mRegUpdateAO;
                lRegLocker1.unlock();

                bits = mModuleFlags;

                do {
                    //error = Get the index of the positioning module based on
                    //the preferred method if available , else get the default
                    //position module
                    index = getIndexPositionModule(bits);

                    if (index >= 0) {
                        TRAPD(ret, QT_TRYCATCH_LEAVING(temp = CQMLBackendAO::NewL(this, RegularUpdate,
                                                              mList[index].mUid)));

                        if ((ret == KErrNone) && (temp != NULL))
                            break;

                        bits = bits & (0XFF ^(1 << index));
                    }
                } while (index >= 0);


                if (temp != NULL) {
                    //successful in creating the subsession for the required
                    //method

                    QMutexLocker lRegLocker2(&m_mutex_RegUpAO);
                    mRegUpdateAO = temp;
                    lRegLocker2.unlock();

                    QMutexLocker lRegLocker_interval2(&m_mutex_interval);
                    mCurrentModuleId = mList[index].mUid;
                    mCurrentMethod = mList[index].mPosMethod;
                    mMinUpdateInterval = mList[index].mTimeToNextFix.Int64() / 1000;
                    lRegLocker_interval2.unlock();

                    mRegUpdateAO->setUpdateInterval(interval);

                    //to be uncommented when startUpdates are done

                    if (mStartUpdates)
                        mRegUpdateAO->startUpdates();

                } else {
                    //no methods available,clean up the resources
                    QMutexLocker lRegLocker3(&m_mutex_RegUpAO);
                    mRegUpdateAO = NULL;
                    lRegLocker3.unlock();

                    QMutexLocker lRegLocker_interval3(&m_mutex_interval);
                    mCurrentModuleId = TUid::Null();
                    mMinUpdateInterval = interval;
                    mCurrentMethod = PositioningMethod(0);
                    lRegLocker_interval3.unlock();

                    emit updateTimeout();
                }

            }

            //check if device status of the request update module changed
            if (id == mReqModuleId) {
                QMutexLocker lReqLocker(&m_mutex_ReqUpAO);
                if (mRegUpdateAO)
                    delete mReqUpdateAO;
                mReqUpdateAO = NULL;
                mReqModuleId = TUid::Null();
                lReqLocker.unlock();

                emit updateTimeout();
            }

        }

    }   //end else

}


// Notification methods from active object. Notifies device status changes
void CQGeoPositionInfoSourceS60::updateDeviceStatus(void)
{
    TPositionModuleStatus moduleStatus;
    TPositionModuleInfo moduleInfo;
    TInt error;

    //mListSize = 0 : called updateDeviceStatus() first time to initialise the array
    if (mListSize == 0) {
        TUint modCount;

        //count on the modules currently supported by the device
        mPositionServer.GetNumModules(modCount);

        //module ID of the default module
        error = mPositionServer.GetDefaultModuleId(mCurrentModuleId);

        if (error != KErrNone)
            mCurrentModuleId = TUid::Null();

        for (TUint i = 0; i < modCount; i++) {
            //get module information
            mPositionServer.GetModuleInfoByIndex(i, moduleInfo);

            //get module status
            mPositionServer.GetModuleStatus(moduleStatus, moduleInfo.ModuleId());

            //update the properties of the module in the mList array
            updateStatus(moduleInfo, moduleStatus.DeviceStatus());

            mModuleFlags |= (1 << i);
        }
    } else {
        //UpdateDeviceStatus() called after registering for NotifyModuleStatusEvent

        //get the module id from the status event
        TPositionModuleId id = mStatusEvent.ModuleId();

        //get module information
        mPositionServer.GetModuleInfoById(id, moduleInfo);

        //get current status of the module
        mStatusEvent.GetModuleStatus(moduleStatus);

        //update the properties of the module in the mList array
        updateStatus(moduleInfo, moduleStatus.DeviceStatus());


    }

    //register for next NotifyModuleStatusEvent
    mDevStatusUpdateAO->notifyDeviceStatus(mStatusEvent);

}

//
void CQGeoPositionInfoSourceS60::TPositionInfo2QGeoPositionInfo(
    HPositionGenericInfo *aPosInfo1, QGeoPositionInfo& aPosInfo2)
{
    TPosition pos;
    QGeoCoordinate  coord;
    float val;

    aPosInfo1->GetPosition(pos);


    coord.setLatitude(pos.Latitude());
    coord.setLongitude(pos.Longitude());
    coord.setAltitude(pos.Altitude());

    //store the QGeoCoordinate values
    aPosInfo2.setCoordinate(coord);

    TDateTime datetime = pos.Time().DateTime();
    QDateTime dt(QDate(datetime.Year() , datetime.Month() + 1, datetime.Day() + 1),
                 QTime(datetime.Hour() , datetime.Minute(), datetime.Second(),
                       datetime.MicroSecond() / 1000),
                 Qt::UTC);

    //store the time stamp
    aPosInfo2.setTimestamp(dt);

    //store the horizontal accuracy
    aPosInfo2.setAttribute(QGeoPositionInfo::HorizontalAccuracy, pos.HorizontalAccuracy());

    //store the vertical accuracy
    aPosInfo2.setAttribute(QGeoPositionInfo::VerticalAccuracy, pos.VerticalAccuracy());

    //check for the horizontal speed
    if (aPosInfo1->IsFieldAvailable(EPositionFieldHorizontalSpeed)) {
        aPosInfo1->GetValue(EPositionFieldHorizontalSpeed, val);
        aPosInfo2.setAttribute(QGeoPositionInfo::GroundSpeed, val);
    }

    //check for the vertcal speed
    if (aPosInfo1->IsFieldAvailable(EPositionFieldVerticalSpeed)) {
        aPosInfo1->GetValue(EPositionFieldVerticalSpeed, val);
        aPosInfo2.setAttribute(QGeoPositionInfo::VerticalSpeed, val);
    }

    //check for the magnetic variation
    if (aPosInfo1->IsFieldAvailable(EPositionFieldMagneticCourseError)) {
        aPosInfo1->GetValue(EPositionFieldMagneticCourseError, val);
        aPosInfo2.setAttribute(QGeoPositionInfo::MagneticVariation, val);
    }

    //check for the heading
    if (aPosInfo1->IsFieldAvailable(EPositionFieldHeading)) {
        aPosInfo1->GetValue(EPositionFieldHeading, val);
        aPosInfo2.setAttribute(QGeoPositionInfo::Direction, val);
    }
}
//
void CQGeoPositionInfoSourceS60::updatePosition(HPositionGenericInfo *aPosInfo, int aError)
{
    QGeoPositionInfo  posInfo;

    if (aError == KErrNone && aPosInfo) {
        //fill posUpdate
        TPositionInfo2QGeoPositionInfo(aPosInfo, posInfo);

        mRegularUpdateTimedOut = false;

        //emit posUpdate
        emit positionUpdated(posInfo);
    } else if (aError == KErrTimedOut) {
        //request has timed out
        if (mStartUpdates) {
            if (!mRegularUpdateTimedOut) {
                mRegularUpdateTimedOut = true;
                emit updateTimeout();
            }
        } else {
            emit updateTimeout();
        }
    } else {
        //posiitoning module is unable to return any position information
        emit updateTimeout();
    }
}

// Returns the PositionServer handle
RPositionServer& CQGeoPositionInfoSourceS60:: getPositionServer()
{
    return mPositionServer;
}


// for request update
void CQGeoPositionInfoSourceS60::requestUpdate(int aTimeout)
{
    TInt index = -1;
    TUint8 bits;

    CQMLBackendAO *temp = NULL;

    if (mRegUpdateAO == NULL || mCurrentModuleId == TUid::Null()) {
        emit updateTimeout();
        return;
    }

    //return if already a request update is pending
    if (mReqUpdateAO && mReqUpdateAO->isRequestPending())
        return;

    if (aTimeout < 0 || (aTimeout != 0 && aTimeout < minimumUpdateInterval())) {
        emit updateTimeout();
        return;
    }

    if (aTimeout == 0)
        aTimeout = 20000;

    bits = mModuleFlags;

    do  {

        //index of the more accurate method in the array
        index = getMoreAccurateMethod(aTimeout, bits);

        //no positioning method method available : emit updateTimeout
        if (index < 0) {
            emit updateTimeout();
            break;
        }
        //if the selected module for request update is same as the previous one reuse the request
        if (mList[index].mUid == mReqModuleId) {
            if (mReqUpdateAO) {
                mReqUpdateAO->requestUpdate(aTimeout);
                return;
            }
        }

        TRAPD(ret, QT_TRYCATCH_LEAVING(temp = CQMLBackendAO::NewL(this, OnceUpdate, mList[index].mUid)));

        if ((ret == KErrNone) && (temp != NULL)) {
            QMutexLocker lReqLocker(&m_mutex_ReqUpAO);
            //delete the old reqest update
            if (mReqUpdateAO)
                delete mReqUpdateAO;

            //set the requestAO to the newly created AO
            mReqUpdateAO = temp;
            //set the request module ID
            mReqModuleId = mList[index].mUid;
            lReqLocker.unlock();

            //start the update
            mReqUpdateAO->requestUpdate(aTimeout);

            return;
        }

        bits = bits & (0XFF ^(1 << index));

    } while (index >= 0);

    //cleanup resources if the invalid requpdate is still stored
    if (mReqUpdateAO) {
        QMutexLocker lReqLocker(&m_mutex_ReqUpAO);
        delete mReqUpdateAO;
        mReqUpdateAO = NULL;
        mReqModuleId = TUid::Null();
        lReqLocker.unlock();
    }

}

// starts the regular updates
void CQGeoPositionInfoSourceS60::startUpdates()
{
    if (mRegUpdateAO == NULL || mCurrentModuleId == TUid::Null()) {
        emit updateTimeout();
        return;
    }

    if (receivers(SIGNAL(positionUpdated(QGeoPositionInfo))) > 0 && !mStartUpdates)
        mRegUpdateAO->startUpdates();
    mRegularUpdateTimedOut = false;
    mStartUpdates = true;
}

// stops the regular updates
void CQGeoPositionInfoSourceS60::stopUpdates()
{
    mStartUpdates = false;

    if (mRegUpdateAO == NULL || mCurrentModuleId == TUid::Null()) {
        emit updateTimeout();
        return;
    }

    mRegUpdateAO->cancelUpdate();
}

void CQGeoPositionInfoSourceS60::setPreferredPositioningMethods(PositioningMethods aMethods)
{


    QGeoPositionInfoSource::setPreferredPositioningMethods(aMethods);

    PositioningMethods preferredMethod(PositioningMethods(0));
    TInt index = -1;
    // the poistioning methods are not supported
    // if the preferred positioning method is the current poistioning

    if (!(mSupportedMethods & aMethods) || (aMethods == PositioningMethods(AllPositioningMethods)) ||
            (aMethods == PositioningMethods(mCurrentMethod)))

        return;

    //TPositionModuleId moduleId = getPreferredPoistionModuleId(methods);

    //if(moduleId == TUid::Null())
    //return;
    CQMLBackendAO *temp = NULL;
    TUint8 bits;
    bits = mModuleFlags;
    do {
        //index = Get the index of the positioning module based on
        //the preferred method if available , else get the default
        //position module
        index = getIndexPositionModule(bits, aMethods);

        if (index == -1)
            return;

        if (index >= 0) {

            TRAPD(error, QT_TRYCATCH_LEAVING(temp = CQMLBackendAO::NewL(this, RegularUpdate,
                                                    mList[index].mUid)));
            if ((temp != NULL) && (error == KErrNone))
                break;

            bits = bits & (0XFF ^(1 << index));
        }
    } while (index >= 0);



    if (temp != NULL) {
        // this is needed as the previous setupdateinterval value should not be lost
        int updateInterval = QGeoPositionInfoSource::updateInterval();
        QMutexLocker lRegLocker(&m_mutex_RegUpAO);
        if (mRegUpdateAO)
            delete mRegUpdateAO;
        mRegUpdateAO = temp;

        mCurrentModuleId = mList[index].mUid ;
        lRegLocker.unlock();

        index = checkModule(mCurrentModuleId);

        if (index >= 0 && index < mListSize) {
            QMutexLocker lRegLocker_interval(&m_mutex_interval);
            mCurrentMethod = mList[index].mPosMethod ;
            mMinUpdateInterval = mList[index].mTimeToNextFix.Int64() / 1000;
            lRegLocker_interval.unlock();
        }

        int value = mRegUpdateAO->setUpdateInterval(updateInterval);
        //as the positioning module has changed,
        //possibility of the minimumupdateinterval being changed
        if (value != updateInterval)
            QGeoPositionInfoSource::setUpdateInterval(value);

    }

}

void CQGeoPositionInfoSourceS60::setUpdateInterval(int aMilliSec)
{


    if (mRegUpdateAO) {
        int interval = mRegUpdateAO->setUpdateInterval(aMilliSec);
        // as the above set value can be minimum value so
        // assigning to the base class data member
        QGeoPositionInfoSource::setUpdateInterval(interval);
    }
}

void CQGeoPositionInfoSourceS60::connectNotify(const char *aSignal)
{
    // start update if it already connected
    if (mStartUpdates && mRegUpdateAO && QLatin1String(aSignal) == SIGNAL(positionUpdated(QGeoPositionInfo)))
        mRegUpdateAO->startUpdates();

}

void CQGeoPositionInfoSourceS60::disconnectNotify(const char *aSignal)
{
    // Cancel updates if slot is disconnected for the positionUpdate() signal.

    if ((mRegUpdateAO) && (QLatin1String(aSignal) == SIGNAL(positionUpdated(QGeoPositionInfo))) && receivers(SIGNAL(positionUpdated(QGeoPositionInfo))) == 0)
        mRegUpdateAO->cancelUpdate();

}

QTM_END_NAMESPACE
