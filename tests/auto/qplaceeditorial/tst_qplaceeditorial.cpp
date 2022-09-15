/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
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

#include <QtCore/QString>
#include <QtTest/QtTest>

#include <QtLocation/QPlaceEditorial>
#include <QtLocation/QPlaceSupplier>
#include <QtLocation/QPlaceUser>

#include "../utils/qlocationtestutils_p.h"

QT_USE_NAMESPACE

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED

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

QT_WARNING_POP

QTEST_APPLESS_MAIN(tst_QPlaceEditorial);

#include "tst_qplaceeditorial.moc"
