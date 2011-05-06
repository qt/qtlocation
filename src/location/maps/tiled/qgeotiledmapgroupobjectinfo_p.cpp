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

#include "qgeotiledmapgroupobjectinfo_p.h"

#include "qgeotiledmapobjectinfo_p.h"
#include "qgeotiledmapdata.h"
#include "qgeotiledmapdata_p.h"

#include "qgeomapobject.h"
#include "qgeomapgroupobject.h"

#include "qgeomapobjectengine_p.h"

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsPathItem>

QTM_BEGIN_NAMESPACE

QGeoTiledMapGroupObjectInfo::QGeoTiledMapGroupObjectInfo(QGeoTiledMapData *mapData, QGeoMapObject *mapObject)
    : QGeoTiledMapObjectInfo(mapData, mapObject)
{

    group = static_cast<QGeoMapGroupObject*>(mapObject);

    connect(group,
            SIGNAL(childAdded(QGeoMapObject*)),
            this,
            SLOT(childAdded(QGeoMapObject*)));
    connect(group,
            SIGNAL(childUpdated(QGeoMapObject*)),
            this,
            SLOT(childUpdated(QGeoMapObject*)));
    connect(group,
            SIGNAL(childRemoved(QGeoMapObject*)),
            this,
            SLOT(childRemoved(QGeoMapObject*)));

    pathItem = new QGraphicsPathItem();
    graphicsItem = pathItem;
    pathItem->setPos(0.0, 0.0);

    QList<QGeoMapObject*> objects = group->childObjects();

    foreach(QGeoMapObject * object, objects) {
        QGeoTiledMapObjectInfo* info = static_cast<QGeoTiledMapObjectInfo*>(object->info());

        if (info)
            info->graphicsItem->setParentItem(graphicsItem);
    }

    updateItem();
}

QGeoTiledMapGroupObjectInfo::~QGeoTiledMapGroupObjectInfo()
{
}

void QGeoTiledMapGroupObjectInfo::childAdded(QGeoMapObject *childObject)
{
    if (!childObject)
        return;

    QGeoTiledMapObjectInfo* info = static_cast<QGeoTiledMapObjectInfo*>(childObject->info());
    if (info && info->graphicsItem) {
        // the child's z value will get updated in QGeoTiledMapGroupObjectInfo::childUpdated
        // we do this in order to keep the same order of operations that we had previously
        childObject->disconnect(childObject, SIGNAL(zValueChanged(int)), info, SLOT(zValueChanged(int)));
        info->graphicsItem->setParentItem(graphicsItem);
        tiledMapDataPrivate->update(mapObject());
        //tiledMapDataPrivate->update(childObject);
    }
}

void QGeoTiledMapGroupObjectInfo::childUpdated(QGeoMapObject *childObject)
{
    if (!childObject)
        return;

    QGeoTiledMapObjectInfo* info = static_cast<QGeoTiledMapObjectInfo*>(childObject->info());
    if (info && info->graphicsItem) {
        //info->graphicsItem->setParentItem(graphicsItem);
        tiledMapDataPrivate->update(mapObject());
        info->zValueChanged(childObject->zValue());
    }
}

void QGeoTiledMapGroupObjectInfo::childRemoved(QGeoMapObject *childObject)
{
    if (childObject && tiledMapDataPrivate->oe) {
        tiledMapDataPrivate->oe->removeObject(childObject);
        updateItem();
    }
}

#include "moc_qgeotiledmapgroupobjectinfo_p.cpp"

QTM_END_NAMESPACE
