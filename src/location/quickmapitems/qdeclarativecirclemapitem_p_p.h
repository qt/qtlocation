/****************************************************************************
**
** Copyright (C) 2020 Paolo Angelelli <paolo.angelelli@gmail.com>
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVECIRCLEMAPITEM_P_P_H
#define QDECLARATIVECIRCLEMAPITEM_P_P_H

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

#include <QtLocation/private/qlocationglobal_p.h>
#include <QtLocation/private/qdeclarativepolygonmapitem_p_p.h>
#include <QtLocation/private/qdeclarativecirclemapitem_p.h>

QT_BEGIN_NAMESPACE

class Q_LOCATION_PRIVATE_EXPORT QGeoMapCircleGeometry : public QGeoMapPolygonGeometry
{
public:
    QGeoMapCircleGeometry();

    void updateScreenPointsInvert(const QList<QDoubleVector2D> &circlePath, const QGeoMap &map);
};

class Q_LOCATION_PRIVATE_EXPORT QDeclarativeCircleMapItemPrivate
{
public:
    static const int CircleSamples = 128; // ToDo: make this radius && ZL dependent?

    QDeclarativeCircleMapItemPrivate(QDeclarativeCircleMapItem &circle) : m_circle(circle)
    {

    }
    QDeclarativeCircleMapItemPrivate(QDeclarativeCircleMapItemPrivate &other) : m_circle(other.m_circle)
    {
    }

    virtual ~QDeclarativeCircleMapItemPrivate();
    virtual void onLinePropertiesChanged() = 0;
    virtual void markSourceDirtyAndUpdate() = 0;
    virtual void onMapSet() = 0;
    virtual void onGeoGeometryChanged() = 0;
    virtual void onItemGeometryChanged() = 0;
    virtual void updatePolish() = 0;
    virtual void afterViewportChanged() = 0;
    virtual QSGNode * updateMapItemPaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *data) = 0;
    virtual bool contains(const QPointF &point) const = 0;

    void updateCirclePath()
    {
        if (!m_circle.map() || m_circle.map()->geoProjection().projectionType() != QGeoProjection::ProjectionWebMercator)
            return;

        const QGeoProjectionWebMercator &p = static_cast<const QGeoProjectionWebMercator&>(m_circle.map()->geoProjection());
        QList<QGeoCoordinate> path;
        calculatePeripheralPoints(path, m_circle.center(), m_circle.radius(), CircleSamples, m_leftBound);
        m_circlePath.clear();
        for (const QGeoCoordinate &c : path)
            m_circlePath << p.geoToMapProjection(c);
    }

    static bool crossEarthPole(const QGeoCoordinate &center, qreal distance);

    static bool preserveCircleGeometry(QList<QDoubleVector2D> &path, const QGeoCoordinate &center,
                                qreal distance, const QGeoProjectionWebMercator &p);
    static void updateCirclePathForRendering(QList<QDoubleVector2D> &path, const QGeoCoordinate &center,
                                      qreal distance, const QGeoProjectionWebMercator &p);

    static void calculatePeripheralPoints(QList<QGeoCoordinate> &path, const QGeoCoordinate &center,
                                   qreal distance, int steps, QGeoCoordinate &leftBound);

    QDeclarativeCircleMapItem &m_circle;
    QList<QDoubleVector2D> m_circlePath;
    QGeoCoordinate m_leftBound;
};

class Q_LOCATION_PRIVATE_EXPORT QDeclarativeCircleMapItemPrivateCPU: public QDeclarativeCircleMapItemPrivate
{
public:

    QDeclarativeCircleMapItemPrivateCPU(QDeclarativeCircleMapItem &circle) : QDeclarativeCircleMapItemPrivate(circle)
    {
    }

    QDeclarativeCircleMapItemPrivateCPU(QDeclarativeCircleMapItemPrivate &other)
    : QDeclarativeCircleMapItemPrivate(other)
    {
    }

    ~QDeclarativeCircleMapItemPrivateCPU() override;

    void onLinePropertiesChanged() override
    {
        // mark dirty just in case we're a width change
        markSourceDirtyAndUpdate();
    }
    void markSourceDirtyAndUpdate() override
    {
        // preserveGeometry is cleared in updateMapItemPaintNode
        m_geometry.markSourceDirty();
        m_borderGeometry.markSourceDirty();
        m_circle.polishAndUpdate();
    }
    void onMapSet() override
    {
        updateCirclePath();
        markSourceDirtyAndUpdate();
    }
    void onGeoGeometryChanged() override
    {
        updateCirclePath();
        markSourceDirtyAndUpdate();
    }
    void onItemGeometryChanged() override
    {
        onGeoGeometryChanged();
    }
    void afterViewportChanged() override
    {
        markSourceDirtyAndUpdate();
    }
    void updatePolish() override;
    QSGNode * updateMapItemPaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *data) override;
    bool contains(const QPointF &point) const override;

    QGeoMapCircleGeometry m_geometry;
    QGeoMapPolylineGeometry m_borderGeometry;
    MapPolygonNode *m_node = nullptr;
};

class Q_LOCATION_PRIVATE_EXPORT QDeclarativeCircleMapItemPrivateOpenGL: public QDeclarativeCircleMapItemPrivate
{
public:
    QDeclarativeCircleMapItemPrivateOpenGL(QDeclarativeCircleMapItem &circle) : QDeclarativeCircleMapItemPrivate(circle)
    {
    }

    QDeclarativeCircleMapItemPrivateOpenGL(QDeclarativeCircleMapItemPrivate &other)
    : QDeclarativeCircleMapItemPrivate(other)
    {
    }

    ~QDeclarativeCircleMapItemPrivateOpenGL() override;

    void onLinePropertiesChanged() override
    {
        m_circle.m_dirtyMaterial = true;
        afterViewportChanged();
    }
    void markScreenDirtyAndUpdate()
    {
        // preserveGeometry is cleared in updateMapItemPaintNode
        m_geometry.markScreenDirty();
        m_borderGeometry.markScreenDirty();
        m_circle.polishAndUpdate();
    }
    void markSourceDirtyAndUpdate() override
    {
        updateCirclePath();
        preserveGeometry();
        m_geometry.markSourceDirty();
        m_borderGeometry.markSourceDirty();
        m_circle.polishAndUpdate();
    }
    void preserveGeometry()
    {
        m_geometry.setPreserveGeometry(true, m_leftBound);
        m_borderGeometry.setPreserveGeometry(true, m_leftBound);
    }
    void onMapSet() override
    {
        markSourceDirtyAndUpdate();
    }
    void onGeoGeometryChanged() override
    {

        markSourceDirtyAndUpdate();
    }
    void onItemGeometryChanged() override
    {
        onGeoGeometryChanged();
    }
    void afterViewportChanged() override
    {
        preserveGeometry();
        markScreenDirtyAndUpdate();
    }
    void updatePolish() override
    {
        if (m_circle.m_circle.isEmpty()) {
            m_geometry.clear();
            m_borderGeometry.clear();
            m_circle.setWidth(0);
            m_circle.setHeight(0);
            return;
        }

        QScopedValueRollback<bool> rollback(m_circle.m_updatingGeometry);
        m_circle.m_updatingGeometry = true;
        const qreal lineWidth = m_circle.m_border.width();
        const QColor &lineColor = m_circle.m_border.color();
        const QColor &fillColor = m_circle.color();
        if (fillColor.alpha() != 0) {
            m_geometry.updateSourcePoints(*m_circle.map(), m_circlePath);
            m_geometry.markScreenDirty();
            m_geometry.updateScreenPoints(*m_circle.map(), lineWidth, lineColor);
        } else {
            m_geometry.clearBounds();
        }

        QGeoMapItemGeometry * geom = &m_geometry;
        m_borderGeometry.clearScreen();
        if (lineColor.alpha() != 0 && lineWidth > 0) {
            m_borderGeometry.updateSourcePoints(*m_circle.map(), m_circle.m_circle);
            m_borderGeometry.markScreenDirty();
            m_borderGeometry.updateScreenPoints(*m_circle.map(), lineWidth);
            geom = &m_borderGeometry;
        }
        m_circle.setWidth(geom->sourceBoundingBox().width());
        m_circle.setHeight(geom->sourceBoundingBox().height());
        m_circle.setPosition(1.0 * geom->firstPointOffset() - QPointF(lineWidth * 0.5,lineWidth * 0.5));
    }

    QSGNode * updateMapItemPaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *data) override
    {
        Q_UNUSED(data);

        if (!m_rootNode || !oldNode) {
            m_rootNode = new QDeclarativePolygonMapItemPrivateOpenGL::RootNode();
            m_node = new MapPolygonNodeGL();
            m_rootNode->appendChildNode(m_node);
            m_polylinenode = new MapPolylineNodeOpenGLExtruded();
            m_rootNode->appendChildNode(m_polylinenode);
            m_rootNode->markDirty(QSGNode::DirtyNodeAdded);
            if (oldNode)
                delete oldNode;
        } else {
            m_rootNode = static_cast<QDeclarativePolygonMapItemPrivateOpenGL::RootNode *>(oldNode);
        }

        const QGeoMap *map = m_circle.map();
        const QMatrix4x4 &combinedMatrix = map->geoProjection().qsgTransform();
        const QDoubleVector3D &cameraCenter = map->geoProjection().centerMercator();

        if (m_borderGeometry.isScreenDirty()) {
            /* Do the border update first */
            m_polylinenode->update(m_circle.m_border.color(),
                                   float(m_circle.m_border.width()),
                                   &m_borderGeometry,
                                   combinedMatrix,
                                   cameraCenter,
                                   Qt::SquareCap,
                                   true,
                                   30); // No LOD for circles
            m_borderGeometry.setPreserveGeometry(false);
            m_borderGeometry.markClean();
        } else {
            m_polylinenode->setSubtreeBlocked(true);
        }
        if (m_geometry.isScreenDirty()) {
            m_node->update(m_circle.m_color,
                         &m_geometry,
                         combinedMatrix,
                         cameraCenter);
            m_geometry.setPreserveGeometry(false);
            m_geometry.markClean();
        } else {
            m_node->setSubtreeBlocked(true);
        }

        m_rootNode->setSubtreeBlocked(false);
        return m_rootNode;
    }
    bool contains(const QPointF &point) const override
    {
        const qreal lineWidth = m_circle.m_border.width();
        const QColor &lineColor = m_circle.m_border.color();
        const QRectF &bounds = (lineColor.alpha() != 0 && lineWidth > 0) ? m_borderGeometry.sourceBoundingBox() : m_geometry.sourceBoundingBox();
        if (bounds.contains(point)) {
            QDeclarativeGeoMap *m = m_circle.quickMap();
            if (m) {
                const QGeoCoordinate crd = m->toCoordinate(m->mapFromItem(&m_circle, point));
                return  m_circle.m_circle.contains(crd) || m_borderGeometry.contains(m_circle.mapToItem(m_circle.quickMap(), point),
                                                                                     m_circle.border()->width(),
                                                                                     static_cast<const QGeoProjectionWebMercator&>(m_circle.map()->geoProjection()));
            } else {
                return  true;
            }
        }
        return false;
    }

    QGeoMapPolygonGeometryOpenGL m_geometry;
    QGeoMapPolylineGeometryOpenGL m_borderGeometry;
    QDeclarativePolygonMapItemPrivateOpenGL::RootNode *m_rootNode = nullptr;
    MapPolygonNodeGL *m_node = nullptr;
    MapPolylineNodeOpenGLExtruded *m_polylinenode = nullptr;
};

QT_END_NAMESPACE

#endif // QDECLARATIVECIRCLEMAPITEM_P_P_H
