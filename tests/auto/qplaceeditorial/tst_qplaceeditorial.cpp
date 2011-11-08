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

#include <QtLocation/QPlaceEditorial>
#include <QtLocation/QPlaceSupplier>

QT_USE_NAMESPACE

class tst_QPlaceEditorial : public QObject
{
    Q_OBJECT

public:
    tst_QPlaceEditorial();

private Q_SLOTS:
    void constructorTest();
    void supplierTest();
    void textTest();
    void titleTest();
    void languageTest();
    void operatorsTest();
};

tst_QPlaceEditorial::tst_QPlaceEditorial()
{
}

void tst_QPlaceEditorial::constructorTest()
{
    QPlaceEditorial testObj;
    testObj.setText("testId");
    QPlaceEditorial *testObjPtr = new QPlaceEditorial(testObj);
    QVERIFY2(testObjPtr != NULL, "Copy constructor - null");
    QVERIFY2(*testObjPtr == testObj, "Copy constructor - compare");
    delete testObjPtr;
}

void tst_QPlaceEditorial::supplierTest()
{
    QPlaceEditorial testObj;
    QVERIFY2(testObj.supplier().supplierId() == QString(), "Wrong default value");
    QPlaceSupplier sup;
    sup.setName("testName1");
    sup.setSupplierId("testId");
    testObj.setSupplier(sup);
    QVERIFY2(testObj.supplier() == sup, "Wrong value returned");
}

void tst_QPlaceEditorial::textTest()
{
    QPlaceEditorial testObj;
    QVERIFY2(testObj.text() == QString(), "Wrong default value");
    testObj.setText("testText");
    QVERIFY2(testObj.text() == "testText", "Wrong value returned");
}

void tst_QPlaceEditorial::titleTest()
{
    QPlaceEditorial testObj;
    QVERIFY2(testObj.title() == QString(), "Wrong default value");
    testObj.setTitle("testText");
    QVERIFY2(testObj.title() == "testText", "Wrong value returned");
}

void tst_QPlaceEditorial::languageTest()
{
    QPlaceEditorial testObj;
    QVERIFY2(testObj.language() == QString(), "Wrong default value");
    testObj.setLanguage("testText");
    QVERIFY2(testObj.language() == "testText", "Wrong value returned");
}

void tst_QPlaceEditorial::operatorsTest()
{
    QPlaceEditorial testObj;
    testObj.setLanguage("testValue");
    QPlaceEditorial testObj2;
    testObj2 = testObj;
    QVERIFY2(testObj == testObj2, "Not copied correctly");
    testObj2.setText("testValue2");
    QVERIFY2(testObj != testObj2, "Object should be different");
}

QTEST_APPLESS_MAIN(tst_QPlaceEditorial);

#include "tst_qplaceeditorial.moc"
