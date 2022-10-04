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
