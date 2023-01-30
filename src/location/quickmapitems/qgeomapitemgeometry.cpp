// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qgeomapitemgeometry_p.h"
#include "qdeclarativegeomap_p.h"
#include <QtQuick/QSGGeometry>

#include <QtPositioning/private/qlocationutils_p.h>
#include <QtPositioning/private/qdoublevector2d_p.h>
#include <QtLocation/private/qgeomap_p.h>
#include <QtLocation/private/qgeoprojection_p.h>

QT_BEGIN_NAMESPACE

QGeoMapItemGeometry::QGeoMapItemGeometry()
{
}

QGeoMapItemGeometry::~QGeoMapItemGeometry()
{

}

/*!
    \internal
*/
void QGeoMapItemGeometry::translate(const QPointF &offset)
{
    for (qsizetype i = 0; i < screenVertices_.size(); ++i)
        screenVertices_[i] += offset;

    firstPointOffset_ += offset;
    screenOutline_.translate(offset);
    screenBounds_.translate(offset);
}

/*!
    \internal
*/
void QGeoMapItemGeometry::allocateAndFill(QSGGeometry *geom) const
{
    const QList<QPointF> &vx = screenVertices_;
    const QList<quint32> &ix = screenIndices_;

    if (isIndexed()) {
        geom->allocate(vx.size(), ix.size());
        if (geom->indexType() == QSGGeometry::UnsignedShortType) {
            quint16 *its = geom->indexDataAsUShort();
            for (qsizetype i = 0; i < ix.size(); ++i)
                its[i] = ix[i];
        } else if (geom->indexType() == QSGGeometry::UnsignedIntType) {
            quint32 *its = geom->indexDataAsUInt();
            for (qsizetype i = 0; i < ix.size(); ++i)
                its[i] = ix[i];
        }
    } else {
        geom->allocate(vx.size());
    }

    QSGGeometry::Point2D *pts = geom->vertexDataAsPoint2D();
    for (qsizetype i = 0; i < vx.size(); ++i)
        pts[i].set(vx[i].x(), vx[i].y());
}

/*!
    \internal
*/
QRectF QGeoMapItemGeometry::translateToCommonOrigin(const QList<QGeoMapItemGeometry *> &geoms)
{
    QGeoCoordinate origin = geoms.at(0)->origin();

    QPainterPath brects;

    // first get max offset
    QPointF maxOffset = geoms.at(0)->firstPointOffset();
    for (const QGeoMapItemGeometry *g : geoms) {
        QPointF o = g->firstPointOffset();
        maxOffset.setX(qMax(o.x(), maxOffset.x()));
        maxOffset.setY(qMax(o.y(), maxOffset.y()));
    }

    // then translate everything
    for (QGeoMapItemGeometry *g : geoms) {
        g->translate(maxOffset - g->firstPointOffset());
        brects.addRect(g->sourceBoundingBox());
    }

    return brects.boundingRect();
}

QT_END_NAMESPACE
