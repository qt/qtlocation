/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEPOLYLINEMAPITEM
#define QDECLARATIVEPOLYLINEMAPITEM

#include "qdeclarativegeomapitembase_p.h"
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>

QT_BEGIN_NAMESPACE

class MapPolylineNode;

class QDeclarativeMapLineProperties : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    explicit QDeclarativeMapLineProperties(QObject *parent = 0);

    QColor color() const;
    void setColor(const QColor &color);

    qreal width() const;
    void setWidth(qreal width);

Q_SIGNALS:
    void widthChanged(qreal width);
    void colorChanged(const QColor &color);

private:
    qreal width_;
    QColor color_;
};

class QDeclarativePolylineMapItem : public QDeclarativeGeoMapItemBase
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeCoordinate> path READ declarativePath NOTIFY pathChanged)
    Q_PROPERTY(QDeclarativeMapLineProperties *line READ line CONSTANT)

public:
    QDeclarativePolylineMapItem(QQuickItem *parent = 0);
    ~QDeclarativePolylineMapItem();

    static void updatePolyline(QPolygonF& points,const QGeoMap& map, const QList<QGeoCoordinate> &path, qreal& w, qreal& h);

    virtual void setMap(QDeclarativeGeoMap* quickMap, QGeoMap *map);
       //from QuickItem
    virtual QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);

    Q_INVOKABLE void addCoordinate(QDeclarativeCoordinate* coordinate);
    Q_INVOKABLE void removeCoordinate(QDeclarativeCoordinate* coordinate);

    QDeclarativeListProperty<QDeclarativeCoordinate> declarativePath();

    bool contains(QPointF point);

    QDeclarativeMapLineProperties *line();

Q_SIGNALS:
    void pathChanged();

protected Q_SLOTS:
    virtual void updateMapItem();
    void updateAfterLinePropertiesChanged();
    void updateAfterCoordinateChanged();

private Q_SLOTS:
    // map size changed
    void handleCameraDataChanged(const QGeoCameraData& cameraData);

private:
    static void path_append(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, QDeclarativeCoordinate *coordinate);
    static int path_count(QDeclarativeListProperty<QDeclarativeCoordinate> *prop);
    static QDeclarativeCoordinate* path_at(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, int index);
    static void path_clear(QDeclarativeListProperty<QDeclarativeCoordinate> *prop);
    void pathPropertyChanged();

private:
    QDeclarativeMapLineProperties line_;
    QList<QDeclarativeCoordinate*> coordPath_;
    QList<QGeoCoordinate> path_;
    QColor color_;
    qreal zoomLevel_;
    QPolygonF polyline_;
    bool dirtyGeometry_;
    bool dirtyMaterial_;
};

//////////////////////////////////////////////////////////////////////

class MapPolylineNode : public QSGGeometryNode
{

public:
    MapPolylineNode();
    ~MapPolylineNode();

    void update(const QColor& fillColor, const QPolygonF& shape, qreal width);

private:
    QSGFlatColorMaterial fill_material_;
    QSGGeometry geometry_;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeMapLineProperties));
QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativePolylineMapItem));

#endif /* QDECLARATIVEPOLYLINEMAPITEM_H_ */
