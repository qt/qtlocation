/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#include <QtTest/QtTest>
#include <qgeoboundingcircle.h>
#include <qgeocoordinate.h>

QT_USE_NAMESPACE

Q_DECLARE_METATYPE(QGeoCoordinate)

class tst_QGeoBoundingCircle : public QObject
{
    Q_OBJECT

private slots:
    void defaultConstructor();
    void centerRadiusConstructor();

    void comparison();
    void type();

    void radius();
    void center();

    void translate_data();
    void translate();

    void valid_data();
    void valid();

    void empty_data();
    void empty();

    void contains_data();
    void contains();

    void clone();
};

void tst_QGeoBoundingCircle::defaultConstructor()
{
    QGeoBoundingCircle c;
    QVERIFY(!c.center().isValid());
    QCOMPARE(c.radius(), qreal(-1.0));
}

void tst_QGeoBoundingCircle::centerRadiusConstructor()
{
    QGeoBoundingCircle c(QGeoCoordinate(1,1), qreal(50.0));
    QCOMPARE(c.center(), QGeoCoordinate(1,1));
    QCOMPARE(c.radius(), qreal(50.0));
}

void tst_QGeoBoundingCircle::comparison()
{
    QGeoBoundingCircle c1(QGeoCoordinate(1,1), qreal(50.0));
    QGeoBoundingCircle c2(QGeoCoordinate(1,1), qreal(50.0));
    QGeoBoundingCircle c3(QGeoCoordinate(1,1), qreal(35.0));
    QGeoBoundingCircle c4(QGeoCoordinate(1,2), qreal(50.0));

    QVERIFY(c1 == c2);
    QVERIFY(c1 != c3);
    QVERIFY(c1 != c4);
    QVERIFY(c2 != c3);
}

void tst_QGeoBoundingCircle::type()
{
    QGeoBoundingCircle c;
    QCOMPARE(c.type(), QGeoBoundingArea::CircleType);
}

void tst_QGeoBoundingCircle::radius()
{
    QGeoBoundingCircle c;
    c.setRadius(1.0);
    QCOMPARE(c.radius(), qreal(1.0));
    c.setRadius(5.0);
    QCOMPARE(c.radius(), qreal(5.0));
}

void tst_QGeoBoundingCircle::center()
{
    QGeoBoundingCircle c;
    c.setCenter(QGeoCoordinate(1,1));
    QCOMPARE(c.center(), QGeoCoordinate(1,1));
    c.setCenter(QGeoCoordinate(5,10));
    QCOMPARE(c.center(), QGeoCoordinate(5,10));
}

void tst_QGeoBoundingCircle::translate_data()
{
    QTest::addColumn<QGeoCoordinate>("center");
    QTest::addColumn<qreal>("radius");
    QTest::addColumn<double>("lat");
    QTest::addColumn<double>("lon");
    QTest::addColumn<QGeoCoordinate>("newCenter");

    QTest::newRow("from 0,0") << QGeoCoordinate(0,0) << qreal(10.0) <<
                                 5.0 << 5.0 << QGeoCoordinate(5.0, 5.0);
    QTest::newRow("across 0,0") << QGeoCoordinate(-2, -2) << qreal(20.0) <<
                                   5.0 << 5.0 << QGeoCoordinate(3.0, 3.0);
    QTest::newRow("backwards across 0,0") << QGeoCoordinate(5,5) << qreal(50.0)
                                              << -13.0 << 5.0
                                              << QGeoCoordinate(-8.0, 10.0);
}

void tst_QGeoBoundingCircle::translate()
{
    QFETCH(QGeoCoordinate, center);
    QFETCH(qreal, radius);
    QFETCH(double, lat);
    QFETCH(double, lon);
    QFETCH(QGeoCoordinate, newCenter);

    QGeoBoundingCircle c(center, radius);
    QGeoBoundingCircle d = c;

    c.translate(lat, lon);

    QCOMPARE(c.radius(), radius);
    QCOMPARE(c.center(), newCenter);

    c = d.translated(lat, lon);
    d.setRadius(1.0);

    QCOMPARE(c.radius(), radius);
    QCOMPARE(d.center(), center);
    QCOMPARE(c.center(), newCenter);
}

void tst_QGeoBoundingCircle::valid_data()
{
    QTest::addColumn<QGeoCoordinate>("center");
    QTest::addColumn<qreal>("radius");
    QTest::addColumn<bool>("valid");

    QTest::newRow("default") << QGeoCoordinate() << qreal(-1.0) << false;
    QTest::newRow("empty coord") << QGeoCoordinate() << qreal(5.0) << false;
    QTest::newRow("NaN coord") << QGeoCoordinate(500, 500) << qreal(5.0) << false;
    QTest::newRow("bad radius") << QGeoCoordinate(10, 10) << qreal(-5.0) << false;
    QTest::newRow("NaN radius") << QGeoCoordinate(10, 10) << qreal(qQNaN()) << false;
    QTest::newRow("zero radius") << QGeoCoordinate(10, 10) << qreal(0.0) << true;
    QTest::newRow("good") << QGeoCoordinate(10, 10) << qreal(5.0) << true;
}

void tst_QGeoBoundingCircle::valid()
{
    QFETCH(QGeoCoordinate, center);
    QFETCH(qreal, radius);
    QFETCH(bool, valid);

    QGeoBoundingCircle c(center, radius);
    QCOMPARE(c.isValid(), valid);
}

void tst_QGeoBoundingCircle::empty_data()
{
    QTest::addColumn<QGeoCoordinate>("center");
    QTest::addColumn<qreal>("radius");
    QTest::addColumn<bool>("empty");

    QTest::newRow("default") << QGeoCoordinate() << qreal(-1.0) << true;
    QTest::newRow("empty coord") << QGeoCoordinate() << qreal(5.0) << true;
    QTest::newRow("NaN coord") << QGeoCoordinate(500, 500) << qreal(5.0) << true;
    QTest::newRow("bad radius") << QGeoCoordinate(10, 10) << qreal(-5.0) << true;
    QTest::newRow("NaN radius") << QGeoCoordinate(10, 10) << qreal(qQNaN()) << true;
    QTest::newRow("zero radius") << QGeoCoordinate(10, 10) << qreal(0.0) << true;
    QTest::newRow("good") << QGeoCoordinate(10, 10) << qreal(5.0) << false;
}

void tst_QGeoBoundingCircle::empty()
{
    QFETCH(QGeoCoordinate, center);
    QFETCH(qreal, radius);
    QFETCH(bool, empty);

    QGeoBoundingCircle c(center, radius);
    QCOMPARE(c.isEmpty(), empty);
}

void tst_QGeoBoundingCircle::contains_data()
{
    QTest::addColumn<QGeoCoordinate>("center");
    QTest::addColumn<qreal>("radius");
    QTest::addColumn<QGeoCoordinate>("probe");
    QTest::addColumn<bool>("result");

    QTest::newRow("own centre") << QGeoCoordinate(1,1) << qreal(100.0) <<
                                   QGeoCoordinate(1,1) << true;
    QTest::newRow("over the hills") << QGeoCoordinate(1,1) << qreal(100.0) <<
                                       QGeoCoordinate(30, 40) << false;
    QTest::newRow("at 0.5*radius") << QGeoCoordinate(1,1) << qreal(100.0) <<
                                      QGeoCoordinate(1.00015374,1.00015274) << true;
    QTest::newRow("at 0.99*radius") << QGeoCoordinate(1,1) << qreal(100.0) <<
                                       QGeoCoordinate(1.00077538, 0.99955527) << true;
    QTest::newRow("at 1.01*radius") << QGeoCoordinate(1,1) << qreal(100.0) <<
                                       QGeoCoordinate(1.00071413, 0.99943423) << false;
}

void tst_QGeoBoundingCircle::contains()
{
    QFETCH(QGeoCoordinate, center);
    QFETCH(qreal, radius);
    QFETCH(QGeoCoordinate, probe);
    QFETCH(bool, result);

    QGeoBoundingCircle c(center, radius);
    QCOMPARE(c.contains(probe), result);
}

void tst_QGeoBoundingCircle::clone()
{
    //check that the clone copies the same data as the original
    QGeoBoundingCircle originalCircle;
    originalCircle.setCenter(QGeoCoordinate(1,1));
    originalCircle.setRadius(500.0);
    QGeoBoundingArea *areaPtr = originalCircle.clone();
    QVERIFY(areaPtr->type() == QGeoBoundingArea::CircleType);
    QGeoBoundingCircle *clonedCirclePtr;
    clonedCirclePtr = static_cast<QGeoBoundingCircle*>(areaPtr);
    QVERIFY2(clonedCirclePtr->center() == QGeoCoordinate(1,1),
             "Center of clone does not match original");
    QVERIFY2(clonedCirclePtr->radius() == 500.0,
             "Radius of clone does not match original");

    //check that when the original is altered, the clone remains unaltered.
    originalCircle.setCenter(QGeoCoordinate(9,9));
    originalCircle.setRadius(99.9);

    QVERIFY2(originalCircle.center() == QGeoCoordinate(9,9),
             "Center of original has not changed");
    QVERIFY2(originalCircle.radius() == 99.9,
             "Radius of original has not changed");

    QVERIFY2(clonedCirclePtr->center() == QGeoCoordinate(1,1),
             "Center of clone references center of original");
    QVERIFY2(clonedCirclePtr->radius() == 500.0,
             "Radius of clone references radius of original");
}

QTEST_MAIN(tst_QGeoBoundingCircle)
#include "tst_qgeoboundingcircle.moc"
