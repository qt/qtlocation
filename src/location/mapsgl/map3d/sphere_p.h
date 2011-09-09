/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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
#ifndef SPHERE_P_H
#define SPHERE_P_H

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

#include <QVector3D>
#include <QPair>
#include <qplane3d.h>
#include <QDebug>

class Q_AUTOTEST_EXPORT Arc {
public:
    Arc();
    Arc(const QPlane3D &plane,
        double sphereRadius,
        const QVector3D  &start = QVector3D(),
        const QVector3D &end = QVector3D());

    bool operator == (const Arc &rhs);

    void setPlane(const QPlane3D &plane);
    QPlane3D plane() const;

    QVector3D center() const;
    double radius() const;
    double sphereRadius() const;

    void setStart(const QVector3D &start);
    QVector3D start() const;

    void setEnd(const QVector3D &end);
    QVector3D end() const;

    QVector3D interpolate(double n) const;
    QPair<QVector3D, QVector3D> intersects(const QPlane3D &plane) const;

    QPair<double, double> xIntersectFactor(double x) const;
    QPair<double, double> yIntersectFactor(double y) const;
    QPair<double, double> tIntersectFactor(double x, double y) const;

    bool belowPlane(const QPlane3D &plane) const;

private:
    QPair<double, double> intersects(double start, double end, double center, double target) const;
    static QPair<double, double> quadraticResults(double a, double b, double c, bool clip = true);

    QPair<QVector3D, QVector3D> intersect2(const QPlane3D &plane) const;

    QPlane3D plane_;
    double sphereRadius_;
    QVector3D center_;
    double radius_;
    QVector3D start_;
    QVector3D end_;
};

QDebug operator << (QDebug dbg, const Arc &arc);

struct PatchData {
    QVector3D point;
    QPlane3D plane;
    int intersectIndex;
    bool done;

    bool operator == (const PatchData &rhs);
};

class Q_AUTOTEST_EXPORT SpherePatch {
public:
    SpherePatch(double radius, const QList<Arc> arcs, bool roundUp = false, bool planar = false);
    SpherePatch(double radius, const QList<PatchData> &data, bool roundUp, bool planar = false);

    bool operator == (const SpherePatch &rhs);
    bool isomorphic(const SpherePatch &rhs);

    int arcCount() const;
    Arc arc(int index) const;

    void setRoundUpTiles(bool roundUp);
    bool roundUpTiles() const;

    QList<SpherePatch> intersect(const QPlane3D &plane);

private:
    bool inFrontOfPlane(const QPlane3D &plane) const;
    bool behindPlane(const QPlane3D &plane) const;
    bool containsPoint(const QVector3D &point) const;

    QList<int> split(SpherePatch &patch, const QPlane3D &plane, int &arcCount, int &doubleCount);
    void handleCircleEdgeIntersection(SpherePatch &patch, const QPlane3D &plane, QList<int> &intersections);
    QList<SpherePatch> noIntersectPatches(const QPlane3D &plane) const;
    void arrangeIntersections(SpherePatch &patch, const QPlane3D &plane, const QList<int> &intersections);
    void setupPlanes(QList<PatchData> &data, const QPlane3D &plane);
    QList<SpherePatch> divide(SpherePatch &patch, const QPlane3D &plane, const QList<int> &intersections);

    QList<PatchData> data_;
//    QList<int> intersections_;

    double radius_;
    //QList<Arc> arcs_;
    bool roundUp_;
    bool planar_;
    bool debug_;
};

QDebug operator << (QDebug dbg, const SpherePatch &patch);

class Q_AUTOTEST_EXPORT Sphere {
public:
    Sphere(double radius = 20000.0);
    Sphere(double radius, const QList<SpherePatch> &patches);

    bool operator == (const Sphere &rhs);
    bool isomorphic (const Sphere &rhs);

    QList<SpherePatch> patches() const;

    void intersect(const QPlane3D &plane);

private:
    double radius_;
    QList<SpherePatch> patches_;
};

Q_AUTOTEST_EXPORT QDebug operator << (QDebug dbg, const Sphere &sphere);


#endif // SPHERE_P_H
