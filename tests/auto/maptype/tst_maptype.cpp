/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/QString>
#include <QtTest/QtTest>

#include <QtLocation/private/qgeomaptype_p.h>

QT_USE_NAMESPACE

class tst_MapType : public QObject
{
    Q_OBJECT

public:
    tst_MapType();

private Q_SLOTS:
    void constructorTest();
    void styleTest();
    void nameTest();
    void descTest();
    void mobileTest();
    void mapIdTest();
    void comparison();
};

tst_MapType::tst_MapType() {}

void tst_MapType::constructorTest()
{
    QGeoMapType *testObjPtr = new QGeoMapType(QGeoMapType::StreetMap, "street name", "street desc", false, 0);
    QVERIFY(testObjPtr);
    delete testObjPtr;

    testObjPtr = new QGeoMapType();
    QVERIFY2(testObjPtr->style() == QGeoMapType::NoMap, "Wrong default value");
    QVERIFY2(testObjPtr->name() == "", "Wrong default value");
    QVERIFY2(testObjPtr->description() == "", "Wrong default value");
    QVERIFY2(testObjPtr->mobile() == false, "Wrong default value");
    QVERIFY2(testObjPtr->mapId() == 0, "Wrong default value");
    delete testObjPtr;
}

void tst_MapType::styleTest()
{
    QGeoMapType testObj = QGeoMapType(QGeoMapType::StreetMap, "street name", "street desc", false, 0);
    QVERIFY2(testObj.style() == QGeoMapType::StreetMap, "Wrong value returned");
}

void tst_MapType::nameTest()
{
    QGeoMapType testObj = QGeoMapType(QGeoMapType::StreetMap, "street name", "street desc", false, 0);
    QVERIFY2(testObj.name() == "street name", "Wrong value returned");
}

void tst_MapType::descTest()
{
    QGeoMapType testObj = QGeoMapType(QGeoMapType::StreetMap, "street name", "street desc", false, 0);
    QVERIFY2(testObj.description() == "street desc", "Wrong value returned");
}

void tst_MapType::mobileTest()
{
    QGeoMapType testObj = QGeoMapType(QGeoMapType::StreetMap, "mobile street name", "mobile street desc", true, 0);
    QVERIFY2(testObj.mobile() == true, "Wrong value returned");
}

void tst_MapType::mapIdTest()
{
    QGeoMapType testObj = QGeoMapType(QGeoMapType::StreetMap, "mobile street name", "mobile street desc", true, 42);
    QVERIFY2(testObj.mapId() == 42, "Wrong value returned");
}

void tst_MapType::comparison()
{
    // TODO Tried using data function but couldn't make QFETCH work
    QGeoMapType t1 = QGeoMapType(QGeoMapType::StreetMap, "street name", "street desc", false, 42);
    QGeoMapType t2 = QGeoMapType(QGeoMapType::StreetMap, "street name", "street desc", false, 42);
    QCOMPARE(t1 == t2, true);
    QCOMPARE(t1 != t2, false);

    t1 = QGeoMapType();
    t2 = QGeoMapType();
    QCOMPARE(t1 == t2, true);

    t1 = QGeoMapType(QGeoMapType::StreetMap, "street name", "street desc", false, 42);
    t2 = QGeoMapType(QGeoMapType::TerrainMap, "street name", "street desc", false, 42);
    QCOMPARE(t1 == t2, false);

    t1 = QGeoMapType(QGeoMapType::StreetMap, "street name", "street desc", false, 42);
    t2 = QGeoMapType(QGeoMapType::StreetMap, "different name", "street desc", false, 42);
    QCOMPARE(t1 == t2, false);

    t1 = QGeoMapType(QGeoMapType::StreetMap, "street name", "street desc", false, 42);
    t2 = QGeoMapType(QGeoMapType::StreetMap, "street name", "different desc", false, 42);
    QCOMPARE(t1 == t2, false);

    t1 = QGeoMapType(QGeoMapType::StreetMap, "street name", "street desc", false, 42);
    t2 = QGeoMapType(QGeoMapType::StreetMap, "street name", "street desc", true, 42);
    QCOMPARE(t1 == t2, false);

    t1 = QGeoMapType(QGeoMapType::StreetMap, "street name", "street desc", false, 42);
    t2 = QGeoMapType(QGeoMapType::StreetMap, "street name", "street desc", false, 99);
    QCOMPARE(t1 == t2, false);
}

QTEST_APPLESS_MAIN(tst_MapType)

#include "tst_maptype.moc"
