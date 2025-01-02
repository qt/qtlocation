// Copyright (C) 2020 Paolo Angelelli <paolo.angelelli@gmail.com>
// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qdeclarativegeomapitemutils_p.h"

#include <QPointF>
#include <QMatrix4x4>
#include <QPainterPath>
#include <QPainterPathStroker>
#include <QtPositioning/QGeoCoordinate>

#include <QtPositioning/private/qclipperutils_p.h>

QT_BEGIN_NAMESPACE

namespace QDeclarativeGeoMapItemUtils {

double distanceSqrPointLine(double p0_x
                          , double p0_y
                          , double p1_x
                          , double p1_y
                          , double p2_x
                          , double p2_y)
{
    const double t_x = p2_x - p1_x;
    const double t_y = p2_y - p1_y;
    const double p_x = p0_x - p1_x;
    const double p_y = p0_y - p1_y;
    const double tsqr = t_x * t_x + t_y * t_y;

    if (tsqr == 0)
        return qInf();

    double alpha = (p_x * t_x + p_y * t_y) / tsqr;
    alpha = qBound<double>(0, alpha, 1);

    const double dx = p_x - t_x * alpha;
    const double dy = p_y - t_y * alpha;

    return dx * dx + dy * dy;
}

void wrapPath(const QList<QGeoCoordinate> &perimeter,
              const QGeoCoordinate &geoLeftBound,
              const QGeoProjectionWebMercator &p,
              QList<QDoubleVector2D> &wrappedPath,
              QList<QDoubleVector2D> &wrappedPathMinus1,
              QList<QDoubleVector2D> &wrappedPathPlus1,
              QDoubleVector2D *leftBoundWrapped)
{
    QList<QDoubleVector2D> path;
    for (const QGeoCoordinate &c : perimeter)
        path << p.geoToMapProjection(c);
    const QDoubleVector2D leftBound = p.geoToMapProjection(geoLeftBound);
    wrappedPath.clear();
    wrappedPathPlus1.clear();
    wrappedPathMinus1.clear();
    // compute 3 sets of "wrapped" coordinates: one w regular mercator, one w regular mercator +- 1.0
    for (QDoubleVector2D coord : path) {
        // We can get NaN if the map isn't set up correctly, or the projection
        // is faulty -- probably best thing to do is abort
        if (!qIsFinite(coord.x()) || !qIsFinite(coord.y()))
            return;

        const bool isPointLessThanUnwrapBelowX = (coord.x() < leftBound.x());
        // unwrap x to preserve geometry if moved to border of map
        if (isPointLessThanUnwrapBelowX)
            coord.setX(coord.x() + 1.0);

        QDoubleVector2D coordP1(coord.x() + 1.0, coord.y());
        QDoubleVector2D coordM1(coord.x() - 1.0, coord.y());

        wrappedPath.append(coord);
        wrappedPathPlus1.append(coordP1);
        wrappedPathMinus1.append(coordM1);
    }
    if (leftBoundWrapped)
        *leftBoundWrapped = leftBound;
}

void wrapPath(const QList<QGeoCoordinate> &perimeter,
              const QGeoCoordinate &geoLeftBound,
              const QGeoProjectionWebMercator &p,
              QList<QDoubleVector2D> &wrappedPath,
              QDoubleVector2D *leftBoundWrapped)
{
    QList<QDoubleVector2D> path;
    for (const QGeoCoordinate &c : perimeter)
        path << p.geoToMapProjection(c);
    const QDoubleVector2D leftBound = p.geoToMapProjection(geoLeftBound);
    wrapPath(path, leftBound,wrappedPath);
    if (leftBoundWrapped)
        *leftBoundWrapped = leftBound;
}

void wrapPath(const QList<QDoubleVector2D> &path,
              const QDoubleVector2D &geoLeftBound,
              QList<QDoubleVector2D> &wrappedPath)
{
    wrappedPath.clear();
    // compute 3 sets of "wrapped" coordinates: one w regular mercator, one w regular mercator +- 1.0
    for (QDoubleVector2D coord : path) {
        // We can get NaN if the map isn't set up correctly, or the projection
        // is faulty -- probably best thing to do is abort
        if (!qIsFinite(coord.x()) || !qIsFinite(coord.y()))
            return;

        const bool isPointLessThanUnwrapBelowX = (coord.x() < geoLeftBound.x());
        // unwrap x to preserve geometry if moved to border of map
        if (isPointLessThanUnwrapBelowX)
            coord.setX(coord.x() + 1.0);

        wrappedPath.append(coord);
    }
}

void clipPolygon(const QList<QDoubleVector2D> &wrappedPath,
                 const QGeoProjectionWebMercator &p,
                 QList<QList<QDoubleVector2D>> &clippedPaths,
                 QDoubleVector2D *leftBoundWrapped,
                 bool closed)
{
    // 2) Clip bounding box
    clippedPaths.clear();
    const QList<QDoubleVector2D> &visibleRegion = p.projectableGeometry();
    if (visibleRegion.size()) {
        QClipperUtils clipper;
        clipper.addSubjectPath(wrappedPath, closed);
        clipper.addClipPolygon(visibleRegion);
        clippedPaths = clipper.execute(QClipperUtils::Intersection, QClipperUtils::pftEvenOdd,
                                       QClipperUtils::pftEvenOdd);

        if (leftBoundWrapped) {
            // 2.1) update srcOrigin_ and leftBoundWrapped with the point with minimum X
            QDoubleVector2D lb(qInf(), qInf());
            for (const QList<QDoubleVector2D> &path : clippedPaths) {
                for (const QDoubleVector2D &p : path) {
                    if (p.x() < lb.x() || (p.x() == lb.x() && p.y() < lb.y()))
                        // y-minimization needed to find the same point on polygon and border
                        lb = p;
                }
            }

            if (qIsInf(lb.x())) // e.g., when the polygon is clipped entirely
                return;

            // 2.2) Prevent the conversion to and from clipper from introducing tiny negative offsets which,
            //      in turn will make the geometry wrap around.
            lb.setX(qMax(leftBoundWrapped->x(), lb.x()));

            *leftBoundWrapped = lb;
            // srcOrigin_ = p.mapProjectionToGeo(p.unwrapMapProjection(lb));
        }
    } else {
        clippedPaths.append(wrappedPath);
    }
}

void projectBbox(const QList<QDoubleVector2D> &clippedBbox,
                 const QGeoProjectionWebMercator &p,
                 QPainterPath &projectedBbox)
{
    projectedBbox.clear();
    bool first = true;
    for (const auto &coord : clippedBbox) {
        QDoubleVector2D point = p.wrappedMapProjectionToItemPosition(coord);
        if (first) {
            first = false;
            projectedBbox.moveTo(point.toPointF());
        } else {
            projectedBbox.lineTo(point.toPointF());
        }
    }
    projectedBbox.closeSubpath();
}


QRectF boundingRectangleFromList(const QList<QDoubleVector2D> &list)
{
    double xMin = qInf();
    double xMax = -qInf();
    double yMin = qInf();
    double yMax = -qInf();
    for (const auto &coord : list) {
        xMin = qMin(xMin, coord.x());
        xMax = qMax(xMax, coord.x());
        yMin = qMin(yMin, coord.y());
        yMax = qMax(yMax, coord.y());
    }
    return QRectF(xMin, yMin, xMax - xMin, yMax - yMin);
}

} // namespace QDeclarativeGeoMapItemUtils

QT_END_NAMESPACE
