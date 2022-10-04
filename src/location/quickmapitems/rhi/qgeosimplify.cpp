/****************************************************************************
**
** Qt adaptation of geosimplify-js
** Copyright (C) 2017 Daniel Patterson
** See 3rdParty/geosimplify.js for the original license.
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

#include "qgeosimplify_p.h"
#include <QtPositioning/private/qlocationutils_p.h>
#include <QtPositioning/private/qdoublevector2d_p.h>
#include <QtPositioning/private/qwebmercator_p.h>

QT_BEGIN_NAMESPACE

namespace {

// p, a and b are intended as "unwrapped" around the left bound
inline QDoubleVector2D closestPoint(const QDoubleVector2D &p,
                                    const QDoubleVector2D &a,
                                    const QDoubleVector2D &b)
{
    if (a == b)
        return a;

    const double u = ((p.x() - a.x()) * (b.x() - a.x()) + (p.y() - a.y()) * (b.y() - a.y()))
                   / (b - a).lengthSquared();
    const QDoubleVector2D intersection(a.x() + u * (b.x() - a.x()) , a.y() + u * (b.y() - a.y()));
    QDoubleVector2D candidate = ((p - a).length() < (p - b).length()) ? a : b;
    if (u > 0 && u < 1
        && (p - intersection).length() < (p - candidate).length()) { // And it falls in the segment
        candidate = intersection;
    }
    return candidate;
}

inline double getDist(QDoubleVector2D a, QDoubleVector2D b, double leftBound)
{
    if (a.x() > 1.0)
        a.setX(a.x() - leftBound); // wrap X
    if (b.x() > 1.0)
        b.setX(b.x() - leftBound); // wrap X
    return QWebMercator::mercatorToCoord(a).distanceTo(QWebMercator::mercatorToCoord(b));
}

// doublevectors Intended as wrapped
inline double getSegDist(const QDoubleVector2D &p,
                         const QDoubleVector2D &a,
                         const QDoubleVector2D &b,
                         double leftBound)
{
    const QDoubleVector2D intersection = closestPoint(p, a, b);
    return getDist(intersection, p, leftBound);
}

inline QGeoCoordinate unwrappedToGeo(QDoubleVector2D p, double leftBound)
{
    if (p.x() > 1.0)
        p.setX(p.x() - leftBound);
    return QWebMercator::mercatorToCoord(p);
}

double pixelDistanceAtZoomAndLatitude(int zoom, double latitude)
{
    const double den = double((1 << (zoom + 8)));
    const double pixelDist = (QLocationUtils::earthMeanCircumference()
                            * std::cos(QLocationUtils::radians(latitude)))
                            / den;
    return pixelDist;
}

// simplification using Ramer-Douglas-Peucker algorithm
void simplifyDouglasPeuckerStepZL(const QList<QDoubleVector2D> &points, double leftBound,
                                  qsizetype first, qsizetype last, int zoomLevel,
                                  QList<QDoubleVector2D> &simplified)
{
    const QGeoCoordinate firstC = unwrappedToGeo(points.at(first), leftBound);
    const QGeoCoordinate lastC = unwrappedToGeo(points.at(last), leftBound);
    double maxDistanceFound = (pixelDistanceAtZoomAndLatitude(zoomLevel, firstC.latitude())
                             + pixelDistanceAtZoomAndLatitude(zoomLevel, lastC.latitude())) * 0.5;
    qsizetype index = -1;

    const auto &firstPoint = points.at(first);
    const auto &lastPoint = points.at(last);
    for (qsizetype i = first + 1; i < last; i++) {
        const double distance = getSegDist(points.at(i),
                                           firstPoint,
                                           lastPoint,
                                           leftBound);

        if (distance > maxDistanceFound) {
            index = i;
            maxDistanceFound = distance;
        }
    }

    if (index > 0) {
        if (index - first > 1) {
            simplifyDouglasPeuckerStepZL(points, leftBound,
                                         first, index, zoomLevel,
                                         simplified);
        }
        simplified.append(points.at(index));
        if (last - index > 1) {
            simplifyDouglasPeuckerStepZL(points, leftBound,
                                         index, last, zoomLevel,
                                         simplified);
        }
    }
}

} // anonymous namespace

namespace  QGeoSimplify {

QList<QDoubleVector2D> geoSimplifyZL(const QList<QDoubleVector2D> &points,
                                     double leftBound, int zoomLevel)
{
    if (points.size() <= 2)
        return points;

    const qsizetype last = points.size() - 1;
    QList<QDoubleVector2D> simplified { points.first() };
    simplifyDouglasPeuckerStepZL(points, leftBound, 0, last, zoomLevel, simplified);
    simplified.append(points.at(last));
    return simplified;
}

}

QT_END_NAMESPACE
