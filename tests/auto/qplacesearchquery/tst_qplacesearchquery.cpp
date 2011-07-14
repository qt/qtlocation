#include <QtCore/QString>
#include <QtTest/QtTest>

#include <qplacesearchquery.h>
#include <qgeoboundingcircle.h>
#include <qgeoboundingbox.h>

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
    void boundingCircleTest();
    void boundingBoxTest();
    void searchAreaTest();
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

void tst_QPlaceSearchQuery::boundingCircleTest()
{
    QPlaceSearchQuery query;
    QVERIFY2(query.searchArea() == NULL, "Wrong default value");
    QGeoBoundingCircle *circle = new QGeoBoundingCircle;
    circle->setCenter(QGeoCoordinate(30,20));
    circle->setRadius(500.0);
    query.setSearchArea(circle);

    QVERIFY(query.searchArea() != NULL);
    QVERIFY(query.searchArea()->type() == QGeoBoundingArea::CircleType);
    QVERIFY(query.searchArea() == circle);

    QGeoBoundingCircle *retrievedCircle =  static_cast<QGeoBoundingCircle *>(query.searchArea());
    QVERIFY2(retrievedCircle->center() == QGeoCoordinate(30,20), "Wrong value returned");
    QVERIFY2(retrievedCircle->radius() == 500.0, "Wrong value returned");
    query.clear();
    QVERIFY2(query.searchArea() == NULL, "Search area not cleared");
}

void tst_QPlaceSearchQuery::boundingBoxTest()
{
    QPlaceSearchQuery query;
    QVERIFY2(query.searchArea() == NULL, "Wrong default value");
    QGeoBoundingBox *box = new QGeoBoundingBox;

    box->setTopLeft(QGeoCoordinate(30,20));
    box->setBottomRight(QGeoCoordinate(10,50));
    query.setSearchArea(box);

    QVERIFY(query.searchArea() != NULL);
    QVERIFY(query.searchArea()->type() == QGeoBoundingArea::BoxType);
    QVERIFY(query.searchArea() == box);

    QGeoBoundingBox *retrievedBox = static_cast<QGeoBoundingBox*>(query.searchArea());
    QVERIFY2(retrievedBox->topLeft() == QGeoCoordinate(30,20), "Wrong value returned");
    QVERIFY2(retrievedBox->bottomRight() == QGeoCoordinate(10,50), "Wrong value returned");

    query.clear();
    QVERIFY2(query.searchArea() == NULL, "Wrong cleared value returned");
}

void tst_QPlaceSearchQuery::searchAreaTest()
{
    //test assignment of new search area over an old search area
    QPlaceSearchQuery *query = new QPlaceSearchQuery;
    QGeoBoundingCircle *circle = new QGeoBoundingCircle;
    circle->setCenter(QGeoCoordinate(30,20));
    circle->setRadius(500.0);
    query->setSearchArea(circle);

    QVERIFY(query->searchArea() == circle);
    QGeoBoundingBox *box = new QGeoBoundingBox;
    box->setTopLeft(QGeoCoordinate(30,20));
    box->setBottomRight(QGeoCoordinate(10,50));
    query->setSearchArea(box);
    QVERIFY2(query->searchArea() == box, "New search area not assigned");
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
