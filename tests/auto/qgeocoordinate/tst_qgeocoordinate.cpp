#include <QtCore/QString>
#include <QtTest/QtTest>
#include <Qt>

#include <qgeocoordinate.h>

QTM_USE_NAMESPACE

class tst_QGeoCoordinate : public QObject
{
    Q_OBJECT

public:
    tst_QGeoCoordinate();

private Q_SLOTS:
    void constructorTest();
    void latitudeTest();
    void longitudeTest();
    void isValidTest();
    void operatorsTest();
};

tst_QGeoCoordinate::tst_QGeoCoordinate()
{
}

void tst_QGeoCoordinate::constructorTest()
{
    QGeoCoordinate testObj;
    Q_UNUSED(testObj);

    QGeoCoordinate *testObjPtr = new QGeoCoordinate(testObj);
    QVERIFY2(testObjPtr != NULL, "Copy constructor - null");
    QVERIFY2(qIsNaN(testObjPtr->latitude()), "Copy constructor - latitude");
    QVERIFY2(qIsNaN(testObjPtr->longitude()), "Copy constructor - longitude");
    delete testObjPtr;
}

void tst_QGeoCoordinate::latitudeTest()
{
    QGeoCoordinate testObj;
    QVERIFY2(qIsNaN(testObj.latitude()), "Wrong default value");
    testObj.setLatitude(20);
    QVERIFY2(testObj.latitude() == 20, "Wrong value returned");
}

void tst_QGeoCoordinate::longitudeTest()
{
    QGeoCoordinate testObj;
    QVERIFY2(qIsNaN(testObj.longitude()), "Wrong default value");
    testObj.setLongitude(20);
    QVERIFY2(testObj.longitude() == 20, "Wrong value returned");
}

void tst_QGeoCoordinate::isValidTest()
{
    QGeoCoordinate testObj;
    QVERIFY2(testObj.isValid() == false, "Wrong default check");
    testObj.setLongitude(20);
    QVERIFY2(testObj.isValid() == false, "Wrong latitude check");
    testObj.setLongitude(360);
    testObj.setLatitude(20);
    QVERIFY2(testObj.isValid() == false, "Wrong longitude check");
    testObj.setLongitude(20);
    QVERIFY2(testObj.isValid() == true, "Place is valid");
}

void tst_QGeoCoordinate::operatorsTest()
{
    QGeoCoordinate testObj;
    testObj.setLatitude(20);
    testObj.setLongitude(20);
    QGeoCoordinate testObj2;
    testObj2 = testObj;
    QVERIFY2(testObj == testObj2, "Not copied correctly");
    testObj2.setLatitude(30);
    QVERIFY2(testObj != testObj2, "Object should be different");
}

QTEST_APPLESS_MAIN(tst_QGeoCoordinate);

#include "tst_qgeocoordinate.moc"
