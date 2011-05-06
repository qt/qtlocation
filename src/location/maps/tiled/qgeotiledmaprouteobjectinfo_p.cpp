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

#include "qgeotiledmaprouteobjectinfo_p.h"

#include "qgeotiledmapdata.h"
#include "qgeotiledmapdata_p.h"

#include "qgeomaprouteobject.h"

#include "qgeoroutesegment.h"

#include <QPointF>

QTM_BEGIN_NAMESPACE

QGeoTiledMapRouteObjectInfo::QGeoTiledMapRouteObjectInfo(QGeoTiledMapData *mapData, QGeoMapObject *mapObject)
    : QGeoTiledMapObjectInfo(mapData, mapObject)
{
    route = static_cast<QGeoMapRouteObject*>(mapObject);

    connect(route,
            SIGNAL(routeChanged(QGeoRoute)),
            this,
            SLOT(routeChanged(QGeoRoute)));
    connect(route,
            SIGNAL(penChanged(QPen)),
            this,
            SLOT(penChanged(QPen)));
    connect(route,
            SIGNAL(detailLevelChanged(quint32)),
            this,
            SLOT(detailLevelChanged(quint32)));

    pathItem = new QGraphicsPathItem();
    graphicsItem = pathItem;

    routeChanged(route->route());
    penChanged(route->pen());
    routeChanged(route->route());
}

QGeoTiledMapRouteObjectInfo::~QGeoTiledMapRouteObjectInfo() {}

void QGeoTiledMapRouteObjectInfo::routeChanged(const QGeoRoute &/*route*/)
{
    regenPath();
    updateItem();
}

void QGeoTiledMapRouteObjectInfo::penChanged(const QPen &/*pen*/)
{
    pathItem->setPen(route->pen());
    updateItem();
}

void QGeoTiledMapRouteObjectInfo::detailLevelChanged(quint32 /*detailLevel*/)
{
    updateItem();
}

void QGeoTiledMapRouteObjectInfo::regenPath()
{
    QList<QGeoCoordinate> path;

    QGeoRouteSegment segment = route->route().firstRouteSegment();
    while (segment.isValid()) {
        path.append(segment.path());
        segment = segment.nextRouteSegment();
    }

    QPainterPath pth;

    if (path.size() > 0) {
        double oldx = 0.0;
        double oldy = 0.0;

        for (int i = 0; i < path.size(); ++i) {
            double x = path.at(i).longitude() * 3600.0;
            double y = path.at(i).latitude() * 3600.0;
            if (i == 0) {
                pth.moveTo(x, y);
            } else {
                if (qAbs(x - oldx) > 180.0 * 3600.0) {
                    if (x > oldx) {
                        x -= 360.0 * 3600.0;
                    } else if (x < oldx) {
                        x += 360.0 * 3600.0;
                    }
                }

                pth.lineTo(x, y);
            }

            oldx = x;
            oldy = y;
        }
    }

    pathItem->setPath(pth);
}

#include "moc_qgeotiledmaprouteobjectinfo_p.cpp"

QTM_END_NAMESPACE

