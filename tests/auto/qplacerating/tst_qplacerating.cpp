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

#include <qplacerating.h>

QT_USE_NAMESPACE

class tst_QPlaceRating : public QObject
{
    Q_OBJECT

public:
    tst_QPlaceRating();

private Q_SLOTS:
    void constructorTest();
    void valueTest();
    void countTest();
    void operatorsTest();
};

tst_QPlaceRating::tst_QPlaceRating()
{
}

void tst_QPlaceRating::constructorTest()
{
    QPlaceRating testObj;
    Q_UNUSED(testObj);

    QPlaceRating *testObjPtr = new QPlaceRating(testObj);
    QVERIFY2(testObjPtr != NULL, "Copy constructor - null");
    QVERIFY2(testObjPtr->count() == 0, "Copy constructor - wrong count");
    QVERIFY2(testObjPtr->value() == 0, "Copy constructor - wrong value");
    QVERIFY2(*testObjPtr == testObj, "Copy constructor - compare");
    delete testObjPtr;
}

void tst_QPlaceRating::valueTest()
{
    QPlaceRating testObj;
    QVERIFY2(testObj.value() == 0, "Wrong default value");
    testObj.setValue(-10.23);
    QVERIFY2(testObj.value() == -10.23, "Wrong value returned");
}

void tst_QPlaceRating::countTest()
{
    QPlaceRating testObj;
    QVERIFY2(testObj.count() == 0, "Wrong default value");
    testObj.setCount(-1002);
    QVERIFY2(testObj.count() == -1002, "Wrong value returned");
}

void tst_QPlaceRating::operatorsTest()
{
    QPlaceRating testObj;
    testObj.setValue(0.123);
    QPlaceRating testObj2;
    testObj2 = testObj;
    QVERIFY2(testObj == testObj2, "Not copied correctly");
    testObj2.setCount(-10);
    QVERIFY2(testObj != testObj2, "Object should be different");
}

QTEST_APPLESS_MAIN(tst_QPlaceRating);

#include "tst_qplacerating.moc"
