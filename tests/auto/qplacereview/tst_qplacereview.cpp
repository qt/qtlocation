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

#include <QtLocation/QPlaceReview>
#include <QtLocation/QPlaceSupplier>

QT_USE_NAMESPACE

class tst_QPlaceReview : public QObject
{
    Q_OBJECT

public:
    tst_QPlaceReview();

private Q_SLOTS:
    void constructorTest();
    void supplierTest();
    void dateTest();
    void descriptionTest();
    void languageTest();
    void ratingTest();
    void reviewIdTest();
    void titleTest();
    void userIdTest();
    void userNameTest();
    void sourceUrlTest();
    void operatorsTest();
};

tst_QPlaceReview::tst_QPlaceReview()
{
}

void tst_QPlaceReview::constructorTest()
{
    QPlaceReview testObj;
    testObj.setLanguage("testId");
    QPlaceReview *testObjPtr = new QPlaceReview(testObj);
    QVERIFY2(testObjPtr != NULL, "Copy constructor - null");
    QVERIFY2(*testObjPtr == testObj, "Copy constructor - compare");
    delete testObjPtr;
}

void tst_QPlaceReview::supplierTest()
{
    QPlaceReview testObj;
    QVERIFY2(testObj.supplier().supplierId() == QString(), "Wrong default value");
    QPlaceSupplier sup;
    sup.setName("testName1");
    sup.setSupplierId("testId");
    testObj.setSupplier(sup);
    QVERIFY2(testObj.supplier() == sup, "Wrong value returned");
}

void tst_QPlaceReview::dateTest()
{
    QPlaceReview testObj;
    QCOMPARE(testObj.dateTime(), QDateTime());

    QDateTime dt = QDateTime::currentDateTime();
    testObj.setDateTime(dt);
    QCOMPARE(testObj.dateTime(), dt);
}

void tst_QPlaceReview::descriptionTest()
{
    QPlaceReview testObj;
    QVERIFY2(testObj.content() == QString(), "Wrong default value");
    testObj.setContent("testText");
    QVERIFY2(testObj.content() == "testText", "Wrong value returned");
}

void tst_QPlaceReview::languageTest()
{
    QPlaceReview testObj;
    QVERIFY2(testObj.language() == QString(), "Wrong default value");
    testObj.setLanguage("testText");
    QVERIFY2(testObj.language() == "testText", "Wrong value returned");
}

void tst_QPlaceReview::ratingTest()
{
    QPlaceReview testObj;
    QVERIFY2(testObj.rating() == 0, "Wrong default value");
    testObj.setRating(-10);
    QVERIFY2(testObj.rating() == -10, "Wrong value returned");
    testObj.setRating(3.4);
    QVERIFY2(testObj.rating() == 3.4, "Wrong value returned");
}

void tst_QPlaceReview::operatorsTest()
{
    QPlaceReview testObj;
    testObj.setContent("testValue");
    QPlaceReview testObj2;
    testObj2 = testObj;
    QVERIFY2(testObj == testObj2, "Not copied correctly");
    testObj2.setLanguage("testValue2");
    QVERIFY2(testObj != testObj2, "Object should be different");
}

void tst_QPlaceReview::reviewIdTest()
{
    QPlaceReview testObj;
    QVERIFY2(testObj.reviewId() == QString(), "Wrong default value");
    testObj.setReviewId("testText");
    QVERIFY2(testObj.reviewId() == "testText", "Wrong value returned");
}
void tst_QPlaceReview::titleTest()
{
    QPlaceReview testObj;
    QVERIFY2(testObj.title() == QString(), "Wrong default value");
    testObj.setTitle("testText");
    QVERIFY2(testObj.title() == "testText", "Wrong value returned");
}
void tst_QPlaceReview::userIdTest()
{
    QPlaceReview testObj;
    QVERIFY2(testObj.userId() == QString(), "Wrong default value");
    testObj.setUserId("testText");
    QVERIFY2(testObj.userId() == "testText", "Wrong value returned");
}
void tst_QPlaceReview::userNameTest()
{
    QPlaceReview testObj;
    QVERIFY2(testObj.userName() == QString(), "Wrong default value");
    testObj.setUserName("testText");
    QVERIFY2(testObj.userName() == "testText", "Wrong value returned");
}
void tst_QPlaceReview::sourceUrlTest()
{
    QPlaceReview testObj;
    QVERIFY2(testObj.sourceUrl().isEmpty(), "Wrong default value");
    const QUrl testUrl = QUrl::fromEncoded("http://example.com/testUrl");
    testObj.setSourceUrl(testUrl);
    QCOMPARE(testObj.sourceUrl(), testUrl);
}

QTEST_APPLESS_MAIN(tst_QPlaceReview)

#include "tst_qplacereview.moc"
