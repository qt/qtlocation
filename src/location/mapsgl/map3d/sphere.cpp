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
#include "sphere_p.h"

#include <cmath>

Arc::Arc() {}

Arc::Arc(const QPlane3D &plane, double sphereRadius, const QVector3D  &start, const QVector3D &end)
    : plane_(plane),
      sphereRadius_(sphereRadius),
      start_(start),
      end_(end)
{
    QVector3D origin;
    double dist = plane.distanceTo(origin);
    center_ = -1.0 * dist * plane.normal().normalized();
    radius_ = sqrt(sphereRadius_ * sphereRadius_ - dist * dist);
}

void Arc::setPlane(const QPlane3D &plane)
{
    plane_ = plane;
    QVector3D origin;
    double dist = plane.distanceTo(origin);
    center_ = -1.0 * dist * plane.normal().normalized();
    radius_ = sqrt(sphereRadius_ * sphereRadius_ - dist * dist);
}

bool Arc::operator ==(const Arc &rhs)
{
    return (qFuzzyCompare(sphereRadius_, rhs.sphereRadius_)
            && qFuzzyCompare(radius_, rhs.radius_)
            && qFuzzyCompare(plane_, rhs.plane_)
            && qFuzzyCompare(center_, rhs.center_)
            && qFuzzyCompare(start_, rhs.start_)
            && qFuzzyCompare(end_, rhs.end_));
}

QPlane3D Arc::plane() const
{
    return plane_;
}

QVector3D Arc::center() const
{
    return center_;
}

double Arc::sphereRadius() const
{
    return sphereRadius_;
}

double Arc::radius() const
{
    return radius_;
}

void Arc::setStart(const QVector3D &start)
{
    start_ = start;
}

QVector3D Arc::start() const
{
    return start_;
}

void Arc::setEnd(const QVector3D &end)
{
    end_ = end;
}

QVector3D Arc::end() const
{
    return end_;
}

QVector3D Arc::interpolate(double n) const
{
    QVector3D a = start_ - center_;
    QVector3D b = end_ - center_;

    a *= (1 - n);
    b *= n;

    a += b;

    a.normalize();
    a *= radius_;
    a += center_;

    return a;
}

QPair<QVector3D, QVector3D> Arc::intersect2(const QPlane3D &plane) const
{
    QVector3D n1 = plane_.normal();
    QVector3D n2 = plane.normal();
    double h1 = QVector3D::dotProduct(n1, plane_.origin());
    double h2 = QVector3D::dotProduct(n2, plane.origin());

    double dot = QVector3D::dotProduct(n1, n2);

    if (dot == 1.0)
        return QPair<QVector3D, QVector3D>(start_, QVector3D());
    if (dot == -1.0)
        return QPair<QVector3D, QVector3D>(QVector3D(), QVector3D());

    QVector3D b = QVector3D::crossProduct(n1, n2);

    double c1 = h1 - h2 * dot;
    double c2 = h2 - h1 * dot;
    double den = 1 - dot * dot;

    n1 *= c1;
    n2 *= c2;

    n1 += n2;
    n1 /= den;

    double r2 = sphereRadius_ * sphereRadius_;

    QVector3D a = n1;

//    QPair<double, double> f = quadraticResults(b.lengthSquared(), 2 * QVector3D::dotProduct(a, b), a.lengthSquared() - r2, false);
//    qWarning() << "f " << f.first << f.second;

    if (r2 >= a.lengthSquared()) {
        double alpha = sqrt(r2 - a.lengthSquared()) / b.length();
        QVector3D v1 = b;
        v1 *= alpha;
        v1 += a;
        QVector3D v2 = b;
        v2 *= -1.0 * alpha;
        v2 += a;


        // TODO
        // need a more robust method to determine whether v1, v2 are between start and end
        // needs to be computationally cheap
        double se = QVector3D::dotProduct(start_, end_);
        bool hasV1 = ((se <= QVector3D::dotProduct(start_, v1)) && (se <= QVector3D::dotProduct(v1, end_)));
        bool hasV2 = ((se <= QVector3D::dotProduct(start_, v2)) && (se <= QVector3D::dotProduct(v2, end_)));

        if (hasV1) {
            if (hasV2 && !qFuzzyCompare(v1, v2)) {
                if ((v1 - start_).lengthSquared() > (v2 - start_).lengthSquared())
                    qSwap(v1, v2);
                return QPair<QVector3D, QVector3D>(v1, v2);
            } else {
                return QPair<QVector3D, QVector3D>(v1, QVector3D());
            }
        } else if (hasV2) {
            return QPair<QVector3D, QVector3D>(v2, QVector3D());
        }
    }

    return QPair<QVector3D, QVector3D>(QVector3D(), QVector3D());
}

QPair<QVector3D, QVector3D> Arc::intersects(const QPlane3D &plane) const
{
//    return intersect2(plane);

    QVector3D va = start_ - center_;
    QVector3D vb = end_ - center_;

    QVector3D n = plane.normal();

    double na = QVector3D::dotProduct(n, va);
    double nb = QVector3D::dotProduct(n, vb);
    double nab = na - nb;
    double no = QVector3D::dotProduct(n, plane.origin());
    double nc = QVector3D::dotProduct(n, center_);
    double ab = QVector3D::dotProduct(va, vb);
    double r2 = radius_ * radius_;
    double rab = 1 - (ab / r2);

    double noc = no - nc;
    noc *= noc;


    double a = nab * nab - 2 * noc * rab;
    double b = 2 * (noc * rab - na * nab);
    double c = na * na - noc;

    QPair<double, double> factors = quadraticResults(a, b, c);

    double f1 = factors.first;
    QVector3D v1;
    bool hasV1 = false;

    double f2 = factors.second;
    QVector3D v2;
    bool hasV2 = false;

    if (f1 != -1.0) {
        v1 = interpolate(f1);
        if (qAbs(plane.distanceTo(v1) / sphereRadius_) < 1e-6) {
            hasV1 = true;
        } else {
            v1 = QVector3D();
        }
    }

    if (f2 != -1.0) {
        v2 = interpolate(f2);
        if (qAbs(plane.distanceTo(v2) / sphereRadius_) < 1e-6) {
            hasV2 = true;
            if (!hasV1) {
                v1 = v2;
                v2 = QVector3D();
            }
        } else {
            v2 = QVector3D();
        }
    }

    return QPair<QVector3D, QVector3D>(v1, v2);
}

QPair<double, double> Arc::xIntersectFactor(double x) const
{
    return intersects(start_.x(), end_.x(), center_.x(), x);
}

QPair<double, double> Arc::yIntersectFactor(double y) const
{
    return intersects(start_.y(), end_.y(), center_.y(), y);
}

QPair<double, double> Arc::tIntersectFactor(double x, double z) const
{
    double t;
    double ax;
    double az;
    double bx;
    double bz;
    double cx;
    double cz;

    // TODO deal with x = y = 0

    if (qAbs(x) <= qAbs(z)) {
        t = x / z;
        ax = start_.x() - center_.x();
        az = start_.z() - center_.z();
        bx = end_.x() - center_.x();
        bz = end_.z() - center_.z();
        cx = center_.x();
        cz = center_.z();
    } else {
        t = z / x;
        ax = start_.z() - center_.z();
        az = start_.x() - center_.x();
        bx = end_.z() - center_.z();
        bz = end_.x() - center_.x();
        cx = center_.z();
        cz = center_.x();
    }


    double r2 = radius_ * radius_;
    double dot = QVector3D::dotProduct(start_ - center_, end_ - center_);
    double dc = (cx - t * cz);
    dc *= dc;

    double ta = t * az - ax;
    double tb = t * bz - bx;
    double tab = ta - tb;

    double a = r2 * tab * tab + 2 * (dot - r2) * dc;
    double b = 2 * (-1 * r2 * ta * tab + (r2 - dot) * dc);
    double c = r2 * ta * ta - r2 * dc;

    QPair<double, double> factors = quadraticResults(a, b, c);

    // Need to check that the signs match

    bool xNeg = (x < 0.0);
    bool zNeg = (z < 0.0);

    double f1 = factors.first;
    bool f1Valid = false;
    if (f1 != -1.0) {
        QVector3D r = interpolate(f1);
        bool rxNeg = (r.x() < 0.0);
        bool rzNeg = (r.z() < 0.0);
        if ((xNeg == rxNeg) && (zNeg == rzNeg)) {
            f1Valid = true;
        } else {
            f1 = -1.0;
        }
    }

    double f2 = factors.second;
    if (f2 != -1.0) {
        QVector3D r = interpolate(f2);
        bool rxNeg = (r.x() < 0.0);
        bool rzNeg = (r.z() < 0.0);
        if ((xNeg == rxNeg) && (zNeg == rzNeg)) {
            if (!f1Valid) {
                f1 = f2;
                f2 = -1.0;
            }
        } else {
            f2 = -1.0;
        }
    }

    return QPair<double, double>(f1, f2);
}

QPair<double, double> Arc::intersects(double startValue, double endValue, double centerValue, double target) const
{
    double r2 = radius_ * radius_;
    double dc = (target - centerValue) * (target - centerValue);
    double dot = QVector3D::dotProduct(start_ - center_, end_ - center_);

    double a = 2 * (r2 - dot) * dc - r2 * (startValue - endValue) * (startValue - endValue);
    double b = 2 * (dot - r2) * dc - 2 * r2 * (startValue - centerValue) * (endValue - startValue);
    double c = r2 * (dc - (startValue - centerValue) * (startValue - centerValue));

    return quadraticResults(a, b, c);
}

QPair<double, double> Arc::quadraticResults(double a, double b, double c, bool clip)
{
    if (a == 0.0)
        return QPair<double, double>(-1.0, -1.0);

    double s = b * b - 4 * a * c;

    if (s < 0.0)
        return QPair<double, double>(-1.0, -1.0);

    double s2 = sqrt(s);

    double n1 = (-b + s2) / (2 * a);
    double n2 = (-b - s2) / (2 * a);

    if (clip) {
        if ((n1 < 0.0) || (n1 > 1.0))
            n1 = -1.0;
        if ((n2 < 0.0) || (n2 > 1.0))
            n2 = -1.0;

        if (n1 != -1.0) {
            if ((n2 != -1.0) && (n1 != n2)) {
                if (n2 < n1)
                    qSwap(n1, n2);
                return QPair<double, double>(n1, n2);
            } else {
                return QPair<double, double>(n1, -1.0);
            }
        } else if (n2 != -1.0) {
            return QPair<double, double>(n2, -1.0);
        } else {
            return QPair<double, double>(-1.0, -1.0);
        }
    } else {
        if (n2 < n1)
            qSwap(n1, n2);
        return QPair<double, double>(n1, n2);
    }
}

bool Arc::belowPlane(const QPlane3D &plane) const
{
    double ds = plane.distanceTo(start_) / sphereRadius_;
    double de = plane.distanceTo(end_) / sphereRadius_;

    if ((ds > 1e-6) || (de > 1e-6))
        return false;

    return ((ds < -1e-6) || (de < -1e-6));
}

bool PatchData::operator == (const PatchData &rhs)
{
    return (qFuzzyCompare(point,rhs.point) && qFuzzyCompare(plane, rhs.plane));
}

SpherePatch::SpherePatch(double radius, const QList<Arc> arcs, bool roundUp, bool planar)
    : radius_(radius),
      roundUp_(roundUp),
      planar_(planar),
      debug_(true)
{
    data_.reserve(arcs.size());
    for (int i = 0; i < arcs.size(); ++i) {
        PatchData d;
        d.point = arcs.at(i).start();
        d.plane = arcs.at(i).plane();
        d.intersectIndex = -1;
        d.done = false;
        data_ << d;
    }
}

SpherePatch::SpherePatch(double radius, const QList<PatchData> &data, bool roundUp, bool planar)
    : data_(data),
      radius_(radius),
      roundUp_(roundUp),
      planar_(planar),
      debug_(true)
{
    int size = data_.size();

    for (int i = 0; i < size; ++i) {
        data_[i].intersectIndex = -1;
        data_[i].done = false;
    }
}

bool SpherePatch::operator ==(const SpherePatch &rhs)
{
    // TODO need to be able to handle rotations of the arc list
    // TODO should also handle list in reverse order and with start/end
    // in the opposite order
    return (qFuzzyCompare(radius_, rhs.radius_)
            && (data_ == rhs.data_)
            && (roundUp_ == rhs.roundUp_));
}

bool SpherePatch::isomorphic(const SpherePatch &rhs)
{
    if (radius_ != rhs.radius_)
        return false;
    if (roundUp_ != rhs.roundUp_)
        return false;
    if (data_.size() != rhs.data_.size())
        return false;
    if (data_.size() == 0)
        return true;

    int index = -1;
    int size  = data_.size();
    PatchData d1 = data_.at(0);
    for (int i = 0; i < size; ++i) {
        if (d1 == rhs.data_[i]) {
            index = i;
            break;
        }
    }

    if (index == -1)
        return false;

    for (int i1 = 0; i1 < size; ++i1) {
        int i2 = (i1 + index) % size;
        if (!(data_[i1] == rhs.data_[i2]))
            return false;
    }

    return true;
}

int SpherePatch::arcCount() const
{
    return data_.size();
}

Arc SpherePatch::arc(int index) const
{
    if (index < 0 || index >= data_.size())
        return Arc();

    int size = data_.size();
    int index2 = (index + 1) % size;

    return Arc(data_.at(index).plane, radius_, data_.at(index).point, data_.at(index2).point);
}

void SpherePatch::setRoundUpTiles(bool roundUp)
{
    roundUp_ = roundUp;
}

bool SpherePatch::roundUpTiles() const
{
    return roundUp_;
}

bool SpherePatch::containsPoint(const QVector3D &point) const
{
    int size = data_.size();

    for (int i = 0; i < size; ++i) {
        if (data_.at(i).plane.distanceTo(point) < 0.0)
            return false;
    }

    return true;
}

bool SpherePatch::inFrontOfPlane(const QPlane3D &plane) const
{
    int size = data_.size();

    for (int i1 = 0; i1 < size; ++i1) {
        if (plane.distanceTo(data_.at(i1).point) < 0)
            return false;
    }

    return true;
}

bool SpherePatch::behindPlane(const QPlane3D &plane) const
{
    int size = data_.size();

    for (int i1 = 0; i1 < size; ++i1) {
        if (plane.distanceTo(data_.at(i1).point) > 0)
            return false;
    }

    return true;
}

QList<SpherePatch> SpherePatch::intersect(const QPlane3D &plane)
{
    QList<SpherePatch> results;

    /*
      We need these tests to rule out several cases that would
      otherwise leave us with two arcs defining the patch.

      We want to save that case for when the plane / sphere
      circle intersects one arc of this patch at two points.

      If it's a small circle we end up with two arcs between
      two points (but the arcs are on different planes.

      If it's a bigger circle there may be some additional
      points on the intersecting plane to make the interpolation
      unambiguous
    */

    QVector3D origin;
    double dist = plane.distanceTo(origin);
    if (dist > radius_) {
        if (debug_)
            qWarning() << "keeping (outside radius)";
        results << *this;
        return results;
    } else if (dist < -1 * radius_) {
        if (debug_)
            qWarning() << "removing (outside radius)";
        return results;
    }

    // co-planar
    QPlane3D negPlane = plane;
    negPlane.setNormal(negPlane.normal() * -1.0);
    int size = data_.size();
    for (int i = 0; i < size; ++i) {
        if (data_[i].plane == plane) {
            if (debug_)
                qWarning() << "keeping (co-planar)";
            results << *this;
            return results;
        } else if (data_[i].plane == negPlane) {
            if (debug_)
                qWarning() << "removing (co-planar)";
            return results;
        }
    }

    Arc intersectArc(plane, radius_);
    bool contains = containsPoint(intersectArc.center());
//    if (!contains) {
//        if (behindPlane(plane)) {
//            if (debug_)
//                qWarning() << "removing (intersection in different patch)";
//            return results;
//        } else if (inFrontOfPlane(plane)) {
//            if (debug_)
//                qWarning() << "keeping (intersection in different patch)";
//            results << *this;
//            return results;
//        }
//    }

    /*
      for each pair of points
      add first point if above or on plane
      add intersection points if not start or end point
    */

    SpherePatch newPatch(radius_, QList<PatchData>(), roundUp_);

    int arcIntersectCount = 0;
    int doubleIntersectCount = 0;

    QList<int> intersections = split(newPatch, plane, arcIntersectCount, doubleIntersectCount);

    if (debug_) {
        qWarning() << newPatch.data_.size() << intersections.size() << arcIntersectCount << doubleIntersectCount << contains;
        for (int i = 0; i < newPatch.data_.size(); ++i)
            qWarning() << "  " << newPatch.data_.at(i).point << newPatch.data_.at(i).plane << newPatch.data_.at(i).intersectIndex;
        for (int i = 0; i < intersections.size(); ++i)
            qWarning() << "    " << intersections.at(i);
    }


    if ((arcIntersectCount == 0) || ((arcIntersectCount == 1) && (newPatch.data_.size() == 1))) {
        if (contains)
            return noIntersectPatches(plane);
        if (debug_)
            qWarning() << "keeping (center point in different patch)";
//        results << *this;

                if (behindPlane(plane)) {
                    if (debug_)
                        qWarning() << "removing (intersection in different patch)";
                    return results;
                } else if (inFrontOfPlane(plane)) {
                    if (debug_)
                        qWarning() << "keeping (intersection in different patch)";
                    results << *this;
                    return results;
                }

        return results;
    }

    //qWarning() << newPatch.data_.size() << intersections.size() << arcIntersectCount << doubleIntersectCount << contains;

    if ((intersections.length() == 2) && (doubleIntersectCount == 1))
        handleCircleEdgeIntersection(newPatch, plane, intersections);

    arrangeIntersections(newPatch, plane, intersections);

    if (debug_) {
        qWarning() << newPatch.data_.size() << intersections.size() << arcIntersectCount << doubleIntersectCount << contains;
        for (int i = 0; i < newPatch.data_.size(); ++i)
            qWarning() << "  " << newPatch.data_.at(i).point << newPatch.data_.at(i).plane << newPatch.data_.at(i).intersectIndex;
        for (int i = 0; i < intersections.size(); ++i)
            qWarning() << "    " << intersections.at(i);
    }

    return divide(newPatch, plane, intersections);
}

QList<int> SpherePatch::split(SpherePatch &patch, const QPlane3D &plane, int &arcCount, int &doubleCount)
{
    QList<int> intersections;

    arcCount = 0;
    doubleCount = 0;

    int size = data_.size();

    for (int i1 = 0; i1 < size; ++i1) {
        int i2 = (i1 + 1) % size;

        PatchData d1 = data_.at(i1);
        PatchData d2 = data_.at(i2);

        QVector3D p1 = d1.point;
        QVector3D p2 = d2.point;

        Arc a = Arc(d1.plane, radius_, p1, p2);

        QPair<QVector3D, QVector3D> intersectPoints = a.intersects(plane);
        QVector3D origin;

        if (intersectPoints.first != origin)
            ++arcCount;

        if (intersectPoints.second != origin)
            ++doubleCount;

        if (debug_)
            qWarning() << "intersects "
                       << intersectPoints.first
                       << intersectPoints.second
                       << "(" << p1 << ", " << p2 << ")";

        double dist = plane.distanceTo(p1);

        if ((dist / radius_ > -1e-6) && (dist <= 2.0 * radius_)) {
            patch.data_ << d1;

            int lastIndex = patch.data_.size() - 1;

            if (intersectPoints.first != origin) {
                if (qFuzzyCompare(intersectPoints.first, p1) && (intersections.isEmpty() || !(intersections.last() == lastIndex))) {
                    intersections << lastIndex;
                }
            }

            if (intersectPoints.second != origin) {
                if (qFuzzyCompare(intersectPoints.second, p1) && (intersections.isEmpty() || !(intersections.last() == lastIndex))) {
                    intersections << lastIndex;
                }
            }

        } else {
            if (debug_)
                qWarning() << "point " << p1 << " below plane";
        }

        if (intersectPoints.first != origin) {
            if (!qFuzzyCompare(intersectPoints.first, p1) && !qFuzzyCompare(intersectPoints.first, p2)) {
                PatchData d = d1;
                d.point = intersectPoints.first;
                patch.data_ << d;
                intersections << patch.data_.size() - 1;
                if (debug_)
                    qWarning() << "adding " << intersectPoints.first << " as intersect";
            }
        }

        if (intersectPoints.second != origin) {
            if (!qFuzzyCompare(intersectPoints.second, p1) && !qFuzzyCompare(intersectPoints.second, p2)) {
                PatchData d = d1;
                d.point = intersectPoints.second;
                patch.data_ << d;
                intersections << patch.data_.size() - 1;
                if (debug_)
                    qWarning() << "adding " << intersectPoints.second << " as intersect";
            }
        }
    }

    return intersections;
}

void SpherePatch::handleCircleEdgeIntersection(SpherePatch &patch, const QPlane3D &plane, QList<int> &intersections)
{
    QList<QVector3D> newPoints;

    Arc a(plane, radius_);

    QVector3D p1 = patch.data_.at(intersections.at(0)).point;
    QVector3D p2 = patch.data_.at(intersections.at(1)).point;

    bool rhplane = (plane.distanceTo(QVector3D()) < 0.0);

    if (rhplane)
        intersections.removeFirst();
    else
        intersections.removeLast();

    QVector3D up = p1 - p2;
    up.normalize();
    up *= a.radius();

    QVector3D right = QVector3D::crossProduct(plane.normal(), up);
    right.normalize();
    right *= a.radius();

    QVector3D down = -1 * up;

    QVector3D left = -1 * right;

    up += a.center();
    right += a.center();
    down += a.center();
    left += a.center();

    if (containsPoint(up) && !qFuzzyCompare(up, p1) && !qFuzzyCompare(up, p2))
        newPoints << up;
    if (containsPoint(right) && !qFuzzyCompare(right, p1) && !qFuzzyCompare(right, p2))
        newPoints << right;
    if (containsPoint(down) && !qFuzzyCompare(down, p1) && !qFuzzyCompare(down, p2))
        newPoints << down;
    if (containsPoint(left) && !qFuzzyCompare(left, p1) && !qFuzzyCompare(left, p2))
        newPoints << left;

    int index = intersections.at(0) + 1;

    while (!newPoints.isEmpty()) {
        int minIndex = 0;
        QVector3D p = patch.data_.at(index - 1).point;
        double minDist = (newPoints.at(0) - p).lengthSquared();
        for (int j = 1; j < newPoints.size(); ++j) {
            double d = (newPoints.at(j) - p).lengthSquared();
            if (d < minDist) {
                minIndex = j;
                minDist = d;
            }
        }

        PatchData d;
        d.point = newPoints.takeAt(minIndex);
        d.plane = plane;
        d.intersectIndex = -1;
        d.done = false;
        patch.data_.insert(index, d);

        intersections << index;
        ++index;
    }
}

void SpherePatch::arrangeIntersections(SpherePatch &patch, const QPlane3D &plane, const QList<int> &intersections)
{
    bool rhplane = (plane.distanceTo(QVector3D()) < 0.0);

    int modifier = 1;
    if (!rhplane)
        modifier = -1;

    if (!rhplane) {
        int intersectSize = intersections.size();
        int dataSize = patch.data_.size();

        for (int i1 = 0; i1 < intersectSize; ++i1) {
            int i2 = (i1 + modifier) % intersectSize;
            if (i2 < 0)
                i2 += intersectSize;
            int index1 = intersections.at(i1);
            int index2 = intersections.at(i2);
            if (index1 != index2) {
                patch.data_[index1].intersectIndex = index2;

                if (planar_) {
                    int i3 = (index1 + 1) % dataSize;
                    if (i3 < 0)
                        i3 += dataSize;
                    if (i3 == index2)
                        patch.data_[index1].plane = plane;
                }
            }
        }
    }
}

void SpherePatch::setupPlanes(QList<PatchData> &data, const QPlane3D &plane)
{
    int ndSize = data.size();

    if (!planar_) {
        for (int i1 = 0; i1 < ndSize; ++i1) {
            int i2 = (i1 + 1) % ndSize;
//                if (!qFuzzyCompare(data.at(i1).plane, data.at(i2).plane)) {
                    if (qAbs(data.at(i1).plane.distanceTo(data.at(i2).point) / radius_) > 1e-6) {
                        data[i1].plane = plane;
                    }
//                }
        }
    }
}

QList<SpherePatch> SpherePatch::divide(SpherePatch &patch, const QPlane3D &plane, const QList<int> &intersections)
{
    QList<SpherePatch> results;

    int size = patch.data_.size();

    bool rhplane = (plane.distanceTo(QVector3D()) < 0.0);

    if ((patch.data_.size() > 1) && rhplane) {
        setupPlanes(patch.data_, plane);
        results << SpherePatch(radius_, patch.data_, roundUp_);
    } else if (size != intersections.size()) {
        int size = patch.data_.size();
        for (int i = 0; i < size; ++i) {
            PatchData d1 = patch.data_.at(i);
            if ((d1.intersectIndex == -1) && !d1.done) {

                bool followingIntersect = false;
                QList<PatchData> newData;

                newData << d1;

                int j = (i + 1) % size;

                int count = 0;
                while ((j != i) && (count != size)) {
                    ++count;

                    PatchData d2 = patch.data_.at(j);
                    newData << d2;

                    int next = (j + 1) % size;

                    // if ([this point is not an intersection])
                    if (d2.intersectIndex == -1) {
                        patch.data_[j].done = true;
                    } else {
                        if (followingIntersect) {
                            // followingIntersect = [intersect exists and is adjacent]
                            followingIntersect = (d2.intersectIndex == next);
                        } else {
                            next = d2.intersectIndex;
                            followingIntersect = true;
                        }
                    }

                    j = next;
                }

                if (count == size + 1) {
                    qWarning() << "GAAAAH!!";
                }

                if (newData.size() > 1) {
                    setupPlanes(newData, plane);
                    SpherePatch sp = SpherePatch(radius_, newData, roundUp_);
                    results << sp;
                }
            }
        }
    }

    return results;
}

QList<SpherePatch> SpherePatch::noIntersectPatches(const QPlane3D &plane) const
{
    QList<SpherePatch> results;

    Arc a(plane, radius_);

    bool allBigger = true;
    bool allSmaller = true;

    int size = data_.size();

    for (int i = 0; i < size; ++i) {
        double dist = (data_.at(i).point - a.center()).length();
        double r = (dist - a.radius()) / radius_;

        if (r < -1e-6)
            allSmaller = false;
        if (r > 1e-6)
            allBigger = false;
    }

    if (allBigger) {
        if (plane.distanceTo(data_.at(0).point) / radius_ > 0.0)
            results << *this;
        return results;
    }

    QList<QVector3D> points;

    const double pi = 3.14159265358;
    double angle = asin(a.radius() / radius_) * 180.0 / pi;
    QVector3D center = a.center().normalized() * radius_;

    for (int i = 0; i < size; ++i) {
        QVector3D axis = QVector3D::crossProduct(center, data_.at(i).point);
        QMatrix4x4 r;
        r.rotate(angle, axis);
        points << r * center;
    }

    double dist = plane.distanceTo(QVector3D());
    if (dist > 0.0) {
        // circle shaped hole

        for (int i1 = 0; i1 < size; ++i1) {
            int i2 = (i1 + 1) % size;

            QVector3D start = data_.at(i1).point;
            QVector3D end = data_.at(i2).point;

            QList<PatchData> data;

            PatchData d1;
            d1.point = start;
            d1.plane = data_.at(i1).plane;
            data << d1;

            PatchData d2;
            d2.point = end;
            d2.plane = QPlane3D(center, QVector3D::normal(center, end));
            data << d2;

            PatchData d3;
            d3.point = points.at(i2);
            d3.plane = plane;
            data << d3;

            PatchData d4;
            d4.point = points.at(i1);
            d4.plane = QPlane3D(center, QVector3D::normal(center, start));
            data << d4;

            results << SpherePatch(radius_, data, roundUp_);
        }

    } else if (dist < 0.0) {
        // solo circle

        QList<PatchData> data;
        for (int i = 0; i < size; ++i) {
            PatchData d;
            d.point = points.at(i);
            d.plane = plane;
            data << d;
        }

        results << SpherePatch(radius_, data, roundUp_, true);
    }

    return results;
}

Sphere::Sphere(double radius, const QList<SpherePatch> &patches)
    : radius_(radius),
      patches_(patches) {}

Sphere::Sphere(double radius)
    : radius_(radius)
{
    QVector3D origin;

    patches_.reserve(8);

    QVector3D pxv = QVector3D(radius_, 0.0, 0.0);
    QVector3D nxv = QVector3D(-1.0 * radius_, 0.0, 0.0);
    QVector3D pyv = QVector3D(0.0, radius_, 0.0);
    QVector3D nyv = QVector3D(0.0, -1.0 * radius_, 0.0);
    QVector3D pzv = QVector3D(0.0, 0.0, radius_);
    QVector3D nzv = QVector3D(0.0, 0.0, -1.0 * radius_);

    QPlane3D pxp = QPlane3D(origin, QVector3D(1.0, 0.0, 0.0));
    QPlane3D nxp = QPlane3D(origin, QVector3D(-1.0, 0.0, 0.0));
    QPlane3D pyp = QPlane3D(origin, QVector3D(0.0, 1.0, 0.0));
    QPlane3D nyp = QPlane3D(origin, QVector3D(0.0, -1.0, 0.0));
    QPlane3D pzp = QPlane3D(origin, QVector3D(0.0, 0.0, 1.0));
    QPlane3D nzp = QPlane3D(origin, QVector3D(0.0, 0.0, -1.0));

    PatchData d;

    QList<PatchData> data;
    data.reserve(3);
    data << d;
    data << d;
    data << d;

    data[0].point = pxv;
    data[0].plane = pzp;
    data[1].point = pyv;
    data[1].plane = pxp;
    data[2].point = pzv;
    data[2].plane = pyp;

    SpherePatch patch000(radius, data, false);
    patches_ << patch000;

    data[0].point = pxv;
    data[0].plane = pyp;
    data[1].point = nzv;
    data[1].plane = pxp;
    data[2].point = pyv;
    data[2].plane = nzp;

    SpherePatch patch001(radius_, data, false);
    patches_ << patch001;

    data[0].point = pxv;
    data[0].plane = nyp;
    data[1].point = pzv;
    data[1].plane = pxp;
    data[2].point = nyv;
    data[2].plane = pzp;

    SpherePatch patch010(radius_, data, false);
    patches_ << patch010;

    data[0].point = pxv;
    data[0].plane = nzp;
    data[1].point = nyv;
    data[1].plane = pxp;
    data[2].point = nzv;
    data[2].plane = nyp;

    SpherePatch patch011(radius_, data, false);
    patches_ << patch011;

    data[0].point = nxv;
    data[0].plane = pyp;
    data[1].point = pzv;
    data[1].plane = nxp;
    data[2].point = pyv;
    data[2].plane = nzp;

    SpherePatch patch100(radius_, data, false);
    patches_ << patch100;

    data[0].point = nxv;
    data[0].plane = nzp;
    data[1].point = pyv;
    data[1].plane = nxp;
    data[2].point = nzv;
    data[2].plane = pyp;

    SpherePatch patch101(radius_, data, false);
    patches_ << patch101;

    data[0].point = nxv;
    data[0].plane = pzp;
    data[1].point = nyv;
    data[1].plane = nxp;
    data[2].point = pzv;
    data[2].plane = nyp;

    SpherePatch patch110(radius_, data, true);
    patches_ << patch110;

    data[0].point = nxv;
    data[0].plane = nyp;
    data[1].point = nyv;
    data[1].plane = nxp;
    data[2].point = nzv;
    data[2].plane = nzp;

    SpherePatch patch111(radius_, data, true);
    patches_ << patch111;
}

bool Sphere::operator ==(const Sphere &rhs)
{
    // TODO either make canonical ordering for patches
    // or do order-independent comparison of the patches
    return (qFuzzyCompare(radius_, rhs.radius_)
            && (patches_ == rhs.patches_));
}

bool Sphere::isomorphic (const Sphere &rhs)
{
    if (radius_ != rhs.radius_)
        return false;
    if (patches_.size() != rhs.patches_.size())
        return false;

    int size = patches_.size();

    if (size == 0)
        return true;

    for (int i = 0; i < size; ++i)
        if (!patches_[i].isomorphic(rhs.patches_.at(i)))
            return false;

    return true;
}

QList<SpherePatch> Sphere::patches() const
{
    return patches_;
}

void Sphere::intersect(const QPlane3D &plane)
{
    int size = patches_.size();

    QList<SpherePatch> newPatches;

    QPlane3D p = plane;
    p.setNormal(p.normal().normalized());

    for (int i = 0; i < size; ++i)
        newPatches.append(patches_[i].intersect(p));

    patches_ = newPatches;
}

QDebug operator << (QDebug dbg, const Arc &arc)
{
    dbg << "    Arc [plane: " << arc.plane() << ", start: " << arc.start() << ", end: " << arc.end() << "]\n";
    return dbg;
}

QDebug operator << (QDebug dbg, const SpherePatch &patch)
{
    dbg << "  Patch [\n";

    int arcCount = patch.arcCount();
    for (int i = 0; i < arcCount; ++i)
        dbg << patch.arc(i);

    dbg << "  ]\n";
    return dbg;
}

QDebug operator << (QDebug dbg, const Sphere &sphere)
{
    dbg << "Sphere [\n";

    QList<SpherePatch> patches = sphere.patches();
    for (int i = 0; i < patches.size(); ++i)
        dbg << patches.at(i);

    dbg << "]\n";
    return dbg;
}
