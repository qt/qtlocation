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

#include <QtCore/QString>
#include <QtTest/QtTest>

#include <qplacecategory.h>

QT_USE_NAMESPACE

class tst_QPlaceCategory : public QObject
{
    Q_OBJECT

public:
    tst_QPlaceCategory();

private Q_SLOTS:
    void constructorTest();
    void categoryIdTest();
    void nameTest();
    void operatorsTest();
};

tst_QPlaceCategory::tst_QPlaceCategory()
{
}

void tst_QPlaceCategory::constructorTest()
{
    QPlaceCategory testObj;
    Q_UNUSED(testObj);

    testObj.setCategoryId("testId");
    QPlaceCategory *testObjPtr = new QPlaceCategory(testObj);
    QVERIFY2(testObjPtr != NULL, "Copy constructor - null");
    QVERIFY2(*testObjPtr == testObj, "Copy constructor - compare");
    delete testObjPtr;
}

void tst_QPlaceCategory::categoryIdTest()
{
    QPlaceCategory testObj;
    QVERIFY2(testObj.categoryId() == QString(), "Wrong default value");
    testObj.setCategoryId("testText");
    QVERIFY2(testObj.categoryId() == "testText", "Wrong value returned");
}

void tst_QPlaceCategory::nameTest()
{
    QPlaceCategory testObj;
    QVERIFY2(testObj.name() == QString(), "Wrong default value");
    testObj.setName("testText");
    QVERIFY2(testObj.name() == "testText", "Wrong value returned");
}

void tst_QPlaceCategory::operatorsTest()
{
    QPlaceCategory testObj;
    testObj.setName("testValue");
    QPlaceCategory testObj2;
    testObj2 = testObj;
    QVERIFY2(testObj == testObj2, "Not copied correctly");
    testObj2.setCategoryId("a3rfg");
    QVERIFY2(testObj != testObj2, "Object should be different");
}

QTEST_APPLESS_MAIN(tst_QPlaceCategory);

#include "tst_qplacecategory.moc"
