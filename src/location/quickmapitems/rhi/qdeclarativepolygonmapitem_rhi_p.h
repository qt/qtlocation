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

#ifndef QDECLARATIVEPOLYGONMAPITEM_RHI_P_H
#define QDECLARATIVEPOLYGONMAPITEM_RHI_P_H

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

#include <QList>
#include <QtCore/QScopedValueRollback>
#include <QtGui/QMatrix4x4>
#include <QColor>

#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>

#include <QtPositioning/QGeoPath>
#include <QtPositioning/QGeoRectangle>
#include <QtPositioning/QGeoPolygon>

#include <QtLocation/private/qlocationglobal_p.h>
#include <QtLocation/private/qgeomapitemgeometry_p.h>
#include <QtLocation/private/qdeclarativegeomapitembase_p.h>
#include <QtLocation/private/qdeclarativepolylinemapitem_p.h>
#include <QtLocation/private/qdeclarativepolygonmapitem_p.h>
#include <QtLocation/private/qdeclarativepolygonmapitem_p_p.h>
#include <QtLocation/private/qdeclarativepolylinemapitem_rhi_p.h>
#include <QtPositioning/private/qdoublevector2d_p.h>

QT_BEGIN_NAMESPACE

class Q_LOCATION_PRIVATE_EXPORT MapPolygonMaterial : public QSGFlatColorMaterial
{
public:
    MapPolygonMaterial()
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

    int compare(const QSGMaterial *other) const override;
    QSGMaterialType *type() const override;

protected:
    QMatrix4x4 m_geoProjection;
    QDoubleVector3D m_center;
    int m_wrapOffset = 0;
};

class Q_LOCATION_PRIVATE_EXPORT MapPolygonShader : public QSGMaterialShader
{
public:
    MapPolygonShader();

    bool updateUniformData(RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect) override;
};

class Q_LOCATION_PRIVATE_EXPORT MapPolygonNodeGL : public MapItemGeometryNode
{

public:
    MapPolygonNodeGL();
    ~MapPolygonNodeGL() override;

    void update(const QColor &fillColor,
                const QGeoMapPolygonGeometryOpenGL *fillShape,
                const QMatrix4x4 &geoProjection,
                const QDoubleVector3D &center);

    MapPolygonMaterial fill_material_;
    QSGGeometry geometry_;
};

class Q_LOCATION_PRIVATE_EXPORT QDeclarativePolygonMapItemPrivateOpenGL: public QDeclarativePolygonMapItemPrivate
{
public:
    struct RootNode : public QSGNode /*QSGTransformNode*/, public VisibleNode
    {
        RootNode() { }

        bool isSubtreeBlocked() const override
        {
            return subtreeBlocked();
        }
    };

    QDeclarativePolygonMapItemPrivateOpenGL(QDeclarativePolygonMapItem &polygon)
        : QDeclarativePolygonMapItemPrivate(polygon)
    {
    }

    ~QDeclarativePolygonMapItemPrivateOpenGL() override;

    void markScreenDirtyAndUpdate()
    {
        // preserveGeometry is cleared in updateMapItemPaintNode
        m_geometry.markScreenDirty();
        m_borderGeometry.markScreenDirty();
        m_poly.polishAndUpdate();
    }
    void onLinePropertiesChanged() override
    {
        m_poly.m_dirtyMaterial = true;
        afterViewportChanged();
    }
    void markSourceDirtyAndUpdate() override
    {
        // preserveGeometry is cleared in updateMapItemPaintNode
        m_geometry.markSourceDirty();
        m_borderGeometry.markSourceDirty();
        m_poly.polishAndUpdate();
    }
    void preserveGeometry()
    {
        m_geometry.setPreserveGeometry(true, m_poly.m_geopoly.boundingGeoRectangle().topLeft());
        m_borderGeometry.setPreserveGeometry(true, m_poly.m_geopoly.boundingGeoRectangle().topLeft());
    }
    void afterViewportChanged() override // This is called when the camera changes, or visibleArea changes.
    {
        // preserveGeometry is cleared in updateMapItemPaintNode
        preserveGeometry();
        markScreenDirtyAndUpdate();
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
    void updatePolish() override;
    QSGNode * updateMapItemPaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *data) override;
    bool contains(const QPointF &point) const override;

    QGeoMapPolygonGeometryOpenGL m_geometry;
    QGeoMapPolylineGeometryOpenGL m_borderGeometry;
    RootNode *m_rootNode = nullptr;
    MapPolygonNodeGL *m_node = nullptr;
    MapPolylineNodeOpenGLExtruded *m_polylinenode = nullptr;
};


QT_END_NAMESPACE

#endif // QDECLARATIVEPOLYGONMAPITEM_RHI_P_H
