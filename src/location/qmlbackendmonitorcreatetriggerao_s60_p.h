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


#ifndef QMLBACKENDMONITORCREATETRIGGER_H_
#define QMLBACKENDMONITORCREATETRIGGER_H_

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

#include "qmlbackendmonitorao_s60_p.h"
#include "qmlbackendmonitorinfo_s60_p.h"

#include <lbtsessiontrigger.h>
#include <lbtgeocircle.h>
#include <lbttriggerconditionarea.h>
#include <lbttriggerentry.h>

QTM_BEGIN_NAMESPACE

class QGeoAreaMonitorS60;

class QMLBackendMonitorCreateTriggerAO : public CActive
{
public :
    static QMLBackendMonitorCreateTriggerAO* NewL(QGeoAreaMonitorS60* aParent , RLbtServer &aLbtServer);

    bool  InitializeTrigger(QGeoAreaMonitorS60* aParent, enTriggerType aType, TCoordinate& aCoordinate, qreal& aRadius);

    bool isTriggerInitialized(QGeoAreaMonitorS60* aParent, enTriggerType aType);

    void  DoCancel();

    void RunL();

    void SetTriggerState(QGeoAreaMonitorS60* aParent, enTriggerType aType, bool aStatus);

    TInt getRadius(qreal& radius);

    ~QMLBackendMonitorCreateTriggerAO();
private :
    QMLBackendMonitorCreateTriggerAO();

    static QMLBackendMonitorCreateTriggerAO* NewLC(QGeoAreaMonitorS60* aParent , RLbtServer &aLbt);

    void ConstructL(RLbtServer &aLbtServ);

    bool isValid() {
        return subsessionCreated && (iTriggerMonitorInfo != NULL);
    }
private:

    CBackendMonitorInfo*  iTriggerMonitorInfo;  //single instance of the CBackendMonitorInfo object

    CActiveSchedulerWait* iActiveSchedulerwait;

    bool subsessionCreated; //check for the successful creation of the subsession

    bool iTriggerCreation;

    RLbt iLbt;  //subsession

};

QTM_END_NAMESPACE

#endif /* QMLBACKENDMONITORCREATETRIGGER_H_ */
