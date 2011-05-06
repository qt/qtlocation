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

#ifndef QMLBACKENDMONITORINFO_H_
#define QMLBACKENDMONITORINFO_H_

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

QTM_BEGIN_NAMESPACE

//linked list information structure
struct CMonitorTriggerInfo {
    QGeoAreaMonitorS60* iParent;

    TLbtTriggerId iTriggerID;

    enum  enTriggerType iType;

    CMonitorTriggerInfo* next;
};


class CBackendMonitorInfo   :  public CBase
{
public :

    static CBackendMonitorInfo  *NewL();

    //return CMonitorTriggerInfo from TriggerID
    CMonitorTriggerInfo* getMonitorTriggerInfo(TLbtTriggerId aTrigID);

    //return CMonitorTriggerInfo from the QGeoAreaMonitorS60
    CMonitorTriggerInfo* getMonitorTriggerInfo(QGeoAreaMonitorS60* aParent, enTriggerType aType);

    //return the header of the linked list
    inline CMonitorTriggerInfo* getMonitorTriggerInfo() {
        return iMonitorInfo;
    }

    //add the entry and exit trigger info to the list
    bool addMonitorTriggerInfo(QGeoAreaMonitorS60* aParent, TLbtTriggerId aTriggerID, enTriggerType aType);

    //remove the CMonitorTriggerInfo corresponding to the TriggerID
    void removeMonitorTriggerInfo(TLbtTriggerId aTrigID);


    ~CBackendMonitorInfo();
private :

    CBackendMonitorInfo() : iMonitorInfo(NULL) { }

    CMonitorTriggerInfo*  iMonitorInfo;

    static CBackendMonitorInfo* iBackendMonitorInfo;
};

QTM_END_NAMESPACE

#endif /* QMLBACKENDMONITORINFO_H_ */
