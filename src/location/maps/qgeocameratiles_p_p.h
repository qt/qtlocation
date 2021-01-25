/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/
#ifndef QGEOCAMERATILES_P_P_H
#define QGEOCAMERATILES_P_P_H

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

#include "qgeocameratiles_p.h"
#include <QtPositioning/private/qwebmercator_p.h>
#include <QtPositioning/private/qdoublevector2d_p.h>
#include <QtPositioning/private/qdoublevector3d_p.h>
#include "qgeomaptype_p.h"
#include "qgeocameradata_p.h"
#include "qgeotilespec_p.h"

#include <QtCore/qvector.h>
#include <QtCore/qset.h>

QT_BEGIN_NAMESPACE

struct Q_LOCATION_PRIVATE_EXPORT Frustum
{
    QDoubleVector3D apex;
    QDoubleVector3D topLeftNear;
    QDoubleVector3D topLeftFar;
    QDoubleVector3D topRightNear;
    QDoubleVector3D topRightFar;
    QDoubleVector3D bottomLeftNear;
    QDoubleVector3D bottomLeftFar;
    QDoubleVector3D bottomRightNear;
    QDoubleVector3D bottomRightFar;
};

typedef QVector<QDoubleVector3D> PolygonVector;

class Q_LOCATION_PRIVATE_EXPORT QGeoCameraTilesPrivate
{
public:
    struct ClippedFootprint
    {
        ClippedFootprint()
        {}
        ClippedFootprint(const PolygonVector &left_, const PolygonVector &mid_, const PolygonVector &right_)
            : left(left_), mid(mid_), right(right_)
        {}
        PolygonVector left;
        PolygonVector mid;
        PolygonVector right;
    };

    struct TileMap
    {
        TileMap();

        void add(int tileX, int tileY);

        QMap<int, QPair<int, int> > data;
    };

    QGeoCameraTilesPrivate();
    ~QGeoCameraTilesPrivate();


    void updateMetadata();
    void updateGeometry();

    Frustum createFrustum(double viewExpansion) const;
    PolygonVector frustumFootprint(const Frustum &frustum) const;

    QPair<PolygonVector, PolygonVector> splitPolygonAtAxisValue(const PolygonVector &polygon, int axis, double value) const;
    ClippedFootprint clipFootprintToMap(const PolygonVector &footprint) const;

    QList<QPair<double, int> > tileIntersections(double p1, int t1, double p2, int t2) const;
    QSet<QGeoTileSpec> tilesFromPolygon(const PolygonVector &polygon) const;

    static QGeoCameraTilesPrivate *get(QGeoCameraTiles *o) {
        return o->d_ptr.data();
    }

public:
    QString m_pluginString;
    QGeoMapType m_mapType;
    int m_mapVersion;
    QGeoCameraData m_camera;
    QSize m_screenSize;
    QRectF m_visibleArea;
    int m_tileSize;
    QSet<QGeoTileSpec> m_tiles;

    int m_intZoomLevel;
    int m_sideLength;
    bool m_dirtyGeometry;
    bool m_dirtyMetadata;
    double m_viewExpansion;

#ifdef QT_LOCATION_DEBUG
    // updateGeometry
    ClippedFootprint m_clippedFootprint;
    PolygonVector m_frustumFootprint;
    Frustum m_frustum;

    // createFrustum
    mutable QDoubleVector3D m_createFrustum_center;
    mutable QDoubleVector3D m_createFrustum_eye;
#endif
};

QT_END_NAMESPACE

#endif // QGEOCAMERATILES_P_P_H
