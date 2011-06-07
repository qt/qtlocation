/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#include "qgeotiledmapcustomobjectinfo_p.h"

#include "qgeotiledmapdata.h"
#include "qgeotiledmapdata_p.h"

#include "qgeomapcustomobject.h"

QTM_BEGIN_NAMESPACE

QGeoTiledMapCustomObjectInfo::QGeoTiledMapCustomObjectInfo(QGeoTiledMapData *mapData, QGeoMapObject *mapObject)
    : QGeoTiledMapObjectInfo(mapData, mapObject)
{
    custom = static_cast<QGeoMapCustomObject*>(mapObject);

    connect(custom,
            SIGNAL(triggerUpdate()),
            this,
            SLOT(updateTriggered()));
    connect(custom,
            SIGNAL(graphicsItemChanged(QGraphicsItem*)),
            this,
            SLOT(graphicsItemChanged(QGraphicsItem*)));
    connect(custom,
            SIGNAL(offsetChanged(QPoint)),
            this,
            SLOT(offsetChanged(QPoint)));

    graphicsItem = 0;

    graphicsItemChanged(this->custom->graphicsItem());
    offsetChanged(this->custom->offset());
}

QGeoTiledMapCustomObjectInfo::~QGeoTiledMapCustomObjectInfo()
{
    //custom->setGraphicsItem(0);
}

void QGeoTiledMapCustomObjectInfo::updateTriggered()
{
    updateItem();
}

void QGeoTiledMapCustomObjectInfo::graphicsItemChanged(QGraphicsItem * /*graphicsItem*/)
{
    graphicsItem = this->custom->graphicsItem();
    updateItem();
}

void QGeoTiledMapCustomObjectInfo::offsetChanged(const QPoint &/*offset*/)
{
    QPoint offset = custom->offset();
    graphicsItem->translate(offset.x(), offset.y());
    updateItem();
}

#include "moc_qgeotiledmapcustomobjectinfo_p.cpp"

QTM_END_NAMESPACE

