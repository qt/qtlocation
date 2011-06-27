#include <QtCore/QString>
#include <QtTest/QtTest>

#include <qplacelocation.h>

QTM_USE_NAMESPACE

class tst_QPlaceLocation : public QObject
{
    Q_OBJECT

public:
    tst_QPlaceLocation();

private Q_SLOTS:
    void constructorTest();
    void additionalDataTest();
    void alternativeLabelsTest();
    void addressTest();
    void displayPositionTest();
    void navigationPositionsTest();
    void labelTest();
    void locationIdTest();
    void locationScoreTest();
    void mapViewTest();
    void operatorsTest();
};

tst_QPlaceLocation::tst_QPlaceLocation()
{
}

void tst_QPlaceLocation::constructorTest()
{
    QPlaceLocation testObj;
    testObj.setLabel("testId");
    QPlaceLocation *testObjPtr = new QPlaceLocation(testObj);
    QVERIFY2(testObjPtr != NULL, "Copy constructor - null");
    QVERIFY2(*testObjPtr == testObj, "Copy constructor - compare");
    delete testObjPtr;
}

void tst_QPlaceLocation::additionalDataTest()
{
    QPlaceLocation testObj;
    QVERIFY2(testObj.additionalData().count() == 0, "Wrong default value");
    QVariantHash list;
    list.insert("key1", "value1");
    list.insert("key2", "value2");
    testObj.setAdditionalData(list);
    QVERIFY2(testObj.additionalData().count() == 2, "Wrong value returned");
    QVERIFY2(testObj.additionalData()["key1"] == "value1", "Wrong value[1] returned");
}

void tst_QPlaceLocation::alternativeLabelsTest()
{
    QPlaceLocation testObj;
    QVERIFY2(testObj.alternativeLabels().count() == 0, "Wrong default value");
    QList<QPlaceAlternativeValue> list;
    QPlaceAlternativeValue altValue;
    list.append(altValue);
    altValue.setKey("testData");
    list.append(altValue);
    testObj.setAlternativeLabels(list);
    QVERIFY2(testObj.alternativeLabels().count() == 2, "Wrong value returned");
    QVERIFY2(testObj.alternativeLabels()[0].key() == QString(), "Wrong key[0] returned");
    QVERIFY2(testObj.alternativeLabels()[1].key() == "testData", "Wrong key[1] returned");
}

void tst_QPlaceLocation::addressTest()
{
    QPlaceLocation testObj;
    QVERIFY2(testObj.address() == QGeoAddress(), "Wrong default value");
    QGeoAddress address;
    address.setCountry("testValue");
    testObj.setAddress(address);
    QVERIFY2(testObj.address() == address, "Wrong value returned");
}

void tst_QPlaceLocation::displayPositionTest()
{
    QPlaceLocation testObj;
    QVERIFY2(testObj.displayPosition() == QGeoCoordinate(), "Wrong default value");
    QGeoCoordinate address;
    address.setLatitude(10);
    testObj.setDisplayPosition(address);
    QVERIFY2(testObj.displayPosition() == address, "Wrong value returned");
}

void tst_QPlaceLocation::navigationPositionsTest()
{
    QPlaceLocation testObj;
    QVERIFY2(testObj.navigationPositions().count() == 0, "Wrong default value");
    QGeoCoordinate coordinate1, coordinate2;
    coordinate1.setLatitude(10);
    coordinate1.setLongitude(10);
    coordinate2.setLatitude(2);
    coordinate2.setLongitude(2);
    QList<QGeoCoordinate> path;
    path.append(coordinate1);
    path.append(coordinate2);
    testObj.setNavigationPositions(path);
    QVERIFY2(testObj.navigationPositions() == path, "Wrong value returned");
    QVERIFY2(testObj.navigationPositions().count() == 2, "Wrong value returned");
}

void tst_QPlaceLocation::labelTest()
{
    QPlaceLocation testObj;
    QVERIFY2(testObj.label() == QString(), "Wrong default value");
    testObj.setLabel("testText");
    QVERIFY2(testObj.label() == "testText", "Wrong value returned");
}

void tst_QPlaceLocation::locationIdTest()
{
    QPlaceLocation testObj;
    QVERIFY2(testObj.locationId() == QString(), "Wrong default value");
    testObj.setLocationId("testText");
    QVERIFY2(testObj.locationId() == "testText", "Wrong value returned");
}

void tst_QPlaceLocation::locationScoreTest()
{
    QPlaceLocation testObj;
    QVERIFY2(testObj.locationScore() == 0, "Wrong default value");
    testObj.setLocationScore(5);
    QVERIFY2(testObj.locationScore() == 5, "Wrong value returned");
}

void tst_QPlaceLocation::mapViewTest()
{
    QPlaceLocation testObj;
    QVERIFY2(testObj.mapView().isValid() == false, "Wrong default value");
    QGeoCoordinate coordinate;
    coordinate.setLatitude(30);
    coordinate.setLongitude(20);
    QGeoBoundingBox box;
    box.setTopLeft(coordinate);
    box.setBottomRight(coordinate);
    testObj.setMapView(box);
    QVERIFY2(testObj.mapView() == box, "Wrong value returned");
}

void tst_QPlaceLocation::operatorsTest()
{
    QPlaceLocation testObj;
    testObj.setLabel("testValue");
    QPlaceLocation testObj2;
    testObj2 = testObj;
    QVERIFY2(testObj == testObj2, "Not copied correctly");
    testObj2.setLocationId("testValue2");
    QVERIFY2(testObj != testObj2, "Object should be different");
}

QTEST_APPLESS_MAIN(tst_QPlaceLocation);

#include "tst_qplacelocation.moc"
