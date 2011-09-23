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

#include <qplaceimage.h>

QT_USE_NAMESPACE

class tst_QPlaceImage : public QObject
{
    Q_OBJECT

public:
    tst_QPlaceImage();

private Q_SLOTS:
    void constructorTest();
    void supplierTest();
    void urlTest();
    void thumbnailUrlTest();
    void idTest();
    void metaInfoTest();
    void mimeTypeTest();
    void operatorsTest();
};

tst_QPlaceImage::tst_QPlaceImage()
{
}

void tst_QPlaceImage::constructorTest()
{
    QPlaceImage testObj;
    testObj.setId("testId");
    QPlaceImage *testObjPtr = new QPlaceImage(testObj);
    QVERIFY2(testObjPtr != NULL, "Copy constructor - null");
    QVERIFY2(*testObjPtr == testObj, "Copy constructor - compare");
    delete testObjPtr;
}

void tst_QPlaceImage::supplierTest()
{
    QPlaceImage testObj;
    QVERIFY2(testObj.supplier().supplierId() == QString(), "Wrong default value");
    QPlaceSupplier sup;
    sup.setName("testName1");
    sup.setSupplierId("testId");
    testObj.setSupplier(sup);
    QVERIFY2(testObj.supplier() == sup, "Wrong value returned");
}

void tst_QPlaceImage::urlTest()
{
    QUrl imageUrl("testText");

    QPlaceImage testObj;
    QVERIFY2(testObj.url() == QString(), "Wrong default value");
    testObj.setUrl(imageUrl);
    QVERIFY2(testObj.url() == imageUrl, "Wrong value returned");
}

void tst_QPlaceImage::thumbnailUrlTest()
{
    QUrl thumbnailUrl("testText");

    QPlaceImage testObj;
    QVERIFY2(testObj.thumbnailUrl() == QString(), "Wrong default value");
    testObj.setThumbnailUrl(thumbnailUrl);
    QVERIFY2(testObj.thumbnailUrl() == thumbnailUrl, "Wrong value returned");
}

void tst_QPlaceImage::idTest()
{
    QPlaceImage testObj;
    QVERIFY2(testObj.id() == QString(), "Wrong default value");
    testObj.setId("testText");
    QVERIFY2(testObj.id() == "testText", "Wrong value returned");
}

void tst_QPlaceImage::metaInfoTest()
{
    QPlaceImage testObj;
    QVERIFY2(testObj.metaInfo() == QString(), "Wrong default value");
    testObj.setMetaInfo("testText");
    QVERIFY2(testObj.metaInfo() == "testText", "Wrong value returned");
}

void tst_QPlaceImage::mimeTypeTest()
{
    QPlaceImage testObj;
    QVERIFY2(testObj.mimeType() == QString(), "Wrong default value");
    testObj.setMimeType("testText");
    QVERIFY2(testObj.mimeType() == "testText", "Wrong value returned");
}

void tst_QPlaceImage::operatorsTest()
{
    QPlaceImage testObj;
    testObj.setMimeType("testValue");
    QPlaceImage testObj2;
    testObj2 = testObj;
    QVERIFY2(testObj == testObj2, "Not copied correctly");
    testObj2.setId("testValue2");
    QVERIFY2(testObj != testObj2, "Object should be different");
}

QTEST_APPLESS_MAIN(tst_QPlaceImage);

#include "tst_qplaceimage.moc"
