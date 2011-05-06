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

#ifndef QGEOTILEDMAPDATA_H
#define QGEOTILEDMAPDATA_H

#include "qgeomapdata.h"
#include "qgeotiledmapreply.h"

#include <QRectF>

QTM_BEGIN_NAMESPACE

class QGeoTiledMapDataPrivate;

class Q_LOCATION_EXPORT QGeoTiledMapData : public QGeoMapData
{
    Q_OBJECT

public:
    QGeoTiledMapData(QGeoMappingManagerEngine *engine);
    virtual ~QGeoTiledMapData();

    void setWindowSize(const QSizeF &size);

    void setZoomLevel(qreal zoomLevel);

    void pan(int dx, int dy);

    void setCenter(const QGeoCoordinate &center);
    QGeoCoordinate center() const;

    void setMapType(QGraphicsGeoMap::MapType mapType);

    void setConnectivityMode(QGraphicsGeoMap::ConnectivityMode connectivityMode);

    virtual QGeoBoundingBox viewport() const;
    virtual void fitInViewport(const QGeoBoundingBox &bounds, bool preserveViewportCenter = false);

    QList<QGeoMapObject*> mapObjectsAtScreenPosition(const QPointF &screenPosition) const;
    QList<QGeoMapObject*> mapObjectsInScreenRect(const QRectF &screenRect) const;

    QPointF coordinateToScreenPosition(const QGeoCoordinate &coordinate) const;
    QGeoCoordinate screenPositionToCoordinate(const QPointF &screenPosition) const;

    virtual QPoint coordinateToWorldReferencePosition(const QGeoCoordinate &coordinate) const;
    virtual QGeoCoordinate worldReferencePositionToCoordinate(const QPoint &pixel) const;

    QPoint worldReferenceViewportCenter() const;
    QSize worldReferenceSize() const;
    QRect worldReferenceViewportRect() const;
    int zoomFactor() const;

    void triggerUpdateMapDisplay(const QRectF &target = QRectF());

protected:
    void paintMap(QPainter *painter, const QStyleOptionGraphicsItem *option);
    void paintObjects(QPainter *painter, const QStyleOptionGraphicsItem *option);

    QGeoMapObjectInfo* createMapObjectInfo(QGeoMapObject *mapObject);

private Q_SLOTS:
    void processRequests();
    void tileFinished();
    void replyFinished(QGeoTiledMapReply *reply);
    void tileError(QGeoTiledMapReply::Error error, QString errorString);

private:
    Q_DECLARE_PRIVATE(QGeoTiledMapData)
    Q_DISABLE_COPY(QGeoTiledMapData)
    friend class QGeoTiledMappingManagerEngine;
    friend class QGeoTiledMapObjectInfo;
};

QTM_END_NAMESPACE

#endif
