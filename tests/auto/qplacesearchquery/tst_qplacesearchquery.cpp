#include <QtCore/QString>
#include <QtTest/QtTest>

#include <qplacesearchquery.h>

QT_USE_NAMESPACE

class tst_QPlaceSearchQuery : public QObject
{
    Q_OBJECT

public:
    tst_QPlaceSearchQuery();

private Q_SLOTS:
    void constructorTest();
    void searchTermTest();
    void categoriesTest();
    void searchCenterTest();
    void boundingBoxTest();
    void didYouMeanSuggestionNumberTest();
    void operatorsTest();
};

tst_QPlaceSearchQuery::tst_QPlaceSearchQuery()
{
}

void tst_QPlaceSearchQuery::constructorTest()
{
    QPlaceSearchQuery testObj;
    Q_UNUSED(testObj);

    QPlaceSearchQuery *testObjPtr = new QPlaceSearchQuery(testObj);
    QVERIFY2(testObjPtr != NULL, "Copy constructor - null");
    QVERIFY2(*testObjPtr == testObj, "Copy constructor - compare");
    delete testObjPtr;
}

void tst_QPlaceSearchQuery::searchTermTest()
{
    QPlaceSearchQuery testObj;
    QVERIFY2(testObj.searchTerm() == QString(), "Wrong default value");
    testObj.setSearchTerm("testText");
    QVERIFY2(testObj.searchTerm() == "testText", "Wrong value returned");
}

void tst_QPlaceSearchQuery::categoriesTest()
{
    QPlaceSearchQuery testObj;
    QVERIFY2(testObj.categories().count() == 0, "Wrong default value");
    QPlaceCategory cat;
    cat.setDescription("testDescription");
    testObj.setCategory(cat);
    QVERIFY2(testObj.categories().count() == 1, "Wrong categories count returned");
    QVERIFY2(testObj.categories()[0] == cat, "Wrong category returned");
}

void tst_QPlaceSearchQuery::searchCenterTest()
{
    QPlaceSearchQuery testObj;
    QVERIFY2(testObj.searchCenter().isValid() == false, "Wrong default value");
    QGeoCoordinate coordinate;
    coordinate.setLatitude(30);
    coordinate.setLongitude(20);
    testObj.setSearchCenter(coordinate);
    QVERIFY2(testObj.searchCenter() == coordinate, "Wrong value returned");
    testObj.clear();
    QVERIFY2(testObj.searchCenter().isValid() == false, "Wrong cleared value returned");
}

void tst_QPlaceSearchQuery::boundingBoxTest()
{
    QPlaceSearchQuery testObj;
    QVERIFY2(testObj.boundingBox().isValid() == false, "Wrong default value");
    QGeoCoordinate coordinate;
    coordinate.setLatitude(30);
    coordinate.setLongitude(20);
    QGeoBoundingBox box;
    box.setTopLeft(coordinate);
    box.setBottomRight(coordinate);
    testObj.setBoundingBox(box);
    QVERIFY2(testObj.boundingBox() == box, "Wrong value returned");
    testObj.clear();
    QVERIFY2(testObj.boundingBox().isValid() == false, "Wrong cleared value returned");
}

void tst_QPlaceSearchQuery::didYouMeanSuggestionNumberTest()
{
    QPlaceSearchQuery testObj;
    QVERIFY2(testObj.didYouMeanSuggestionNumber() == 0, "Wrong default value");
    testObj.setDidYouMeanSuggestionNumber(10);
    QVERIFY2(testObj.didYouMeanSuggestionNumber() == 10, "Wrong value returned");
    testObj.clear();
    QVERIFY2(testObj.didYouMeanSuggestionNumber() == 0, "Wrong cleared value returned");
}

void tst_QPlaceSearchQuery::operatorsTest()
{
    QPlaceSearchQuery testObj;
    testObj.setSearchTerm("testValue");
    QPlaceSearchQuery testObj2;
    testObj2 = testObj;
    QVERIFY2(testObj == testObj2, "Not copied correctly");
    testObj2.setDidYouMeanSuggestionNumber(-5);
    QVERIFY2(testObj != testObj2, "Object should be different");
}

QTEST_APPLESS_MAIN(tst_QPlaceSearchQuery);

#include "tst_qplacesearchquery.moc"
