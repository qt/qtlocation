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

#include "qgeotiledmaprectangleobjectinfo_p.h"

#include <QGraphicsPolygonItem>

#include "qgeotiledmapdata.h"
#include "qgeotiledmapdata_p.h"

#include "qgeoboundingbox.h"

#include "qgeomaprectangleobject.h"

QTM_BEGIN_NAMESPACE

QGeoTiledMapRectangleObjectInfo::QGeoTiledMapRectangleObjectInfo(QGeoTiledMapData *mapData, QGeoMapObject *mapObject)
    : QGeoTiledMapObjectInfo(mapData, mapObject)
{
    rectangle = static_cast<QGeoMapRectangleObject*>(mapObject);

    connect(rectangle,
            SIGNAL(topLeftChanged(QGeoCoordinate)),
            this,
            SLOT(topLeftChanged(QGeoCoordinate)));
    connect(rectangle,
            SIGNAL(bottomRightChanged(QGeoCoordinate)),
            this,
            SLOT(bottomRightChanged(QGeoCoordinate)));
    connect(rectangle,
            SIGNAL(penChanged(QPen)),
            this,
            SLOT(penChanged(QPen)));
    connect(rectangle,
            SIGNAL(brushChanged(QBrush)),
            this,
            SLOT(brushChanged(QBrush)));

    polygonItem = new QGraphicsPolygonItem();
    graphicsItem = polygonItem;

    topLeftChanged(rectangle->topLeft());
    bottomRightChanged(rectangle->bottomRight());
    penChanged(rectangle->pen());
    brushChanged(rectangle->brush());
}

QGeoTiledMapRectangleObjectInfo::~QGeoTiledMapRectangleObjectInfo() {}

void QGeoTiledMapRectangleObjectInfo::topLeftChanged(const QGeoCoordinate &/*topLeft*/)
{
    regenPolygon();
    updateItem();
}

void QGeoTiledMapRectangleObjectInfo::bottomRightChanged(const QGeoCoordinate &/*bottomRight*/)
{
    regenPolygon();
    updateItem();
}

void QGeoTiledMapRectangleObjectInfo::penChanged(const QPen &/*pen*/)
{
    polygonItem->setPen(rectangle->pen());
    updateItem();
}

void QGeoTiledMapRectangleObjectInfo::brushChanged(const QBrush &/*brush*/)
{
    polygonItem->setBrush(rectangle->brush());
    updateItem();
}

void QGeoTiledMapRectangleObjectInfo::regenPolygon()
{
    QPolygonF poly;

    if (!rectangle->bounds().isValid())
        return;

    const QGeoCoordinate tl = rectangle->bounds().topLeft();
    if (!tl.isValid())
        return;

    const QGeoCoordinate br = rectangle->bounds().bottomRight();
    if (!br.isValid())
        return;

    double left = tl.longitude() * 3600.0;
    double right = br.longitude() * 3600.0;
    double top = tl.latitude() * 3600.0;
    double bottom = br.latitude() * 3600.0;

    if (left > right)
        right += 360.0 * 3600.0;

    poly << QPointF(left, top);
    poly << QPointF(right, top);
    poly << QPointF(right, bottom);
    poly << QPointF(left, bottom);

    polygonItem->setPolygon(poly);
}

#include "moc_qgeotiledmaprectangleobjectinfo_p.cpp"

QTM_END_NAMESPACE


