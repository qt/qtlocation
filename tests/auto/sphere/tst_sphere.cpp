/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#include <QObject>
#include <QtTest/QtTest>

#include "../../../src/location/mapsgl/map3d/sphere_p.h"
#include "../../../src/location/mapsgl/map3d/viewportcamera_p.h"
#include "../../../src/location/mapsgl/frustum_p.h"
#include "../../../src/location/mapsgl/map3d/projection3d_p.h"

#include <QMetaType>

#include <cmath>

Q_DECLARE_METATYPE(Sphere)

class tst_Sphere : public QObject
{
    Q_OBJECT

public:
    tst_Sphere() {

    }

public slots:
    void initTestCase();

private slots:
    void x_plane();
    void x_plane_data();

    void xy_plane_straight();
    void xy_plane_straight_data();

    void xy_plane_skew();
    void xy_plane_skew_data();

    void xyz_plane();
    void xyz_plane_data();

    void camera();

private:
    Sphere emptySphere(double radius) const;
    Sphere basicSphere(double radius) const;

    bool test(const QPlane3D &plane, const Sphere &sphereOut);

    QList<QPlane3D> planes() const;
    Sphere planeTest(const QList<QPlane3D> &planes, const QList<int> &planeIndices) const;
};

void tst_Sphere::initTestCase()
{
#if defined(Q_OS_LINUX)
    QSKIP("Fails on Linux - see QTBUG-22634");
#endif
}

Sphere tst_Sphere::emptySphere(double radius) const
{
    return Sphere(radius, QList<SpherePatch>());
}

Sphere tst_Sphere::basicSphere(double radius) const
{
    QList<Arc> arcs1In;

    QVector3D origin;

    QVector3D pxv = QVector3D(radius, 0.0, 0.0);
    QVector3D pyv = QVector3D(0.0, radius, 0.0);
    QVector3D pzv = QVector3D(0.0, 0.0, radius);

    QPlane3D pxp = QPlane3D(origin, QVector3D(1.0, 0.0, 0.0));
    QPlane3D pyp = QPlane3D(origin, QVector3D(0.0, 1.0, 0.0));
    QPlane3D pzp = QPlane3D(origin, QVector3D(0.0, 0.0, 1.0));

    arcs1In << Arc(pzp, radius, pxv, pyv);
    arcs1In << Arc(pxp, radius, pyv, pzv);
    arcs1In << Arc(pyp, radius, pzv, pxv);

    SpherePatch patch1In(radius, arcs1In);

    QList<SpherePatch> patchesIn;

    patchesIn << patch1In;

    Sphere sphereIn(radius, patchesIn);

    return sphereIn;
}

bool tst_Sphere::test(const QPlane3D &plane, const Sphere &sphereOut)
{
    Sphere sphereIn = basicSphere(20000.0);

    sphereIn.intersect(plane);

    if (sphereIn == sphereOut) {
        return true;
    } else {
        qWarning() << sphereIn;
        qWarning() << sphereOut;

        return false;
    }
}

void tst_Sphere::x_plane()
{
    QFETCH(QPlane3D, plane);
    QFETCH(Sphere, sphereOut);

    QVERIFY(test(plane, sphereOut));
}

void tst_Sphere::x_plane_data()
{
    QTest::addColumn<QPlane3D>("plane");
    QTest::addColumn<Sphere>("sphereOut");

    double radius = 20000.0;

    QVector3D origin;

    QPlane3D pxp = QPlane3D(origin, QVector3D(1.0, 0.0, 0.0));
    QPlane3D pyp = QPlane3D(origin, QVector3D(0.0, 1.0, 0.0));
    QPlane3D pzp = QPlane3D(origin, QVector3D(0.0, 0.0, 1.0));

    Sphere empty = emptySphere(radius);
    Sphere basic = basicSphere(radius);

    double factor = radius * 0.0;
    double factor2 = sqrt((radius * radius) - (factor * factor));

    QTest::newRow("positive x forward 0.0")
            << QPlane3D(QVector3D(factor, 0.0, 0.0), QVector3D(1.0, 0.0, 0.0))
            << basic;

    QTest::newRow("positive x backward 0.0")
            << QPlane3D(QVector3D(factor, 0.0, 0.0), QVector3D(-1.0, 0.0, 0.0))
            << empty;

    QTest::newRow("negative x forward 0.0")
            << QPlane3D(QVector3D(-1.0 * factor, 0.0, 0.0), QVector3D(1.0, 0.0, 0.0))
            << basic;

    QTest::newRow("negative x backward 0.0")
            << QPlane3D(QVector3D(-1.0 * factor, 0.0, 0.0), QVector3D(-1.0, 0.0, 0.0))
            << empty;


    factor = radius * 0.5;
    factor2 = sqrt((radius * radius) - (factor * factor));

    QPlane3D pxf050_plane = QPlane3D(QVector3D(factor, 0.0, 0.0), QVector3D(1.0, 0.0, 0.0));

    QList<Arc> pxf050_patch0_arcs;
    pxf050_patch0_arcs << Arc(pzp, radius,
                              QVector3D(radius, 0, 0),
                              QVector3D(factor, factor2, 0));
    pxf050_patch0_arcs << Arc(pxf050_plane, radius,
                              QVector3D(factor, factor2, 0),
                              QVector3D(factor, 0, factor2));
    pxf050_patch0_arcs << Arc(pyp, radius,
                              QVector3D(factor, 0, factor2),
                              QVector3D(radius, 0, 0));

    QList<SpherePatch> pxf050_patches;
    pxf050_patches << SpherePatch(radius, pxf050_patch0_arcs);

    Sphere pxf050 = Sphere(radius, pxf050_patches);

    QTest::newRow("positive x forward 0.5")
            << pxf050_plane
            << pxf050;

    QPlane3D pxb050_plane = QPlane3D(QVector3D(factor, 0.0, 0.0), QVector3D(-1.0, 0.0, 0.0));

    QList<Arc> pxb050_patch0_arcs;
    pxb050_patch0_arcs << Arc(pxp, radius,
                              QVector3D(0, radius, 0),
                              QVector3D(0, 0, radius));
    pxb050_patch0_arcs << Arc(pyp, radius,
                              QVector3D(0, 0, radius),
                              QVector3D(factor, 0, factor2));
    pxb050_patch0_arcs << Arc(pxb050_plane, radius,
                              QVector3D(factor, 0, factor2),
                              QVector3D(factor, factor2, 0));
    pxb050_patch0_arcs << Arc(pzp, radius,
                              QVector3D(factor, factor2, 0),
                              QVector3D(0, radius, 0));


    QList<SpherePatch> pxb050_patches;
    pxb050_patches << SpherePatch(radius, pxb050_patch0_arcs);

    Sphere pxb050 = Sphere(radius, pxb050_patches);

    QTest::newRow("positive x backward 0.5")
            << pxb050_plane
            << pxb050;

    QTest::newRow("negative x forward 0.5")
            << QPlane3D(QVector3D(-1.0 * factor, 0.0, 0.0), QVector3D(1.0, 0.0, 0.0))
            << basic;

    QTest::newRow("negative x backward 0.5")
            << QPlane3D(QVector3D(-1.0 * factor, 0.0, 0.0), QVector3D(-1.0, 0.0, 0.0))
            << empty;


    factor = radius * 1.0;


    QTest::newRow("positive x forward 1.0")
            << QPlane3D(QVector3D(factor, 0.0, 0.0), QVector3D(1.0, 0.0, 0.0))
            << empty;

    QTest::newRow("positive x backward 1.0")
            << QPlane3D(QVector3D(factor, 0.0, 0.0), QVector3D(-1.0, 0.0, 0.0))
            << basic;

    QTest::newRow("negative x forward 1.0")
            << QPlane3D(QVector3D(-1.0 * factor, 0.0, 0.0), QVector3D(1.0, 0.0, 0.0))
            << basic;

    QTest::newRow("negative x backward 1.0")
            << QPlane3D(QVector3D(-1.0 * factor, 0.0, 0.0), QVector3D(-1.0, 0.0, 0.0))
            << empty;


    factor = radius * 1.25;

    QTest::newRow("positive x forward 1.25")
            << QPlane3D(QVector3D(factor, 0.0, 0.0), QVector3D(1.0, 0.0, 0.0))
            << empty;

    QTest::newRow("positive x backward 1.25")
            << QPlane3D(QVector3D(factor, 0.0, 0.0), QVector3D(-1.0, 0.0, 0.0))
            << basic;

    QTest::newRow("negative x forward 1.25")
            << QPlane3D(QVector3D(-1.0 * factor, 0.0, 0.0), QVector3D(1.0, 0.0, 0.0))
            << basic;

    QTest::newRow("negative x backward 1.25")
            << QPlane3D(QVector3D(-1.0 * factor, 0.0, 0.0), QVector3D(-1.0, 0.0, 0.0))
            << empty;
}

void tst_Sphere::xy_plane_straight()
{
    QFETCH(QPlane3D, plane);
    QFETCH(Sphere, sphereOut);

    QVERIFY(test(plane, sphereOut));
}

void tst_Sphere::xy_plane_straight_data()
{
    QTest::addColumn<QPlane3D>("plane");
    QTest::addColumn<Sphere>("sphereOut");

    double radius = 20000.0;

    QVector3D origin;

    QPlane3D pxp = QPlane3D(origin, QVector3D(1.0, 0.0, 0.0));
    QPlane3D pyp = QPlane3D(origin, QVector3D(0.0, 1.0, 0.0));
    QPlane3D pzp = QPlane3D(origin, QVector3D(0.0, 0.0, 1.0));

    Sphere empty = emptySphere(radius);
    Sphere basic = basicSphere(radius);

    double factor1 = radius * 0.0;
    double factor2 = radius * 0.0;

    QPlane3D pxpyf_000_000_plane = QPlane3D(QVector3D(factor1, factor2, 0.0), QVector3D(1.0, 1.0, 0.0).normalized());

    QTest::newRow("positive x positive y forward 0.0 0.0")
            << pxpyf_000_000_plane
            << basic;

    QPlane3D pxpyb_000_000_plane = QPlane3D(QVector3D(factor1, factor2, 0.0), QVector3D(-1.0, -1.0, 0.0).normalized());

    QTest::newRow("positive x positive y backward 0.0 0.0")
            << pxpyb_000_000_plane
            << empty;

    QPlane3D pxnyf_000_000_plane = QPlane3D(QVector3D(factor1, -1.0 * factor2, 0.0), QVector3D(1.0, -1.0, 0.0).normalized());

    double f1 = radius / sqrt(2);
    double f2 = radius / sqrt(2);

    QList<Arc> pxnyf_000_000_patch0_arcs;
    pxnyf_000_000_patch0_arcs << Arc(pzp, radius,
                              QVector3D(radius, 0, 0),
                              QVector3D(f1, f2, 0));
    pxnyf_000_000_patch0_arcs << Arc(pxnyf_000_000_plane, radius,
                              QVector3D(f1, f2, 0),
                              QVector3D(0, 0, radius));
    pxnyf_000_000_patch0_arcs << Arc(pyp, radius,
                              QVector3D(0, 0, radius),
                              QVector3D(radius, 0, 0));


    QList<SpherePatch> pxnyf_000_000_patches;
    pxnyf_000_000_patches << SpherePatch(radius, pxnyf_000_000_patch0_arcs);

    Sphere pxnyf_000_000 = Sphere(radius, pxnyf_000_000_patches);

    QTest::newRow("positive x negative y forward 0.0 0.0")
            << pxnyf_000_000_plane
            << pxnyf_000_000;

    QPlane3D pxnyb_000_000_plane = QPlane3D(QVector3D(factor1, -1.0 * factor2, 0.0), QVector3D(-1.0, 1.0, 0.0).normalized());

    QList<Arc> pxnyb_000_000_patch0_arcs;
    pxnyb_000_000_patch0_arcs << Arc(pxp, radius,
                                     QVector3D(0, radius, 0),
                                     QVector3D(0, 0, radius));
    pxnyb_000_000_patch0_arcs << Arc(pxnyb_000_000_plane, radius,
                                     QVector3D(0, 0, radius),
                                     QVector3D(f1, f2, 0));
    pxnyb_000_000_patch0_arcs << Arc(pzp, radius,
                                     QVector3D(f1, f2, 0),
                                     QVector3D(0, radius, 0));


    QList<SpherePatch> pxnyb_000_000_patches;
    pxnyb_000_000_patches << SpherePatch(radius, pxnyb_000_000_patch0_arcs);

    Sphere pxnyb_000_000(radius, pxnyb_000_000_patches);

    QTest::newRow("positive x negative y backward 0.0 0.0")
            << pxnyb_000_000_plane
            << pxnyb_000_000;

    QPlane3D nxpyf_000_000_plane = QPlane3D(QVector3D(-1.0 * factor1, factor2, 0.0), QVector3D(1.0, -1.0, 0.0).normalized());

    QList<Arc> nxpyf_000_000_patch0_arcs;
    nxpyf_000_000_patch0_arcs << Arc(pzp, radius,
                              QVector3D(radius, 0, 0),
                              QVector3D(f1, f2, 0));
    nxpyf_000_000_patch0_arcs << Arc(nxpyf_000_000_plane, radius,
                              QVector3D(f1, f2, 0),
                              QVector3D(0, 0, radius));
    nxpyf_000_000_patch0_arcs << Arc(pyp, radius,
                              QVector3D(0, 0, radius),
                              QVector3D(radius, 0, 0));

    QList<SpherePatch> nxpyf_000_000_patches;
    nxpyf_000_000_patches << SpherePatch(radius, nxpyf_000_000_patch0_arcs);

    Sphere nxpyf_000_000 = Sphere(radius, nxpyf_000_000_patches);

    QTest::newRow("negative x positive y forward 0.0 0.0")
            << nxpyf_000_000_plane
            << nxpyf_000_000;

    QPlane3D nxpyb_000_000_plane = QPlane3D(QVector3D(-1.0 * factor1, factor2, 0.0), QVector3D(-1.0, 1.0, 0.0).normalized());

    QList<Arc> nxpyb_000_000_patch0_arcs;
    nxpyb_000_000_patch0_arcs << Arc(pxp, radius,
                                     QVector3D(0, radius, 0),
                                     QVector3D(0, 0, radius));
    nxpyb_000_000_patch0_arcs << Arc(nxpyb_000_000_plane, radius,
                                     QVector3D(0, 0, radius),
                                     QVector3D(f1, f2, 0));
    nxpyb_000_000_patch0_arcs << Arc(pzp, radius,
                                     QVector3D(f1, f2, 0),
                                     QVector3D(0, radius, 0));

    QList<SpherePatch> nxpyb_000_000_patches;
    nxpyb_000_000_patches << SpherePatch(radius, nxpyb_000_000_patch0_arcs);

    Sphere nxpyb_000_000(radius, nxpyb_000_000_patches);

    QTest::newRow("negative x positive y backward 0.0 0.0")
            << nxpyb_000_000_plane
            << nxpyb_000_000;

    QPlane3D nxnyf_000_000_plane = QPlane3D(QVector3D(-1.0 * factor1, -1.0 * factor2, 0.0), QVector3D(1.0, 1.0, 0.0).normalized());

    QTest::newRow("negative x negative y forward 0.0 0.0")
            << nxnyf_000_000_plane
            << basic;

    QPlane3D nxnyb_000_000_plane = QPlane3D(QVector3D(-1.0 * factor1, -1.0 * factor2, 0.0), QVector3D(-1.0, -1.0, 0.0).normalized());

    QTest::newRow("negative x negative y backward 0.0 0.0")
            << nxnyb_000_000_plane
            << empty;

    factor1 = radius * 0.25;
    factor2 = radius * 0.25;

    QPlane3D pxpyf_025_025_plane = QPlane3D(QVector3D(factor1, factor2, 0.0), QVector3D(1.0, 1.0, 0.0).normalized());

    f1 = radius / 2.0;
    f2 = radius * sqrt(3) / 2.0;

    QList<Arc> pxpyf_025_025_patch0_arcs;
    pxpyf_025_025_patch0_arcs << Arc(pzp, radius,
                                     QVector3D(radius, 0, 0),
                                     QVector3D(0, radius, 0));
    pxpyf_025_025_patch0_arcs << Arc(pxp, radius,
                                     QVector3D(0, radius, 0),
                                     QVector3D(0, f1, f2));
    pxpyf_025_025_patch0_arcs << Arc(pxpyf_025_025_plane, radius,
                                     QVector3D(0, f1, f2),
                                     QVector3D(f1, 0, f2));
    pxpyf_025_025_patch0_arcs << Arc(pyp, radius,
                                     QVector3D(f1, 0, f2),
                                     QVector3D(radius, 0, 0));

    QList<SpherePatch> pxpyf_025_025_patches;
    pxpyf_025_025_patches << SpherePatch(radius, pxpyf_025_025_patch0_arcs);

    Sphere pxpyf_025_025(radius, pxpyf_025_025_patches);

    QTest::newRow("positive x positive y forward 0.25 0.25")
            << pxpyf_025_025_plane
            << pxpyf_025_025;

    QPlane3D pxpyb_025_025_plane = QPlane3D(QVector3D(factor1, factor2, 0.0), QVector3D(-1.0, -1.0, 0.0).normalized());

    QList<Arc> pxpyb_025_025_patch0_arcs;
    pxpyb_025_025_patch0_arcs << Arc(pyp, radius,
                                     QVector3D(0, 0, radius),
                                     QVector3D(f1, 0, f2));
    pxpyb_025_025_patch0_arcs << Arc(pxpyb_025_025_plane, radius,
                                     QVector3D(f1, 0, f2),
                                     QVector3D(0, f1, f2));
    pxpyb_025_025_patch0_arcs << Arc(pxp, radius,
                                     QVector3D(0, f1, f2),
                                     QVector3D(0, 0, radius));

    QList<SpherePatch> pxpyb_025_025_patches;
    pxpyb_025_025_patches << SpherePatch(radius, pxpyb_025_025_patch0_arcs);

    Sphere pxpyb_025_025(radius, pxpyb_025_025_patches);

    QTest::newRow("positive x positive y backward 0.25 0.25")
            << pxpyb_025_025_plane
            << pxpyb_025_025;

    QPlane3D pxnyf_025_025_plane = QPlane3D(QVector3D(factor1, -1.0 * factor2, 0.0), QVector3D(1.0, -1.0, 0.0).normalized());

    /*
      f3 = positive intersection of x = y + 0.5 * r and x^2 + y^2 = r^2
      f4 = f3 - 0.5 * r
    */

    double f3 = radius * (1.0 + sqrt(7)) / 4.0;
    double f4 = f3 - radius / 2.0;

    QList<Arc> pxnyf_025_025_patch0_arcs;
    pxnyf_025_025_patch0_arcs << Arc(pzp, radius,
                                     QVector3D(radius, 0, 0),
                                     QVector3D(f3, f4, 0));
    pxnyf_025_025_patch0_arcs << Arc(pxnyf_025_025_plane, radius,
                                     QVector3D(f3, f4, 0),
                                     QVector3D(f1, 0, f2));
    pxnyf_025_025_patch0_arcs << Arc(pyp, radius,
                                     QVector3D(f1, 0, f2),
                                     QVector3D(radius, 0, 0));

    QList<SpherePatch> pxnyf_025_025_patches;
    pxnyf_025_025_patches << SpherePatch(radius, pxnyf_025_025_patch0_arcs);

    Sphere pxnyf_025_025(radius, pxnyf_025_025_patches);

    QTest::newRow("positive x negative y forward 0.25 0.25")
            << pxnyf_025_025_plane
            << pxnyf_025_025;

    QPlane3D pxnyb_025_025_plane = QPlane3D(QVector3D(factor1, -1.0 * factor2, 0.0), QVector3D(-1.0, 1.0, 0.0).normalized());

    QList<Arc> pxnyb_025_025_patch0_arcs;
    pxnyb_025_025_patch0_arcs << Arc(pxp, radius,
                                     QVector3D(0, radius, 0),
                                     QVector3D(0, 0, radius));
    pxnyb_025_025_patch0_arcs << Arc(pyp, radius,
                                     QVector3D(0, 0, radius),
                                     QVector3D(f1, 0, f2));
    pxnyb_025_025_patch0_arcs << Arc(pxnyb_025_025_plane, radius,
                                     QVector3D(f1, 0, f2),
                                     QVector3D(f3, f4, 0));
    pxnyb_025_025_patch0_arcs << Arc(pzp, radius,
                                     QVector3D(f3, f4, 0),
                                     QVector3D(0, radius, 0));

    QList<SpherePatch> pxnyb_025_025_patches;
    pxnyb_025_025_patches << SpherePatch(radius, pxnyb_025_025_patch0_arcs);

    Sphere pxnyb_025_025(radius, pxnyb_025_025_patches);

    QTest::newRow("positive x negative y backward 0.25 0.25")
            << pxnyb_025_025_plane
            << pxnyb_025_025;

    QPlane3D nxpyf_025_025_plane = QPlane3D(QVector3D(-1.0 * factor1, factor2, 0.0), QVector3D(1.0, -1.0, 0.0).normalized());

    QList<Arc> nxpyf_025_025_patch0_arcs;
    nxpyf_025_025_patch0_arcs << Arc(pzp, radius,
                                     QVector3D(radius, 0, 0),
                                     QVector3D(f4, f3, 0));
    nxpyf_025_025_patch0_arcs << Arc(nxpyf_025_025_plane, radius,
                                     QVector3D(f4, f3, 0),
                                     QVector3D(0, f1, f2));
    nxpyf_025_025_patch0_arcs << Arc(pxp, radius,
                                     QVector3D(0, f1, f2),
                                     QVector3D(0, 0, radius));
    nxpyf_025_025_patch0_arcs << Arc(pyp, radius,
                                     QVector3D(0, 0, radius),
                                     QVector3D(radius, 0, 0));

    QList<SpherePatch> nxpyf_025_025_patches;
    nxpyf_025_025_patches << SpherePatch(radius, nxpyf_025_025_patch0_arcs);

    Sphere nxpyf_025_025(radius, nxpyf_025_025_patches);

    QTest::newRow("negative x positive y forward 0.25 0.25")
            << nxpyf_025_025_plane
            << nxpyf_025_025;

    QPlane3D nxpyb_025_025_plane = QPlane3D(QVector3D(-1.0 * factor1, factor2, 0.0), QVector3D(-1.0, 1.0, 0.0).normalized());

    QList<Arc> nxpyb_025_025_patch0_arcs;
    nxpyb_025_025_patch0_arcs << Arc(pzp, radius,
                                     QVector3D(f4, f3, 0),
                                     QVector3D(0, radius, 0));
    nxpyb_025_025_patch0_arcs << Arc(pxp, radius,
                                     QVector3D(0, radius, 0),
                                     QVector3D(0, f1, f2));
    nxpyb_025_025_patch0_arcs << Arc(nxpyb_025_025_plane, radius,
                                     QVector3D(0, f1, f2),
                                     QVector3D(f4, f3, 0));

    QList<SpherePatch> nxpyb_025_025_patches;
    nxpyb_025_025_patches << SpherePatch(radius, nxpyb_025_025_patch0_arcs);

    Sphere nxpyb_025_025(radius, nxpyb_025_025_patches);

    QTest::newRow("negative x positive y backward 0.25 0.25")
            << nxpyb_025_025_plane
            << nxpyb_025_025;

    QPlane3D nxnyf_025_025_plane = QPlane3D(QVector3D(-1.0 * factor1, -1.0 * factor2, 0.0), QVector3D(1.0, 1.0, 0.0).normalized());

    QTest::newRow("negative x negative y forward 0.25 0.25")
            << nxnyf_025_025_plane
            << basic;

    QPlane3D nxnyb_025_025_plane = QPlane3D(QVector3D(-1.0 * factor1, -1.0 * factor2, 0.0), QVector3D(-1.0, -1.0, 0.0).normalized());

    QTest::newRow("negative x negative y backward 0.25 0.25")
            << nxnyb_025_025_plane
            << empty;

    factor1 = radius * 0.5;
    factor2 = radius * 0.5;

    f1 = radius * 0.5;
    f2 = radius / sqrt(2);

    QPlane3D pxpyf_050_050_plane = QPlane3D(QVector3D(factor1, factor2, 0.0), QVector3D(1.0, 1.0, 0.0).normalized());

    QList<Arc> pxpyf_050_050_patch0_arcs;
    pxpyf_050_050_patch0_arcs << Arc(pzp, radius,
                                     QVector3D(radius, 0, 0),
                                     QVector3D(0, radius, 0));
    pxpyf_050_050_patch0_arcs << Arc(pxpyf_050_050_plane, radius,
                                     QVector3D(0, radius, 0),
                                     QVector3D(f1, f1, f2));
    pxpyf_050_050_patch0_arcs << Arc(pxpyf_050_050_plane, radius,
                                     QVector3D(f1, f1, f2),
                                     QVector3D(radius, 0, 0));

    QList<SpherePatch> pxpyf_050_050_patches;
    pxpyf_050_050_patches << SpherePatch(radius, pxpyf_050_050_patch0_arcs);

    Sphere pxpyf_050_050(radius, pxpyf_050_050_patches);

    QTest::newRow("positive x positive y forward 0.5 0.5")
            << pxpyf_050_050_plane
            << pxpyf_050_050;

    QPlane3D pxpyb_050_050_plane = QPlane3D(QVector3D(factor1, factor2, 0.0), QVector3D(-1.0, -1.0, 0.0).normalized());

    QList<Arc> pxpyb_050_050_patch0_arcs;
    pxpyb_050_050_patch0_arcs << Arc(pxp, radius,
                                     QVector3D(0, radius, 0),
                                     QVector3D(0, 0, radius));
    pxpyb_050_050_patch0_arcs << Arc(pyp, radius,
                                     QVector3D(0, 0, radius),
                                     QVector3D(radius, 0, 0));
    pxpyb_050_050_patch0_arcs << Arc(pxpyb_050_050_plane, radius,
                                     QVector3D(radius, 0, 0),
                                     QVector3D(f1, f1, f2));
    pxpyb_050_050_patch0_arcs << Arc(pxpyb_050_050_plane, radius,
                                     QVector3D(f1, f1, f2),
                                     QVector3D(0, radius, 0));

    QList<SpherePatch> pxpyb_050_050_patches;
    pxpyb_050_050_patches << SpherePatch(radius, pxpyb_050_050_patch0_arcs);

    Sphere pxpyb_050_050(radius, pxpyb_050_050_patches);

    QTest::newRow("positive x positive y backward 0.5 0.5")
            << pxpyb_050_050_plane
            << pxpyb_050_050;

    QPlane3D pxnyf_050_050_plane = QPlane3D(QVector3D(factor1, -1.0 * factor2, 0.0), QVector3D(1.0, -1.0, 0.0).normalized());

    QTest::newRow("positive x negative y forward 0.5 0.5")
            << pxnyf_050_050_plane
            << empty;

    QPlane3D pxnyb_050_050_plane = QPlane3D(QVector3D(factor1, -1.0 * factor2, 0.0), QVector3D(-1.0, 1.0, 0.0).normalized());

    QTest::newRow("positive x negative y backward 0.5 0.5")
            << pxnyb_050_050_plane
            << basic;

    QPlane3D nxpyf_050_050_plane = QPlane3D(QVector3D(-1.0 * factor1, factor2, 0.0), QVector3D(1.0, -1.0, 0.0).normalized());

    QTest::newRow("negative x positive y forward 0.5 0.5")
            << nxpyf_050_050_plane
            << basic;

    QPlane3D nxpyb_050_050_plane = QPlane3D(QVector3D(-1.0 * factor1, factor2, 0.0), QVector3D(-1.0, 1.0, 0.0).normalized());

    QTest::newRow("negative x positive y backward 0.5 0.5")
            << nxpyb_050_050_plane
            << empty;

    QPlane3D nxnyf_050_050_plane = QPlane3D(QVector3D(-1.0 * factor1, -1.0 * factor2, 0.0), QVector3D(1.0, 1.0, 0.0).normalized());

    QTest::newRow("negative x negative y forward 0.5 0.5")
            << nxnyf_050_050_plane
            << basic;

    QPlane3D nxnyb_050_050_plane = QPlane3D(QVector3D(-1.0 * factor1, -1.0 * factor2, 0.0), QVector3D(-1.0, -1.0, 0.0).normalized());

    QTest::newRow("negative x negative y backward 0.5 0.5")
            << nxnyb_050_050_plane
            << empty;

    factor1 = radius * 0.6;
    factor2 = radius * 0.6;

    f1 = radius * 0.6;
    f2 = f1 * sqrt(7.0 / 9.0);
    f3 = f1 + f2 / sqrt(2);
    f4 = f1 - f2 / sqrt(2);

    QPlane3D pxpyf_060_060_plane = QPlane3D(QVector3D(factor1, factor2, 0.0), QVector3D(1.0, 1.0, 0.0).normalized());

    QList<Arc> pxpyf_060_060_patch0_arcs;
    pxpyf_060_060_patch0_arcs << Arc(pzp, radius,
                                     QVector3D(f3, f4, 0),
                                     QVector3D(f4, f3, 0));
    pxpyf_060_060_patch0_arcs << Arc(pxpyf_060_060_plane, radius,
                                     QVector3D(f4, f3, 0),
                                     QVector3D(f1, f1, f2));
    pxpyf_060_060_patch0_arcs << Arc(pxpyf_060_060_plane, radius,
                                     QVector3D(f1, f1, f2),
                                     QVector3D(f3, f4, 0));

    QList<SpherePatch> pxpyf_060_060_patches;
    pxpyf_060_060_patches << SpherePatch(radius, pxpyf_060_060_patch0_arcs);

    Sphere pxpyf_060_060(radius, pxpyf_060_060_patches);

    QTest::newRow("positive x positive y forward 0.6 0.6")
            << pxpyf_060_060_plane
            << pxpyf_060_060;

    QPlane3D pxpyb_060_060_plane = QPlane3D(QVector3D(factor1, factor2, 0.0), QVector3D(-1.0, -1.0, 0.0).normalized());

    QList<Arc> pxpyb_060_060_patch0_arcs;
    pxpyb_060_060_patch0_arcs << Arc(pzp, radius,
                                     QVector3D(radius, 0, 0),
                                     QVector3D(f3, f4, 0));
    pxpyb_060_060_patch0_arcs << Arc(pxpyb_060_060_plane, radius,
                                     QVector3D(f3, f4, 0),
                                     QVector3D(f1, f1, f2));
    pxpyb_060_060_patch0_arcs << Arc(pxpyb_060_060_plane, radius,
                                     QVector3D(f1, f1, f2),
                                     QVector3D(f4, f3, 0));
    pxpyb_060_060_patch0_arcs << Arc(pzp, radius,
                                     QVector3D(f4, f3, 0),
                                     QVector3D(0, radius, 0));
    pxpyb_060_060_patch0_arcs << Arc(pxp, radius,
                                     QVector3D(0, radius, 0),
                                     QVector3D(0, 0, radius));
    pxpyb_060_060_patch0_arcs << Arc(pyp, radius,
                                     QVector3D(0, 0, radius),
                                     QVector3D(radius, 0, 0));

    QList<SpherePatch> pxpyb_060_060_patches;
    pxpyb_060_060_patches << SpherePatch(radius, pxpyb_060_060_patch0_arcs);

    Sphere pxpyb_060_060(radius, pxpyb_060_060_patches);

    QTest::newRow("positive x positive y backward 0.6 0.6")
            << pxpyb_060_060_plane
            << pxpyb_060_060;

    QPlane3D pxnyf_060_060_plane = QPlane3D(QVector3D(factor1, -1.0 * factor2, 0.0), QVector3D(1.0, -1.0, 0.0).normalized());

    QTest::newRow("positive x negative y forward 0.6 0.6")
            << pxnyf_060_060_plane
            << empty;

    QPlane3D pxnyb_060_060_plane = QPlane3D(QVector3D(factor1, -1.0 * factor2, 0.0), QVector3D(-1.0, 1.0, 0.0).normalized());

    QTest::newRow("positive x negative y backward 0.6 0.6")
            << pxnyb_060_060_plane
            << basic;

    QPlane3D nxpyf_060_060_plane = QPlane3D(QVector3D(-1.0 * factor1, factor2, 0.0), QVector3D(1.0, -1.0, 0.0).normalized());

    QTest::newRow("negative x positive y forward 0.6 0.6")
            << nxpyf_060_060_plane
            << basic;

    QPlane3D nxpyb_060_060_plane = QPlane3D(QVector3D(-1.0 * factor1, factor2, 0.0), QVector3D(-1.0, 1.0, 0.0).normalized());

    QTest::newRow("negative x positive y backward 0.6 0.6")
            << nxpyb_060_060_plane
            << empty;

    QPlane3D nxnyf_060_060_plane = QPlane3D(QVector3D(-1.0 * factor1, -1.0 * factor2, 0.0), QVector3D(1.0, 1.0, 0.0).normalized());

    QTest::newRow("negative x negative y forward 0.6 0.6")
            << nxnyf_060_060_plane
            << basic;

    QPlane3D nxnyb_060_060_plane = QPlane3D(QVector3D(-1.0 * factor1, -1.0 * factor2, 0.0), QVector3D(-1.0, -1.0, 0.0).normalized());

    QTest::newRow("negative x negative y backward 0.6 0.6")
            << nxnyb_060_060_plane
            << empty;

    factor1 = radius * 0.75;
    factor2 = radius * 0.75;

    QPlane3D pxpyf_075_075_plane = QPlane3D(QVector3D(factor1, factor2, 0.0), QVector3D(1.0, 1.0, 0.0).normalized());

    QTest::newRow("positive x positive y forward 0.75 0.75")
            << pxpyf_075_075_plane
            << empty;

    QPlane3D pxpyb_075_075_plane = QPlane3D(QVector3D(factor1, factor2, 0.0), QVector3D(-1.0, -1.0, 0.0).normalized());

    QTest::newRow("positive x positive y backward 0.75 0.75")
            << pxpyb_075_075_plane
            << basic;

    QPlane3D pxnyf_075_075_plane = QPlane3D(QVector3D(factor1, -1.0 * factor2, 0.0), QVector3D(1.0, -1.0, 0.0).normalized());

    QTest::newRow("positive x negative y forward 0.75 0.75")
            << pxnyf_075_075_plane
            << empty;

    QPlane3D pxnyb_075_075_plane = QPlane3D(QVector3D(factor1, -1.0 * factor2, 0.0), QVector3D(-1.0, 1.0, 0.0).normalized());

    QTest::newRow("positive x negative y backward 0.75 0.75")
            << pxnyb_075_075_plane
            << basic;

    QPlane3D nxpyf_075_075_plane = QPlane3D(QVector3D(-1.0 * factor1, factor2, 0.0), QVector3D(1.0, -1.0, 0.0).normalized());

    QTest::newRow("negative x positive y forward 0.75 0.75")
            << nxpyf_075_075_plane
            << basic;

    QPlane3D nxpyb_075_075_plane = QPlane3D(QVector3D(-1.0 * factor1, factor2, 0.0), QVector3D(-1.0, 1.0, 0.0).normalized());

    QTest::newRow("negative x positive y backward 0.75 0.75")
            << nxpyb_075_075_plane
            << empty;

    QPlane3D nxnyf_075_075_plane = QPlane3D(QVector3D(-1.0 * factor1, -1.0 * factor2, 0.0), QVector3D(1.0, 1.0, 0.0).normalized());

    QTest::newRow("negative x negative y forward 0.75 0.75")
            << nxnyf_075_075_plane
            << basic;

    QPlane3D nxnyb_075_075_plane = QPlane3D(QVector3D(-1.0 * factor1, -1.0 * factor2, 0.0), QVector3D(-1.0, -1.0, 0.0).normalized());

    QTest::newRow("negative x negative y backward 0.75 0.75")
            << nxnyb_075_075_plane
            << empty;
}

void tst_Sphere::xy_plane_skew()
{
    QFETCH(QPlane3D, plane);
    QFETCH(Sphere, sphereOut);

    QVERIFY(test(plane, sphereOut));
}

void tst_Sphere::xy_plane_skew_data()
{
    QTest::addColumn<QPlane3D>("plane");
    QTest::addColumn<Sphere>("sphereOut");

    double radius = 20000.0;

    QVector3D origin;

    QPlane3D pxp = QPlane3D(origin, QVector3D(1.0, 0.0, 0.0));
    QPlane3D pyp = QPlane3D(origin, QVector3D(0.0, 1.0, 0.0));
    QPlane3D pzp = QPlane3D(origin, QVector3D(0.0, 0.0, 1.0));

    Sphere empty = emptySphere(radius);
    Sphere basic = basicSphere(radius);

    double factor1 = radius * 0.25;
    double factor2 = radius * 0.5;

    double f1 = radius * 0.5;
    double f2 = radius * sqrt(3) / 2.0;
    double f3 = radius * 0.8;
    double f4 = radius * 0.6;

    QPlane3D pxpyf_025_050_plane = QPlane3D(QVector3D(factor1, factor2, 0.0),
                                            QVector3D(0.5, 0.25, 0.0).normalized());

    QList<Arc> pxpyf_025_050_patch0_arcs;
    pxpyf_025_050_patch0_arcs << Arc(pzp, radius,
                                     QVector3D(radius, 0, 0),
                                     QVector3D(0, radius, 0));
    pxpyf_025_050_patch0_arcs << Arc(pxpyf_025_050_plane, radius,
                                     QVector3D(0, radius, 0),
                                     QVector3D(f1, 0, f2));
    pxpyf_025_050_patch0_arcs << Arc(pyp, radius,
                                     QVector3D(f1, 0, f2),
                                     QVector3D(radius, 0, 0));

    QList<SpherePatch> pxpyf_025_050_patches;
    pxpyf_025_050_patches << SpherePatch(radius, pxpyf_025_050_patch0_arcs);

    Sphere pxpyf_025_050(radius, pxpyf_025_050_patches);

    QTest::newRow("positive x positive y forward 0.25 0.5")
            << pxpyf_025_050_plane
            << pxpyf_025_050;

    QPlane3D pxpyb_025_050_plane = QPlane3D(QVector3D(factor1, factor2, 0.0),
                                            QVector3D(-0.5, -0.25, 0.0).normalized());

    QList<Arc> pxpyb_025_050_patch0_arcs;
    pxpyb_025_050_patch0_arcs << Arc(pyp, radius,
                                     QVector3D(0, 0, radius),
                                     QVector3D(f1, 0, f2));
    pxpyb_025_050_patch0_arcs << Arc(pxpyb_025_050_plane, radius,
                                     QVector3D(f1, 0, f2),
                                     QVector3D(0, radius, 0));
    pxpyb_025_050_patch0_arcs << Arc(pxp, radius,
                                     QVector3D(0, radius, 0),
                                     QVector3D(0, 0, radius));


    QList<SpherePatch> pxpyb_025_050_patches;
    pxpyb_025_050_patches << SpherePatch(radius, pxpyb_025_050_patch0_arcs);

    Sphere pxpyb_025_050(radius, pxpyb_025_050_patches);

    QTest::newRow("positive x positive y backward 0.25 0.5")
            << pxpyb_025_050_plane
            << pxpyb_025_050;

    QPlane3D pxnyf_025_050_plane = QPlane3D(QVector3D(factor1, -1.0 * factor2, 0.0),
                                            QVector3D(-0.5, 0.25, 0.0).normalized());

    QList<Arc> pxnyf_025_050_patch0_arcs;
    pxnyf_025_050_patch0_arcs << Arc(pxp, radius,
                                     QVector3D(0, radius, 0),
                                     QVector3D(0, 0, radius));
    pxnyf_025_050_patch0_arcs << Arc(pyp, radius,
                                     QVector3D(0, 0, radius),
                                     QVector3D(f1, 0, f2));
    pxnyf_025_050_patch0_arcs << Arc(pxnyf_025_050_plane, radius,
                                     QVector3D(f1, 0, f2),
                                     QVector3D(f3, f4, 0));
    pxnyf_025_050_patch0_arcs << Arc(pzp, radius,
                                     QVector3D(f3, f4, 0),
                                     QVector3D(0, radius, 0));

    QList<SpherePatch> pxnyf_025_050_patches;
    pxnyf_025_050_patches << SpherePatch(radius, pxnyf_025_050_patch0_arcs);

    Sphere pxnyf_025_050(radius, pxnyf_025_050_patches);

    QTest::newRow("positive x negative y forward 0.25 0.5")
            << pxnyf_025_050_plane
            << pxnyf_025_050;

    QPlane3D pxnyb_025_050_plane = QPlane3D(QVector3D(factor1, -1.0 * factor2, 0.0),
                                            QVector3D(0.5, -0.25, 0.0).normalized());

    QList<Arc> pxnyb_025_050_patch0_arcs;
    pxnyb_025_050_patch0_arcs << Arc(pzp, radius,
                                     QVector3D(radius, 0, 0),
                                     QVector3D(f3, f4, 0));
    pxnyb_025_050_patch0_arcs << Arc(pxnyb_025_050_plane, radius,
                                     QVector3D(f3, f4, 0),
                                     QVector3D(f1, 0, f2));
    pxnyb_025_050_patch0_arcs << Arc(pyp, radius,
                                     QVector3D(f1, 0, f2),
                                     QVector3D(radius, 0, 0));

    QList<SpherePatch> pxnyb_025_050_patches;
    pxnyb_025_050_patches << SpherePatch(radius, pxnyb_025_050_patch0_arcs);

    Sphere pxnyb_025_050(radius, pxnyb_025_050_patches);

    QTest::newRow("positive x negative y backward 0.25 0.5")
            << pxnyb_025_050_plane
            << pxnyb_025_050;

    QPlane3D nxpyf_025_050_plane = QPlane3D(QVector3D(-1.0 * factor1, factor2, 0.0),
                                            QVector3D(0.5, -0.25, 0.0).normalized());

    QTest::newRow("negative x positive y forward 0.25 0.5")
            << nxpyf_025_050_plane
            << basic;

    QPlane3D nxpyb_025_050_plane = QPlane3D(QVector3D(-1.0 * factor1, factor2, 0.0),
                                            QVector3D(-0.5, 0.25, 0.0).normalized());

    QTest::newRow("negative x positive y backward 0.25 0.5")
            << nxpyb_025_050_plane
            << empty;

    QPlane3D nxnyf_025_050_plane = QPlane3D(QVector3D(-1.0 * factor1, -1.0 * factor2, 0.0),
                                            QVector3D(0.5, 0.25, 0.0).normalized());

    QTest::newRow("negative x negative y forward 0.25 0.5")
            << nxnyf_025_050_plane
            << basic;

    QPlane3D nxnyb_025_050_plane = QPlane3D(QVector3D(-1.0 * factor1, -1.0 * factor2, 0.0),
                                            QVector3D(-0.5, -0.25, 0.0).normalized());

    QTest::newRow("negative x negative y backward 0.25 0.5")
            << nxnyb_025_050_plane
            << empty;

    factor1 = radius * 0.5;
    factor2 = radius * 0.25;

    QPlane3D pxpyf_050_025_plane = QPlane3D(QVector3D(factor1, factor2, 0.0),
                                            QVector3D(0.25, 0.5, 0.0).normalized());

    QList<Arc> pxpyf_050_025_patch0_arcs;
    pxpyf_050_025_patch0_arcs << Arc(pzp, radius,
                                     QVector3D(radius, 0, 0),
                                     QVector3D(0, radius, 0));
    pxpyf_050_025_patch0_arcs << Arc(pxp, radius,
                                     QVector3D(0, radius, 0),
                                     QVector3D(0, f1, f2));
    pxpyf_050_025_patch0_arcs << Arc(pxpyf_050_025_plane, radius,
                                     QVector3D(0, f1, f2),
                                     QVector3D(radius, 0, 0));

    QList<SpherePatch> pxpyf_050_025_patches;
    pxpyf_050_025_patches << SpherePatch(radius, pxpyf_050_025_patch0_arcs);

    Sphere pxpyf_050_025(radius, pxpyf_050_025_patches);

    QTest::newRow("positive x positive y forward 0.5 0.25")
            << pxpyf_050_025_plane
            << pxpyf_050_025;

    QPlane3D pxpyb_050_025_plane = QPlane3D(QVector3D(factor1, factor2, 0.0),
                                            QVector3D(-0.25, -0.5, 0.0).normalized());

    QList<Arc> pxpyb_050_025_patch0_arcs;
    pxpyb_050_025_patch0_arcs << Arc(pyp, radius,
                                     QVector3D(0, 0, radius),
                                     QVector3D(radius, 0, 0));
    pxpyb_050_025_patch0_arcs << Arc(pxpyb_050_025_plane, radius,
                                     QVector3D(radius, 0, 0),
                                     QVector3D(0, f1, f2));
    pxpyb_050_025_patch0_arcs << Arc(pxp, radius,
                                     QVector3D(0, f1, f2),
                                     QVector3D(0, 0, radius));

    QList<SpherePatch> pxpyb_050_025_patches;
    pxpyb_050_025_patches << SpherePatch(radius, pxpyb_050_025_patch0_arcs);

    Sphere pxpyb_050_025(radius, pxpyb_050_025_patches);

    QTest::newRow("positive x positive y backward 0.5 0.25")
            << pxpyb_050_025_plane
            << pxpyb_050_025;

    QPlane3D pxnyf_050_025_plane = QPlane3D(QVector3D(factor1, -1.0 * factor2, 0.0),
                                            QVector3D(-0.25, 0.5, 0.0).normalized());

    QTest::newRow("positive x negative y forward 0.5 0.25")
            << pxnyf_050_025_plane
            << basic;

    QPlane3D pxnyb_050_025_plane = QPlane3D(QVector3D(factor1, -1.0 * factor2, 0.0),
                                            QVector3D(0.25, -0.5, 0.0).normalized());

    QTest::newRow("positive x negative y backward 0.5 0.25")
            << pxnyb_050_025_plane
            << empty;

    QPlane3D nxpyf_050_025_plane = QPlane3D(QVector3D(-1.0 * factor1, factor2, 0.0),
                                            QVector3D(0.25, -0.5, 0.0).normalized());

    QList<Arc> nxpyf_050_025_patch0_arcs;
    nxpyf_050_025_patch0_arcs << Arc(pzp, radius,
                                     QVector3D(radius, 0, 0),
                                     QVector3D(f4, f3, 0));
    nxpyf_050_025_patch0_arcs << Arc(nxpyf_050_025_plane, radius,
                                     QVector3D(f4, f3, 0),
                                     QVector3D(0, f1, f2));
    nxpyf_050_025_patch0_arcs << Arc(pxp, radius,
                                     QVector3D(0, f1, f2),
                                     QVector3D(0, 0, radius));
    nxpyf_050_025_patch0_arcs << Arc(pyp, radius,
                                     QVector3D(0, 0, radius),
                                     QVector3D(radius, 0, 0));

    QList<SpherePatch> nxpyf_050_025_patches;
    nxpyf_050_025_patches << SpherePatch(radius, nxpyf_050_025_patch0_arcs);

    Sphere nxpyf_050_025(radius, nxpyf_050_025_patches);

    QTest::newRow("negative x positive y forward 0.5 0.25")
            << nxpyf_050_025_plane
            << nxpyf_050_025;

    QPlane3D nxpyb_050_025_plane = QPlane3D(QVector3D(-1.0 * factor1, factor2, 0.0),
                                            QVector3D(-0.25, 0.5, 0.0).normalized());

    QList<Arc> nxpyb_050_025_patch0_arcs;
    nxpyb_050_025_patch0_arcs << Arc(pzp, radius,
                                     QVector3D(f4, f3, 0),
                                     QVector3D(0, radius, 0));
    nxpyb_050_025_patch0_arcs << Arc(pxp, radius,
                                     QVector3D(0, radius, 0),
                                     QVector3D(0, f1, f2));
    nxpyb_050_025_patch0_arcs << Arc(nxpyb_050_025_plane, radius,
                                     QVector3D(0, f1, f2),
                                     QVector3D(f4, f3, 0));

    QList<SpherePatch> nxpyb_050_025_patches;
    nxpyb_050_025_patches << SpherePatch(radius, nxpyb_050_025_patch0_arcs);

    Sphere nxpyb_050_025(radius, nxpyb_050_025_patches);

    QTest::newRow("negative x positive y backward 0.5 0.25")
            << nxpyb_050_025_plane
            << nxpyb_050_025;

    QPlane3D nxnyf_050_025_plane = QPlane3D(QVector3D(-1.0 * factor1, -1.0 * factor2, 0.0),
                                            QVector3D(0.25, 0.5, 0.0).normalized());

    QTest::newRow("negative x negative y forward 0.5 0.25")
            << nxnyf_050_025_plane
            << basic;

    QPlane3D nxnyb_050_025_plane = QPlane3D(QVector3D(-1.0 * factor1, -1.0 * factor2, 0.0),
                                            QVector3D(-0.25, -0.5, 0.0).normalized());

    QTest::newRow("negative x negative y backward 0.5 0.25")
            << nxnyb_050_025_plane
            << empty;
}

void tst_Sphere::xyz_plane()
{
    QFETCH(QPlane3D, plane);
    QFETCH(Sphere, sphereOut);

    QVERIFY(test(plane, sphereOut));
}

void tst_Sphere::xyz_plane_data()
{
    QTest::addColumn<QPlane3D>("plane");
    QTest::addColumn<Sphere>("sphereOut");

    double radius = 20000.0;

    QVector3D origin;

    QPlane3D pxp = QPlane3D(origin, QVector3D(1.0, 0.0, 0.0));
    QPlane3D pyp = QPlane3D(origin, QVector3D(0.0, 1.0, 0.0));
    QPlane3D pzp = QPlane3D(origin, QVector3D(0.0, 0.0, 1.0));

    Sphere empty = emptySphere(radius);
    Sphere basic = basicSphere(radius);

    double factor1 = radius * 0.0;
    double factor2 = radius * 0.0;
    double factor3 = radius * 0.0;

    QPlane3D pxpypzf_000 = QPlane3D(QVector3D(factor1, factor2, factor3),
                                    QVector3D(1.0, 1.0, 1.0).normalized());

    QTest::newRow("positive x positive y positive z forward 0.0")
            << pxpypzf_000
            << basic;

    QPlane3D pxpypzb_000 = QPlane3D(QVector3D(factor1, factor2, factor3),
                                    QVector3D(-1.0, -1.0, -1.0).normalized());

    QTest::newRow("positive x positive y positive z backward 0.0")
            << pxpypzb_000
            << empty;

    factor1 = radius * 0.25;
    factor2 = radius * 0.25;
    factor3 = radius * 0.25;

    QPlane3D pxpypzf_025 = QPlane3D(QVector3D(factor1, factor2, factor3),
                                    QVector3D(1.0, 1.0, 1.0).normalized());

    QTest::newRow("positive x positive y positive z forward 0.25")
            << pxpypzf_025
            << basic;

    QPlane3D pxpypzb_025 = QPlane3D(QVector3D(factor1, factor2, factor3),
                                    QVector3D(-1.0, -1.0, -1.0).normalized());

    QTest::newRow("positive x positive y positive z backward 0.25")
            << pxpypzb_025
            << empty;

    factor1 = radius / 3.0;
    factor2 = radius / 3.0;
    factor3 = radius / 3.0;

    QPlane3D pxpypzf_033_plane = QPlane3D(QVector3D(factor1, factor2, factor3),
                                    QVector3D(1.0, 1.0, 1.0).normalized());

    QTest::newRow("positive x positive y positive z forward 0.33")
            << pxpypzf_033_plane
            << basic;

    QPlane3D pxpypzb_033_plane = QPlane3D(QVector3D(factor1, factor2, factor3),
                                          QVector3D(-1.0, -1.0, -1.0).normalized());

    QTest::newRow("positive x positive y positive z backward 0.33")
            << pxpypzb_033_plane
            << empty;

    factor1 = radius * 0.4;
    factor2 = radius * 0.4;
    factor3 = radius * 0.4;

    QPlane3D pxpypzf_040_plane = QPlane3D(QVector3D(factor1, factor2, factor3),
                                          QVector3D(1.0, 1.0, 1.0).normalized());

    double f1 = 19483.3;
    double f2 = 4516.69;

    QList<Arc> pxpypzf_040_patch0_arcs;
    pxpypzf_040_patch0_arcs << Arc(pzp, radius,
                                     QVector3D(f1, f2, 0),
                                     QVector3D(f2, f1, 0));
    pxpypzf_040_patch0_arcs << Arc(pxpypzf_040_plane, radius,
                                     QVector3D(f2, f1, 0),
                                     QVector3D(0, f1, f2));
    pxpypzf_040_patch0_arcs << Arc(pxp, radius,
                                     QVector3D(0, f1, f2),
                                     QVector3D(0, f2, f1));
    pxpypzf_040_patch0_arcs << Arc(pxpypzf_040_plane, radius,
                                     QVector3D(0, f2, f1),
                                     QVector3D(f2, 0, f1));
    pxpypzf_040_patch0_arcs << Arc(pyp, radius,
                                     QVector3D(f2, 0, f1),
                                     QVector3D(f1, 0, f2));
    pxpypzf_040_patch0_arcs << Arc(pxpypzf_040_plane, radius,
                                     QVector3D(f1, 0, f2),
                                     QVector3D(f1, f2, 0));

    QList<SpherePatch> pxpypzf_040_patches;
    pxpypzf_040_patches << SpherePatch(radius, pxpypzf_040_patch0_arcs);

    Sphere pxpypzf_040(radius, pxpypzf_040_patches);

    QTest::newRow("positive x positive y positive z forward 0.4")
            << pxpypzf_040_plane
            << pxpypzf_040;

    QPlane3D pxpypzb_040_plane = QPlane3D(QVector3D(factor1, factor2, factor3),
                                          QVector3D(-1.0, -1.0, -1.0).normalized());

    QList<Arc> pxpypzb_040_patch0_arcs;
    pxpypzb_040_patch0_arcs << Arc(pzp, radius,
                                   QVector3D(radius, 0, 0),
                                   QVector3D(f1, f2, 0));
    pxpypzb_040_patch0_arcs << Arc(pxpypzb_040_plane, radius,
                                   QVector3D(f1, f2, 0),
                                   QVector3D(f1, 0, f2));
    pxpypzb_040_patch0_arcs << Arc(pyp, radius,
                                   QVector3D(f1, 0, f2),
                                   QVector3D(radius, 0, 0));

    QList<Arc> pxpypzb_040_patch1_arcs;
    pxpypzb_040_patch1_arcs << Arc(pxp, radius,
                                   QVector3D(0, radius, 0),
                                   QVector3D(0, f1, f2));
    pxpypzb_040_patch1_arcs << Arc(pxpypzb_040_plane, radius,
                                   QVector3D(0, f1, f2),
                                   QVector3D(f2, f1, 0));
    pxpypzb_040_patch1_arcs << Arc(pzp, radius,
                                   QVector3D(f2, f1, 0),
                                   QVector3D(0, radius, 0));

    QList<Arc> pxpypzb_040_patch2_arcs;
    pxpypzb_040_patch2_arcs << Arc(pyp, radius,
                                   QVector3D(0, 0, radius),
                                   QVector3D(f2, 0, f1));
    pxpypzb_040_patch2_arcs << Arc(pxpypzb_040_plane, radius,
                                   QVector3D(f2, 0, f1),
                                   QVector3D(0, f2, f1));
    pxpypzb_040_patch2_arcs << Arc(pxp, radius,
                                   QVector3D(0, f2, f1),
                                   QVector3D(0, 0, radius));

    QList<SpherePatch> pxpypzb_040_patches;

    pxpypzb_040_patches << SpherePatch(radius, pxpypzb_040_patch0_arcs);
    pxpypzb_040_patches << SpherePatch(radius, pxpypzb_040_patch1_arcs);
    pxpypzb_040_patches << SpherePatch(radius, pxpypzb_040_patch2_arcs);

    Sphere pxpypzb_040(radius, pxpypzb_040_patches);

    QTest::newRow("positive x positive y positive z backward 0.4")
            << pxpypzb_040_plane
            << pxpypzb_040;

    factor1 = radius * 0.5;
    factor2 = radius * 0.5;
    factor3 = radius * 0.5;

    f1 = 18165.0;
    f2 = 5917.52;

    QPlane3D pxpypzf_050_plane = QPlane3D(QVector3D(factor1, factor2, factor3),
                                          QVector3D(1.0, 1.0, 1.0).normalized());

    QList<Arc> pxpypzf_050_patch0_arcs;
    pxpypzf_050_patch0_arcs << Arc(pxpypzf_050_plane, radius,
                                     QVector3D(f1, f2, f2),
                                     QVector3D(f2, f1, f2));
    pxpypzf_050_patch0_arcs << Arc(pxpypzf_050_plane, radius,
                                     QVector3D(f2, f1, f2),
                                     QVector3D(f2, f2, f1));
    pxpypzf_050_patch0_arcs << Arc(pxpypzf_050_plane, radius,
                                     QVector3D(f2, f2, f1),
                                     QVector3D(f1, f2, f2));

    QList<SpherePatch> pxpypzf_050_patches;
    pxpypzf_050_patches << SpherePatch(radius, pxpypzf_050_patch0_arcs);

    Sphere pxpypzf_050(radius, pxpypzf_050_patches);

    QTest::newRow("positive x positive y positive z forward 0.5")
            << pxpypzf_050_plane
            << pxpypzf_050;

    QPlane3D pxpypzb_050_plane = QPlane3D(QVector3D(factor1, factor2, factor3),
                                          QVector3D(-1.0, -1.0, -1.0).normalized());

//    QList<Arc> pxpypzb_050_patch0_arcs;
//    pxpypzb_050_patch0_arcs << Arc(pzp, radius,
//                                   QVector3D(radius, 0, 0),
//                                   QVector3D(f1, f2, 0));
//    pxpypzb_050_patch0_arcs << Arc(pxpypzb_050_plane, radius,
//                                   QVector3D(f1, f2, 0),
//                                   QVector3D(f1, 0, f2));
//    pxpypzb_050_patch0_arcs << Arc(pyp, radius,
//                                   QVector3D(f1, 0, f2),
//                                   QVector3D(radius, 0, 0));

//    QList<Arc> pxpypzb_050_patch1_arcs;
//    pxpypzb_050_patch1_arcs << Arc(pxp, radius,
//                                   QVector3D(0, radius, 0),
//                                   QVector3D(0, f1, f2));
//    pxpypzb_050_patch1_arcs << Arc(pxpypzb_050_plane, radius,
//                                   QVector3D(0, f1, f2),
//                                   QVector3D(f2, f1, 0));
//    pxpypzb_050_patch1_arcs << Arc(pzp, radius,
//                                   QVector3D(f2, f1, 0),
//                                   QVector3D(0, radius, 0));

//    QList<Arc> pxpypzb_050_patch2_arcs;
//    pxpypzb_050_patch2_arcs << Arc(pyp, radius,
//                                   QVector3D(0, 0, radius),
//                                   QVector3D(f2, 0, f1));
//    pxpypzb_050_patch2_arcs << Arc(pxpypzb_050_plane, radius,
//                                   QVector3D(f2, 0, 1),
//                                   QVector3D(0, f2, f1));
//    pxpypzb_050_patch2_arcs << Arc(pxp, radius,
//                                   QVector3D(0, f2, f1),
//                                   QVector3D(0, 0, radius));

    QList<SpherePatch> pxpypzb_050_patches;

//    pxpypzb_050_patches << SpherePatch(radius, pxpypzb_050_patch0_arcs);
//    pxpypzb_050_patches << SpherePatch(radius, pxpypzb_050_patch1_arcs);
//    pxpypzb_050_patches << SpherePatch(radius, pxpypzb_050_patch2_arcs);

    Sphere pxpypzb_050(radius, pxpypzb_050_patches);

    QTest::newRow("positive x positive y positive z backward 0.5")
            << pxpypzb_050_plane
            << pxpypzb_050;


    factor1 = radius / sqrt(3.0);
    factor2 = radius / sqrt(3.0);
    factor3 = radius / sqrt(3.0);

    QPlane3D pxpypzf_057_plane = QPlane3D(QVector3D(factor1, factor2, factor3),
                                          QVector3D(1.0, 1.0, 1.0).normalized());

    QTest::newRow("positive x positive y positive z forward 0.57")
            << pxpypzf_057_plane
            << empty;

    QPlane3D pxpypzb_057_plane = QPlane3D(QVector3D(factor1, factor2, factor3),
                                          QVector3D(-1.0, -1.0, -1.0).normalized());

    QTest::newRow("positive x positive y positive z backward 0.57")
            << pxpypzb_057_plane
            << basic;

    factor1 = radius * 0.6;
    factor2 = radius * 0.6;
    factor3 = radius * 0.6;

    QPlane3D pxpypzf_060_plane = QPlane3D(QVector3D(factor1, factor2, factor3),
                                          QVector3D(1.0, 1.0, 1.0).normalized());

    QTest::newRow("positive x positive y positive z forward 0.6")
            << pxpypzf_060_plane
            << empty;

    QPlane3D pxpypzb_060_plane = QPlane3D(QVector3D(factor1, factor2, factor3),
                                          QVector3D(-1.0, -1.0, -1.0).normalized());

    QTest::newRow("positive x positive y positive z backward 0.6")
            << pxpypzb_060_plane
            << basic;
}

QList<QPlane3D> tst_Sphere::planes() const
{
    CameraData cameraData;

    ViewportCamera cam;
    cam.setProjection(new Projection3D(20000.0));
    cam.setCameraData(cameraData);
    Frustum f = cam.toFrustum();

    QList<QPlane3D> planes;
    planes << f.plane(Frustum::Near);
    planes << f.plane(Frustum::Far);
    planes << f.plane(Frustum::Top);
    planes << f.plane(Frustum::Bottom);
    planes << f.plane(Frustum::Left);
    planes << f.plane(Frustum::Right);

    return planes;
}

Sphere tst_Sphere::planeTest(const QList<QPlane3D> &planes, const QList<int> &planeIndices) const
{
    Sphere sphere;

    for (int i = 0; i < planeIndices.size(); ++i) {
        sphere.intersect(planes.at(planeIndices.at(i)));
    }

    return sphere;
}

void tst_Sphere::camera()
{
    QList<QPlane3D> p = planes();
    // 2-tuples
//    for (int i = 0; i < 5; ++i) {
//        for (int j = i + 1; j < 6; ++j) {
//            QList<int> i1;
//            i1 << i << j;

//            Sphere s1 = planeTest(p, i1);

//            QList<int> i2;
//            i2 << j << i;

//            Sphere s2 = planeTest(p, i2);

//            if (!s1.isomorphic(s2)) {
//                QList<SpherePatch> p1 = s1.patches();
//                QList<SpherePatch> p2 = s2.patches();
//                if (p1.size() != p2.size()) {
//                    qWarning() << i << j << "sizes different";
//                } else {
//                    int s = p1.size();
//                    for (int k = 0; k < s; ++k) {
//                        if (!p1[k].isomorphic(p2[k])) {
//                            qWarning() << i << j << k;
////                                QList<SpherePatch> l1a;
////                                l1a << p1[k];
////                                Sphere sphere1a(20000.0, l1a);
////                                qWarning() << sphere1a;

////                                QList<SpherePatch> l2a;
////                                l2a << p2[k];
////                                Sphere sphere2a(20000.0, l2a);
////                                qWarning() << sphere2a;
//                        }
//                        //qWarning() << k << (p1[k].isomorphic(p2[k]));
//                    }
//                }
//            }
//        }
//    }

    /*
      intersect(...)
      1 2 0
      1 3 0
      1 4 0
      1 5 0
      3 4 7

      intersect2(...)
      1 2 0
      1 3 0
      1 4 0
      1 5 0
      +2 4 7
      3 4 7
      +3 4 8
    */

    int plane1 = 1;
    int plane2 = 2;
    int patch = 0;

    qWarning() << "     ******     ";

    Sphere sphere1;
    sphere1.intersect(p.at(plane1));
    SpherePatch patch1 = sphere1.patches().at(patch);

    qWarning() << "     ******     ";
    QList<SpherePatch> l1a;
    l1a << patch1;
    Sphere sphere1a(20000.0, l1a);
    qWarning() << sphere1a;
    qWarning() << "     ******     ";

    l1a = patch1.intersect(p.at(plane2));

    qWarning() << "     ******     ";
    Sphere sphere1b(20000.0, l1a);
    qWarning() << sphere1b;
    qWarning() << "     ******     ";

    Sphere sphere2;
    sphere2.intersect(p.at(plane2));
    SpherePatch patch2 = sphere2.patches().at(patch);

    qWarning() << "     ******     ";
    QList<SpherePatch> l2a;
    l2a << patch2;
    Sphere sphere2a(20000.0, l2a);
    qWarning() << sphere2a;
    qWarning() << "     ******     ";

    l2a = patch2.intersect(p.at(plane1));

    qWarning() << "     ******     ";
    Sphere sphere2b(20000.0, l2a);
    qWarning() << sphere2b;
    qWarning() << "     ******     ";
}

QTEST_MAIN(tst_Sphere)
#include "tst_sphere.moc"

