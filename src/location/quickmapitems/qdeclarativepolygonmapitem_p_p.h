// Copyright (C) 2020 Paolo Angelelli <paolo.angelelli@gmail.com>
// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QDECLARATIVEPOLYGONMAPITEM_P_P_H
#define QDECLARATIVEPOLYGONMAPITEM_P_P_H

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

#include <QtCore/QList>
#include <QtCore/QScopedValueRollback>
#include <QtGui/QMatrix4x4>
#include <QtGui/QColor>

#include <QSGFlatColorMaterial>

#include <QtLocation/private/qlocationglobal_p.h>
#include <QtLocation/private/qgeomapitemgeometry_p.h>
#include <QtLocation/private/qdeclarativepolygonmapitem_p.h>
#include <QtLocation/private/qdeclarativepolylinemapitem_p_p.h>

#include <QtPositioning/private/qdoublevector2d_p.h>

QT_BEGIN_NAMESPACE

class QQuickShape;
class QQuickShapePath;

class Q_LOCATION_PRIVATE_EXPORT QGeoMapPolygonGeometry : public QGeoMapItemGeometry
{
public:
    QGeoMapPolygonGeometry();

    inline void setAssumeSimple(bool value) { assumeSimple_ = value; }

    void updateSourcePoints(const QGeoMap &map,
                            const QList<QDoubleVector2D> &path);

#ifndef MAPITEMS_USE_SHAPES
    void updateScreenPoints(const QGeoMap &map, qreal strokeWidth = 0.0);
#endif

    QPainterPath srcPath() const { return srcPath_; }
    qreal maxCoord() const { return maxCoord_; }

protected:
    QPainterPath srcPath_;
    qreal maxCoord_ = 0.0;
    bool assumeSimple_ = false;
};

#ifndef MAPITEMS_USE_SHAPES
class Q_LOCATION_PRIVATE_EXPORT MapPolygonNode : public MapItemGeometryNode
{

public:
    MapPolygonNode();
    ~MapPolygonNode() override;

    void update(const QColor &fillColor, const QColor &borderColor,
                const QGeoMapItemGeometry *fillShape,
                const QGeoMapItemGeometry *borderShape);
private:
    QSGFlatColorMaterial fill_material_;
    MapPolylineNode *border_;
    QSGGeometry geometry_;
};
#endif

class Q_LOCATION_PRIVATE_EXPORT QDeclarativePolygonMapItemPrivate
{
    Q_DISABLE_COPY_MOVE(QDeclarativePolygonMapItemPrivate)
public:
    QDeclarativePolygonMapItemPrivate(QDeclarativePolygonMapItem &polygon)
        : m_poly(polygon)
    {
    }

    virtual ~QDeclarativePolygonMapItemPrivate();
    virtual void onLinePropertiesChanged() = 0;
    virtual void markSourceDirtyAndUpdate() = 0;
    virtual void onMapSet() = 0;
    virtual void onGeoGeometryChanged() = 0;
    virtual void onGeoGeometryUpdated() = 0;
    virtual void onItemGeometryChanged() = 0;
    virtual void updatePolish() = 0;
    virtual void afterViewportChanged() = 0;
    virtual QSGNode * updateMapItemPaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *data) = 0;
    virtual bool contains(const QPointF &point) const = 0;

    QDeclarativePolygonMapItem &m_poly;
};

class Q_LOCATION_PRIVATE_EXPORT QDeclarativePolygonMapItemPrivateCPU: public QDeclarativePolygonMapItemPrivate
{
public:
    QDeclarativePolygonMapItemPrivateCPU(QDeclarativePolygonMapItem &polygon);
    ~QDeclarativePolygonMapItemPrivateCPU() override;

    void onLinePropertiesChanged() override
    {
        // mark dirty just in case we're a width change
        markSourceDirtyAndUpdate();
    }
    void markSourceDirtyAndUpdate() override
    {
        // preserveGeometry is cleared in updateMapItemPaintNode
        m_geometry.markSourceDirty();
#ifndef MAPITEMS_USE_SHAPES
        m_borderGeometry.markSourceDirty();
#endif
        m_poly.polishAndUpdate();
    }
    void regenerateCache()
    {
        if (!m_poly.map() || m_poly.map()->geoProjection().projectionType() != QGeoProjection::ProjectionWebMercator)
            return;
        const QGeoProjectionWebMercator &p = static_cast<const QGeoProjectionWebMercator&>(m_poly.map()->geoProjection());
        m_geopathProjected.clear();
        m_geopathProjected.reserve(m_poly.m_geopoly.size());
        for (const QGeoCoordinate &c : m_poly.m_geopoly.perimeter())
            m_geopathProjected << p.geoToMapProjection(c);
    }
    void updateCache()
    {
        if (!m_poly.map() || m_poly.map()->geoProjection().projectionType() != QGeoProjection::ProjectionWebMercator)
            return;
        const QGeoProjectionWebMercator &p = static_cast<const QGeoProjectionWebMercator&>(m_poly.map()->geoProjection());
        m_geopathProjected << p.geoToMapProjection(m_poly.m_geopoly.perimeter().last());
    }
    void preserveGeometry()
    {
        m_geometry.setPreserveGeometry(true, m_poly.m_geopoly.boundingGeoRectangle().topLeft());
#ifndef MAPITEMS_USE_SHAPES
        m_borderGeometry.setPreserveGeometry(true, m_poly.m_geopoly.boundingGeoRectangle().topLeft());
#endif
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
    QSGNode *updateMapItemPaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *data) override;
    bool contains(const QPointF &point) const override;

    QList<QDoubleVector2D> m_geopathProjected;
    QGeoMapPolygonGeometry m_geometry;
#ifdef MAPITEMS_USE_SHAPES
    QQuickShape *m_shape = nullptr;
    QQuickShapePath *m_shapePath = nullptr;
    QDeclarativeGeoMapPainterPath *m_painterPath = nullptr;
#else
    QGeoMapPolylineGeometry m_borderGeometry;
    MapPolygonNode *m_node = nullptr;
#endif
};

QT_END_NAMESPACE

#endif // QDECLARATIVEPOLYGONMAPITEM_P_P_H
