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
#include "qdeclarativepolylinemapitem_p.h"
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>

QT_BEGIN_NAMESPACE

class MapRectangleNode;

class QDeclarativeRectangleMapItem: public QDeclarativeGeoMapItemBase
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeCoordinate* topLeft READ topLeft WRITE setTopLeft NOTIFY topLeftChanged)
    Q_PROPERTY(QDeclarativeCoordinate* bottomRight READ bottomRight WRITE setBottomRight NOTIFY bottomRightChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QDeclarativeMapLineProperties *border READ border)

public:
    QDeclarativeRectangleMapItem(QQuickItem *parent = 0);
    ~QDeclarativeRectangleMapItem();

    virtual void setMap(QDeclarativeGeoMap* quickMap, QGeoMap *map);
    //from QuickItem
    virtual QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);

    QDeclarativeCoordinate* topLeft();
    void setTopLeft(QDeclarativeCoordinate *center);

    QDeclarativeCoordinate* bottomRight();
    void setBottomRight(QDeclarativeCoordinate *center);

    QColor color() const;
    void setColor(const QColor &color);

    QDeclarativeMapLineProperties *border();

    void dragEnded();
    bool contains(QPointF point);

Q_SIGNALS:
    void topLeftChanged(const QDeclarativeCoordinate *topLeft);
    void bottomRightChanged(const QDeclarativeCoordinate *bottomRight);
    void colorChanged(const QColor &color);

protected Q_SLOTS:
    virtual void updateMapItem();
    void updateMapItemAssumeDirty();

private Q_SLOTS:
    // map size changed
    void handleCameraDataChanged(const QGeoCameraData& cameraData);

private:
    QDeclarativeCoordinate* topLeft_;
    QDeclarativeCoordinate* bottomRight_;
    QDeclarativeCoordinate internalTopLeft_;
    QDeclarativeCoordinate internalBottomRight_;
    QDeclarativeMapLineProperties border_;
    QColor color_;
    qreal zoomLevel_;
    QRectF rectangle_;
    QPolygonF borderPoly_;
    bool dirtyGeometry_;
    bool dirtyMaterial_;
};

//////////////////////////////////////////////////////////////////////

class MapRectangleNode: public QSGGeometryNode
{

public:
    MapRectangleNode();
    ~MapRectangleNode();

    void update(const QColor& fillColor, const QRectF& shape,
                const QPolygonF& borderPoly, const QColor& borderColor,
                qreal borderWidth);

private:
    QSGFlatColorMaterial fillMaterial_;
    QSGGeometry geometry_;
    MapPolylineNode *border_;

};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeRectangleMapItem));

#endif /* QDECLARATIVERECTANGLEMAPITEM_H_ */
