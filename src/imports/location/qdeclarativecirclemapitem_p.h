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

#ifndef QDECLARATIVECIRCLEMAPITEM_H
#define QDECLARATIVECIRCLEMAPITEM_H

#include "qdeclarativegeomapitembase_p.h"
#include <QPen>
#include <QBrush>

QT_BEGIN_NAMESPACE

class CircleMapPaintedItem;

class QDeclarativeCircleMapItem : public QDeclarativeGeoMapItemBase
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeCoordinate* center READ center WRITE setCenter NOTIFY centerChanged)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    QDeclarativeCircleMapItem(QQuickItem *parent = 0);
    ~QDeclarativeCircleMapItem();

    QDeclarativeCoordinate* center();
    void setCenter(QDeclarativeCoordinate* center);

    qreal radius() const;
    void setRadius(qreal radius);

    QColor color() const;
    void setColor(const QColor &color);

    void dragStarted();
    void dragEnded();
    bool contains(QPointF point);

Q_SIGNALS:
    void centerChanged(const QDeclarativeCoordinate *center);
    void radiusChanged(qreal radius);
    void colorChanged(const QColor &color);

protected Q_SLOTS:
    void update();
    // from qquickitem
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

private Q_SLOTS:
    void handleCameraDataChanged(const CameraData& cameraData);
    void handleCenterCoordinateChanged();

private:
    QDeclarativeCoordinate internalCoordinate_;
    QDeclarativeCoordinate *center_;
    CircleMapPaintedItem *circleItem_;
    QColor color_;
    bool inUpdate_;
    bool zoomLevel_;
    bool dragActive_;
};

//////////////////////////////////////////////////////////////////////

class CircleMapPaintedItem: public QQuickPaintedItem
{

public:
    CircleMapPaintedItem(QQuickItem *parent = 0);
    ~CircleMapPaintedItem();

    void setMap(Map* map);
    Map* map();
    void setZoomLevel(qreal zoomLevel);
    qreal zoomLevel() const;

    void setCenter(const QGeoCoordinate &center);
    const QGeoCoordinate& center() const;

    void setRadius(qreal radius);
    qreal radius() const;

    void paint(QPainter *painter);

    QPen pen() const;
    void setPen(const QPen &pen);

    QBrush brush() const;
    void setBrush(const QBrush &brush);

    bool contains(QPointF point);

private:
    void updateGeometry();
    void calcualtePeripheralPoints(QList<QGeoCoordinate>& path, const QGeoCoordinate& center, qreal distance, int steps) const;

    Map *map_;
    qreal zoomLevel_;
    QGeoCoordinate centerCoord_;
    qreal radius_;
    QPen pen_;
    QBrush brush_;
    QPolygonF polygon_;
    QList<QGeoCoordinate> path_;
    bool initialized_;
};

QT_END_NAMESPACE

#endif /* QDECLARATIVECIRCLEMAPITEM_H */
