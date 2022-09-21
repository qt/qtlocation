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

#ifndef QDECLARATIVEGEOMAPITEMUTILS_P_H
#define QDECLARATIVEGEOMAPITEMUTILS_P_H

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
#include <QtLocation/private/qgeoprojection_p.h>
#include <QtPositioning/private/qdoublevector2d_p.h>


QT_BEGIN_NAMESPACE

namespace QDeclarativeGeoMapItemUtils
{
    struct vec2 {
        float x;
        float y;
        vec2(const QDoubleVector2D &p)
        {
            x = float(p.x());
            y = float(p.y());
        }
        vec2() = default;
        vec2 &operator=(const QPointF &p)
        {
            x = float(p.x());
            y = float(p.y());
            return *this;
        }
        vec2 &operator=(const QDoubleVector2D &p)
        {
            x = float(p.x());
            y = float(p.y());
            return *this;
        }
        QDoubleVector2D toDoubleVector2D() const
        {
            return QDoubleVector2D(double(x), double(y));
        }
    };

    void wrapPath(const QList<QGeoCoordinate> &perimeter
                , const QGeoCoordinate &geoLeftBound
                , const QGeoProjectionWebMercator &p
                , QList<QDoubleVector2D> &wrappedPath
                , QList<QDoubleVector2D> &wrappedPathMinus1
                , QList<QDoubleVector2D> &wrappedPathPlus1
                , QDoubleVector2D *leftBoundWrapped = nullptr);

    void wrapPath(const QList<QGeoCoordinate> &perimeter
                , const QGeoCoordinate &geoLeftBound
                , const QGeoProjectionWebMercator &p
                , QList<QDoubleVector2D> &wrappedPath
                , QDoubleVector2D *leftBoundWrapped = nullptr);

    void wrapPath(const QList<QDoubleVector2D> &path
                , const QDoubleVector2D &geoLeftBound
                , QList<QDoubleVector2D> &wrappedPath);

    void clipPolygon(const QList<QDoubleVector2D> &wrappedPath
                   , const QGeoProjectionWebMercator &p
                   , QList<QList<QDoubleVector2D> > &clippedPaths
                   , QDoubleVector2D *leftBoundWrapped = nullptr
                   , bool closed = true);

    void projectBbox(const QList<QDoubleVector2D> &clippedBbox
                   , const QGeoProjectionWebMercator &p
                   , QPainterPath &projectedBbox);

};

QT_END_NAMESPACE

#endif // QDECLARATIVEGEOMAPITEMUTILS_P_H
