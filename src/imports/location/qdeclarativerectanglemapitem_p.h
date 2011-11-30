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

#ifndef QDECLARATIVERECTANGLEMAPITEM_H_
#define QDECLARATIVERECTANGLEMAPITEM_H_

#include "qdeclarativegeomapitembase_p.h"
#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeomap_p.h"
#include <QPen>
#include <QBrush>

QT_BEGIN_NAMESPACE

class RectangleMapPaintedItem;
class QDeclarativeGeoMapQuickItem;

class QDeclarativeRectangleMapItem: public QDeclarativeGeoMapItemBase
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeCoordinate* topLeft READ topLeft WRITE setTopLeft NOTIFY topLeftChanged)
    Q_PROPERTY(QDeclarativeCoordinate* bottomRight READ bottomRight WRITE setBottomRight NOTIFY bottomRightChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    QDeclarativeRectangleMapItem(QQuickItem *parent = 0);
    ~QDeclarativeRectangleMapItem();

    QDeclarativeCoordinate* topLeft();
    void setTopLeft(QDeclarativeCoordinate *center);

    QDeclarativeCoordinate* bottomRight();
    void setBottomRight(QDeclarativeCoordinate *center);

    QColor color() const;
    void setColor(const QColor &color);

Q_SIGNALS:
    void topLeftChanged(const QDeclarativeCoordinate *topLeft);
    void bottomRightChanged(const QDeclarativeCoordinate *bottomRight);
    void colorChanged(const QColor &color);

protected Q_SLOTS:
    void update();

private Q_SLOTS:
    // map size changed
    void handleCameraDataChanged(const CameraData& cameraData);
    void handleTopLeftCoordinateChanged();
    void handleBottomRightCoordinateChanged();

private:
    QDeclarativeGeoMapQuickItem *quickItem_;
    RectangleMapPaintedItem *rectangleMapPaintedItem_;
    QDeclarativeCoordinate* topLeft_;
    QDeclarativeCoordinate* bottomRight_;
    QColor color_;
};

//////////////////////////////////////////////////////////////////////

class RectangleMapPaintedItem: public QQuickPaintedItem
{
    Q_OBJECT

public:
    RectangleMapPaintedItem(QQuickItem *parent = 0);
    ~RectangleMapPaintedItem();

    void setMap(Map* map);

    void setZoomLevel(qreal zoomLevel);
    qreal zoomLevel() const;

    QGeoCoordinate topLeft() const;
    void setTopLeft(const QGeoCoordinate &topLeft);

    QGeoCoordinate bottomRight() const;
    void setBottomRight(const QGeoCoordinate &bottomRight);

    void paint(QPainter *painter);

    QPen pen() const;
    void setPen(const QPen &pen);

    QBrush brush() const;
    void setBrush(const QBrush &brush);

    QGeoCoordinate quickItemCoordinate() const;
    QPointF quickItemAnchorPoint() const;

private:
    void updateGeometry();

    Map *map_;
    qreal zoomLevel_;
    QGeoCoordinate topLeftCoord_;
    QGeoCoordinate bottomRightCoord_;
    QGeoCoordinate quickItemCoordinate_;
    QPointF quickItemAnchorPoint_;
    QPen pen_;
    QBrush brush_;
    QRectF rect_;
    bool initialized_;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeRectangleMapItem));

#endif /* QDECLARATIVERECTANGLEMAPITEM_H_ */
