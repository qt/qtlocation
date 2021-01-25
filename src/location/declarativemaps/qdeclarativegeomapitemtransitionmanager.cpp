/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include "qdeclarativegeomapitemtransitionmanager_p.h"
#include "qdeclarativegeomapitemview_p.h"

QT_BEGIN_NAMESPACE

QDeclarativeGeoMapItemTransitionManager::QDeclarativeGeoMapItemTransitionManager(QObject *mapItem)
    : QQuickTransitionManager(), m_mapItem(mapItem)
{
}

void QDeclarativeGeoMapItemTransitionManager::transitionEnter()
{
    if (m_transitionState == ExitTransition)
        cancel();

    if (!prepareEnterTransition())
        return;

    if (m_view && m_view->m_enter)
        transition(enterActions, m_view->m_enter, m_mapItem);
    else
        finished();
}

void QDeclarativeGeoMapItemTransitionManager::transitionExit()
{
    if (!prepareExitTransition())
        return;

    if (m_view && m_view->m_exit)
        transition(exitActions, m_view->m_exit, m_mapItem);
    else
        finished();
}

void QDeclarativeGeoMapItemTransitionManager::finished()
{
    if (m_transitionState == EnterTransition)
        finalizeEnterTransition();
    else if (m_transitionState == ExitTransition)
        finalizeExitTransition();
}


QT_END_NAMESPACE
