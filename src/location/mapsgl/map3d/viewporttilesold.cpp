/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "viewporttilesold_p.h"

#include "sphere_p.h"
#include "tilespec.h"
#include "viewportcamera_p.h"

#include <QLinkedList>
#include <QVector2D>

#include <cmath>

ViewportTilesPrivateOld::ViewportTilesPrivateOld() {}

ViewportTilesPrivateOld::~ViewportTilesPrivateOld() {}

QList<TileSpec> ViewportTilesPrivateOld::visibleTiles(const ViewportCamera &viewportCamera) const
{
    Frustum frustum = viewportCamera.toFrustum();

    int lineIntersections = 0;

    QList<Arc> points = pointIntersections(frustum, &lineIntersections);

    if (lineIntersections < 3) {
        return visibleTilesFromPlanes(viewportCamera);
    }

    return tilesFromSphere(sphereFromArcs(points));
}

Sphere ViewportTilesPrivateOld::sphereFromArcs(QList<Arc> &arcs) const
{
    QList<int> zeroIndices = addDatelineCrossings(arcs);

    int zeroes = zeroIndices.size();

    if (zeroes == 0) {
        QList<SpherePatch> patches;
        patches << SpherePatch(sphere().radius(), arcs, false);
        return Sphere(sphere().radius(), patches);
    } else if (zeroes == 1) {
        return sphereForPole(arcs, zeroIndices.at(0));
    } else if (zeroes == 2) {
        return sphereForDateline(arcs, zeroIndices.at(0), zeroIndices.at(1));
    } else {
        qWarning() << "zeroes: " << zeroes;
        return Sphere(sphere().radius());
    }
}

/*
  We assume we a have a polygon with a pole in the middle, and a point on the dateline.

  We split the polygon into two, and then add copies of each point shifted up into the
  tile in the first or last row of tiles (for the north or south pole respectively) in
  reverse order.

  This is pretty buggy at present.
*/
Sphere ViewportTilesPrivateOld::sphereForPole(const QList<Arc> &arcs, int zeroIndex1) const
{
//    QVector3D northPole = QVector3D(0, sphere().radius(), 0);
//    QVector3D southPole = QVector3D(0, -1.0 * sphere().radius(), 0);

//    qWarning() << "north";
//    for (int i = 0; i < size; ++i) {
//        qWarning() << "  " << arcs.at(i).plane().distanceTo(northPole);
//    }

//    qWarning() << "south";
//    for (int i = 0; i < size; ++i) {
//        qWarning() << "  " << arcs.at(i).plane().distanceTo(southPole);
//    }

    int size = arcs.size();
    double y = 0.0;
    for (int i = 0; i < size; ++i) {
        y += arcs.at(i).start().y();
    }

    int z = 1 << zoomLevel();
    double newY = 0.5 / z;
    if (y < 0.0)
        newY = (z - 0.5) / z;

    QPlane3D datelinePlane(QVector3D(0.0, 0.0, 0.0),
                           QVector3D(0.0, 0.0, 1.0));

    double topY = sphere().mercatorToPoint(0, newY).y();

    QPlane3D topPlane(QVector3D(0.0, topY, 0.0),
                      QVector3D(0.0, -1.0, 0.0));

    QList<SpherePatch> patches;

    QList<Arc> patch1;

    for (int i = zeroIndex1; i < zeroIndex1 + size; ++i) {
        int index = i % size;
        patch1 << arcs.at(index);
    }

    QList<Arc> patch2;

    for (int i = zeroIndex1; i < zeroIndex1 + size; ++i) {
        int index = size - (i % size) - 1;

        QVector2D start = sphere().pointToMercator(arcs.at(index).end());
        QVector2D end = sphere().pointToMercator(arcs.at(index).start());

        QVector3D start2 = sphere().mercatorToPoint(start.x(), newY);
        QVector3D end2 = sphere().mercatorToPoint(end.x(), newY);

        Arc a = Arc(topPlane, sphere().radius(), start2, end2);

        patch2 << a;
    }

    Arc endCap1 = Arc(datelinePlane, sphere().radius(), patch1.last().end(), patch2.first().start());
    Arc endCap2 = Arc(datelinePlane, sphere().radius(), patch2.last().end(), patch1.first().start());

    QList<Arc> patch;

    patch.append(patch1);
    patch.append(endCap1);
    patch.append(patch2);
    patch.append(endCap2);

    patches << SpherePatch(sphere().radius(), patch, false);

    return Sphere(sphere().radius(), patches);
}

//  This assumes that addDatelineCrossings has already been called, in which
//  case the polygon points will have points on the dateline instead of lines
//    crossing the dateline.

//  In that case we just need to split the polygon into two - one before the dateline
//  and one after the dateline - and add the visible tiles for both polygons.
Sphere ViewportTilesPrivateOld::sphereForDateline(const QList<Arc> &arcs, int zeroIndex1, int zeroIndex2) const
{
    QList<SpherePatch> patches;

    int size = arcs.size();

    QList<Arc> lines1;
    bool roundUp1 = false;
    for (int i = zeroIndex1; i < zeroIndex2; ++i) {
        Arc v = arcs.at(i);
        if ((i != zeroIndex1) && (v.start().x() < 0))
            roundUp1 = true;
        lines1.append(v);
    }

    QPlane3D datelinePlane(QVector3D(0.0, 0.0, 0.0),
                           QVector3D(0.0, 0.0, 1.0));

    lines1 << Arc(datelinePlane, sphere().radius(), lines1.last().end(), lines1.first().start());

    patches << SpherePatch(sphere().radius(), lines1, roundUp1);

    QList<Arc> lines2;
    bool roundUp2 = false;
    for (int i = zeroIndex2; i < zeroIndex1 + size; ++i) {
        int index = i % size;
        Arc v = arcs.at(index);
        if ((i != zeroIndex2) && (v.start().x() < 0))
            roundUp2 = true;
        lines2.append(v);
    }

    lines2 << Arc(datelinePlane, sphere().radius(), lines2.last().end(), lines2.first().start());

    patches << SpherePatch(sphere().radius(), lines2, roundUp2);

    return Sphere(sphere().radius(), patches);
}

QList<int> ViewportTilesPrivateOld::addDatelineCrossings(QList<Arc> &points) const
{
    QList<int> results;

    for (int i = 0; i < points.size(); ++i) {
        Arc p = points.at(i);
        QPair<double, double> factors = p.xIntersectFactor(0.0);

        bool found = false;
        double tf;
        double te = 0.0;

        if ((factors.first > 0.0) && (factors.first < 1.0)) {
            QVector3D v = p.interpolate(factors.first);
            if ((qAbs(v.x()) / sphere().radius() < 1e-6) && (v.z() >= 0.0)) {
                found = true;
                tf = factors.first;
                te = qAbs(v.x());
            }
        }

        if ((factors.second > 0.0) && (factors.second < 1.0)) {
            QVector3D v = p.interpolate(factors.second);
            if ((qAbs(v.x()) / sphere().radius() < 1e-6) && (v.z() >= 0.0)) {
                if (!found || (qAbs(v.x()) < te)) {
                    found = true;
                    tf = factors.second;
                }
            }
        }

//        for (int j = 0; j < factors.size(); ++j) {
//            double n = factors.at(j);
//            if ((n == 0.0) || (n == 1.0))
//                continue;

//            QVector3D v = p.interpolate(n);

//            double relativeError = qAbs(v.x() / sphere().radius());

////            qWarning() << "    checked " << i << n << v.x() << v.z() << relativeError;

//            if ((relativeError < 1e-6) && (v.z() >= 0)) {
//                double e = qAbs(v.x());
//                if (!found || (e < te)) {
//                    tf = n;
//                    te = e;
//                    found = true;
//                }
//            }
//        }

        if (found) {
            QVector3D v = p.interpolate(tf);

            qWarning() << "    found " << tf << v.x() << v.z();

                Arc q = p;
                p.setEnd(v);
                points[i] = p;
                q.setStart(v);
                points.insert(i + 1, q);
                ++i;
                results << i;
        }
    }

    return results;

//        QVector3D s = points.at(i)
//        QVector3D e = points.at((i + 1) % points.size());
//        if ((s.x() != 0.0) && (e.x() != 0) && (s.x() != e.x())) {
//            double n = s.x() / (s.x() - e.x());
//            QVector3D v = (1.0 - n) * s + n * e;
//            if ((v.z() >= 0.0) && (0.0 <= n) && (n <= 1.0))
//                points.insert(i + 1, v);
//        }
}

bool ViewportTilesPrivateOld::LengthSorter::operator()(const IntersectionPoint &lhs, const IntersectionPoint &rhs)
{
    bool lPlane = (lhs.planes & base.planes);
    bool rPlane = (rhs.planes & base.planes);
    if (lPlane == rPlane)
        return (lhs.point - base.point).lengthSquared() < (rhs.point - base.point).lengthSquared();
    else
        return lPlane;
}

/*
    We find all of the points of intersection between the earth sphere and
    the lines along the edges of the view frustum.

    We then sort them so that form a polygon.  The last point becomes
    the first result.  We then sort the points by distance^2 from the last
    result, and the nearest point becomes the last result.
*/
QList<Arc> ViewportTilesPrivateOld::pointIntersections(const Frustum &frustum, int *linesIntersected) const
{
    QList<ViewportTilesPrivateOld::IntersectionPoint> points;

    if (linesIntersected)
        *linesIntersected = 0;

    double radius = sphere().radius();

    QList<ViewportTilesPrivateOld::IntersectionPoint> v;

    v = lineIntersectsSphere(radius,
                             frustum.topLeftNear(),
                             frustum.topLeftFar(),
                             Frustum::TopLeftNear,
                             Frustum::TopLeftFar);
    if (linesIntersected && !v.isEmpty())
        ++(*linesIntersected);
    points.append(v);

    v = lineIntersectsSphere(radius,
                             frustum.topRightNear(),
                             frustum.topRightFar(),
                             Frustum::TopRightNear,
                             Frustum::TopRightFar);
    if (linesIntersected && !v.isEmpty())
        ++(*linesIntersected);
    points.append(v);

    v = lineIntersectsSphere(radius,
                             frustum.bottomRightNear(),
                             frustum.bottomRightFar(),
                             Frustum::BottomRightNear,
                             Frustum::BottomRightFar);
    if (linesIntersected && !v.isEmpty())
        ++(*linesIntersected);
    points.append(v);

    v = lineIntersectsSphere(radius,
                             frustum.bottomLeftNear(),
                             frustum.bottomLeftFar(),
                             Frustum::BottomLeftNear,
                             Frustum::BottomLeftFar);
    if (linesIntersected && !v.isEmpty())
        ++(*linesIntersected);
    points.append(v);

    v = lineIntersectsSphere(radius,
                             frustum.topLeftFar(),
                             frustum.topRightFar(),
                             Frustum::TopLeftFar,
                             Frustum::TopRightFar);
    if (linesIntersected && !v.isEmpty())
        ++(*linesIntersected);
    points.append(v);

    v = lineIntersectsSphere(radius,
                             frustum.topRightFar(),
                             frustum.bottomRightFar(),
                             Frustum::TopRightFar,
                             Frustum::BottomRightFar);
    if (linesIntersected && !v.isEmpty())
        ++(*linesIntersected);
    points.append(v);

    v = lineIntersectsSphere(radius,
                             frustum.bottomRightFar(),
                             frustum.bottomLeftFar(),
                             Frustum::BottomRightFar,
                             Frustum::BottomLeftFar);
    if (linesIntersected && !v.isEmpty())
        ++(*linesIntersected);
    points.append(v);

    v = lineIntersectsSphere(radius,
                             frustum.bottomLeftFar(),
                             frustum.topLeftFar(),
                             Frustum::BottomLeftFar,
                             Frustum::TopLeftFar);
    if (linesIntersected && !v.isEmpty())
        ++(*linesIntersected);
    points.append(v);

    v = lineIntersectsSphere(radius,
                             frustum.topLeftNear(),
                             frustum.topRightNear(),
                             Frustum::TopLeftNear,
                             Frustum::TopRightNear);
    if (linesIntersected && !v.isEmpty())
        ++(*linesIntersected);
    points.append(v);

    v = lineIntersectsSphere(radius,
                             frustum.topRightNear(),
                             frustum.bottomRightNear(),
                             Frustum::TopRightNear,
                             Frustum::BottomRightNear);
    if (linesIntersected && !v.isEmpty())
        ++(*linesIntersected);
    points.append(v);

    v = lineIntersectsSphere(radius,
                             frustum.bottomRightNear(),
                             frustum.bottomLeftNear(),
                             Frustum::BottomRightNear,
                             Frustum::BottomLeftNear);
    if (linesIntersected && !v.isEmpty())
        ++(*linesIntersected);
    points.append(v);

    v = lineIntersectsSphere(radius,
                             frustum.bottomLeftNear(),
                             frustum.topLeftNear(),
                             Frustum::BottomLeftNear,
                             Frustum::TopLeftNear);
    if (linesIntersected && !v.isEmpty())
        ++(*linesIntersected);
    points.append(v);

    QList<Arc> results;

    if (points.isEmpty())
        return results;

    LengthSorter sorter;
    sorter.base = points.first();
    qSort(points.begin(), points.end(), sorter);
    for (int i = points.size() - 1; i > 0; --i) {
        if (points.at(i).point == points.at(i - 1).point)
            points.removeAt(i);
    }

    QList<ViewportTilesPrivateOld::IntersectionPoint>::iterator i;
    for (i = points.begin(); i != points.end(); ++i) {
        sorter.base = *i;
        if (i + 1 != points.end())
            qSort(i + 1, points.end(), sorter);
    }

    QVector3D origin;


    int size = points.size();
    for (int i = 0; i < size; ++i) {
        ViewportTilesPrivateOld::IntersectionPoint a = points.at(i);
        ViewportTilesPrivateOld::IntersectionPoint b = points.at((i + 1) % size);


        QPlane3D plane = frustum.plane(a.planes & b.planes);
        if (plane.origin() != origin)
            results << Arc(plane, sphere().radius(), a.point, b.point);

//        if (plane.origin() == origin) {
//            result.center = origin;
//            result.radius = sphere_.radius();
//        } else {
//            results << Arc(plane, sphere_.radius(), a.point, b.point);
//            result.center = -1.0 * plane.distanceTo(origin) * plane.normal().normalized();
//            result.radius = sqrt(sphere_.radius() * sphere_.radius() - result.center().lengthSquared());
//        }
//        result.setStart(a.point);
//        result.setEnd(b.point);

//        results << result;
    }

    return results;
}

/*
  This returns the points along the line from start to end that intersect a sphere with radius \a radius.

  It returns the intersection in order of proximity to start.
*/
QList<ViewportTilesPrivateOld::IntersectionPoint> ViewportTilesPrivateOld::lineIntersectsSphere(double radius,
                                                                  const QVector3D &start,
                                                                  const QVector3D &end,
                                                                  Frustum::Planes planesStart,
                                                                  Frustum::Planes planesEnd)
{
    QVector3D v = end - start;

    double maxN = v.length();

    v.normalize();

    double b = 2 * QVector3D::dotProduct(start, v);
    double c = start.lengthSquared() - (radius * radius);

    double s = (b * b) - (4 * c);

    if (s < 0)
        return QList<ViewportTilesPrivateOld::IntersectionPoint>();

    double s2 = sqrt(s);

    double n1 = -1.0 * (b - s2) / 2.0;
    if (n1 > maxN)
        n1 = -1.0;
    if (n1 < 0)
        n1 = -1.0;

    double n2 = -1.0 * (b + s2) / 2.0;
    if (n2 > maxN)
        n2 = -1.0;
    if (n2 < 0)
        n2 = -1.0;

    if (n1 < 0.0) {
        if (n2 >= 0)
            n1 = n2;
    } else if (n2 < 0) {
        n2 = n1;
    } else {
        if (n2 < n1)
            qSwap(n1, n2);
    }

    QList<ViewportTilesPrivateOld::IntersectionPoint> results;

    if (n1 != -1.0) {
        ViewportTilesPrivateOld::IntersectionPoint r1;

        r1.point = interpolate(start, end, n1);

        if (r1.point == start)
            r1.planes = planesStart;
        else if (r1.point == end)
            r1.planes = planesEnd;
        else
            r1.planes = (planesStart & planesEnd);
        results << r1;

        if ((n2 != -1.0) && (n2 != n1)) {
            ViewportTilesPrivateOld::IntersectionPoint r2;

            r2.point = interpolate(start, end, n2);

            if (r2.point == start)
                r2.planes = planesStart;
            else if (r2.point == end)
                r2.planes = planesEnd;
            else
                r2.planes = (planesStart & planesEnd);
            results << r2;
        }
    }

    return results;
}

QVector3D ViewportTilesPrivateOld::interpolate(const QVector3D &start, const QVector3D &end, double n)
{
    QVector3D v = end - start;
    v.normalize();
    return start + n * v;
}

QPair<QVector3D, double> ViewportTilesPrivateOld::tileInfo(const TileSpec &spec) const
{
    int z = 1 << spec.zoom();

    double xl = spec.x() * 1.0 / z;
    double xc = (spec.x() + 0.5) / z;
    double xr = ((spec.x() + 1) % z * 1.0) / z;
    double yt = (spec.y() * 1.0) / z;
    double yc = (spec.y() + 0.5) / z;
    double yb = ((spec.y() + 1) * 1.0) / z;

    QVector3D tl = sphere().mercatorToPoint(xl, yt);
    QVector3D br = sphere().mercatorToPoint(xr, yb);
    QVector3D c = sphere().mercatorToPoint(xc, yc);

    double r = sqrt(qMax((tl - c).lengthSquared(), (br - c).lengthSquared()));

    return QPair<QVector3D, double>(c, 2 * r);
}

/*
  This is the least good of the functions in here.

  It's used when we don't have enough of a frustum intersect to determine which
  tiles to fetch analytically.

  It treats tiles as spheres and then determines if the sphere is inside
  the frustum.  If it is, it considers the neighbours of the tile.

  There are smarter ways to do this via plane intersection (hence the
  function name).  I'll revisit this soon.
*/
QList<TileSpec> ViewportTilesPrivateOld::visibleTilesFromPlanes(const ViewportCamera &viewportCamera) const
{
    QList<TileSpec> results;

    QVector3D center = viewportCamera.center();
    QVector3D eye = viewportCamera.eye();

    QVector3D v = center - eye;
    v.normalize();

    QVector2D t = sphere().pointToTile(center, zoomLevel());

    QLinkedList<TileSpec> queue;
    QSet<TileSpec> queued;
    QSet<TileSpec> done;

    int zoom = zoomLevel();

    queue << TileSpec(0, zoom, t.x(), t.y());
    queued << TileSpec(0, zoom, t.x(), t.y());

    int z = 1 << zoom;

    Frustum frustum = viewportCamera.toFrustum();

    while (!queue.isEmpty()) {
        TileSpec spec = queue.takeLast();
        QPair<QVector3D, double> info = tileInfo(spec);
        if (frustum.contains(info.first, info.second) && (QVector3D::dotProduct(v, info.first - eye) > 0)) {
            results << spec;

            QList<TileSpec> tiles;

            int xp = spec.x() - 1;
            if (xp < 0)
                xp = z - 1;
            int xn = (spec.x() + 1) % z;

            tiles << TileSpec(0, zoom, xp, spec.y());
            tiles << TileSpec(0, zoom, xn, spec.y());

            if (spec.y() == 0) {
                int x2 = (spec.x() + z / 2) % z;
                int xp2 = ((spec.x() + z / 2 - 1) % z);
                if (xp2 < 0)
                    xp2 += z;
                int xn2 = ((spec.x() + z / 2 + 1) % z);
                tiles << TileSpec(0, zoom, xp2, spec.y());
                tiles << TileSpec(0, zoom, x2, spec.y());
                tiles << TileSpec(0, zoom, xn2, spec.y());
            } else {
                tiles << TileSpec(0, zoom, xp, spec.y() - 1);
                tiles << TileSpec(0, zoom, spec.x(), spec.y() - 1);
                tiles << TileSpec(0, zoom, xn, spec.y() - 1);
            }

            if (spec.y() == (z - 1)) {
                int x2 = (spec.x() + z / 2) % z;
                int xp2 = ((spec.x() + z / 2 - 1) % z);
                if (xp2 < 0)
                    xp2 += z;
                int xn2 = ((spec.x() + z / 2 + 1) % z);
                tiles << TileSpec(0, zoom, xp2, spec.y());
                tiles << TileSpec(0, zoom, x2, spec.y());
                tiles << TileSpec(0, zoom, xn2, spec.y());
            } else {
                tiles << TileSpec(0, zoom, xp, spec.y() + 1);
                tiles << TileSpec(0, zoom, t.x(), spec.y() + 1);
                tiles << TileSpec(0, zoom, xn, spec.y() + 1);
            }

            for (int i = 0; i < tiles.size(); ++i) {
                TileSpec ts = tiles.at(i);
                if (!done.contains(ts) && !queued.contains(ts)) {
                    queue << ts;
                    queued << ts;
                }
            }
        }
        done << spec;
        queued.remove(spec);
    }

    return results;
}
