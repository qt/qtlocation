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

#ifndef QGEOPOSITIONINFOSOURCES60_H
#define QGEOPOSITIONINFOSOURCES60_H

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

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <lbs.h>
#include <qmutex.h>
#include "qgeopositioninfosource.h"
#include "qmlbackendao_s60_p.h"
#include "notificationcallback_s60_p.h"

#define MAX_SIZE 25

QTM_BEGIN_NAMESPACE

// CLASS DECLARATION
//forward declaration
class CQMLBackendAO;

class CPosMethodInfo
{
public:
    // A unique id for the positioning module
    TPositionModuleId  mUid;

    // Positioning Module (satellite/ non-satellite)
    QGeoPositionInfoSource::PositioningMethod  mPosMethod;

    //
    int  mStatus;

    // time to first and next fix from the location server
    TTimeIntervalMicroSeconds  mTimeToFirstFix;
    TTimeIntervalMicroSeconds  mTimeToNextFix;

    // Accuracy
    double mHorizontalAccuracy;

    // Flags whether the positioning technology is currently available or not.
    bool mIsAvailable;
};


/**
 *  CQGeoPositionInfoSourceS60
 *
 */
class CQGeoPositionInfoSourceS60 : public INotificationCallback,
        public QGeoPositionInfoSource

{
public:
    // Constructors and destructor
    /**
     * Destructor.
     */
    ~CQGeoPositionInfoSourceS60();

    /**
     * Two-phased constructor.
     */
    static CQGeoPositionInfoSourceS60* NewL(QObject* aParent);

    /**
     * Two-phased constructor.
     */
    static CQGeoPositionInfoSourceS60* NewLC(QObject* aParent);

    /**
     * returns the last known position
     */
    QGeoPositionInfo lastKnownPosition(bool aFromSatellitePositioningMethodsOnly = false) const;

    /**
     * returns the minimum update interval
     */
    int minimumUpdateInterval() const;

    /**
     * Sets the preferred PositioningMethod, if available
     * otherwise sets the default poistioning mmethod
     */
    void setPreferredPositioningMethods(PositioningMethods aMethods);

    /**
     * Sets the interval for the regular position notifications
     */
    void setUpdateInterval(int aMilliSec);

    /**
     * Returns the supported Positioning Methods
     */
    PositioningMethods supportedPositioningMethods() const {
        return mSupportedMethods;
    }


    //Applications using qt api's should avoid calling below methods as it is
    //used internally by CQGeoPositionInfoSourceS60 for maintaining different states

    /**
     * Notification methods from active object.
     * Notifies device status, position value, and status
     */
    void updateDeviceStatus(void) ;

    /**
     * Update the position info
     */
    void updatePosition(HPositionGenericInfo* aPosInfo, int aError);


    /**
     * Gets the handle of the PositionServer
     */
    RPositionServer& getPositionServer();

    /*
     * checks whether the object is valid
     */
    inline TBool isValid() {
        if (mDevStatusUpdateAO && mRegUpdateAO)
            return TRUE;
        else
            return FALSE;
    }

    inline TPositionModuleId getCurrentPositonModuleID() {
        return mCurrentModuleId;
    }

    inline TPositionModuleId getRequestUpdateModuleID() {
        return mReqModuleId;
    }

public slots :
    // for request update
    void requestUpdate(int timeout = 5000);

    // starts the regular updates
    virtual void startUpdates();

    // stops the regular updates
    virtual void stopUpdates();


private:

    /**
     * Constructor for performing 1st stage construction
     */
    CQGeoPositionInfoSourceS60(QObject* aParent = 0);

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();

    void updateStatus(TPositionModuleInfo &aModInfo, TInt aStatus);

    void updateAvailableTypes(void);

    //get the index of the module in the List array
    TInt checkModule(TPositionModuleId aId) ;//const;

    //get the index of the position module based on the preferred methods
    TInt getIndexPositionModule(TUint8 aBits, PositioningMethods aPosMethods = AllPositioningMethods) const;

    //get the more accuarte method with time to first fix < than timeout
    TInt getMoreAccurateMethod(TInt aTimeout, TUint8 aBits);

    void TPositionInfo2QGeoPositionInfo(HPositionGenericInfo *mPosInfo,
                                        QGeoPositionInfo& posUpdate);

protected:
    void connectNotify(const char *aSignal);

    void disconnectNotify(const char *aSignal);

private:
    /**
    *  current module ID
    */
    TPositionModuleId mCurrentModuleId;

    /**
    *prvmoduleID
    */
    TPositionModuleId mReqModuleId;

    /**
    * Active object for device status updates
    */
    CQMLBackendAO * mDevStatusUpdateAO;

    /**
    * Active object for requestUpdate
    */
    CQMLBackendAO * mReqUpdateAO;

    /**
    * Active object for regular updates.
    */
    CQMLBackendAO * mRegUpdateAO;

    /**
     * Positioner server
     */
    RPositionServer mPositionServer;

    /**
    *  list of supported position methods
    */
    CPosMethodInfo mList[MAX_SIZE];

    PositioningMethods mSupportedMethods;

    PositioningMethod mCurrentMethod;

    /**
     * maintaiss the size of thr CPosMethodInfo array
     */
    int mListSize;

    int mMinUpdateInterval;

    /*
     * query for the status
     */
    TPositionModuleStatusEvent  mStatusEvent;

    // mutex for making thread safe
    QMutex          m_mutex;
    QMutex          m_mutex_interval;

    // mutex for ReqUpdate Active Ojbect
    QMutex          m_mutex_ReqUpAO;

    // mutex for RegUpdate Active Ojbect
    QMutex          m_mutex_RegUpAO;

    /*
     * maintain the startUpdates status
     */
    TBool mStartUpdates;

    TBool mRegularUpdateTimedOut;

    /*
     * flags for the modules
     */
    TUint8 mModuleFlags;
};

QTM_END_NAMESPACE

#endif // CQGEOPOSITIONINFOSOURCES60_H
