#include <QtCore/QString>
#include <QtTest/QtTest>

#include <qgeoplace.h>

QT_USE_NAMESPACE

class tst_QGeoPlace : public QObject
{
    Q_OBJECT

public:
    tst_QGeoPlace();

private Q_SLOTS:
    void constructorTest();
    void additionalDataTest();
    void alternativeNamesTest();
    void businessInformationTest();
    void categoriesTest();
    void contactsTest();
    void descriptionsTest();
    void detailsFetchedTest();
    void locationTest();
    void alternativeLocationsTest();
    void ratingTest();
    void suppliersTest();
    void feedsTest();
    void mediaTest();
    void mediaCountTest();
    void nameTest();
    void placeIdTest();
    void reviewsTest();
    void reviewCountTest();
    void shortDescriptionTest();
    void tagsTest();
    void operatorsTest();
};

tst_QGeoPlace::tst_QGeoPlace()
{
}

void tst_QGeoPlace::constructorTest()
{
    QGeoPlace testObj;
    testObj.setPlaceId("testId");
    QPlaceBusinessInformation obj;
    obj.setOpeningNote("10");
    testObj.setBusinessInformation(obj);
    QGeoLocation sup;
    sup.setLabel("testName1");
    sup.setLocationId("testId");
    QList<QGeoLocation> list;
    list.append(sup);
    sup.setLocationId("testName2");
    list.append(sup);
    testObj.setAlternativeLocations(list);

    QGeoPlace *testObjPtr = new QGeoPlace(testObj);

    QVERIFY2(testObjPtr != NULL, "Copy constructor - null");
    QVERIFY2(*testObjPtr == testObj, "Copy constructor - compare");

    delete testObjPtr;
}

void tst_QGeoPlace::additionalDataTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.additionalData().count() == 0, "Wrong default value");
    QVariantHash list;
    list.insert("key1", "value1");
    list.insert("key2", "value2");
    testObj.setAdditionalData(list);
    QVERIFY2(testObj.additionalData().count() == 2, "Wrong value returned");
    QVERIFY2(testObj.additionalData()["key1"] == "value1", "Wrong value[1] returned");
}

void tst_QGeoPlace::alternativeNamesTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.alternativeNames().count() == 0, "Wrong default value");
    QList<QPlaceAlternativeValue> list;
    QPlaceAlternativeValue altValue;
    list.append(altValue);
    altValue.setKey("testData");
    list.append(altValue);
    testObj.setAlternativeNames(list);
    QVERIFY2(testObj.alternativeNames().count() == 2, "Wrong value returned");
    QVERIFY2(testObj.alternativeNames()[0].key() == QString(), "Wrong key[0] returned");
    QVERIFY2(testObj.alternativeNames()[1].key() == "testData", "Wrong key[1] returned");
}

void tst_QGeoPlace::nameTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.name() == QString(), "Wrong default value");
    testObj.setName("testText");
    QVERIFY2(testObj.name() == "testText", "Wrong value returned");
}

void tst_QGeoPlace::placeIdTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.placeId() == QString(), "Wrong default value");
    testObj.setPlaceId("testText");
    QVERIFY2(testObj.placeId() == "testText", "Wrong value returned");
}

void tst_QGeoPlace::shortDescriptionTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.shortDescription() == QString(), "Wrong default value");
    testObj.setShortDescription("testText");
    QVERIFY2(testObj.shortDescription() == "testText", "Wrong value returned");
}

void tst_QGeoPlace::mediaCountTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.mediaCount() == 0, "Wrong default value");
    testObj.setMediaCount(10);
    QVERIFY2(testObj.mediaCount() == 10, "Wrong value returned");
}

void tst_QGeoPlace::reviewCountTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.reviewCount() == 0, "Wrong default value");
    testObj.setReviewCount(10);
    QVERIFY2(testObj.reviewCount() == 10, "Wrong value returned");
}

void tst_QGeoPlace::ratingTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.rating() == QPlaceRating(), "Wrong default value");
    QPlaceRating obj;
    obj.setCount(10);
    testObj.setRating(obj);
    QVERIFY2(testObj.rating() == obj, "Wrong value returned");
}

void tst_QGeoPlace::locationTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.location() == QGeoLocation(), "Wrong default value");
    QGeoLocation obj;
    obj.setCoordinate(QGeoCoordinate(10,20));
    testObj.setLocation(obj);
    QVERIFY2(testObj.location() == obj, "Wrong value returned");
}

void tst_QGeoPlace::businessInformationTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.businessInformation() == QPlaceBusinessInformation(), "Wrong default value");
    QPlaceBusinessInformation obj;
    obj.setOpeningNote("10");
    testObj.setBusinessInformation(obj);
    QVERIFY2(testObj.businessInformation() == obj, "Wrong value returned");
}

void tst_QGeoPlace::descriptionsTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.descriptions().count() == 0, "Wrong default value");
    QPlaceDescription sup;
    sup.setContent("testId");
    QList<QPlaceDescription> list;
    list.append(sup);
    sup.setContent("testName2");
    list.append(sup);
    testObj.setDescriptions(list);
    QVERIFY2(testObj.descriptions().count() == 2, "Wrong value returned");
}

void tst_QGeoPlace::detailsFetchedTest()
{
    QGeoPlace testPlace;
    QVERIFY2(testPlace.detailsFetched() == false, "Wrong default value");
    testPlace.setDetailsFetched(true);
    QVERIFY2(testPlace.detailsFetched() == true, "Wrong value returned");
    testPlace.setDetailsFetched(false);
    QVERIFY2(testPlace.detailsFetched() == false, "Wrong value returned");
}

void tst_QGeoPlace::mediaTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.media().items() == 0, "Wrong default value");
    QPlaceMediaObject sup;
    sup.setThumbnailUrl("testId");
    QList<QPlaceMediaObject> list;
    list.append(sup);
    sup.setUrl("testName2");
    list.append(sup);
    QPlacePaginationList<QPlaceMediaObject> mediaList;
    mediaList.setStart(20);
    mediaList.setData(list);
    testObj.setMedia(mediaList);
    QVERIFY2(testObj.media().items() == 2, "Wrong value returned");
    QVERIFY2(testObj.media().data()[1].url() == "testName2", "Wrong value returned");
    QVERIFY2(testObj.media().stop() == 21, "Wrong value returned");
}

void tst_QGeoPlace::reviewsTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.reviews().items() == 0, "Wrong default value");
    QPlaceReview sup;
    sup.setDescription("testId");
    QList<QPlaceReview> list;
    list.append(sup);
    sup.setLanguage("testName2");
    list.append(sup);
    QPlacePaginationList<QPlaceReview> reviewsList;
    reviewsList.setData(list);
    testObj.setReviews(reviewsList);
    QVERIFY2(testObj.reviews().items() == 2, "Wrong value returned");
    QVERIFY2(testObj.reviews().data()[1].language() == "testName2", "Wrong value returned");
    QVERIFY2(testObj.reviews().start() == 0, "Wrong value returned");
    QVERIFY2(testObj.reviews().stop() == 1, "Wrong value returned");
}

void tst_QGeoPlace::contactsTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.contacts().count() == 0, "Wrong default value");
    QPlaceContact sup;
    sup.setDescription("testId");
    QList<QPlaceContact> list;
    list.append(sup);
    sup.setDescription("testName2");
    list.append(sup);
    testObj.setContacts(list);
    QVERIFY2(testObj.contacts().count() == 2, "Wrong value returned");
}

void tst_QGeoPlace::categoriesTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.categories().count() == 0, "Wrong default value");
    QPlaceCategory sup;
    sup.setCategoryId("testId");
    QList<QPlaceCategory> list;
    list.append(sup);
    sup.setCategoryId("testName2");
    list.append(sup);
    testObj.setCategories(list);
    QVERIFY2(testObj.categories().count() == 2, "Wrong value returned");
}

void tst_QGeoPlace::alternativeLocationsTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.alternativeLocations().count() == 0, "Wrong default value");
    QGeoLocation sup;
    sup.setLabel("testName1");
    sup.setLocationId("testId");
    QList<QGeoLocation> list;
    list.append(sup);
    sup.setLocationId("testName2");
    list.append(sup);
    testObj.setAlternativeLocations(list);
    QVERIFY2(testObj.alternativeLocations().count() == 2, "Wrong value returned");
}

void tst_QGeoPlace::suppliersTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.suppliers().count() == 0, "Wrong default value");
    QPlaceSupplier sup;
    sup.setName("testName1");
    sup.setSupplierId("testId");
    QList<QPlaceSupplier> list;
    list.append(sup);
    sup.setName("testName2");
    list.append(sup);
    testObj.setSuppliers(list);
    QVERIFY2(testObj.suppliers().count() == 2, "Wrong value returned");
}

void tst_QGeoPlace::feedsTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.feeds().count() == 0, "Wrong default value");
    QStringList list;
    list << "1" << "2" << "3";
    testObj.setFeeds(list);
    QVERIFY2(testObj.feeds().count() == 3, "Wrong value returned");
    QVERIFY2(testObj.feeds()[1] == "2", "Wrong [1] value returned");
}

void tst_QGeoPlace::tagsTest()
{
    QGeoPlace testObj;
    QVERIFY2(testObj.tags().count() == 0, "Wrong default value");
    QStringList list;
    list << "1" << "2" << "3";
    testObj.setTags(list);
    QVERIFY2(testObj.tags().count() == 3, "Wrong value returned");
    QVERIFY2(testObj.tags()[1] == "2", "Wrong [1] value returned");
}

void tst_QGeoPlace::operatorsTest()
{
    QGeoPlace testObj;
    testObj.setPlaceId("testId");
    QPlaceBusinessInformation obj;
    obj.setOpeningNote("10");
    testObj.setBusinessInformation(obj);
    QGeoLocation sup;
    sup.setLabel("testName1");
    sup.setLocationId("testId");
    QList<QGeoLocation> list;
    list.append(sup);
    sup.setLocationId("testName2");
    list.append(sup);
    testObj.setAlternativeLocations(list);
    QStringList tags;
    tags << "1" << "2" << "3";
    testObj.setTags(tags);
    QGeoLocation loc;
    loc.setLocationScore(10);
    testObj.setLocation(loc);

    QGeoPlace testObj2;
    testObj2 = testObj;
    QVERIFY2(testObj == testObj2, "Not copied correctly");
    testObj2.setPlaceId("342-456");
    QVERIFY2(testObj != testObj2, "Object should be different");
}

QTEST_APPLESS_MAIN(tst_QGeoPlace);

#include "tst_qgeoplace.moc"
