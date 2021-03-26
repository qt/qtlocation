/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QtTest/private/qpropertytesthelper_p.h>
#include <QtPositioning/private/qgeocoordinateobject_p.h>

QT_USE_NAMESPACE

class tst_QGeoCoordinateObject : public QObject
{
    Q_OBJECT
private slots:
    void constructor();
    void equality();
    void equality_data();
    void coordinateBinding();
};

void tst_QGeoCoordinateObject::constructor()
{
    QGeoCoordinateObject defaultConstructed;
    QCOMPARE(defaultConstructed.coordinate(), QGeoCoordinate());

    QGeoCoordinate c(1.0, 2.0, 3.0);
    QGeoCoordinateObject co(c);
    QCOMPARE(co.coordinate(), c);
}

void tst_QGeoCoordinateObject::equality()
{
    QFETCH(QGeoCoordinate, lhs);
    QFETCH(QGeoCoordinate, rhs);
    QFETCH(bool, expectedResult);

    QGeoCoordinateObject leftObj(lhs);
    QGeoCoordinateObject rightObj(rhs);

    QVERIFY(leftObj == lhs);
    QVERIFY(rightObj == rhs);

    QCOMPARE(leftObj == rhs, expectedResult);
    QCOMPARE(leftObj != rhs, !expectedResult);
    QCOMPARE(rightObj == lhs, expectedResult);
    QCOMPARE(rightObj != lhs, !expectedResult);
    QCOMPARE(leftObj == rightObj, expectedResult);
    QCOMPARE(leftObj != rightObj, !expectedResult);
}

void tst_QGeoCoordinateObject::equality_data()
{
    QTest::addColumn<QGeoCoordinate>("lhs");
    QTest::addColumn<QGeoCoordinate>("rhs");
    QTest::addColumn<bool>("expectedResult");

    QTest::newRow("two invalid") << QGeoCoordinate() << QGeoCoordinate() << true;
    QTest::newRow("same valid") << QGeoCoordinate(1.0, 2.0, 3.0) << QGeoCoordinate(1.0, 2.0, 3.0)
                                << true;
    QTest::newRow("invalid vs valid") << QGeoCoordinate() << QGeoCoordinate(1.0, 2.0, 3.0) << false;
    QTest::newRow("different valid")
            << QGeoCoordinate(1.0, 2.0, 3.0) << QGeoCoordinate(2.0, 3.0, 4.0) << false;
}

void tst_QGeoCoordinateObject::coordinateBinding()
{
    QGeoCoordinateObject obj;
    const QGeoCoordinate initial(1, 2, 3);
    const QGeoCoordinate changed(4, 5, 6);
    QTestPrivate::testReadWritePropertyBasics<QGeoCoordinateObject, QGeoCoordinate>(
            obj, initial, changed, "coordinate");
}

QTEST_GUILESS_MAIN(tst_QGeoCoordinateObject)
#include "tst_qgeocoordinateobject.moc"
