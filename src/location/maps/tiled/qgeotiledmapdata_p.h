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

#ifndef QGEOTILEDMAPDATA_P_H
#define QGEOTILEDMAPDATA_P_H

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

#include "qgeomapdata_p.h"
#include "qgeomapobject.h"
#include "qgeomapobject_p.h"
#include "projwrapper_p.h"

#include <QRectF>
#include <QHash>
#include <QCache>
#include <QPair>
#include <QList>

class QGraphicsScene;

QTM_BEGIN_NAMESPACE

class QGeoTiledMapData;
class QGeoTiledMapRequest;
class QGeoTiledMapReply;
class QGeoTiledMapObjectInfo;
class QGeoMapObjectEngine;

class QGeoTiledMapDataPrivate : public QGeoMapDataPrivate
{
public:
    QGeoTiledMapDataPrivate(QGeoTiledMapData *parent, QGeoMappingManagerEngine *engine);
    ~QGeoTiledMapDataPrivate();

    void updateMapImage();
    void clearRequests();

    QPointF windowOffset() const;

    void paintMap(QPainter *painter, const QStyleOptionGraphicsItem *option);
    void paintObjects(QPainter *painter, const QStyleOptionGraphicsItem *option);

    void cleanupCaches();

    bool intersects(QGeoMapObject *mapObject, const QRectF &rect);

    QRect screenRectForZoomFactor(int zoomFactor);
    void updateScreenRect();

    bool containedInScreen(const QPoint &point) const;
    bool intersectsScreen(const QRect &rect) const;
    QList<QPair<QRect, QRect> > intersectedScreen(const QRect &rect, bool translateToScreen = true) const;

    virtual QPoint screenPositionToWorldReferencePosition(const QPointF &screenPosition) const;
    virtual QPoint coordinateToWorldReferencePosition(double lon, double lat) const;
    virtual QPointF coordinateToScreenPosition(double lon, double lat) const;

    virtual void addObject(QGeoMapObject *object);
    virtual void removeObject(QGeoMapObject *object);

    virtual void update(QObject *object);

    QGeoMapObjectEngine *oe;

    int zoomFactor;

    QPoint worldReferenceViewportCenter;
    QSize worldReferenceSize;
    QRect worldReferenceViewportRect;

    QRect worldReferenceViewportRectLeft;
    QRect worldReferenceViewportRectRight;

    QSet<QRect> requestRects;
    QSet<QRect> replyRects;

    QList<QGeoTiledMapRequest> requests;
    QSet<QGeoTiledMapReply*> replies;

    QCache<QGeoTiledMapRequest, QImage> cache;
    QCache<QGeoTiledMapRequest, QPixmap> zoomCache;

    ProjCoordinateSystem spherical;
    ProjCoordinateSystem wgs84;

    Q_DECLARE_PUBLIC(QGeoTiledMapData)
private:
    Q_DISABLE_COPY(QGeoTiledMapDataPrivate)
};

class QGeoTileIterator
{
public:
    QGeoTileIterator(const QGeoTiledMapDataPrivate *mapData);
    QGeoTileIterator(QGraphicsGeoMap::ConnectivityMode connectivityMode,
                     QGraphicsGeoMap::MapType mapType,
                     const QRect &screenRect,
                     const QSize &tileSize,
                     int zoomLevel);

    bool hasNext();
    QGeoTiledMapRequest next();

private:
    bool atEnd;
    int row;
    int col;
    int width;
    QRect screenRect;
    QSize tileSize;
    QGraphicsGeoMap::MapType mapType;
    QGraphicsGeoMap::ConnectivityMode connectivityMode;
    int zoomLevel;
    QPoint currTopLeft;
    QRect tileRect;
};

QTM_END_NAMESPACE

#endif
