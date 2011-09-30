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

#include <qplace.h>
#include <qplaceimage.h>
#include <qplaceattribute.h>
#include <QtLocation/QPlaceEditorial>

QT_USE_NAMESPACE

class tst_Place : public QObject
{
    Q_OBJECT

public:
    tst_Place();

private Q_SLOTS:
    void constructorTest();
    void categoriesTest();
    void detailsFetchedTest();
    void locationTest();
    void ratingTest();
    void suppliersTest();
    void imageContentTest();
    void reviewContentTest();
    void editorialContentTest();
    void contentCountTest();
    void nameTest();
    void placeIdTest();
    void attributionTest();
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

void tst_Place::editorialContentTest()
{
    QPlace place;
    QVERIFY2(place.content(QPlaceContent::EditorialType).count() == 0, "Wrong default value");

    QPlaceEditorial dummyEditorial;
    dummyEditorial.setTitle(QLatin1String("Editorial 1"));

    QPlaceEditorial dummyEditorial2;
    dummyEditorial2.setTitle(QLatin1String("Editorial 2"));

    QPlaceEditorial dummyEditorial3;
    dummyEditorial3.setTitle(QLatin1String("Editorial 3"));

    QPlaceContent::Collection editorialCollection;
    editorialCollection.insert(0,dummyEditorial);
    editorialCollection.insert(1, dummyEditorial2);
    editorialCollection.insert(2, dummyEditorial3);

    place.setContent(QPlaceContent::EditorialType, editorialCollection);
    QPlaceContent::Collection retrievedCollection = place.content(QPlaceContent::EditorialType);

    QCOMPARE(retrievedCollection.count(), 3);
    QCOMPARE(QPlaceEditorial(retrievedCollection.value(0)), dummyEditorial);
    QCOMPARE(QPlaceEditorial(retrievedCollection.value(1)), dummyEditorial2);
    QCOMPARE(QPlaceEditorial(retrievedCollection.value(2)), dummyEditorial3);

    //replace the second and insert a sixth editorial
    //indexes 4 and 5 are "missing"
    QPlaceEditorial dummyEditorial2New;
    dummyEditorial2.setTitle(QLatin1String("Editorial 2 new"));

    QPlaceEditorial dummyEditorial6;
    dummyEditorial6.setTitle(QLatin1String("Editorial 6"));

    editorialCollection.clear();
    editorialCollection.insert(1, dummyEditorial2New);
    editorialCollection.insert(5, dummyEditorial6);
    place.addContent(QPlaceContent::EditorialType, editorialCollection);

    retrievedCollection = place.content(QPlaceContent::EditorialType);
    QCOMPARE(retrievedCollection.count(), 4);
    QCOMPARE(QPlaceEditorial(retrievedCollection.value(0)), dummyEditorial);
    QCOMPARE(QPlaceEditorial(retrievedCollection.value(1)), dummyEditorial2New);
    QCOMPARE(QPlaceEditorial(retrievedCollection.value(2)), dummyEditorial3);
    QCOMPARE(QPlaceEditorial(retrievedCollection.value(3)), QPlaceEditorial());
    QCOMPARE(QPlaceEditorial(retrievedCollection.value(4)), QPlaceEditorial());
    QCOMPARE(QPlaceEditorial(retrievedCollection.value(5)), dummyEditorial6);
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

void tst_Place::attributionTest()
{
    QPlace testPlace;
    QVERIFY(testPlace.attribution().isEmpty());
    testPlace.setAttribution(QLatin1String("attribution"));
    QCOMPARE(testPlace.attribution(), QLatin1String("attribution"));
    testPlace.setAttribution(QString());
    QVERIFY(testPlace.attribution().isEmpty());
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
