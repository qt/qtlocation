/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativegeomapitembase_p.h"

QT_BEGIN_NAMESPACE

QDeclarativeGeoMapItemBase::QDeclarativeGeoMapItemBase(QQuickItem *parent)
    : QQuickItem(parent),
      quickMap_(0),
      map_(0)
{
    setParentItem(parent);
    setFlag(ItemHasContents, false);
    setAcceptHoverEvents(false);
}

QDeclarativeGeoMapItemBase::~QDeclarativeGeoMapItemBase()
{
}

void QDeclarativeGeoMapItemBase::componentComplete()
{
    componentComplete_ = true;
}

bool QDeclarativeGeoMapItemBase::contains(QPointF point)
{
    Q_UNUSED(point);
    return true;
}

void QDeclarativeGeoMapItemBase::dragStarted()
{
}

void QDeclarativeGeoMapItemBase::dragEnded()
{
}

void QDeclarativeGeoMapItemBase::setMap(QDeclarativeGeoMap* quickMap, Map *map)
{
    QLOC_TRACE2(quickMap, quickMap_);
    if (quickMap == quickMap_)
        return;
    if (quickMap && quickMap_)
        return; // don't allow association to more than one map
    quickMap_ = quickMap;
    map_ = map;
    this->update();
}

void QDeclarativeGeoMapItemBase::update() {}

QDeclarativeGeoMap* QDeclarativeGeoMapItemBase::quickMap()
{
    return quickMap_;
}

Map* QDeclarativeGeoMapItemBase::map()
{
    return map_;
}

#include "moc_qdeclarativegeomapitembase_p.cpp"

QT_END_NAMESPACE
