#include <QtCore/QString>
#include <QtTest/QtTest>

#include <qplace.h>
#include <qplaceimage.h>
#include <qplaceattribute.h>

QT_USE_NAMESPACE

class tst_Place : public QObject
{
    Q_OBJECT

public:
    tst_Place();

private Q_SLOTS:
    void constructorTest();
    void categoriesTest();
    void descriptionsTest();
    void detailsFetchedTest();
    void locationTest();
    void ratingTest();
    void suppliersTest();
    void feedsTest();
    void imageContentTest();
    void reviewContentTest();
    void contentCountTest();
    void nameTest();
    void placeIdTest();
    void tagsTest();
    void primaryPhoneTest();
    void primaryFaxTest();
    void primaryEmailTest();
    void primaryUrlTest();
    void operatorsTest();
    void extendedAttributeTest();
};

tst_Place::tst_Place()
{
}

void tst_Place::constructorTest()
{
    QPlace testObj;
    testObj.setPlaceId("testId");
    QPlaceAttribute paymentMethods;
    paymentMethods.setLabel("Payment methods");
    paymentMethods.setText("Visa");
    testObj.insertExtendedAttribute(QPlaceAttribute::PaymentMethods, paymentMethods);
    QGeoLocation loc;
    loc.setCoordinate(QGeoCoordinate(10,20));
    testObj.setLocation(loc);
    QPlace *testObjPtr = new QPlace(testObj);

    QVERIFY2(testObjPtr != NULL, "Copy constructor - null");
    QVERIFY2(*testObjPtr == testObj, "Copy constructor - compare");

    delete testObjPtr;
}

void tst_Place::nameTest()
{
    QPlace testObj;
    QVERIFY2(testObj.name() == QString(), "Wrong default value");
    testObj.setName("testText");
    QVERIFY2(testObj.name() == "testText", "Wrong value returned");
}

void tst_Place::placeIdTest()
{
    QPlace testObj;
    QVERIFY2(testObj.placeId() == QString(), "Wrong default value");
    testObj.setPlaceId("testText");
    QVERIFY2(testObj.placeId() == "testText", "Wrong value returned");
}

void tst_Place::contentCountTest()
{
    QPlace testObj;
    QVERIFY2(testObj.contentCount(QPlaceContent::ImageType) == 0, "Wrong default value");
    testObj.setContentCount(QPlaceContent::ImageType, 50);
    QVERIFY2(testObj.contentCount(QPlaceContent::ImageType) == 50, "Wrong value returned");

    testObj.setContentCount(QPlaceContent::ImageType,0);
    QVERIFY2(testObj.contentCount(QPlaceContent::ImageType) == 0, "Wrong value returned");
}

void tst_Place::ratingTest()
{
    QPlace testObj;
    QVERIFY2(testObj.rating() == QPlaceRating(), "Wrong default value");
    QPlaceRating obj;
    obj.setCount(10);
    testObj.setRating(obj);
    QVERIFY2(testObj.rating() == obj, "Wrong value returned");
}

void tst_Place::locationTest()
{
    QPlace testObj;
    QVERIFY2(testObj.location() == QGeoLocation(), "Wrong default value");
    QGeoLocation obj;
    obj.setCoordinate(QGeoCoordinate(10,20));
    testObj.setLocation(obj);
    QVERIFY2(testObj.location() == obj, "Wrong value returned");
}

void tst_Place::descriptionsTest()
{
    QPlace testObj;
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

void tst_Place::detailsFetchedTest()
{
    QPlace testPlace;
    QVERIFY2(testPlace.detailsFetched() == false, "Wrong default value");
    testPlace.setDetailsFetched(true);
    QVERIFY2(testPlace.detailsFetched() == true, "Wrong value returned");
    testPlace.setDetailsFetched(false);
    QVERIFY2(testPlace.detailsFetched() == false, "Wrong value returned");
}

void tst_Place::imageContentTest()
{
    QUrl thumbnailUrl("testId");

    QPlace place;
    QVERIFY2(place.content(QPlaceContent::ImageType).count() ==0,"Wrong default value");

    QPlaceImage dummyImage;
    dummyImage.setUrl(QUrl("www.dummy.one"));

    QPlaceImage dummyImage2;
    dummyImage2.setUrl(QUrl("www.dummy.two"));

    QPlaceImage dummyImage3;
    dummyImage3.setUrl(QUrl("www.dummy.three"));

    QPlaceContent::Collection imageCollection;
    imageCollection.insert(0,dummyImage);
    imageCollection.insert(1, dummyImage2);
    imageCollection.insert(2, dummyImage3);

    place.setContent(QPlaceContent::ImageType, imageCollection);
    QPlaceContent::Collection retrievedCollection = place.content(QPlaceContent::ImageType);

    QCOMPARE(retrievedCollection.count(), 3);
    QCOMPARE(QPlaceImage(retrievedCollection.value(0)), dummyImage);
    QCOMPARE(QPlaceImage(retrievedCollection.value(1)), dummyImage2);
    QCOMPARE(QPlaceImage(retrievedCollection.value(2)), dummyImage3);

    //replace the second and insert a sixth image
    //indexes 4 and 5 are "missing"
    QPlaceImage dummyImage2New;
    dummyImage2.setUrl(QUrl("www.dummy.two.new"));

    QPlaceImage dummyImage6;
    dummyImage6.setUrl(QUrl("www.dummy.six"));

    imageCollection.clear();
    imageCollection.insert(1, dummyImage2New);
    imageCollection.insert(5, dummyImage6);
    place.addContent(QPlaceContent::ImageType, imageCollection);

    retrievedCollection = place.content(QPlaceContent::ImageType);
    QCOMPARE(retrievedCollection.count(), 4);
    QCOMPARE(QPlaceImage(retrievedCollection.value(0)), dummyImage);
    QCOMPARE(QPlaceImage(retrievedCollection.value(1)), dummyImage2New);
    QCOMPARE(QPlaceImage(retrievedCollection.value(2)), dummyImage3);
    QCOMPARE(QPlaceImage(retrievedCollection.value(3)), QPlaceImage());
    QCOMPARE(QPlaceImage(retrievedCollection.value(4)), QPlaceImage());
    QCOMPARE(QPlaceImage(retrievedCollection.value(5)), dummyImage6);
}

void tst_Place::reviewContentTest()
{
    QPlace place;
    QVERIFY2(place.content(QPlaceContent::ReviewType).count() ==0,"Wrong default value");

    QPlaceReview dummyReview;
    dummyReview.setTitle(QLatin1String("Review 1"));

    QPlaceReview dummyReview2;
    dummyReview2.setTitle(QLatin1String("Review 2"));

    QPlaceReview dummyReview3;
    dummyReview3.setTitle(QLatin1String("Review 3"));

    QPlaceContent::Collection reviewCollection;
    reviewCollection.insert(0,dummyReview);
    reviewCollection.insert(1, dummyReview2);
    reviewCollection.insert(2, dummyReview3);

    place.setContent(QPlaceContent::ReviewType, reviewCollection);
    QPlaceContent::Collection retrievedCollection = place.content(QPlaceContent::ReviewType);

    QCOMPARE(retrievedCollection.count(), 3);
    QCOMPARE(QPlaceReview(retrievedCollection.value(0)), dummyReview);
    QCOMPARE(QPlaceReview(retrievedCollection.value(1)), dummyReview2);
    QCOMPARE(QPlaceReview(retrievedCollection.value(2)), dummyReview3);

    //replace the second and insert a sixth review
    //indexes 4 and 5 are "missing"
    QPlaceReview dummyReview2New;
    dummyReview2.setTitle(QLatin1String("Review 2 new"));

    QPlaceReview dummyReview6;
    dummyReview6.setTitle(QLatin1String("Review 6"));

    reviewCollection.clear();
    reviewCollection.insert(1, dummyReview2New);
    reviewCollection.insert(5, dummyReview6);
    place.addContent(QPlaceContent::ReviewType, reviewCollection);

    retrievedCollection = place.content(QPlaceContent::ReviewType);
    QCOMPARE(retrievedCollection.count(), 4);
    QCOMPARE(QPlaceReview(retrievedCollection.value(0)), dummyReview);
    QCOMPARE(QPlaceReview(retrievedCollection.value(1)), dummyReview2New);
    QCOMPARE(QPlaceReview(retrievedCollection.value(2)), dummyReview3);
    QCOMPARE(QPlaceReview(retrievedCollection.value(3)), QPlaceReview());
    QCOMPARE(QPlaceReview(retrievedCollection.value(4)), QPlaceReview());
    QCOMPARE(QPlaceReview(retrievedCollection.value(5)), dummyReview6);
}

void tst_Place::categoriesTest()
{
    QPlace testObj;
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

void tst_Place::suppliersTest()
{
    QPlace testObj;
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

void tst_Place::feedsTest()
{
    QPlace testObj;
    QVERIFY2(testObj.feeds().count() == 0, "Wrong default value");
    QStringList list;
    list << "1" << "2" << "3";
    testObj.setFeeds(list);
    QVERIFY2(testObj.feeds().count() == 3, "Wrong value returned");
    QVERIFY2(testObj.feeds()[1] == "2", "Wrong [1] value returned");
}

void tst_Place::tagsTest()
{
    QPlace testObj;
    QVERIFY2(testObj.tags().count() == 0, "Wrong default value");
    QStringList list;
    list << "1" << "2" << "3";
    testObj.setTags(list);
    QVERIFY2(testObj.tags().count() == 3, "Wrong value returned");
    QVERIFY2(testObj.tags()[1] == "2", "Wrong [1] value returned");
}

void tst_Place::primaryPhoneTest()
{
    QPlace place;
    QVERIFY2(place.primaryPhone().isEmpty(), "Wrong default value");

    place.setPrimaryPhone("555-5555");
    QCOMPARE(place.primaryPhone(), QString("555-5555"));

    place.setPrimaryPhone(QString());
    QCOMPARE(place.primaryPhone(), QString());
}

void tst_Place::primaryFaxTest()
{
    QPlace place;
    QVERIFY2(place.primaryFax().isEmpty(), "Wrong default value");

    place.setPrimaryFax("555-5555");
    QCOMPARE(place.primaryFax(), QString("555-5555"));

    place.setPrimaryFax(QString());
    QCOMPARE(place.primaryFax(), QString());
}

void tst_Place::primaryEmailTest()
{
    QPlace place;
    QVERIFY2(place.primaryEmail().isEmpty(), "Wrong default value");

    place.setPrimaryEmail("test@test.com");
    QCOMPARE(place.primaryEmail(), QString("test@test.com"));


    place.setPrimaryEmail(QString());
    QCOMPARE(place.primaryEmail(), QString());
}

void tst_Place::primaryUrlTest()
{
    QPlace place;
    QVERIFY2(place.primaryUrl().isEmpty(), "Wron default value");

    place.setPrimaryUrl(QUrl("www.winterfell.com"));
    QCOMPARE(place.primaryUrl(), QUrl("www.winterfell.com"));

    place.setPrimaryUrl(QUrl());
    QCOMPARE(place.primaryUrl(), QUrl());
}

void tst_Place::operatorsTest()
{
    QPlace testObj;
    testObj.setPlaceId("testId");
    QPlaceAttribute paymentMethods;
    paymentMethods.setLabel("Payment methods");
    paymentMethods.setText("Visa");
    testObj.insertExtendedAttribute(QPlaceAttribute::PaymentMethods, paymentMethods);
    QStringList tags;
    tags << "1" << "2" << "3";
    testObj.setTags(tags);
    QGeoLocation loc;
    loc.setCoordinate(QGeoCoordinate(10,20));
    testObj.setLocation(loc);

    QPlace testObj2;
    testObj2 = testObj;
    QVERIFY2(testObj == testObj2, "Not copied correctly");
    testObj2.setPlaceId("342-456");
    QVERIFY2(testObj != testObj2, "Object should be different");
}

void tst_Place::extendedAttributeTest()
{
    QPlace place;
    QVERIFY2(place.extendedAttributes().isEmpty(), "Invalid default attributes");
    QPlaceAttribute smoking;
    smoking.setLabel("Public Smoking");
    smoking.setText("No");

    //test insertion of an attribution
    place.insertExtendedAttribute("Smoking", smoking);
    QVERIFY(place.extendedAttributes().contains("Smoking"));
    QCOMPARE(place.extendedAttributes().value("Smoking").label(), QLatin1String("Public Smoking"));
    QCOMPARE(place.extendedAttributes().value("Smoking").text(), QLatin1String("No"));
    QCOMPARE(place.extendedAttributes().count(), 1);

    QPlaceAttribute shelter;
    shelter.setLabel("Outdoor shelter");
    shelter.setText("Yes");

    //test insertion again
    place.insertExtendedAttribute("Shelter", shelter);
    QVERIFY(place.extendedAttributes().contains("Shelter"));
    QCOMPARE(place.extendedAttributes().value("Shelter").label(), QLatin1String("Outdoor shelter"));
    QCOMPARE(place.extendedAttributes().value("Shelter").text(), QLatin1String("Yes"));

    QCOMPARE(place.extendedAttributes().count(), 2);

    //test overwriting an attribute using an insertion
    shelter.setText("No");
    place.insertExtendedAttribute("Shelter", shelter);

    QPlaceAttribute wireless;
    wireless.setLabel("Wifi");
    wireless.setText("None");

    QPlaceAttribute lan;
    lan.setLabel("Lan");
    lan.setText("Available");

    //test setting a new set of attributes
    QPlace::ExtendedAttributes attributes;
    attributes.insert("Wireless", wireless);
    attributes.insert("Lan", lan);

    place.setExtendedAttributes(attributes);

    QVERIFY(place.extendedAttributes().contains("Wireless"));
    QVERIFY(place.extendedAttributes().contains("Lan"));
    QCOMPARE(place.extendedAttributes().value("Wireless").label(), QLatin1String("Wifi"));
    QCOMPARE(place.extendedAttributes().value("Wireless").text(), QLatin1String("None"));
    QCOMPARE(place.extendedAttributes().value("Lan").label(), QLatin1String("Lan"));
    QCOMPARE(place.extendedAttributes().value("Lan").text(), QLatin1String("Available"));

    QVERIFY(!place.extendedAttributes().contains("Shelter"));
    QVERIFY(!place.extendedAttributes().contains("Smoking"));
    QCOMPARE(place.extendedAttributes().count(), 2);

    //test clearing of attributes
    QPlace::ExtendedAttributes noAttributes;
    place.setExtendedAttributes(noAttributes);
    QCOMPARE(place.extendedAttributes().count(), 0);
}

QTEST_APPLESS_MAIN(tst_Place);

#include "tst_qplace.moc"
