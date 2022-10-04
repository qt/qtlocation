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

#include "qdeclarativegeomapitemutils_p.h"
#include "qdeclarativepolygonmapitem_p.h"
#include "qdeclarativepolylinemapitem_p_p.h"
#include "qdeclarativepolygonmapitem_p_p.h"
#include "qdeclarativepolygonmapitem_rhi_p.h"
#include "qdeclarativerectanglemapitem_p_p.h"
#include "error_messages_p.h"

#include <QtCore/QScopedValueRollback>
#include <qnumeric.h>
#include <QPainter>
#include <QPainterPath>
#include <QtQml/QQmlInfo>
#include <QtQuick/qsgnode.h>

#include <QtQuick/private/qsgmaterialshader_p.h>
#include <QtLocation/private/qgeomap_p.h>
#include <QtPositioning/private/qlocationutils_p.h>
#include <QtPositioning/private/qdoublevector2d_p.h>
#include <QtPositioning/private/qclipperutils_p.h>
#include <QtPositioning/private/qgeopolygon_p.h>
#include <QtPositioning/private/qwebmercator_p.h>

QT_BEGIN_NAMESPACE

MapPolygonShader::MapPolygonShader() : QSGMaterialShader(*new QSGMaterialShaderPrivate(this))
{
    setShaderFileName(VertexStage, QLatin1String(":/location/quickmapitems/rhi/shaders/polygon.vert.qsb"));
    setShaderFileName(FragmentStage, QLatin1String(":/location/quickmapitems/rhi/shaders/polygon.frag.qsb"));
}

bool MapPolygonShader::updateUniformData(RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect)
{
    Q_ASSERT(oldEffect == nullptr || newEffect->type() == oldEffect->type());
    MapPolygonMaterial *oldMaterial = static_cast<MapPolygonMaterial *>(oldEffect);
    MapPolygonMaterial *newMaterial = static_cast<MapPolygonMaterial *>(newEffect);

    const QColor &c = newMaterial->color();
    const QMatrix4x4 &geoProjection = newMaterial->geoProjection();
    const QDoubleVector3D &center = newMaterial->center();

    // It is safer to use vec4 instead on vec3, as described in:
    // https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)#Memory_layout
    QVector4D vecCenter, vecCenter_lowpart;
    for (int i = 0; i < 3; i++)
        QLocationUtils::split_double(center.get(i), &vecCenter[i], &vecCenter_lowpart[i]);
    vecCenter[3] = 0;
    vecCenter_lowpart[3] = 0;

    int offset = 0;
    char *buf_p = state.uniformData()->data();

    if (state.isMatrixDirty()) {
        const QMatrix4x4 m = state.projectionMatrix();
        memcpy(buf_p + offset, m.constData(), 4*4*4);
    }
    offset += 4*4*4;

    memcpy(buf_p + offset, geoProjection.constData(), 4*4*4); offset+=4*4*4;

    memcpy(buf_p + offset, &vecCenter, 4*4); offset += 4*4;

    memcpy(buf_p + offset, &vecCenter_lowpart, 4*4); offset+=4*4;

    const float wrapOffset = newMaterial->wrapOffset();
    memcpy(buf_p + offset, &wrapOffset, 4); offset+=4;

    offset += 4+4+4; // Padding

    if (oldMaterial == nullptr || c != oldMaterial->color() || state.isOpacityDirty()) {
        float opacity = state.opacity() * c.alphaF();
        QVector4D v(c.redF() * opacity,
                    c.greenF() *  opacity,
                    c.blueF() * opacity,
                    opacity);
        memcpy(buf_p + offset, &v, 4*4);
    }
    offset+=4*4;

    return true;
}

QSGMaterialShader *MapPolygonMaterial::createShader(QSGRendererInterface::RenderMode renderMode) const
{
    Q_UNUSED(renderMode);
    return new MapPolygonShader();
}

int MapPolygonMaterial::compare(const QSGMaterial *other) const
{
    const MapPolygonMaterial &o = *static_cast<const MapPolygonMaterial *>(other);
    if (o.m_center == m_center && o.m_geoProjection == m_geoProjection && o.m_wrapOffset == m_wrapOffset)
        return QSGFlatColorMaterial::compare(other);
    return -1;
}

QSGMaterialType *MapPolygonMaterial::type() const
{
    static QSGMaterialType type;
    return &type;
}

MapPolygonNodeGL::MapPolygonNodeGL() :
    //fill_material_(this),
    fill_material_(),
    geometry_(QSGGeometry::defaultAttributes_Point2D(), 0)
{
    geometry_.setDrawingMode(QSGGeometry::DrawTriangles);
    QSGGeometryNode::setMaterial(&fill_material_);
    QSGGeometryNode::setGeometry(&geometry_);
}

MapPolygonNodeGL::~MapPolygonNodeGL()
{
}

/*!
    \internal
*/
void MapPolygonNodeGL::update(const QColor &fillColor,
                              const QGeoMapPolygonGeometryOpenGL *fillShape,
                              const QMatrix4x4 &geoProjection,
                              const QDoubleVector3D &center)
{
    if (fillShape->m_screenIndices.size() < 3 || fillColor.alpha() == 0) {
        setSubtreeBlocked(true);
        return;
    }
    setSubtreeBlocked(false);

    QSGGeometry *fill = QSGGeometryNode::geometry();
    if (fillShape->m_dataChanged || !fill->vertexCount()) {
        fillShape->allocateAndFillPolygon(fill);
        markDirty(DirtyGeometry);
        fillShape->m_dataChanged = false;
    }

    //if (fillColor != fill_material_.color()) // Any point in optimizing this?
    {
        fill_material_.setColor(fillColor);
        fill_material_.setGeoProjection(geoProjection);
        fill_material_.setCenter(center);
        fill_material_.setWrapOffset(fillShape->m_wrapOffset - 1);
        setMaterial(&fill_material_);
        markDirty(DirtyMaterial);
    }
}

QDeclarativePolygonMapItemPrivateOpenGL::~QDeclarativePolygonMapItemPrivateOpenGL() {}


void QDeclarativePolygonMapItemPrivateOpenGL::updatePolish()
{
    if (m_poly.m_geopoly.perimeter().length() == 0) { // Possibly cleared
        m_geometry.clear();
        m_borderGeometry.clear();
        m_poly.setWidth(0);
        m_poly.setHeight(0);
        return;
    }

    QScopedValueRollback<bool> rollback(m_poly.m_updatingGeometry);
    m_poly.m_updatingGeometry = true;
    const qreal lineWidth = m_poly.m_border.width();
    const QColor &lineColor = m_poly.m_border.color();
    const QColor &fillColor = m_poly.color();
    if (fillColor.alpha() != 0) {
        m_geometry.updateSourcePoints(*m_poly.map(), m_poly.m_geopoly);
        m_geometry.markScreenDirty();
        m_geometry.updateScreenPoints(*m_poly.map(), lineWidth, lineColor);
    } else {
        m_geometry.clearBounds();
    }

    QGeoMapItemGeometry * geom = &m_geometry;
    m_borderGeometry.clearScreen();
    if (lineColor.alpha() != 0 && lineWidth > 0) {
        m_borderGeometry.updateSourcePoints(*m_poly.map(), m_poly.m_geopoly);
        m_borderGeometry.markScreenDirty();
        m_borderGeometry.updateScreenPoints(*m_poly.map(), lineWidth);
        geom = &m_borderGeometry;
    }
    m_poly.setWidth(geom->sourceBoundingBox().width());
    m_poly.setHeight(geom->sourceBoundingBox().height());
    m_poly.setPosition(1.0 * geom->firstPointOffset() - QPointF(lineWidth * 0.5,lineWidth * 0.5));
}

QSGNode *QDeclarativePolygonMapItemPrivateOpenGL::updateMapItemPaintNode(QSGNode *oldNode,
                                                        QQuickItem::UpdatePaintNodeData *data)
{
    Q_UNUSED(data);

    if (!m_rootNode || !oldNode) {
        m_rootNode = new RootNode();
        m_node = new MapPolygonNodeGL();
        m_rootNode->appendChildNode(m_node);
        m_polylinenode = new MapPolylineNodeOpenGLExtruded();
        m_rootNode->appendChildNode(m_polylinenode);
        m_rootNode->markDirty(QSGNode::DirtyNodeAdded);
        if (oldNode)
            delete oldNode;
    } else {
        m_rootNode = static_cast<RootNode *>(oldNode);
    }

    const QGeoMap *map = m_poly.map();
    const QMatrix4x4 &combinedMatrix = map->geoProjection().qsgTransform();
    const QDoubleVector3D &cameraCenter = map->geoProjection().centerMercator();

    if (m_borderGeometry.isScreenDirty()) {
        /* Do the border update first */
        m_polylinenode->update(m_poly.m_border.color(),
                               float(m_poly.m_border.width()),
                               &m_borderGeometry,
                               combinedMatrix,
                               cameraCenter,
                               Qt::SquareCap,
                               true,
                               30); // No LOD for polygons just yet.
                                    // First figure out what to do with holes.
        m_borderGeometry.setPreserveGeometry(false);
        m_borderGeometry.markClean();
    } else {
        m_polylinenode->setSubtreeBlocked(true);
    }
    if (m_geometry.isScreenDirty()) {
        m_node->update(m_poly.m_color,
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

bool QDeclarativePolygonMapItemPrivateOpenGL::contains(const QPointF &point) const
{
    const qreal lineWidth = m_poly.m_border.width();
    const QColor &lineColor = m_poly.m_border.color();
    const QRectF &bounds = (lineColor.alpha() != 0 && lineWidth > 0)
                         ? m_borderGeometry.sourceBoundingBox()
                         : m_geometry.sourceBoundingBox();
    if (bounds.contains(point)) {
        QDeclarativeGeoMap *m = m_poly.quickMap();
        if (m) {
            const QGeoCoordinate crd = m->toCoordinate(m->mapFromItem(&m_poly, point));
            const auto &projection = static_cast<const QGeoProjectionWebMercator&>(
                                                 m_poly.map()->geoProjection());
            return m_poly.m_geopoly.contains(crd)
                || m_borderGeometry.contains(m_poly.mapToItem(m_poly.quickMap(), point),
                                             m_poly.border()->width(), projection);
        } else {
            return  true;
        }
    }
    return false;
}

QT_END_NAMESPACE
