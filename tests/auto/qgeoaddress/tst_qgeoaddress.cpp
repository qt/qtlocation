#include <QtCore/QString>
#include <QtTest/QtTest>

#include <qgeoaddress.h>

QTM_USE_NAMESPACE

class tst_QGeoAddress : public QObject
{
    Q_OBJECT

public:
    tst_QGeoAddress();

private Q_SLOTS:
    void constructorTest();
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
    void postcodeTest();
    void stateTest();
    void streetTest();
//    void suiteTest();
    void operatorsTest();
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

//void tst_QGeoAddress::additionalDataTest()
//{
//    QGeoAddress testObj;
//    QVERIFY2(testObj.additionalData().count() == 0, "Wrong default value");
//    QVariantHash list;
//    list.insert("key1", "value1");
//    list.insert("key2", "value2");
//    testObj.setAdditionalData(list);
//    QVERIFY2(testObj.additionalData().count() == 2, "Wrong value returned");
//    QVERIFY2(testObj.additionalData()["key1"] == "value1", "Wrong value[1] returned");
//}

//void tst_QGeoAddress::alternativeAttributesTest()
//{
//    QGeoAddress testObj;
//    QVERIFY2(testObj.alternativeAttributes().count() == 0, "Wrong default value");
//    QList<QPlaceAlternativeValue> list;
//    QPlaceAlternativeValue altValue;
//    list.append(altValue);
//    altValue.setKey("testData");
//    list.append(altValue);
//    testObj.setAlternativeAttributes(list);
//    QVERIFY2(testObj.alternativeAttributes().count() == 2, "Wrong value returned");
//    QVERIFY2(testObj.alternativeAttributes()[0].key() == QString(), "Wrong key[0] returned");
//    QVERIFY2(testObj.alternativeAttributes()[1].key() == "testData", "Wrong key[1] returned");
//}

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

void tst_QGeoAddress::postcodeTest()
{
    QGeoAddress testObj;
    QVERIFY2(testObj.postcode() == QString(), "Wrong default value");
    testObj.setPostcode("testText");
    QVERIFY2(testObj.postcode() == "testText", "Wrong value returned");
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

QTEST_APPLESS_MAIN(tst_QGeoAddress);

#include "tst_qgeoaddress.moc"
