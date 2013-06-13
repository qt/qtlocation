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

#include <qplacesupplier.h>

QT_USE_NAMESPACE

class tst_QPlaceSupplier : public QObject
{
    Q_OBJECT

public:
    tst_QPlaceSupplier();

private Q_SLOTS:
    void constructorTest();
    void nameTest();
    void supplierIdTest();
    void urlTest();
    void iconTest();
    void operatorsTest();
    void isEmptyTest();
};

tst_QPlaceSupplier::tst_QPlaceSupplier()
{
}

void tst_QPlaceSupplier::constructorTest()
{
    QPlaceSupplier testObj;
    Q_UNUSED(testObj);

    QPlaceSupplier *testObjPtr = new QPlaceSupplier(testObj);
    QVERIFY2(testObjPtr != NULL, "Copy constructor - null");
    QVERIFY2(*testObjPtr == testObj, "Copy constructor - compare");
    delete testObjPtr;
}

void tst_QPlaceSupplier::nameTest()
{
    QPlaceSupplier testObj;
    QVERIFY2(testObj.name() == QString(), "Wrong default value");
    testObj.setName("testText");
    QVERIFY2(testObj.name() == "testText", "Wrong value returned");
}

void tst_QPlaceSupplier::supplierIdTest()
{
    QPlaceSupplier testObj;
    QVERIFY2(testObj.supplierId() == QString(), "Wrong default value");
    testObj.setSupplierId("testText");
    QVERIFY2(testObj.supplierId() == "testText", "Wrong value returned");
}

void tst_QPlaceSupplier::urlTest()
{
    QPlaceSupplier testObj;
    const QUrl testUrl = QUrl::fromEncoded("http://example.com/testUrl");
    QVERIFY2(testObj.url() == QString(), "Wrong default value");
    testObj.setUrl(testUrl);
    QVERIFY2(testObj.url() == testUrl, "Wrong value returned");
}

void tst_QPlaceSupplier::iconTest()
{
    QPlaceSupplier testObj;
    QVERIFY(testObj.icon().isEmpty());
    QPlaceIcon icon;
    QVariantMap iconParams;
    iconParams.insert(QPlaceIcon::SingleUrl, QUrl::fromEncoded("http://example.com/icon.png"));
    icon.setParameters(iconParams);
    testObj.setIcon(icon);
    QCOMPARE(testObj.icon(), icon);
    QCOMPARE(testObj.icon().url(), QUrl::fromEncoded("http://example.com/icon.png"));

    testObj.setIcon(QPlaceIcon());
    QVERIFY(testObj.icon().isEmpty());
    QCOMPARE(testObj.icon().url(), QUrl());
}

void tst_QPlaceSupplier::operatorsTest()
{
    QPlaceSupplier testObj;
    testObj.setName(QLatin1String("Acme"));
    QPlaceIcon icon;
    QVariantMap iconParams;
    iconParams.insert(QPlaceIcon::SingleUrl, QUrl::fromEncoded("http://example.com/icon.png"));
    icon.setParameters(iconParams);
    testObj.setIcon(icon);
    testObj.setSupplierId(QLatin1String("34292"));

    QPlaceSupplier testObj2;
    testObj2 = testObj;
    QVERIFY2(testObj == testObj2, "Not copied correctly");
    testObj2.setSupplierId(QLatin1String("testValue2"));
    QVERIFY2(testObj != testObj2, "Object should be different");
}

void tst_QPlaceSupplier::isEmptyTest()
{
    QPlaceIcon icon;
    QVariantMap iconParametersMap;
    iconParametersMap.insert(QLatin1String("Para"), QLatin1String("meter"));
    icon.setParameters(iconParametersMap);
    QVERIFY(!icon.isEmpty());

    QPlaceSupplier supplier;

    QVERIFY(supplier.isEmpty());

    // name
    supplier.setName(QLatin1String("Name"));
    QVERIFY(!supplier.isEmpty());
    supplier.setName(QString());
    QVERIFY(supplier.isEmpty());

    // supplierId
    supplier.setSupplierId(QLatin1String("1"));
    QVERIFY(!supplier.isEmpty());
    supplier.setSupplierId(QString());
    QVERIFY(supplier.isEmpty());

    // url
    supplier.setUrl(QUrl(QLatin1String("www.example.com")));
    QVERIFY(!supplier.isEmpty());
    supplier.setUrl(QUrl());
    QVERIFY(supplier.isEmpty());

    // icon
    supplier.setIcon(icon);
    QVERIFY(!supplier.isEmpty());
    supplier.setIcon(QPlaceIcon());
    QVERIFY(supplier.isEmpty());
}

QTEST_APPLESS_MAIN(tst_QPlaceSupplier);

#include "tst_qplacesupplier.moc"
