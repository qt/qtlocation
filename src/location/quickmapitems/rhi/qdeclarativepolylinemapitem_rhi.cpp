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

#include "qdeclarativepolygonmapitem_rhi_p.h"

#include <QtQuick/private/qsgmaterialshader_p.h>
#include <QtPositioning/private/qlocationutils_p.h>

QT_BEGIN_NAMESPACE

const QSGGeometry::AttributeSet &MapPolylineNodeOpenGL::attributesMapPolylineTriangulated()
{
    return MapPolylineEntry::attributes();
}

MapPolylineNodeOpenGL::MapPolylineNodeOpenGL()
: m_geometryTriangulating(MapPolylineNodeOpenGL::attributesMapPolylineTriangulated(),
            0 /* vtx cnt */, 0 /* index cnt */, QSGGeometry::UnsignedIntType /* index type */)
{
    m_geometryTriangulating.setDrawingMode(QSGGeometry::DrawTriangles);
    QSGGeometryNode::setMaterial(&fill_material_);
    QSGGeometryNode::setGeometry(&m_geometryTriangulating);
}

MapPolylineNodeOpenGL::~MapPolylineNodeOpenGL()
{

}

bool QGeoMapPolylineGeometryOpenGL::allocateAndFillEntries(QSGGeometry *geom,
                                                           bool closed,
                                                           unsigned int zoom) const
{
    // Select LOD. Generate if not present. Assign it to m_screenVertices;
    if (m_dataChanged) {
        // it means that the data really changed.
        // So synchronously produce LOD 1, and enqueue the requested one if != 0 or 1.
        // Select 0 if 0 is requested, or 1 in all other cases.
        selectLODOnDataChanged(zoom, m_bboxLeftBoundWrapped.x());
    } else {
        // Data has not changed, but active LOD != requested LOD.
        // So, if there are no active tasks, try to change to the correct one.
        if (!selectLODOnLODMismatch(zoom, m_bboxLeftBoundWrapped.x(), closed))
            return false;
    }

    const QList<QDeclarativeGeoMapItemUtils::vec2> &v = *m_screenVertices;
    if (v.size() < 2) {
        geom->allocate(0, 0);
        return true;
    }
    const int numSegments = (v.size() - 1);

    const int numIndices = numSegments * 6; // six vertices per line segment
    geom->allocate(numIndices);
    MapPolylineNodeOpenGL::MapPolylineEntry *vertices =
            static_cast<MapPolylineNodeOpenGL::MapPolylineEntry *>(geom->vertexData());

    for (int i = 0; i < numSegments; ++i) {
        MapPolylineNodeOpenGL::MapPolylineEntry e;
        const QDeclarativeGeoMapItemUtils::vec2 &cur = v[i];
        const QDeclarativeGeoMapItemUtils::vec2 &next = v[i+1];
        e.triangletype = 1.0;
        e.next = next;
        e.prev = cur;
        e.pos = cur;
        e.direction = 1.0;
        e.vertextype = -1.0;
        vertices[i*6] = e;
        e.direction = -1.0;
        vertices[i*6+1] = e;
        e.pos = next;
        e.vertextype = 1.0;
        vertices[i*6+2] = e;

        // Second tri
        e.triangletype = -1.0;
        e.direction = -1.0;
        vertices[i*6+3] = e;
        e.direction = 1.0;
        vertices[i*6+4] = e;
        e.pos = cur;
        e.vertextype = -1.0;
        vertices[i*6+5] = e;

        if (i != 0) {
           vertices[i*6].prev = vertices[i*6+1].prev = vertices[i*6+5].prev = v[i-1];
        } else {
            if (closed) {
                vertices[i*6].prev = vertices[i*6+1].prev = vertices[i*6+5].prev = v[numSegments - 1];
            } else {
                vertices[i*6].triangletype = vertices[i*6+1].triangletype = vertices[i*6+5].triangletype = 2.0;
            }
        }
        if (i != numSegments - 1) {
            vertices[i*6+2].next = vertices[i*6+3].next = vertices[i*6+4].next = v[i+2];
        } else {
            if (closed) {
                vertices[i*6+2].next = vertices[i*6+3].next = vertices[i*6+4].next = v[1];
            } else {
                vertices[i*6+2].triangletype = vertices[i*6+3].triangletype = vertices[i*6+4].triangletype = 3.0;
            }
        }
    }
    return true;
}

void MapPolylineNodeOpenGL::update(const QColor &fillColor,
                                   float lineWidth,
                                   const QGeoMapPolylineGeometryOpenGL *shape,
                                   const QMatrix4x4 &geoProjection,
                                   const QDoubleVector3D &center,
                                   const Qt::PenCapStyle capStyle,
                                   bool closed,
                                   unsigned int zoom)
{
    // shape->size() == number of triangles
    if (shape->m_screenVertices->size() < 2
            || lineWidth < 0.5 || fillColor.alpha() == 0) { // number of points
        setSubtreeBlocked(true);
        return;
    } else {
        setSubtreeBlocked(false);
    }

    QSGGeometry *fill = QSGGeometryNode::geometry();
    if (shape->m_dataChanged || !shape->isLODActive(zoom) || !fill->vertexCount()) { // fill->vertexCount for when node gets destroyed by MapItemBase bcoz of opacity, then recreated.
        if (shape->allocateAndFillEntries(fill, closed, zoom)) {
            markDirty(DirtyGeometry);
            shape->m_dataChanged = false;
        }
    }

    // Update this
//    if (fillColor != fill_material_.color())
    {
        fill_material_.setWrapOffset(shape->m_wrapOffset - 1);
        fill_material_.setColor(fillColor);
        fill_material_.setGeoProjection(geoProjection);
        fill_material_.setCenter(center);
        fill_material_.setLineWidth(lineWidth);
        fill_material_.setMiter(capStyle != Qt::FlatCap);
        setMaterial(&fill_material_);
        markDirty(DirtyMaterial);
    }
}

MapPolylineShader::MapPolylineShader() : QSGMaterialShader(*new QSGMaterialShaderPrivate(this))
{
    // Heavily adapted from https://github.com/mattdesl/webgl-lines/blob/master/projected/vert.glsl,
    // that is (c) Matt DesLauriers, and released under the MIT license.
    setShaderFileName(VertexStage, QLatin1String(":/location/quickmapitems/rhi/shaders/polyline.vert.qsb"));
    setShaderFileName(FragmentStage, QLatin1String(":/location/quickmapitems/rhi/shaders/polyline.frag.qsb"));
}

bool MapPolylineShader::updateUniformData(QSGMaterialShader::RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect)
{
    Q_ASSERT(oldEffect == nullptr || newEffect->type() == oldEffect->type());
    MapPolylineMaterial *oldMaterial = static_cast<MapPolylineMaterial *>(oldEffect);
    MapPolylineMaterial *newMaterial = static_cast<MapPolylineMaterial *>(newEffect);

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

    const float lineWidth = newMaterial->lineWidth();
    memcpy(buf_p + offset, &lineWidth, 4); offset+=4;

    const QRectF viewportRect = state.viewportRect();
    const float aspect = float(viewportRect.width() / viewportRect.height());
    memcpy(buf_p + offset, &aspect, 4); offset+=4;

    offset += 4; // Padding

    int miter = newMaterial->miter();
    memcpy(buf_p + offset, &miter, 4); offset+=4;

    if (oldMaterial == nullptr || c != oldMaterial->color() || state.isOpacityDirty()) {
        float opacity = state.opacity() * c.alphaF();
        QVector4D v(c.redF() * opacity,
                    c.greenF() *  opacity,
                    c.blueF() * opacity,
                    opacity);
        memcpy(buf_p + offset, &v, 4*4);
    }
    offset+=4*4;

    const float wrapOffset = newMaterial->wrapOffset();
    memcpy(buf_p + offset, &wrapOffset, 4); offset+=4;

    return true;
}

QSGMaterialShader *MapPolylineMaterial::createShader(QSGRendererInterface::RenderMode renderMode) const
{
    Q_UNUSED(renderMode);
    return new MapPolylineShader();
}

QSGMaterialType *MapPolylineMaterial::type() const
{
    static QSGMaterialType type;
    return &type;
}

int MapPolylineMaterial::compare(const QSGMaterial *other) const
{
    const MapPolylineMaterial &o = *static_cast<const MapPolylineMaterial *>(other);
    if (o.m_center == m_center
            && o.m_geoProjection == m_geoProjection
            && o.m_wrapOffset == m_wrapOffset
            && o.m_lineWidth == m_lineWidth
            && o.m_miter == m_miter)
    {
        return QSGFlatColorMaterial::compare(other);
    }
    return -1;
}

bool QDeclarativePolylineMapItemPrivateOpenGL::contains(const QPointF &point) const
{
    return m_geometry.contains(m_poly.mapToItem(m_poly.quickMap(), point),
                                m_poly.line()->width(),
                                static_cast<const QGeoProjectionWebMercator&>(m_poly.map()->geoProjection()));
}

void QDeclarativePolylineMapItemPrivateOpenGL::updatePolish()
{
    if (m_poly.m_geopath.path().length() == 0) { // Possibly cleared
        m_geometry.clear();
        m_geometry.clear();
        m_poly.setWidth(0);
        m_poly.setHeight(0);
        return;
    }

    QScopedValueRollback<bool> rollback(m_poly.m_updatingGeometry);
    m_poly.m_updatingGeometry = true;
    const qreal lineWidth = m_poly.m_line.width();
    m_geometry.updateSourcePoints(*m_poly.map(), m_poly.m_geopath);
    m_geometry.markScreenDirty();
    m_geometry.updateScreenPoints(*m_poly.map(), lineWidth);

    m_poly.setWidth(m_geometry.sourceBoundingBox().width());
    m_poly.setHeight(m_geometry.sourceBoundingBox().height());
    m_poly.setPosition(1.0 * m_geometry.firstPointOffset() - QPointF(lineWidth * 0.5,lineWidth * 0.5));
}

QDeclarativePolylineMapItemPrivateOpenGL::~QDeclarativePolylineMapItemPrivateOpenGL() {}

QSGNode *QDeclarativePolylineMapItemPrivateOpenGL::updateMapItemPaintNode(QSGNode *oldNode,
                                                                          QQuickItem::UpdatePaintNodeData *data)
{
    Q_UNUSED(data);
    const QGeoMap *map = m_poly.map();
    const QGeoProjectionWebMercator &p = static_cast<const QGeoProjectionWebMercator&>(map->geoProjection());
    const QMatrix4x4 &combinedMatrix = p.qsgTransform();
    const QDoubleVector3D &cameraCenter = p.centerMercator();
    const QColor &color = m_poly.m_line.color();
    const float lineWidth = m_poly.m_line.width();

    MapPolylineNodeOpenGL *nodeTri = nullptr;
    if (!m_nodeTri || !oldNode) {
        if (oldNode)
            delete oldNode;
        nodeTri = new MapPolylineNodeOpenGL();
    } else {
        nodeTri = static_cast<MapPolylineNodeOpenGL *>(oldNode);
    }

    //TODO: update only material
    if (m_geometry.isScreenDirty() || m_poly.m_dirtyMaterial) {
        nodeTri->update(color,
                        lineWidth  ,
                        &m_geometry,
                        combinedMatrix,
                        cameraCenter,
                        Qt::FlatCap,
                        false,
                        m_poly.zoomForLOD(int(map->cameraData().zoomLevel())));
        m_geometry.setPreserveGeometry(false);
        m_geometry.markClean();
        m_poly.m_dirtyMaterial = false;
    }
    m_nodeTri = nodeTri;
    return nodeTri;
}


QT_END_NAMESPACE
