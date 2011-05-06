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


#include <QObject>
#include <QDateTime>
#include <QDebug>
#include <limits.h>
#include "qgeopositioninfosource_s60_p.h"
#include "qgeosatelliteinfosource_s60_p.h"
#include "qgeosatelliteinfosource.h"
#include "qgeosatelliteinfo.h"

#include <QList>

QTM_BEGIN_NAMESPACE

// constructor
CQGeoSatelliteInfoSourceS60::CQGeoSatelliteInfoSourceS60(QObject* aParent) : QGeoSatelliteInfoSource(aParent),
        mCurrentModuleId(TUid::Null()),
        mReqModuleId(TUid::Null()),
        mDevStatusUpdateAO(NULL),
        mReqUpdateAO(NULL),
        mRegUpdateAO(NULL),
        mListSize(0),
        mMinUpdateInterval(0),
        mStartUpdates(FALSE),
        mModuleFlags(0)
{
    memset(mList, 0 , MAX_SIZE * sizeof(CSatMethodInfo));
}

// destructor
CQGeoSatelliteInfoSourceS60::~CQGeoSatelliteInfoSourceS60()
{
    if (mReqUpdateAO)
        delete mReqUpdateAO;

    if (mRegUpdateAO)
        delete mRegUpdateAO;

    if (mDevStatusUpdateAO)
        delete mDevStatusUpdateAO;
}

// static function NewLC
CQGeoSatelliteInfoSourceS60* CQGeoSatelliteInfoSourceS60::NewLC(QObject* aParent)
{
    CQGeoSatelliteInfoSourceS60* self =
        new(ELeave) CQGeoSatelliteInfoSourceS60(aParent);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// static function NewL
CQGeoSatelliteInfoSourceS60* CQGeoSatelliteInfoSourceS60::NewL(QObject * aParent)
{
    RProcess thisProcess;
    if (!thisProcess.HasCapability(ECapabilityLocation)) {
        qWarning() << "QGeoSatelliteInfoSource::createDefaultSource() requires the Symbian Location capability to succeed on the Symbian platform.";
        return 0;
    }

    CQGeoSatelliteInfoSourceS60* self = CQGeoSatelliteInfoSourceS60::NewLC(aParent);
    CleanupStack::Pop();

    //check if the second phase construction is successful
    if (!self->isValid()) {
        delete self;
        self = NULL;
    }

    return self;
}

// 2nd phase constructor
void CQGeoSatelliteInfoSourceS60::ConstructL()
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

        CleanupStack::PushL(mDevStatusUpdateAO);

        /*
        if(mCurrentModuleId != TUid::Null())
            mRegUpdateAO = CQMLBackendAOSatellite::NewL(this,RegularUpdate,mCurrentModuleId);
        */
        TUint8 bits;
        TInt index = -1;

        CQMLBackendAO *temp = NULL;

        bits = mModuleFlags;

        do {
            //error = Get the index of the positioning module based on
            //priority position module providing the satellite fix
            index = getIndexPositionModule(bits);

            if (index >= 0) {
                TRAPD(ret, QT_TRYCATCH_LEAVING(temp = CQMLBackendAO::NewL(this, RegularUpdate,
                                                      mList[index].mUid)));

                if ((ret == KErrNone) && (temp != NULL))
                    break;

                bits = bits & (0XFF ^(1 << index));
            }
        } while (index >= 0);

        if (index >= 0) {
            mRegUpdateAO = temp;
            mRegUpdateAO->setUpdateInterval(0);
            mCurrentModuleId = mList[index].mUid;
        } else {
            delete temp;
        }

        CleanupStack::Pop(2);
    }

}

//private function : to retrieve the index of the supplied module id from the mList array
TInt CQGeoSatelliteInfoSourceS60::checkModule(TPositionModuleId aId)//const
{
    QMutexLocker locker(&m_mutex);
    TInt i;
    for (i = 0; i < mListSize; i++)
        if (mList[i].mUid == aId)
            return i;
    return -1;
}

//
int CQGeoSatelliteInfoSourceS60::minimumUpdateInterval() const
{
    if (mCurrentModuleId == TUid::Null())
        return 0;

    return mMinUpdateInterval;

    /*TInt i = checkModule(mCurrentModuleId);
    if (i != -1)
        return mList[i].mTimeToNextFix.Int64() / 1000;           //divide by 1000, to convert microsecond to milisecond
    return 0;*/
}

//private function : get the index of the mList that supports the preferred method if
//available,else returns the index of the default module
TInt CQGeoSatelliteInfoSourceS60::getIndexPositionModule(TUint8 aBits) const
{
    TInt index;

    TPositionModuleId modID = TUid::Null();

    //index = -1 : no methods available in the mList that supports preferred methods
    index = -1;

    for (TInt i = 0; i < mListSize ; i++) {
        //check the module properties to select the preferred method,search should not
        //not select an unavailable method,error device or index matching aLastIndex
        if ((mList[i].mIsAvailable)
                && (mList[i].mStatus != TPositionModuleStatus::EDeviceUnknown)
                && (mList[i].mStatus != TPositionModuleStatus::EDeviceError)
                && (((aBits >> i) & 1))) {
            return i;
        }
    }

    return index;
}

//private function : to get the index of the positioning method with time to first fix
//lesser than timeout
TInt CQGeoSatelliteInfoSourceS60::getMoreAccurateMethod(TInt aTimeout, TUint8 aBits)
{
    TInt index = -1;
    double temp = -1.0;

    TTimeIntervalMicroSeconds microSeconds;

    //convert the timeout --> micro seconds
    microSeconds = aTimeout * 1000;

    if (microSeconds == 0)
        microSeconds = INT_MAX;

    for (TInt i = 0 ; i < mListSize; i++) {
        if (mList[i].mIsAvailable
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

    if (index != -1)
        return index;

    bool minSet = false;
    microSeconds = 0;

    for (TInt i = 0 ; i < mListSize; i++) {
        if (mList[i].mIsAvailable
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
void CQGeoSatelliteInfoSourceS60::updateStatus(TPositionModuleInfo &aModInfo, TInt aStatus)
{

    TInt i, index;
    TPositionModuleId id;
    TBool available;
    TReal32 accuracy;
    TTimeIntervalMicroSeconds time_to_first_fix, time_to_next_fix;
    TPositionQuality quality;
    CQMLBackendAO *temp = NULL;

    //query for the following parameters
    id = aModInfo.ModuleId();

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

        mMinUpdateInterval = mList[mListSize-1].mTimeToNextFix.Int64() / 1000;
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

        lLocker.unlock();

        //if the mCurrentModuleId is NULL, try updating the reg update with the available
        //positioning method
        if (mCurrentModuleId == TUid::Null() && (available == TRUE) &&
                (aStatus != TPositionModuleStatus::EDeviceUnknown) &&
                (aStatus != TPositionModuleStatus::EDeviceError)) {
            TInt interval = 0;

            TRAPD(ret, QT_TRYCATCH_LEAVING(temp = CQMLBackendAO::NewL(this, RegularUpdate,
                                                  mList[i].mUid)));

            if ((ret == KErrNone) && (temp != NULL)) {
                temp->setUpdateInterval(interval);

                QMutexLocker lRegLocker(&m_mutex_RegUpAO);
                if (mRegUpdateAO)
                    delete mRegUpdateAO;
                mRegUpdateAO = temp;

                //to be uncommented when startUpdates are done

                if (mStartUpdates)
                    mRegUpdateAO->startUpdates();


                mCurrentModuleId = mList[i].mUid;
                mMinUpdateInterval = mList[i].mTimeToNextFix.Int64() / 1000;
                lRegLocker.unlock();

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
                TInt interval = 0;

                TUint8 bits;

                QMutexLocker lRegLocker1(&m_mutex_RegUpAO);
                if (mRegUpdateAO)
                    delete  mRegUpdateAO;
                lRegLocker1.unlock();

                bits = mModuleFlags;

                do {
                    //error = Get the index of the positioning module based on
                    //priority position module providing the satellite fix
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

                    mCurrentModuleId = mList[index].mUid;
                    mMinUpdateInterval = mList[i].mTimeToNextFix.Int64() / 1000;
                    lRegLocker2.unlock();

                    mRegUpdateAO->setUpdateInterval(interval);

                    if (mStartUpdates)
                        mRegUpdateAO->startUpdates();

                } else {
                    //no methods available,clean up the resources
                    QMutexLocker lRegLocker3(&m_mutex_RegUpAO);
                    mRegUpdateAO = NULL;
                    mCurrentModuleId = TUid::Null();
                    mMinUpdateInterval = interval;
                    lRegLocker3.unlock();
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
                emit requestTimeout();
            }

        }

    }   //end else

}


// Notification methods from active object. Notifies device status changes
void CQGeoSatelliteInfoSourceS60::updateDeviceStatus(void)
{
    TPositionModuleStatus moduleStatus;
    TPositionModuleInfo moduleInfo;

    //mListSize = 0 : called updateDeviceStatus() first time to initialise the array
    if (mListSize == 0) {
        TUint modCount;

        //count on the modules currently supported by the device
        mPositionServer.GetNumModules(modCount);

        for (TUint i = 0; i < modCount; i++) {
            //get module information
            mPositionServer.GetModuleInfoByIndex(i, moduleInfo);

            if (moduleInfo.Capabilities() & TPositionModuleInfo::ECapabilitySatellite) {
                //get module status
                mPositionServer.GetModuleStatus(moduleStatus, moduleInfo.ModuleId());

                //update the properties of the module in the mList array
                updateStatus(moduleInfo, moduleStatus.DeviceStatus());

                mModuleFlags |= (1 << i);
            }
        }
    } else {
        //UpdateDeviceStatus() called afetr registering for NotifyModuleStatusEvent

        //get the module id from the status event
        TPositionModuleId id = mStatusEvent.ModuleId();

        //get module information
        mPositionServer.GetModuleInfoById(id, moduleInfo);

        if (moduleInfo.Capabilities() & TPositionModuleInfo::ECapabilitySatellite) {
            //get current status of the module
            mStatusEvent.GetModuleStatus(moduleStatus);

            //update the properties of the module in the mList array
            updateStatus(moduleInfo, moduleStatus.DeviceStatus());
        }

    }

    //register for next NotifyModuleStatusEvent
    mDevStatusUpdateAO->notifyDeviceStatus(mStatusEvent);

}

//
void CQGeoSatelliteInfoSourceS60::TPositionSatelliteInfo2QGeoSatelliteInfo(
    TPositionSatelliteInfo  &aSatInfo, QList<QGeoSatelliteInfo> &qListSatInView,
    QList<QGeoSatelliteInfo> &qListSatInUse)
{
    TInt satInView = aSatInfo.NumSatellitesInView();
    TSatelliteData satData;
    QGeoSatelliteInfo qInfo;

    for (TInt i = 0; i < satInView; i++) {
        aSatInfo.GetSatelliteData(i, satData);
        qInfo.setSignalStrength(satData.SignalStrength());
        qInfo.setPrnNumber(satData.SatelliteId());
        qInfo.setAttribute(QGeoSatelliteInfo::Elevation, satData.Elevation());
        qInfo.setAttribute(QGeoSatelliteInfo::Azimuth, satData.Azimuth());
        if (satData.IsUsed() == TRUE) {
            qListSatInUse.append(qInfo);
        }
        qListSatInView.append(qInfo);
    }
}
//
void CQGeoSatelliteInfoSourceS60::updatePosition(TPositionSatelliteInfo &aSatInfo,
        int aError, bool isStartUpdate)
{
    QList<QGeoSatelliteInfo> qListSatInUse;
    QList<QGeoSatelliteInfo> qListSatInView;
    if (aError == KErrNone) {
        //fill posUpdate
        TPositionSatelliteInfo2QGeoSatelliteInfo(aSatInfo, qListSatInView,
                qListSatInUse);
        if ((receivers(SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo>&))) > 0)) {
            if ((isStartUpdate == FALSE) || (mqListSatInView != qListSatInView)) {
                emit satellitesInViewUpdated(qListSatInView);
                mqListSatInView = qListSatInView;
            }
        }

        if ((receivers(SIGNAL(satellitesInUseUpdated(const QList<QGeoSatelliteInfo>&))) > 0)) {
            if ((isStartUpdate == FALSE) || (mqListSatInUse != qListSatInUse)) {
                emit satellitesInUseUpdated(qListSatInUse);
                mqListSatInUse = qListSatInUse;
            }
        }
    }

    else if (aError == KErrTimedOut) {
        //request has timed out
        emit requestTimeout();
    }

    else {
        //request has resulted in an error. could be changed to emit error signal when available.
        emit requestTimeout();
    }
}

// for request update
void CQGeoSatelliteInfoSourceS60::requestUpdate(int aTimeout)
{
    TInt index = -1;
    TUint8 bits;

    CQMLBackendAO *temp = NULL;

    if (mRegUpdateAO == NULL || mCurrentModuleId == TUid::Null()) {
        emit requestTimeout();
        return;
    }

    //requestupdate
    //return if already a request update is pending
    if (mReqUpdateAO && mReqUpdateAO->isRequestPending())
        return;

    if (aTimeout < 0) {
        emit requestTimeout();
        return;
    }

    bits = mModuleFlags;

    do  {

        //index of the more accurate method in the array
        index = getMoreAccurateMethod(aTimeout, bits);

        //no positioning method method available : emit requestTimeout
        if (index < 0) {
            emit requestTimeout();
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
        lReqLocker.unlock();
        mReqModuleId = TUid::Null();
    }

}

// starts the regular updates
void CQGeoSatelliteInfoSourceS60::startUpdates()
{
    if (mRegUpdateAO == NULL || mCurrentModuleId == TUid::Null()) {
        emit requestTimeout();
        return;
    }

    if (mRegUpdateAO && ((receivers(SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo>&))) > 0) ||
                         (receivers(SIGNAL(satellitesInUseUpdated(const QList<QGeoSatelliteInfo>&))) > 0)))
        mRegUpdateAO->startUpdates();

    mStartUpdates = true;
}

// stops the regular updates
void CQGeoSatelliteInfoSourceS60::stopUpdates()
{
    mStartUpdates = false;

    if (mRegUpdateAO == NULL || mCurrentModuleId == TUid::Null()) {
        emit requestTimeout();
        return;
    }

    if (mReqUpdateAO)
        mRegUpdateAO->cancelUpdate();
}


void CQGeoSatelliteInfoSourceS60::connectNotify(const char *aSignal)
{
    // start update if it already connected
    if (mStartUpdates && mRegUpdateAO && ((QLatin1String(aSignal) == SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo>&))) ||
                                          (QLatin1String(aSignal) == SIGNAL(satellitesInUseUpdated(const QList<QGeoSatelliteInfo>&)))))
        mRegUpdateAO->startUpdates();

}

void CQGeoSatelliteInfoSourceS60::disconnectNotify(const char *aSignal)
{
    // Cancel updates if slot is disconnected for the positionUpdate() signal.

    if ((mRegUpdateAO) && (QLatin1String(aSignal) == SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo>&))) && (receivers(SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo>&))) == 0) &&
            (QLatin1String(aSignal) == SIGNAL(satellitesInUseUpdated(const QList<QGeoSatelliteInfo>&))) && (receivers(SIGNAL(satellitesInUseUpdated(const QList<QGeoSatelliteInfo>&))) == 0))
        mRegUpdateAO->cancelUpdate();

}

QTM_END_NAMESPACE

