// Copyright (C) 2020 Paolo Angelelli <paolo.angelelli@gmail.com>
// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QDECLARATIVEPOLYLINEMAPITEM_P_P_H
#define QDECLARATIVEPOLYLINEMAPITEM_P_P_H

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

#include <QtCore/QScopedValueRollback>
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>

#include <QtLocation/private/qlocationglobal_p.h>
#include <QtLocation/private/qdeclarativepolylinemapitem_p.h>
#include <QtLocation/private/qgeomapitemgeometry_p.h>

#include <QtPositioning/private/qdoublevector2d_p.h>

QT_BEGIN_NAMESPACE

class QSGMaterialShader;

class Q_LOCATION_PRIVATE_EXPORT QGeoMapPolylineGeometry : public QGeoMapItemGeometry
{
public:
    QGeoMapPolylineGeometry();

    void updateSourcePoints(const QGeoMap &map,
                            const QList<QDoubleVector2D> &path,
                            const QGeoCoordinate geoLeftBound);

    void updateScreenPoints(const QGeoMap &map,
                            qreal strokeWidth,
                            bool adjustTranslation = true);

    void clearSource();

    bool contains(const QPointF &point) const override;

    QList<QList<QDoubleVector2D> > clipPath(const QGeoMap &map,
                    const QList<QDoubleVector2D> &path,
                    QDoubleVector2D &leftBoundWrapped);

    void pathToScreen(const QGeoMap &map,
                      const QList<QList<QDoubleVector2D> > &clippedPaths,
                      const QDoubleVector2D &leftBoundWrapped);

public:
    QList<qreal> srcPoints_;
    QList<QPainterPath::ElementType> srcPointTypes_;

#ifdef QT_LOCATION_DEBUG
    QList<QDoubleVector2D> m_wrappedPath;
    QList<QList<QDoubleVector2D>> m_clippedPaths;
#endif

    friend class QDeclarativeCircleMapItem;
    friend class QDeclarativePolygonMapItem;
    friend class QDeclarativeRectangleMapItem;
};

class Q_LOCATION_PRIVATE_EXPORT VisibleNode
{
public:
    VisibleNode();
    virtual ~VisibleNode();

    bool subtreeBlocked() const;
    void setSubtreeBlocked(bool blocked);
    bool visible() const;
    void setVisible(bool visible);

    bool m_blocked : 1;
    bool m_visible : 1;
};

class Q_LOCATION_PRIVATE_EXPORT MapItemGeometryNode : public QSGGeometryNode, public VisibleNode
{
public:
    ~MapItemGeometryNode() override;
    bool isSubtreeBlocked() const override;
};

class Q_LOCATION_PRIVATE_EXPORT MapPolylineNode : public MapItemGeometryNode
{
public:
    MapPolylineNode();
    ~MapPolylineNode() override;

    void update(const QColor &fillColor, const QGeoMapItemGeometry *shape);

protected:
    QSGFlatColorMaterial fill_material_;
    QSGGeometry geometry_;
};

class Q_LOCATION_PRIVATE_EXPORT QDeclarativePolylineMapItemPrivate
{
    Q_DISABLE_COPY_MOVE(QDeclarativePolylineMapItemPrivate)
public:
    QDeclarativePolylineMapItemPrivate(QDeclarativePolylineMapItem &poly)
        : m_poly(poly)
    {
    }

    virtual ~QDeclarativePolylineMapItemPrivate();
    virtual void markSourceDirtyAndUpdate() = 0;
    virtual void onMapSet() = 0;
    virtual void onLinePropertiesChanged() = 0;
    virtual void onGeoGeometryChanged() = 0;
    virtual void onGeoGeometryUpdated() = 0;
    virtual void onItemGeometryChanged() = 0;
    virtual void updatePolish() = 0;
    virtual void afterViewportChanged() = 0;
    virtual QSGNode * updateMapItemPaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *data) = 0;
    virtual bool contains(const QPointF &point) const = 0;

    QDeclarativePolylineMapItem &m_poly;
};

class Q_LOCATION_PRIVATE_EXPORT QDeclarativePolylineMapItemPrivateCPU: public QDeclarativePolylineMapItemPrivate
{
public:
    QDeclarativePolylineMapItemPrivateCPU(QDeclarativePolylineMapItem &poly)
        : QDeclarativePolylineMapItemPrivate(poly)
    {
    }

    ~QDeclarativePolylineMapItemPrivateCPU() override;
    void onLinePropertiesChanged() override
    {
        // mark dirty just in case we're a width change
        markSourceDirtyAndUpdate();
    }
    void markSourceDirtyAndUpdate() override
    {
        m_geometry.markSourceDirty();
        m_poly.polishAndUpdate();
    }
    void regenerateCache();
    void updateCache();
    void preserveGeometry()
    {
        m_geometry.setPreserveGeometry(true, m_poly.m_geopath.boundingGeoRectangle().topLeft());
    }
    void afterViewportChanged() override
    {
        // preserveGeometry is cleared in updateMapItemPaintNode
        preserveGeometry();
        markSourceDirtyAndUpdate();
    }
    void onMapSet() override
    {
        regenerateCache();
        markSourceDirtyAndUpdate();
    }
    void onGeoGeometryChanged() override
    {
        regenerateCache();
        preserveGeometry();
        markSourceDirtyAndUpdate();
    }
    void onGeoGeometryUpdated() override
    {
        updateCache();
        preserveGeometry();
        markSourceDirtyAndUpdate();
    }
    void onItemGeometryChanged() override
    {
        onGeoGeometryChanged();
    }
    void updatePolish() override;
    QSGNode *updateMapItemPaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData * /*data*/) override;
    bool contains(const QPointF &point) const override;

    QList<QDoubleVector2D> m_geopathProjected;
    QGeoMapPolylineGeometry m_geometry;
    MapPolylineNode *m_node = nullptr;
};

QT_END_NAMESPACE

#endif // QDECLARATIVEPOLYLINEMAPITEM_P_P_H
