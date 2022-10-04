/****************************************************************************
 **
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

#include "qdeclarativecirclemapitem_rhi_p.h"

QT_BEGIN_NAMESPACE

QDeclarativeCircleMapItemPrivateOpenGL::~QDeclarativeCircleMapItemPrivateOpenGL() {}

void QDeclarativeCircleMapItemPrivateOpenGL::updatePolish()
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

QSGNode *QDeclarativeCircleMapItemPrivateOpenGL::updateMapItemPaintNode(QSGNode *oldNode,
                                                        QQuickItem::UpdatePaintNodeData *data)
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

bool QDeclarativeCircleMapItemPrivateOpenGL::contains(const QPointF &point) const
{
    const qreal lineWidth = m_circle.m_border.width();
    const QColor &lineColor = m_circle.m_border.color();
    const QRectF &bounds = (lineColor.alpha() != 0 && lineWidth > 0)
                         ? m_borderGeometry.sourceBoundingBox()
                         : m_geometry.sourceBoundingBox();
    if (bounds.contains(point)) {
        QDeclarativeGeoMap *m = m_circle.quickMap();
        if (m) {
            const QGeoCoordinate crd = m->toCoordinate(m->mapFromItem(&m_circle, point));
            const auto &projection = static_cast<const QGeoProjectionWebMercator&>(
                                                 m_circle.map()->geoProjection());
            return m_circle.m_circle.contains(crd)
                || m_borderGeometry.contains(m_circle.mapToItem(m_circle.quickMap(), point),
                                             m_circle.border()->width(), projection);
        } else {
            return  true;
        }
    }
    return false;
}

QT_END_NAMESPACE

