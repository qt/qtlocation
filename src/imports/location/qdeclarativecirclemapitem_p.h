/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVECIRCLEMAPITEM_H
#define QDECLARATIVECIRCLEMAPITEM_H

#include "qdeclarativegeomapitembase_p.h"
#include "qdeclarativepolylinemapitem_p.h"
#include "qdeclarativepolygonmapitem_p.h"
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>

QT_BEGIN_NAMESPACE

class QDeclarativeGeoMapQuickItem;

class QGeoMapCircleGeometry : public QGeoMapPolygonGeometry
{
    Q_OBJECT

public:
    explicit QGeoMapCircleGeometry(QObject *parent = 0);

    void updateScreenPointsInvert(const QGeoMap &map);


};

class QDeclarativeCircleMapItem : public QDeclarativeGeoMapItemBase
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeCoordinate *center READ center WRITE setCenter NOTIFY centerChanged)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QDeclarativeMapLineProperties *border READ border CONSTANT)

public:
    explicit QDeclarativeCircleMapItem(QQuickItem *parent = 0);
    ~QDeclarativeCircleMapItem();

    virtual void setMap(QDeclarativeGeoMap *quickMap, QGeoMap *map);
    virtual QSGNode *updateMapItemPaintNode(QSGNode *, UpdatePaintNodeData *);

    QDeclarativeCoordinate *center();
    void setCenter(QDeclarativeCoordinate *center);

    qreal radius() const;
    void setRadius(qreal radius);

    QColor color() const;
    void setColor(const QColor &color);

    QDeclarativeMapLineProperties *border();

    void dragStarted();
    void dragEnded();
    bool contains(const QPointF &point) const;

Q_SIGNALS:
    void centerChanged(const QDeclarativeCoordinate *center);
    void radiusChanged(qreal radius);
    void colorChanged(const QColor &color);

protected Q_SLOTS:
    virtual void updateMapItem();
    void updateMapItemAssumeDirty();
    void afterViewportChanged(const QGeoMapViewportChangeEvent &event);

private:
    bool preserveCircleGeometry(QList<QGeoCoordinate> &path, const QGeoCoordinate &center,
                                qreal distance, QGeoCoordinate &leftBoundCoord);
    void updateCirclePathForRendering(QList<QGeoCoordinate> &path, const QGeoCoordinate &center,
                                      qreal distance);

private:
    //TODO: pimpl
    //TODO: this should be in base class done in
    QDeclarativeCoordinate internalCoordinate_;
    QDeclarativeCoordinate *center_;
    QDeclarativeMapLineProperties border_;
    QColor color_;
    qreal radius_;
    QList<QGeoCoordinate> circlePath_;
    bool dirtyMaterial_;
    QGeoMapCircleGeometry geometry_;
    QGeoMapPolylineGeometry borderGeometry_;
};

//////////////////////////////////////////////////////////////////////

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeCircleMapItem));

#endif /* QDECLARATIVECIRCLEMAPITEM_H */
