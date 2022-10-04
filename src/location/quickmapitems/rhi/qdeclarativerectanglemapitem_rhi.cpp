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

#include "qdeclarativerectanglemapitem_rhi_p.h"

QT_BEGIN_NAMESPACE

QDeclarativeRectangleMapItemPrivateOpenGL::~QDeclarativeRectangleMapItemPrivateOpenGL() {}


void QDeclarativeRectangleMapItemPrivateOpenGL::updatePolish()
{
    if (!m_rect.topLeft().isValid() || !m_rect.bottomRight().isValid()) {
        m_geometry.clear();
        m_borderGeometry.clear();
        m_rect.setWidth(0);
        m_rect.setHeight(0);
        return;
    }

    QScopedValueRollback<bool> rollback(m_rect.m_updatingGeometry);
    m_rect.m_updatingGeometry = true;
    const qreal lineWidth = m_rect.m_border.width();
    const QColor &lineColor = m_rect.m_border.color();
    const QColor &fillColor = m_rect.color();
    if (fillColor.alpha() != 0) {
        m_geometry.updateSourcePoints(*m_rect.map(), m_rect.m_rectangle);
        m_geometry.markScreenDirty();
        m_geometry.updateScreenPoints(*m_rect.map(), lineWidth, lineColor);
    } else {
        m_geometry.clearBounds();
    }

    QGeoMapItemGeometry * geom = &m_geometry;
    m_borderGeometry.clearScreen();
    if (lineColor.alpha() != 0 && lineWidth > 0) {
        m_borderGeometry.updateSourcePoints(*m_rect.map(), m_rect.m_rectangle);
        m_borderGeometry.markScreenDirty();
        m_borderGeometry.updateScreenPoints(*m_rect.map(), lineWidth);
        geom = &m_borderGeometry;
    }
    m_rect.setWidth(geom->sourceBoundingBox().width());
    m_rect.setHeight(geom->sourceBoundingBox().height());
    m_rect.setPosition(1.0 * geom->firstPointOffset() - QPointF(lineWidth * 0.5,lineWidth * 0.5));
}

QSGNode *QDeclarativeRectangleMapItemPrivateOpenGL::updateMapItemPaintNode(QSGNode *oldNode,
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

    const QGeoMap *map = m_rect.map();
    const QMatrix4x4 &combinedMatrix = map->geoProjection().qsgTransform();
    const QDoubleVector3D &cameraCenter = map->geoProjection().centerMercator();

    if (m_borderGeometry.isScreenDirty()) {
        /* Do the border update first */
        m_polylinenode->update(m_rect.m_border.color(),
                               float(m_rect.m_border.width()),
                               &m_borderGeometry,
                               combinedMatrix,
                               cameraCenter,
                               Qt::SquareCap,
                               true,
                               30); // No LOD for rectangles
        m_borderGeometry.setPreserveGeometry(false);
        m_borderGeometry.markClean();
    } else {
        m_polylinenode->setSubtreeBlocked(true);
    }
    if (m_geometry.isScreenDirty()) {
        m_node->update(m_rect.m_color,
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

bool QDeclarativeRectangleMapItemPrivateOpenGL::contains(const QPointF &point) const
{
    const qreal lineWidth = m_rect.m_border.width();
    const QColor &lineColor = m_rect.m_border.color();
    const QRectF &bounds = (lineColor.alpha() != 0 && lineWidth > 0)
                         ? m_borderGeometry.sourceBoundingBox()
                         : m_geometry.sourceBoundingBox();
    if (bounds.contains(point)) {
        QDeclarativeGeoMap *m = m_rect.quickMap();
        if (m) {
            const QGeoCoordinate crd = m->toCoordinate(m->mapFromItem(&m_rect, point));
            const auto &projection = static_cast<const QGeoProjectionWebMercator&>(
                                                 m_rect.map()->geoProjection());
            return m_rect.m_rectangle.contains(crd)
                || m_borderGeometry.contains(m_rect.mapToItem(m_rect.quickMap(), point),
                                             m_rect.border()->width(), projection);
        } else {
            return  true;
        }
    }
    return false;
}

QT_END_NAMESPACE

