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


#include "qmlbackendmonitorinfo_s60_p.h"

QTM_BEGIN_NAMESPACE

//singleton class object
CBackendMonitorInfo* CBackendMonitorInfo::iBackendMonitorInfo = NULL;

CBackendMonitorInfo* CBackendMonitorInfo::NewL()
{
    if (!iBackendMonitorInfo)
        iBackendMonitorInfo = new CBackendMonitorInfo;

    return iBackendMonitorInfo;
}

//returns the CMonitorTriggerInfo info based on the aTrigID
CMonitorTriggerInfo* CBackendMonitorInfo::getMonitorTriggerInfo(TLbtTriggerId aTrigID)
{
    CMonitorTriggerInfo* currentNode = iMonitorInfo;
    while (currentNode && (currentNode->iTriggerID != aTrigID)) {
        currentNode = currentNode->next;
    }
    return currentNode;
}

//returns the CMonitorTriggerInfo info based on the aParent,aType from the linked list
CMonitorTriggerInfo* CBackendMonitorInfo::getMonitorTriggerInfo(QGeoAreaMonitorS60* aParent, enTriggerType aType)
{
    CMonitorTriggerInfo* currentNode = iMonitorInfo;
    while (currentNode) {
        if ((currentNode->iParent == aParent) && (currentNode->iType == aType))
            break;

        currentNode = currentNode->next;
    }
    return currentNode;
}

//creates and adds a new CMonitorTriggerInfo object to the end of linked list
bool CBackendMonitorInfo::addMonitorTriggerInfo(QGeoAreaMonitorS60* aParent, TLbtTriggerId aTriggerID, enTriggerType aType)
{
    CMonitorTriggerInfo* currentNode = NULL;

    CMonitorTriggerInfo* temp = new CMonitorTriggerInfo;

    if (!temp) {
        return FALSE;
    }

    temp->iTriggerID = aTriggerID;
    temp->iParent = aParent;
    temp->iType = aType;
    temp->next = NULL;

    currentNode = iMonitorInfo;

    while (currentNode && (currentNode->next != NULL))
        currentNode = currentNode->next;

    if (!currentNode) {
        iMonitorInfo = temp;
    } else {
        currentNode->next = temp;
    }

    return TRUE;
}

//deletes the node corresponding to aTrigID
void CBackendMonitorInfo::removeMonitorTriggerInfo(TLbtTriggerId aTrigID)
{
    CMonitorTriggerInfo* currentNode = NULL;

    CMonitorTriggerInfo* prevNode = NULL;

    currentNode = prevNode = iMonitorInfo;

    while (currentNode && (currentNode->iTriggerID != aTrigID)) {
        prevNode = currentNode;
        currentNode = currentNode->next;
    }

    if (currentNode == NULL)
        return;

    else if (currentNode == iMonitorInfo) {
        iMonitorInfo = currentNode->next;
    }

    else {
        prevNode->next = currentNode->next;
    }

    delete currentNode;
}

//deletes all the nodes in the linked list
CBackendMonitorInfo::~CBackendMonitorInfo()
{
    CMonitorTriggerInfo* currentNode = NULL;
    CMonitorTriggerInfo* prevNode = NULL;

    prevNode = iMonitorInfo;

    while (prevNode) {
        currentNode = prevNode;
        prevNode = prevNode->next;
        delete currentNode;
    }

    iMonitorInfo = NULL;
    iBackendMonitorInfo = NULL;
}

QTM_END_NAMESPACE
