/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/QString>
#include <QtTest/QtTest>

#include <qgeoaddress.h>

QT_USE_NAMESPACE

class tst_QGeoAddress : public QObject
{
    Q_OBJECT

public:
    tst_QGeoAddress();

private Q_SLOTS:
    void constructorTest();
    void formattedAddress();
//TODO: there are various field we don't have yet in QGeoAddress
//       will need to either remove or enable these tests
//    void additionalDataTest();
//    void alternativeAttributesTest();
    void cityTest();
    void countryCodeTest();
    void countryTest();
    void countyTest();
    void districtTest();
//    void floorTest();
//    void houseNumberTest();
//    void labelTest();
    void postalCodeTest();
    void stateTest();
    void streetTest();
//    void suiteTest();
    void operatorsTest();
    void emptyClearTest();
};

tst_QGeoAddress::tst_QGeoAddress()
{
}

void tst_QGeoAddress::constructorTest()
{
    QGeoAddress testObj;

    testObj.setStreet("testId");
    QGeoAddress *testObjPtr = new QGeoAddress(testObj);
    QVERIFY2(testObjPtr != NULL, "Copy constructor - null");
    QVERIFY2(*testObjPtr == testObj, "Copy constructor - compare");
    delete testObjPtr;
}

void tst_QGeoAddress::formattedAddress()
{
    QGeoAddress address;
    QVERIFY(address.formattedAddress().isEmpty());
    address.setFormattedAddress(QLatin1String("123 Fake Street\nSpringfield"));
    QCOMPARE(address.formattedAddress(), QLatin1String("123 Fake Street\nSpringfield"));
}

void tst_QGeoAddress::cityTest()
{
    QGeoAddress testObj;
    QVERIFY2(testObj.city() == QString(), "Wrong default value");
    testObj.setCity("testText");
    QVERIFY2(testObj.city() == "testText", "Wrong value returned");
}

void tst_QGeoAddress::countryCodeTest()
{
    QGeoAddress testObj;
    QVERIFY2(testObj.countryCode() == QString(), "Wrong default value");
    testObj.setCountryCode("testText");
    QVERIFY2(testObj.countryCode() == "testText", "Wrong value returned");
}

void tst_QGeoAddress::countryTest()
{
    QGeoAddress testObj;
    QVERIFY2(testObj.country() == QString(), "Wrong default value");
    testObj.setCountry("testText");
    QVERIFY2(testObj.country() == "testText", "Wrong value returned");
}

void tst_QGeoAddress::countyTest()
{
    QGeoAddress testObj;
    QVERIFY2(testObj.county() == QString(), "Wrong default value");
    testObj.setCounty("testText");
    QVERIFY2(testObj.county() == "testText", "Wrong value returned");
}

void tst_QGeoAddress::districtTest()
{
    QGeoAddress testObj;
    QVERIFY2(testObj.district() == QString(), "Wrong default value");
    testObj.setDistrict("testText");
    QVERIFY2(testObj.district() == "testText", "Wrong value returned");
}

//  TODO: currently don't have floor in QGeoAddress
//void tst_QGeoAddress::floorTest()
//{
//    QGeoAddress testObj;
//    QVERIFY2(testObj.floor() == QString(), "Wrong default value");
//    testObj.setFloor("testText");
//    QVERIFY2(testObj.floor() == "testText", "Wrong value returned");
//}

//TODO: Atm not sure if we will have house number in API.
//void tst_QGeoAddress::houseNumberTest()
//{
//    QGeoAddress testObj;
//    QVERIFY2(testObj.houseNumber() == QString(), "Wrong default value");
//    testObj.setHouseNumber("testText");
//    QVERIFY2(testObj.houseNumber() == "testText", "Wrong value returned");
//}

//void tst_QGeoAddress::labelTest()
//{
//    QGeoAddress testObj;
//    QVERIFY2(testObj.label() == QString(), "Wrong default value");
//    testObj.setLabel("testText");
//    QVERIFY2(testObj.label() == "testText", "Wrong value returned");
//}

void tst_QGeoAddress::postalCodeTest()
{
    QGeoAddress testObj;
    QVERIFY2(testObj.postalCode() == QString(), "Wrong default value");
    testObj.setPostalCode("testText");
    QVERIFY2(testObj.postalCode() == "testText", "Wrong value returned");
}

void tst_QGeoAddress::stateTest()
{
    QGeoAddress testObj;
    QVERIFY2(testObj.state() == QString(), "Wrong default value");
    testObj.setState("testText");
    QVERIFY2(testObj.state() == "testText", "Wrong value returned");
}

void tst_QGeoAddress::streetTest()
{
    QGeoAddress testObj;
    QVERIFY2(testObj.street() == QString(), "Wrong default value");
    testObj.setStreet("testText");
    QVERIFY2(testObj.street() == "testText", "Wrong value returned");
}

// TODO: curenlty we don't have suite in QGeoAddress
//       will need to either remove or enable
//void tst_QGeoAddress::suiteTest()
//{
//    QGeoAddress testObj;
//    QVERIFY2(testObj.suite() == QString(), "Wrong default value");
//    testObj.setSuite("testText");
//    QVERIFY2(testObj.suite() == "testText", "Wrong value returned");
//}

void tst_QGeoAddress::operatorsTest()
{
    QGeoAddress testObj;
    testObj.setStreet("testValue");
    QGeoAddress testObj2;
    testObj2 = testObj;
    QVERIFY2(testObj == testObj2, "Not copied correctly");
    testObj2.setCountry("testValue2");
    QVERIFY2(testObj != testObj2, "Object should be different");
}

void tst_QGeoAddress::emptyClearTest()
{
    QGeoAddress testObj;
    QVERIFY(testObj.isEmpty());

    testObj.setCountry(QLatin1String("country"));
    QVERIFY(!testObj.isEmpty());
    testObj.clear();

    testObj.setCountryCode(QLatin1String("countryCode"));
    QVERIFY(!testObj.isEmpty());
    testObj.clear();

    testObj.setState(QLatin1String("state"));
    QVERIFY(!testObj.isEmpty());
    testObj.clear();

    testObj.setCounty(QLatin1String("county"));
    QVERIFY(!testObj.isEmpty());
    testObj.clear();

    testObj.setCity(QLatin1String("city"));
    QVERIFY(!testObj.isEmpty());
    testObj.clear();

    testObj.setDistrict(QLatin1String("district"));
    QVERIFY(!testObj.isEmpty());
    testObj.clear();

    testObj.setPostalCode(QLatin1String("postalCode"));
    QVERIFY(!testObj.isEmpty());
    testObj.clear();

    testObj.setStreet(QLatin1String("street"));
    QVERIFY(!testObj.isEmpty());
    testObj.clear();

    testObj.setFormattedAddress(QLatin1String("formatted address"));
    QVERIFY(!testObj.isEmpty());
    testObj.clear();

    QVERIFY(testObj.isEmpty());
}

QTEST_APPLESS_MAIN(tst_QGeoAddress)

#include "tst_qgeoaddress.moc"
