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

#include "qgeotiledmapcircleobjectinfo_p.h"

#include "qgeotiledmapobjectinfo_p.h"
#include "qgeotiledmapdata.h"
#include "qgeotiledmapdata_p.h"

#include "qgeomapcircleobject.h"

#include <QGraphicsItem>
#include <QGraphicsEllipseItem>

QTM_BEGIN_NAMESPACE

QGeoTiledMapCircleObjectInfo::QGeoTiledMapCircleObjectInfo(QGeoTiledMapData *mapData, QGeoMapObject *mapObject)
    : QGeoTiledMapObjectInfo(mapData, mapObject)
{
    circle = static_cast<QGeoMapCircleObject*>(mapObject);

    connect(circle,
            SIGNAL(radiusChanged(qreal)),
            this,
            SLOT(radiusChanged(qreal)));
    connect(circle,
            SIGNAL(penChanged(QPen)),
            this,
            SLOT(penChanged(QPen)));
    connect(circle,
            SIGNAL(brushChanged(QBrush)),
            this,
            SLOT(brushChanged(QBrush)));

    ellipseItem = new QGraphicsEllipseItem;
    ellipseItem->setPos(0, 0);

    graphicsItem = ellipseItem;

    radiusChanged(circle->radius());
    brushChanged(circle->brush());
    penChanged(circle->pen());
}

QGeoTiledMapCircleObjectInfo::~QGeoTiledMapCircleObjectInfo() {}

void QGeoTiledMapCircleObjectInfo::radiusChanged(qreal /*radius*/)
{
    qreal radius = circle->radius();
    ellipseItem->setRect(-1*radius, -1*radius, 2*radius, 2*radius);
    updateItem();
}

void QGeoTiledMapCircleObjectInfo::penChanged(const QPen &pen)
{
    ellipseItem->setPen(pen);
    updateItem();
}

void QGeoTiledMapCircleObjectInfo::brushChanged(const QBrush &brush)
{
    ellipseItem->setBrush(brush);
    updateItem();
}

#include "moc_qgeotiledmapcircleobjectinfo_p.cpp"

QTM_END_NAMESPACE
