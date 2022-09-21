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

#ifndef QDECLARATIVEPOLYLINEMAPITEM_RHI_P_H
#define QDECLARATIVEPOLYLINEMAPITEM_RHI_P_H

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
#include <QSGMaterialShader>

#include <QtLocation/private/qlocationglobal_p.h>
#include <QtLocation/private/qdeclarativepolylinemapitem_p.h>
#include <QtLocation/private/qdeclarativepolylinemapitem_p_p.h>
#include <QtLocation/private/qgeomapitemgeometry_p.h>
#include <QtLocation/private/qgeomapitemgeometry_rhi_p.h>

#include <QtPositioning/private/qdoublevector2d_p.h>

#include <array>

QT_BEGIN_NAMESPACE

class Q_LOCATION_PRIVATE_EXPORT MapPolylineShaderLineStrip : public QSGMaterialShader
{
public:
    MapPolylineShaderLineStrip();

    bool updateUniformData(RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect) override;
};

class Q_LOCATION_PRIVATE_EXPORT MapPolylineMaterial : public QSGFlatColorMaterial
{
public:
    MapPolylineMaterial()
        : QSGFlatColorMaterial()
    {
        // Passing RequiresFullMatrix is essential in order to prevent the
        // batch renderer from baking in simple, translate-only transforms into
        // the vertex data. The shader will rely on the fact that
        // vertexCoord.xy is the Shape-space coordinate and so no modifications
        // are welcome.
        setFlag(Blending | RequiresFullMatrix);
    }

    QSGMaterialShader *createShader(QSGRendererInterface::RenderMode renderMode) const override;

    void setGeoProjection(const QMatrix4x4 &p)
    {
        m_geoProjection = p;
    }

    QMatrix4x4 geoProjection() const
    {
        return m_geoProjection;
    }

    void setCenter(const QDoubleVector3D &c)
    {
        m_center = c;
    }

    QDoubleVector3D center() const
    {
        return m_center;
    }

    int wrapOffset() const
    {
        return m_wrapOffset;
    }

    void setWrapOffset(int wrapOffset)
    {
        m_wrapOffset = wrapOffset;
    }

    void setLineWidth(float lw)
    {
        m_lineWidth = lw;
    }

    float lineWidth() const
    {
        return m_lineWidth;
    }

    QSGMaterialType *type() const override;
    int compare(const QSGMaterial *other) const override;

protected:
    QMatrix4x4 m_geoProjection;
    QDoubleVector3D m_center;
    int m_wrapOffset = 0;
    float m_lineWidth = 1.0;
};


class Q_LOCATION_PRIVATE_EXPORT MapPolylineShaderExtruded : public QSGMaterialShader
{
public:
    MapPolylineShaderExtruded();

    bool updateUniformData(RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect) override;
};

class Q_LOCATION_PRIVATE_EXPORT MapPolylineNodeOpenGLLineStrip : public MapItemGeometryNode
{
public:
    MapPolylineNodeOpenGLLineStrip();
    ~MapPolylineNodeOpenGLLineStrip() override;

    void update(const QColor &fillColor,
                const qreal lineWidth,
                const QGeoMapPolylineGeometryOpenGL *shape,
                const QMatrix4x4 &geoProjection,
                const QDoubleVector3D &center,
                const Qt::PenCapStyle capStyle = Qt::SquareCap);

protected:
    MapPolylineMaterial fill_material_;
    QSGGeometry geometry_;
};

class Q_LOCATION_PRIVATE_EXPORT MapPolylineMaterialExtruded : public MapPolylineMaterial
{
public:
    MapPolylineMaterialExtruded() : MapPolylineMaterial()
    {

    }
    QSGMaterialShader *createShader(QSGRendererInterface::RenderMode renderMode) const override;

    void setMiter(int m)
    {
        m_miter = m;
    }

    int miter() const
    {
        return m_miter;
    }

    QSGMaterialType *type() const override;
    int compare(const QSGMaterial *other) const override;

    int m_miter = 0;
};

class Q_LOCATION_PRIVATE_EXPORT MapPolylineNodeOpenGLExtruded : public MapItemGeometryNode
{
public:

    typedef struct MapPolylineEntry {
         QDeclarativeGeoMapItemUtils::vec2 pos;
         QDeclarativeGeoMapItemUtils::vec2 prev;
         QDeclarativeGeoMapItemUtils::vec2 next;
         float direction;
         float triangletype; // es2 does not support int attribs
         float vertextype;

         static const QSGGeometry::AttributeSet &attributes()
         {
             static const QSGGeometry::Attribute dataTri[] = {
                 QSGGeometry::Attribute::createWithAttributeType(0, 2,
                    QSGGeometry::FloatType, QSGGeometry::PositionAttribute) // pos
                 ,QSGGeometry::Attribute::createWithAttributeType(1, 2,
                    QSGGeometry::FloatType, QSGGeometry::UnknownAttribute) // next
                 ,QSGGeometry::Attribute::createWithAttributeType(2, 2,
                    QSGGeometry::FloatType, QSGGeometry::UnknownAttribute) // previous
                 ,QSGGeometry::Attribute::createWithAttributeType(3, 1,
                    QSGGeometry::FloatType, QSGGeometry::UnknownAttribute)  // direction
                 ,QSGGeometry::Attribute::createWithAttributeType(4, 1,
                    QSGGeometry::FloatType, QSGGeometry::UnknownAttribute)  // triangletype
                 ,QSGGeometry::Attribute::createWithAttributeType(5, 1,
                    QSGGeometry::FloatType, QSGGeometry::UnknownAttribute)  // vertextype
             };
             static const QSGGeometry::AttributeSet attrsTri = {
                6,
                sizeof(MapPolylineNodeOpenGLExtruded::MapPolylineEntry),
                dataTri
            };
             return attrsTri;
         }
    } MapPolylineEntry;

    MapPolylineNodeOpenGLExtruded();
    ~MapPolylineNodeOpenGLExtruded() override;

    void update(const QColor &fillColor,
                float lineWidth,
                const QGeoMapPolylineGeometryOpenGL *shape,
                const QMatrix4x4 &geoProjection,
                const QDoubleVector3D &center,
                const Qt::PenCapStyle capStyle = Qt::FlatCap,
                bool closed = false,
                unsigned int zoom = 30);

    static const QSGGeometry::AttributeSet &attributesMapPolylineTriangulated();

protected:
    MapPolylineMaterialExtruded fill_material_;
    QSGGeometry m_geometryTriangulating;
};

class Q_LOCATION_PRIVATE_EXPORT QDeclarativePolylineMapItemPrivateOpenGLLineStrip: public QDeclarativePolylineMapItemPrivate
{
public:

    QDeclarativePolylineMapItemPrivateOpenGLLineStrip(QDeclarativePolylineMapItem &poly)
        : QDeclarativePolylineMapItemPrivate(poly)
    {
    }

    ~QDeclarativePolylineMapItemPrivateOpenGLLineStrip() override;
    void onLinePropertiesChanged() override
    {
        afterViewportChanged();
    }
    void markSourceDirtyAndUpdate() override
    {
        m_geometry.markSourceDirty();
        m_poly.polishAndUpdate();
    }
    void preserveGeometry()
    {
        m_geometry.setPreserveGeometry(true, m_poly.m_geopath.boundingGeoRectangle().topLeft());
    }
    void onMapSet() override
    {
        markSourceDirtyAndUpdate();
    }
    void onGeoGeometryChanged() override
    {
        preserveGeometry();
        markSourceDirtyAndUpdate();
    }
    void onGeoGeometryUpdated() override
    {
        preserveGeometry();
        markSourceDirtyAndUpdate();
    }
    void onItemGeometryChanged() override
    {
        onGeoGeometryChanged();
    }
    void afterViewportChanged() override
    {
        preserveGeometry();
        m_poly.polishAndUpdate();
    }
    bool contains(const QPointF &point) const override;
    void updatePolish() override;
    QSGNode * updateMapItemPaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *data) override;

    QGeoMapPolylineGeometryOpenGL m_geometry;
    MapPolylineNodeOpenGLLineStrip *m_node = nullptr;
};

class Q_LOCATION_PRIVATE_EXPORT QDeclarativePolylineMapItemPrivateOpenGLExtruded: public QDeclarativePolylineMapItemPrivateOpenGLLineStrip
{
public:

    QDeclarativePolylineMapItemPrivateOpenGLExtruded(QDeclarativePolylineMapItem &poly)
        : QDeclarativePolylineMapItemPrivateOpenGLLineStrip(poly)
    {
    }

    ~QDeclarativePolylineMapItemPrivateOpenGLExtruded() override;

    QSGNode * updateMapItemPaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *data) override;

    MapPolylineNodeOpenGLExtruded *m_nodeTri = nullptr;
};

QT_END_NAMESPACE

#endif // QDECLARATIVEPOLYLINEMAPITEM_RHI_P_H
