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

#include <QCoreApplication>
#include <QString>
#include <QtTest/QtTest>

#include <qgeoboundingbox.h>
#include <qgeoboundingcircle.h>
#include <qgeoserviceprovider.h>
#include <qplacemanager.h>
#include <qplacecategory.h>
#include <qplacecontentreply.h>
#include <qplacesearchsuggestionreply.h>
#include <qplacesearchrequest.h>
#include <qplacesearchresult.h>
#include <qplacesearchreply.h>
#include <qplacecontactdetail.h>
#include <qplacesearchrequest.h>

#include "jsondbcleaner.h"

#ifndef QTRY_VERIFY
#define QTRY_VERIFY(__expr) \
        do { \
        const int __step = 50; \
        const int __timeout = 5000; \
        if (!(__expr)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && !(__expr); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QVERIFY(__expr); \
    } while (0)
#endif

#ifndef WAIT_UNTIL
#define WAIT_UNTIL(__expr) \
        do { \
        const int __step = 50; \
        const int __timeout = 5000; \
        if (!(__expr)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && !(__expr); __i+=__step) { \
            QTest::qWait(__step); \
        } \
    } while (0)
#endif

Q_DECLARE_METATYPE(QPlaceReply::Error);
Q_DECLARE_METATYPE(QPlaceReply *);
Q_DECLARE_METATYPE(QPlaceIdReply *);

QT_USE_NAMESPACE

class tst_QPlaceManagerJsonDb : public QObject
{
    Q_OBJECT
public:
    tst_QPlaceManagerJsonDb();

private Q_SLOTS:
    void initTestCase();
    void cleanup();

    void saveAndRemovePlace();
    void updatePlace();
    void simpleSaveAndRemoveCategory();
    void saveAndRemoveCategory();
    void updateCategory();
    void savePlaceWithCategory();

    void searchByName();
    void searchByBox();
    void searchByCircle();
    void searchWithLexicalPlaceNameHint();
    void searchWithDistanceHint();
    void searchByCategory();
    void icons();
    void unsupportedFunctions();
    void supportedFeatures();
    void categoryFunctions();
    void contactDetails();
    void contactDetails_data();
    void mulipleDetailTypes();
    void placeNotifications();
    void categoryNotifications();

private:
    bool doSavePlace(const QPlace &place,
                QPlaceReply::Error expectedError = QPlaceReply::NoError,
                QString *placeId = 0);
    void doSavePlaces(QList<QPlace> &places);
    void doSavePlaces(const QList<QPlace *> &places);

    bool doRemovePlace(const QPlace &place,
                       QPlaceReply::Error expectedError = QPlaceReply::NoError);

    bool doSearch(const QPlaceSearchRequest &request,
                  QList<QPlaceSearchResult> *results,
             QPlaceReply::Error expectedError = QPlaceReply::NoError);

    bool doSearch(const QPlaceSearchRequest &request,
                  QList<QPlace> *results,
             QPlaceReply::Error expectedError = QPlaceReply::NoError);

    bool doFetchDetails(QString placeId,
                        QPlace *place,
                        QPlaceReply::Error expectedError = QPlaceReply::NoError);

    bool doSaveCategory(const QPlaceCategory &category,
                        QPlaceReply::Error expectedError = QPlaceReply::NoError,
                        QString *categoryId = 0);

    bool doSaveCategory(const QPlaceCategory &category,
                        const QString &parentId,
                        QPlaceReply::Error expectedError = QPlaceReply::NoError,
                        QString *categoryId = 0);

    bool doRemoveCategory(const QPlaceCategory &category,
                          QPlaceReply::Error expectedError = QPlaceReply::NoError);

    bool checkSignals(QPlaceReply *reply, QPlaceReply::Error expectedError);


    bool compareResults(const QList<QPlaceSearchResult> &results, const QList<QPlace> &expectedResults);

    QGeoServiceProvider *provider;
    QPlaceManager *placeManager;
    QCoreApplication *coreApp;
    JsonDbCleaner *dbCleaner ;
};

tst_QPlaceManagerJsonDb::tst_QPlaceManagerJsonDb()
{
    //We need a QCoreApplication instance for
    //event dispatching
    int argc;
    char **argv=0;
    coreApp = new QCoreApplication(argc, argv);

    dbCleaner = new JsonDbCleaner(this);
}

void tst_QPlaceManagerJsonDb::initTestCase()
{
    qRegisterMetaType<QPlaceReply::Error>();
    qRegisterMetaType<QPlaceReply *>();
    qRegisterMetaType<QPlaceIdReply *>();

    QStringList providers = QGeoServiceProvider::availableServiceProviders();
    providers.contains("nokia_places_jsondb");

    provider = new QGeoServiceProvider("nokia_places_jsondb");
    placeManager = provider->placeManager();
    QVERIFY(placeManager);

    cleanup();
}

void tst_QPlaceManagerJsonDb::saveAndRemovePlace()
{
    QPlace place;
    place.setName("Char");
    QGeoLocation location;

    location.setCoordinate(QGeoCoordinate(10,20));

    QGeoAddress address;
    address.setStreet("1 test street");
    address.setCity("New york");
    address.setCountry("USA");
    location.setAddress(address);
    place.setLocation(location);

    QPlaceContactDetail phone;
    phone.setValue("555-5555");
    place.appendContactDetail(QPlaceContactDetail::Phone, phone);

    QPlaceContactDetail fax;
    fax.setValue("999-9999");
    place.appendContactDetail(QPlaceContactDetail::Fax, fax);

    QPlaceContactDetail email;
    email.setValue("email@adddresss.com");
    place.appendContactDetail(QPlaceContactDetail::Email, email);

    QPlaceContactDetail website;
    website.setValue("www.example.com");
    place.appendContactDetail(QPlaceContactDetail::Website, website);

    //Save a place
    QString placeId;
    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));

    //ensure we can retrieve it's details
    QPlace retrievedPlace;
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    place.setPlaceId(placeId);
    QVERIFY(retrievedPlace == place);

    //try removing a place
    QVERIFY(doRemovePlace(place, QPlaceReply::NoError));

    //ensure it is actually deleted
    QVERIFY(doFetchDetails(placeId, &retrievedPlace, QPlaceReply::PlaceDoesNotExistError));
    QCOMPARE(retrievedPlace, QPlace());

    //try removing a place that does not exist;
    QVERIFY(doRemovePlace(place, QPlaceReply::PlaceDoesNotExistError));


    place.setVisibility(QtLocation::PublicVisibility);

    QVERIFY(doSavePlace(place, QPlaceReply::UnsupportedError, 0));
}

void tst_QPlaceManagerJsonDb::updatePlace()
{
    //save a place and check that we can retrieve it
    QPlace place;
    place.setName("Sydney");
    QGeoAddress address;
    address.setStreet("original street");
    QGeoLocation location;
    location.setAddress(address);
    place.setLocation(location);

    QString placeId;
    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));
    QVERIFY(doFetchDetails(placeId, &place));

    //update the place again with some changes
    place.setName(QLatin1String("Brisbane"));
    address.setStreet(QLatin1String("new street"));
    address.setCountry(QLatin1String("Australia"));
    location.setAddress(address);
    place.setLocation(location);

    QPlace retrievedPlace;
    QVERIFY(doSavePlace(place, QPlaceReply::NoError));
    QVERIFY(doFetchDetails(place.placeId(), &retrievedPlace));
    QCOMPARE(retrievedPlace.name(), QLatin1String("Brisbane"));
    QCOMPARE(retrievedPlace.location().address().street(), QLatin1String("new street"));
    QCOMPARE(retrievedPlace.location().address().country(), QLatin1String("Australia"));

    //try updating a non-existent place
    place.setPlaceId("Non-existent id");
    QVERIFY(doSavePlace(place, QPlaceReply::PlaceDoesNotExistError));
}

void tst_QPlaceManagerJsonDb::simpleSaveAndRemoveCategory()
{
    QString categoryId;
    QPlaceCategory restaurant;
    restaurant.setName("Restaurant");
    QVERIFY(doSaveCategory(restaurant, QPlaceReply::NoError, &categoryId));
    restaurant.setCategoryId(categoryId);

    QList<QPlaceCategory> categories = placeManager->childCategories();
    QCOMPARE(categories.count(), 1);

    QPlaceCategory retrievedCategory = placeManager->category(categoryId);
    QCOMPARE(retrievedCategory, restaurant);

    QVERIFY(doRemoveCategory(restaurant));

    QCOMPARE(placeManager->category(categoryId), QPlaceCategory());
    categories = placeManager->childCategories();
    QCOMPARE(categories.count(), 0);
}

void tst_QPlaceManagerJsonDb::saveAndRemoveCategory()
{
    QString categoryId;
    QPlaceCategory restaurant;
    restaurant.setName("Restaurant");
    QVERIFY(doSaveCategory(restaurant, QPlaceReply::NoError, &categoryId));
    restaurant.setCategoryId(categoryId);

    QPlaceCategory fastFood;
    fastFood.setName("Fast Food");
    QVERIFY(doSaveCategory(fastFood, restaurant.categoryId(), QPlaceReply::NoError, &categoryId));
    fastFood.setCategoryId(categoryId);

    QPlaceCategory fineDining;
    fineDining.setName("Fine dining");
    QVERIFY(doSaveCategory(fineDining, restaurant.categoryId(), QPlaceReply::NoError, &categoryId));
    fineDining.setCategoryId(categoryId);

    QPlaceCategory pancakes;
    pancakes.setName("Pancakes");
    QVERIFY(doSaveCategory(pancakes, restaurant.categoryId(), QPlaceReply::NoError, &categoryId));
    pancakes.setCategoryId(categoryId);

    QPlaceCategory pizza;
    pizza.setName("Pizza");
    QVERIFY(doSaveCategory(pizza, fastFood.categoryId(), QPlaceReply::NoError, &categoryId));
    pizza.setCategoryId(categoryId);

    QPlaceCategory burgers;
    burgers.setName("burgers");
    QVERIFY(doSaveCategory(burgers, fastFood.categoryId(), QPlaceReply::NoError, &categoryId));
    burgers.setCategoryId(categoryId);

    QPlaceCategory accommodation;
    accommodation.setName("Accommodation");
    QVERIFY(doSaveCategory(accommodation, QPlaceReply::NoError,&categoryId));
    accommodation.setCategoryId(categoryId);

    QPlaceCategory hotel;
    hotel.setName("Hotel");
    QVERIFY(doSaveCategory(hotel, accommodation.categoryId(), QPlaceReply::NoError, &categoryId));
    hotel.setCategoryId(categoryId);

    QPlaceCategory motel;
    motel.setName("Motel");
    QVERIFY(doSaveCategory(motel, accommodation.categoryId(), QPlaceReply::NoError, &categoryId));
    motel.setCategoryId(categoryId);

    QList<QPlaceCategory> topLevelCategories = placeManager->childCategories();

    QVERIFY(topLevelCategories.contains(restaurant));
    QVERIFY(topLevelCategories.contains(accommodation));
    QVERIFY(topLevelCategories.count() == 2);

    QList<QPlaceCategory> categories = placeManager->childCategories(restaurant.categoryId());
    QVERIFY(categories.contains(fastFood));
    QVERIFY(categories.contains(fineDining));
    QVERIFY(categories.contains(pancakes));
    QCOMPARE(categories.count(), 3);

    categories = placeManager->childCategories(fastFood.categoryId());
    QVERIFY(categories.contains(pizza));
    QVERIFY(categories.contains(burgers));
    QCOMPARE(categories.count(), 2);

    categories = placeManager->childCategories(accommodation.categoryId());
    QVERIFY(categories.contains(hotel));
    QVERIFY(categories.contains(motel));
    QCOMPARE(categories.count(), 2);

    QVERIFY(doRemoveCategory(fastFood));
    categories = placeManager->childCategories(restaurant.categoryId());
    QVERIFY(categories.contains(fineDining));
    QVERIFY(categories.contains(pancakes));
    QCOMPARE(categories.count(), 2);

    QVERIFY(placeManager->category(pizza.categoryId()) == QPlaceCategory());
    QVERIFY(placeManager->category(burgers.categoryId()) == QPlaceCategory());

    QVERIFY(doRemoveCategory(accommodation));
    categories = placeManager->childCategories();
    QVERIFY(categories.contains(restaurant));
    QCOMPARE(categories.count(), 1);
}

void tst_QPlaceManagerJsonDb::updateCategory()
{
    //Test updating a category name
    QPlaceCategory category;
    category.setName("Foood");
    QString categoryId;
    QVERIFY(doSaveCategory(category, QPlaceReply::NoError, &categoryId));
    category.setCategoryId(categoryId);

    QList<QPlaceCategory> categories;
    categories = placeManager->childCategories();
    QVERIFY(categories.contains(category));

    category.setName("Food");
    QVERIFY(doSaveCategory(category, QPlaceReply::NoError, &categoryId));
    categories = placeManager->childCategories();
    QVERIFY(categories.contains(category));
    QCOMPARE(categories.count(), 1);

    //Test updating a category's parent
    QPlaceCategory restaurant;
    restaurant.setName("Restaurant");
    QVERIFY(doSaveCategory(restaurant, QPlaceReply::NoError, &categoryId));
    restaurant.setCategoryId(categoryId);

    QPlaceCategory fastFood;
    fastFood.setName("Fast Food");
    QVERIFY(doSaveCategory(fastFood, restaurant.categoryId(), QPlaceReply::NoError, &categoryId));
    fastFood.setCategoryId(categoryId);

    QPlaceCategory fineDining;
    fineDining.setName("Fine dining");
    QVERIFY(doSaveCategory(fineDining, restaurant.categoryId(), QPlaceReply::NoError, &categoryId));
    fineDining.setCategoryId(categoryId);

    QPlaceCategory pancakes;
    pancakes.setName("Pancakes");
    QVERIFY(doSaveCategory(pancakes, restaurant.categoryId(), QPlaceReply::NoError, &categoryId));
    pancakes.setCategoryId(categoryId);

    QPlaceCategory pizza;
    pizza.setName("Pizza");
    QVERIFY(doSaveCategory(pizza, fastFood.categoryId(), QPlaceReply::NoError, &categoryId));
    pizza.setCategoryId(categoryId);

    QPlaceCategory burgers;
    burgers.setName("burgers");
    QVERIFY(doSaveCategory(burgers, fastFood.categoryId(), QPlaceReply::NoError, &categoryId));
    burgers.setCategoryId(categoryId);

    //resave pizza as a child of fine dining
    QVERIFY(doSaveCategory(pizza, fineDining.categoryId()));

    //check that fast food no longer has  pizza as a child
    categories = placeManager->childCategories(fastFood.categoryId());
    QVERIFY(categories.contains(burgers));
    QCOMPARE(categories.count(), 1);

    //check that fine dining has pizza as a child
    categories = placeManager->childCategories(fineDining.categoryId());
    QVERIFY(categories.contains(pizza));
    QCOMPARE(categories.count(), 1);
}

void tst_QPlaceManagerJsonDb::savePlaceWithCategory()
{
    QString categoryId;
    QPlaceCategory restaurant;
    restaurant.setName("Restaurant");
    QVERIFY(doSaveCategory(restaurant, QPlaceReply::NoError, &categoryId));
    restaurant.setCategoryId(categoryId);

    QString placeId;
    QPlace monolithBurgers;
    monolithBurgers.setName("Monolith burgers");
    monolithBurgers.setCategory(restaurant);
    QVERIFY(doSavePlace(monolithBurgers,QPlaceReply::NoError, &placeId));
    monolithBurgers.setPlaceId(placeId);

    QPlace retrievedPlace;
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QCOMPARE(retrievedPlace.categories().count(), 1);
    QCOMPARE(retrievedPlace.categories().first(), restaurant);

    //update place with multiple categories
    QPlaceCategory facilities;
    facilities.setName("facilities");
    QVERIFY(doSaveCategory(facilities, QPlaceReply::NoError, &categoryId));
    facilities.setCategoryId(categoryId);

    QList<QPlaceCategory> categories;
    categories << facilities << restaurant;
    monolithBurgers.setCategories(categories);
    QVERIFY(doSavePlace(monolithBurgers,QPlaceReply::NoError, &placeId));

    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QCOMPARE(retrievedPlace.categories().count(), 2);
    QSet<QPlaceCategory> catSet;

    QVERIFY(retrievedPlace.categories().contains(facilities));
    QVERIFY(retrievedPlace.categories().contains(restaurant));
}

void tst_QPlaceManagerJsonDb::searchByName()
{
    QPlace adelaide, adel, ad, brisbane;
    adelaide.setName("Adelaide");
    adel.setName("adel");
    ad.setName("ad");
    brisbane.setName("brisbane");

    QList<QPlace> places;
    places << adelaide << adel << ad << brisbane;
    doSavePlaces(places);

    //test that search has a 'startsWith' behaviour
    //and is case insensitive
    QPlaceSearchRequest request;
    request.setSearchTerm("adel");
    QList<QPlaceSearchResult> results;
    QVERIFY(doSearch(request, &results));
    QCOMPARE(results.count(), 2);

    //Search for a non-exisent place
    request.setSearchTerm("Nowhere");
    QVERIFY(doSearch(request, &results));
    QCOMPARE(results.count(), 0);
}

void tst_QPlaceManagerJsonDb::searchByBox()
{
    QList<QGeoCoordinate> outBox;
    QList<QGeoCoordinate> inBox1;

    inBox1 << QGeoCoordinate(-5.0, -5.0);
    inBox1 << QGeoCoordinate(-5.0, 0.0);
    inBox1 << QGeoCoordinate(-5.0, 5.0);
    inBox1 << QGeoCoordinate(0.0, -5.0);
    inBox1 << QGeoCoordinate(0.0, 0.0);
    inBox1 << QGeoCoordinate(0.0, 5.0);
    inBox1 << QGeoCoordinate(5.0, -5.0);
    inBox1 << QGeoCoordinate(5.0, 0.0);
    inBox1 << QGeoCoordinate(5.0, 5.0);

    outBox << QGeoCoordinate(-5.0, -6.0);
    outBox << QGeoCoordinate(5.0, -6.0);
    outBox << QGeoCoordinate(-6.0, -5.0);
    outBox << QGeoCoordinate(6.0, -5.0);
    outBox << QGeoCoordinate(-6.0, 5.0);
    outBox << QGeoCoordinate(6.0, 5.0);
    outBox << QGeoCoordinate(-5.0, 6.0);
    outBox << QGeoCoordinate(5.0, 6.0);

    QList<QGeoCoordinate> inBox2;

    inBox2 << QGeoCoordinate(-5.0, 10.0);
    inBox2 << QGeoCoordinate(-5.0, 15.0);
    inBox2 << QGeoCoordinate(-5.0, 20.0);
    inBox2 << QGeoCoordinate(0.0, 10.0);
    inBox2 << QGeoCoordinate(0.0, 15.0);
    inBox2 << QGeoCoordinate(0.0, 20.0);
    inBox2 << QGeoCoordinate(5.0, 10.0);
    inBox2 << QGeoCoordinate(5.0, 15.0);
    inBox2 << QGeoCoordinate(5.0, 20.0);

    outBox << QGeoCoordinate(-5.0, 9.0);
    outBox << QGeoCoordinate(5.0,  9.0);
    outBox << QGeoCoordinate(-6.0, 10.0);
    outBox << QGeoCoordinate(6.0,  10.0);
    outBox << QGeoCoordinate(-6.0, 20.0);
    outBox << QGeoCoordinate(6.0, 20.0);
    outBox << QGeoCoordinate(-5.0, 21.0);
    outBox << QGeoCoordinate(5.0, 21.0);

    QList<QGeoCoordinate> inBox3;

    inBox3 << QGeoCoordinate(10.0, -5.0);
    inBox3 << QGeoCoordinate(10.0, 0.0);
    inBox3 << QGeoCoordinate(10.0, 5.0);
    inBox3 << QGeoCoordinate(15.0, -5.0);
    inBox3 << QGeoCoordinate(15.0, 0.0);
    inBox3 << QGeoCoordinate(15.0, 5.0);
    inBox3 << QGeoCoordinate(20.0, -5.0);
    inBox3 << QGeoCoordinate(20.0, 0.0);
    inBox3 << QGeoCoordinate(20.0, 5.0);

    outBox << QGeoCoordinate(10.0, -6.0);
    outBox << QGeoCoordinate(20.0, -6.0);
    outBox << QGeoCoordinate(9.0, -5.0);
    outBox << QGeoCoordinate(21.0, -5.0);
    outBox << QGeoCoordinate(9.0, 5.0);
    outBox << QGeoCoordinate(21.0, 5.0);
    outBox << QGeoCoordinate(10.0, 6.0);
    outBox << QGeoCoordinate(20.0, 6.0);

    QList<QGeoCoordinate> inBox4;

    inBox4 << QGeoCoordinate(10.0, 10.0);
    inBox4 << QGeoCoordinate(10.0, 15.0);
    inBox4 << QGeoCoordinate(10.0, 20.0);
    inBox4 << QGeoCoordinate(15.0, 10.0);
    inBox4 << QGeoCoordinate(15.0, 15.0);
    inBox4 << QGeoCoordinate(15.0, 20.0);
    inBox4 << QGeoCoordinate(20.0, 10.0);
    inBox4 << QGeoCoordinate(20.0, 15.0);
    inBox4 << QGeoCoordinate(20.0, 20.0);

    outBox << QGeoCoordinate(10.0, 9.0);
    outBox << QGeoCoordinate(20.0, 9.0);
    outBox << QGeoCoordinate(9.0, 10.0);
    outBox << QGeoCoordinate(21.0, 10.0);
    outBox << QGeoCoordinate(9.0, 20.0);
    outBox << QGeoCoordinate(21.0, 20.0);
    outBox << QGeoCoordinate(10.0, 21.0);
    outBox << QGeoCoordinate(20.0, 21.0);

    QList<QGeoCoordinate> inBox5;

    inBox5 << QGeoCoordinate(-5.0, 175.0);
    inBox5 << QGeoCoordinate(-5.0, 180.0);
    inBox5 << QGeoCoordinate(-5.0, -175.0);
    inBox5 << QGeoCoordinate(0.0, 175.0);
    inBox5 << QGeoCoordinate(0.0, 180.0);
    inBox5 << QGeoCoordinate(0.0, -175.0);
    inBox5 << QGeoCoordinate(5.0, 175.0);
    inBox5 << QGeoCoordinate(5.0, 180.0);
    inBox5 << QGeoCoordinate(5.0, -175.0);

    outBox << QGeoCoordinate(-6.0, 175.0);
    outBox << QGeoCoordinate(-6.0, -175.0);
    outBox << QGeoCoordinate(-5.0, 174.0);
    outBox << QGeoCoordinate(-5.0, -174.0);
    outBox << QGeoCoordinate(5.0, 174.0);
    outBox << QGeoCoordinate(5.0, -174.0);
    outBox << QGeoCoordinate(6.0, 175.0);
    outBox << QGeoCoordinate(6.0, -175.0);

    QList<QGeoCoordinate> coords = outBox;
    coords.append(inBox1);
    coords.append(inBox2);
    coords.append(inBox3);
    coords.append(inBox4);
    coords.append(inBox5);

    QList<QPlace> places;
    for (int i = 0; i < coords.size(); ++i) {
        QPlace place;
        QGeoLocation location;
        location.setCoordinate(coords.at(i));
        place.setLocation(location);
        places << place;
    }
    doSavePlaces(places);

    QPlaceSearchRequest request;
    request.setSearchArea(new QGeoBoundingBox(QGeoCoordinate(5.0, -5.0), QGeoCoordinate(-5.0, 5.0)));

    QList<QPlaceSearchResult> results1;
    doSearch(request, &results1);
    QList<QPlace> places1;
    foreach (const QPlaceSearchResult  &result, results1)
        places1.append(result.place());

    QCOMPARE(places1.size(), inBox1.size());

    QSet<QString> testSet1;
    for (int i = 0; i < places1.size(); ++i)
        testSet1.insert(places1.at(i).location().coordinate().toString());

    QSet<QString> inBoxSet1;
    for (int i = 0; i < inBox1.size(); ++i)
        inBoxSet1.insert(inBox1.at(i).toString());

    QCOMPARE(testSet1, inBoxSet1);

    request.setSearchArea(new QGeoBoundingBox(QGeoCoordinate(5.0, 10.0), QGeoCoordinate(-5.0, 20.0)));
    QList<QPlace> places2;
    doSearch(request, &places2);
    QCOMPARE(places2.size(), inBox2.size());

    QSet<QString> testSet2;
    for (int i = 0; i < places2.size(); ++i)
        testSet2.insert(places2.at(i).location().coordinate().toString());

    QSet<QString> inBoxSet2;
    for (int i = 0; i < inBox2.size(); ++i)
        inBoxSet2.insert(inBox2.at(i).toString());

    QCOMPARE(testSet2, inBoxSet2);

    request.setSearchArea(new QGeoBoundingBox(QGeoCoordinate(20.0, -5.0), QGeoCoordinate(10.0, 5.0)));
    QList<QPlace> places3;
    doSearch(request, &places3);

    QCOMPARE(places3.size(), inBox3.size());

    QSet<QString> testSet3;
    for (int i = 0; i < places3.size(); ++i)
        testSet3.insert(places3.at(i).location().coordinate().toString());

    QSet<QString> inBoxSet3;
    for (int i = 0; i < inBox3.size(); ++i)
        inBoxSet3.insert(inBox3.at(i).toString());

    QCOMPARE(testSet3, inBoxSet3);

    request.setSearchArea(new QGeoBoundingBox(QGeoCoordinate(20.0, 10.0), QGeoCoordinate(10.0, 20.0)));
    QList<QPlace> places4;
    doSearch(request, &places4);
    QCOMPARE(places4.size(), inBox4.size());

    QSet<QString> testSet4;
    for (int i = 0; i < places4.size(); ++i)
        testSet4.insert(places4.at(i).location().coordinate().toString());

    QSet<QString> inBoxSet4;
    for (int i = 0; i < inBox4.size(); ++i)
        inBoxSet4.insert(inBox4.at(i).toString());

    QCOMPARE(testSet4, inBoxSet4);

    request.setSearchArea(new QGeoBoundingBox(QGeoCoordinate(5.0, 175.0), QGeoCoordinate(-5.0, -175.0)));
    QList<QPlace> places5;
    doSearch(request, &places5);
    QCOMPARE(places5.size(), inBox5.size());

    QSet<QString> testSet5;
    for (int i = 0; i < places5.size(); ++i) {
        if (places5.at(i).location().coordinate().longitude() == -180.0) {
            QGeoLocation location;
            location.setCoordinate(QGeoCoordinate(places5.at(i).location().coordinate().latitude(), 180.0));
            places5[i].setLocation(location);
        }
        testSet5.insert(places5.at(i).location().coordinate().toString());
    }

    QSet<QString> inBoxSet5;
    for (int i = 0; i < inBox5.size(); ++i) {
        if (inBox5.at(i).longitude() == -180.0) {
            inBox5[i].setLongitude(180.0);
        }
        inBoxSet5.insert(inBox5.at(i).toString());
    }

    QCOMPARE(testSet5, inBoxSet5);

    //try a box that finds nothing
    request.setSearchArea(new QGeoBoundingBox(QGeoCoordinate(-70,-70), QGeoCoordinate(-80,-60)));
    QVERIFY(doSearch(request, &places));
    QCOMPARE(places.count(), 0);

    //--- Test error conditions
    //bottom right latitude > top left latitude
    request.setSearchArea(new QGeoBoundingBox(QGeoCoordinate(20,20), QGeoCoordinate(50,30)));
    QVERIFY(doSearch(request,&places, QPlaceReply::BadArgumentError));

    //try an invalid coordinate for one of the corners
    request.setSearchArea(new QGeoBoundingBox(QGeoCoordinate(qQNaN(),20),QGeoCoordinate(10,30)));
    QVERIFY(doSearch(request,&places, QPlaceReply::BadArgumentError));
}

void tst_QPlaceManagerJsonDb::searchByCircle()
{
    QList<QGeoCoordinate> greenwhichFilterCoords;
    QList<QGeoCoordinate> datelineFilterCoords;
    QList<QGeoCoordinate> northPoleFilterCoords;
    QList<QGeoCoordinate> southPoleFilterCoords;
    QList<QGeoCoordinate> northFilterCoords;
    QList<QGeoCoordinate> eastFilterCoords;
    QList<QGeoCoordinate> northeastFilterCoords;

    QList<QGeoCoordinate> greenwhichLmCoords;
    QList<QGeoCoordinate> datelineLmCoords;
    QList<QGeoCoordinate> northPoleLmCoords;
    QList<QGeoCoordinate> southPoleLmCoords;
    QList<QGeoCoordinate> northLmCoords;
    QList<QGeoCoordinate> eastLmCoords;
    QList<QGeoCoordinate> northeastLmCoords;

    greenwhichFilterCoords << QGeoCoordinate(-0.1, -0.1);
    greenwhichFilterCoords << QGeoCoordinate(0.1, -0.1);
    greenwhichFilterCoords << QGeoCoordinate(-0.1, 0.1);
    greenwhichFilterCoords << QGeoCoordinate(0.1, 0.1);

    datelineFilterCoords << QGeoCoordinate(-0.1, -179.9);
    datelineFilterCoords << QGeoCoordinate(0.1, -179.9);
    datelineFilterCoords << QGeoCoordinate(-0.1, 179.9);
    datelineFilterCoords << QGeoCoordinate(0.1, 179.9);

    northPoleFilterCoords << QGeoCoordinate(89.9, -179.9);
    northPoleFilterCoords << QGeoCoordinate(89.9, -0.1);
    northPoleFilterCoords << QGeoCoordinate(89.9, 0.1);
    northPoleFilterCoords << QGeoCoordinate(89.9, 179.9);

    southPoleFilterCoords << QGeoCoordinate(-89.9, -179.9);
    southPoleFilterCoords << QGeoCoordinate(-89.9, -0.1);
    southPoleFilterCoords << QGeoCoordinate(-89.9, 0.1);
    southPoleFilterCoords << QGeoCoordinate(-89.9, 179.9);

    eastFilterCoords << QGeoCoordinate(-0.1, 10.0);
    eastFilterCoords << QGeoCoordinate(0.1, 10.0);
    northFilterCoords << QGeoCoordinate(10.0, -0.1);
    northFilterCoords << QGeoCoordinate(10.0, 0.1);
    northeastFilterCoords << QGeoCoordinate(10.0, 10.0);

    greenwhichLmCoords << QGeoCoordinate(-1.0, -1.0);
    greenwhichLmCoords << QGeoCoordinate(1.0, -1.0);
    greenwhichLmCoords << QGeoCoordinate(-1.0, 1.0);
    greenwhichLmCoords << QGeoCoordinate(1.0, 1.0);

    datelineLmCoords << QGeoCoordinate(-1.0, -179.0);
    datelineLmCoords << QGeoCoordinate(1.0, -179.0);
    datelineLmCoords << QGeoCoordinate(-1.0, 179.0);
    datelineLmCoords << QGeoCoordinate(1.0, 179.0);

    northPoleLmCoords << QGeoCoordinate(89.0, -179.0);
    northPoleLmCoords << QGeoCoordinate(89.0, -1.0);
    northPoleLmCoords << QGeoCoordinate(89.0, 1.0);
    northPoleLmCoords << QGeoCoordinate(89.0, 179.0);

    southPoleLmCoords << QGeoCoordinate(-89.0, -179.0);
    southPoleLmCoords << QGeoCoordinate(-89.0, -1.0);
    southPoleLmCoords << QGeoCoordinate(-89.0, 1.0);
    southPoleLmCoords << QGeoCoordinate(-89.0, 179.0);

    eastLmCoords << QGeoCoordinate(-1.0, 11.0);
    eastLmCoords << QGeoCoordinate(1.0, 11.0);
    northLmCoords << QGeoCoordinate(11.0, -1.0);
    northLmCoords << QGeoCoordinate(11.0, 1.0);
    northeastLmCoords << QGeoCoordinate(11.0, 11.0);

    QList<QList<QGeoCoordinate> > coords;
    coords << greenwhichLmCoords;
    coords << datelineLmCoords;

    coords << northPoleLmCoords;
    coords << southPoleLmCoords;
    coords << eastLmCoords;
    coords << northLmCoords;
    coords << northeastLmCoords;

    for (int i = 0; i < coords.size(); ++i) {
        QList<QGeoCoordinate> c = coords.at(i);
        for (int j = 0; j < c.size(); ++j) {
            QPlace place;
            QGeoLocation location;
            location.setCoordinate(c.at(j));
            place.setLocation(location);
            doSavePlace(place);
        }
    }

    QList<QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> > > testSets;
    testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(greenwhichFilterCoords, greenwhichLmCoords);
    testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(datelineFilterCoords, datelineLmCoords);

    testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(northPoleFilterCoords, northPoleLmCoords);
    testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(southPoleFilterCoords, southPoleLmCoords);

    testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(northFilterCoords, northLmCoords);
    testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(eastFilterCoords, eastLmCoords);
    testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(northeastFilterCoords, northeastLmCoords);

    qreal dist = QGeoCoordinate(0.0, 0.0).distanceTo(QGeoCoordinate(5.0, 5.0));

    QList<QPlaceSearchResult> results;
    QList<QPlace> places;
    for (int i = 0; i < testSets.size(); ++i) {
        QList<QGeoCoordinate> filterCoords = testSets.at(i).first;
        QList<QGeoCoordinate> plCoords = testSets.at(i).second;

        QPlaceSearchRequest request;
        for (int j = 0; j < filterCoords.size(); ++j) {
            request.setSearchArea(new QGeoBoundingCircle(filterCoords.at(j),dist));

            if (i ==2 || i ==3) {
                //TODO: Testing poles, ignore fo rnow
                continue;
            } else {
                doSearch(request, &results);
                foreach (const QPlaceSearchResult &result, results)
                    places.append(result.place());
            }

            if (places.size() != plCoords.size()) {
                for (int k = 0; k < places.size(); ++k)
                    qWarning() << "pl" << places.at(k).location().coordinate().toString(QGeoCoordinate::Degrees);
                for (int k = 0; k < plCoords.size(); ++k)
                    qWarning() << "plCoords" << plCoords.at(k).toString(QGeoCoordinate::Degrees);
            }

            QCOMPARE(places.size(), plCoords.size());

            for (int k = 0; k < places.size(); ++k) {
                QVERIFY(plCoords.contains(places.at(k).location().coordinate()));
            }
            results.clear();
            places.clear();
        }
    }

    //--- Test error conditions and edge cases
    //try a circle that covers the north pole
    QPlaceSearchRequest request;
    request.setSearchArea(new QGeoBoundingCircle(QGeoCoordinate(89.91,0),11000));
    QVERIFY(doSearch(request,&places, QPlaceReply::BadArgumentError));
    QCOMPARE(places.count(), 0);

    //try a circle that's close to but does not cover the north pole
    request.setSearchArea(new QGeoBoundingCircle(QGeoCoordinate(89.91,0),9000));
    QVERIFY(doSearch(request,&places, QPlaceReply::NoError));

    //try a circle that covers the south pole
    request.setSearchArea(new QGeoBoundingCircle(QGeoCoordinate(-89.91,180),11000));
    QVERIFY(doSearch(request, &places, QPlaceReply::BadArgumentError));
    QCOMPARE(places.count(), 0);

    //try a circle that's close to but does not cover the south pole
    request.setSearchArea(new QGeoBoundingCircle(QGeoCoordinate(-89.91,180),9000));
    QVERIFY(doSearch(request, &places, QPlaceReply::NoError));

    //try a radius of 0
    request.setSearchArea(new QGeoBoundingCircle(QGeoCoordinate(11.0,11.0), 0));
    QVERIFY(doSearch(request, &places));
    QCOMPARE(places.count(), 1);
    QCOMPARE(places.at(0).location().coordinate().latitude(), 11.0);
    QCOMPARE(places.at(0).location().coordinate().longitude(), 11.0);

    //try an invalid center
    request.setSearchArea(new QGeoBoundingCircle(QGeoCoordinate(), 5000));
    QVERIFY(doSearch(request, &places, QPlaceReply::BadArgumentError));
    QVERIFY(places.isEmpty());

    //try an invalid latitude for the center
    request.setSearchArea(new QGeoBoundingCircle(QGeoCoordinate(qQNaN(),50), 5000));
    QVERIFY(doSearch(request, &places, QPlaceReply::BadArgumentError));
    QVERIFY(places.isEmpty());

    //try a proximity filter with an out of range latitude
    request.setSearchArea(new QGeoBoundingCircle(QGeoCoordinate(90,10), 5000));
    QVERIFY(doSearch(request, &places, QPlaceReply::BadArgumentError));
    QVERIFY(places.isEmpty());

    request.setSearchArea(new QGeoBoundingCircle(QGeoCoordinate(150,10), 5000));
    QVERIFY(doSearch(request, &places, QPlaceReply::BadArgumentError));
    QVERIFY(places.isEmpty());

    //try a proximity filter with an out of range longitude
    request.setSearchArea(new QGeoBoundingCircle(QGeoCoordinate(-12,180.1),5000));
    QVERIFY(doSearch(request, &places, QPlaceReply::BadArgumentError));
    QVERIFY(places.isEmpty());

    request.setSearchArea(new QGeoBoundingCircle(QGeoCoordinate(-12,-180.1),5000));
    QVERIFY(doSearch(request, &places, QPlaceReply::BadArgumentError));
    QVERIFY(places.isEmpty());
}

void tst_QPlaceManagerJsonDb::searchWithLexicalPlaceNameHint()
{
    QPlace melbourne, sydney, adelaide, brisbane;
    melbourne.setName(QLatin1String("Melbourne"));
    sydney.setName(QLatin1String("Sydney"));
    adelaide.setName(QLatin1String("Adelaide"));
    brisbane.setName(QLatin1String("Brisbane"));

    QList<QPlace *> places;
    places << &melbourne << &sydney << &adelaide << &brisbane;
    doSavePlaces(places);

    QPlaceSearchRequest searchRequest;
    searchRequest.setRelevanceHint(QPlaceSearchRequest::LexicalPlaceNameHint);
    QList<QPlace> results;
    QVERIFY(doSearch(searchRequest, &results));
    QCOMPARE(results.count(), places.count());
    QCOMPARE(results.at(0), adelaide);
    QCOMPARE(results.at(1), brisbane);
    QCOMPARE(results.at(2), melbourne);
    QCOMPARE(results.at(3), sydney);
}

void tst_QPlaceManagerJsonDb::searchWithDistanceHint()
{
    QList<QPlace *> places;

    QPlace place1;
    place1.setName(QLatin1String("LM1"));
    QGeoLocation location;
    location.setCoordinate(QGeoCoordinate(20,19));
    place1.setLocation(location);
    places << &place1;

    QPlace place2;
    place2.setName(QLatin1String("LM2"));
    location.setCoordinate(QGeoCoordinate(20,50));
    place2.setLocation(location);
    places << &place2;

    QPlace place3;
    place3.setName(QLatin1String("LM3"));
    location.setCoordinate(QGeoCoordinate(20, 30));
    place3.setLocation(location);
    places << &place3;

    QPlace place4;
    place4.setName(QLatin1String("LM4"));
    location.setCoordinate(QGeoCoordinate(5,20));
    place4.setLocation(location);
    places << &place4;

    QPlace place5;
    place5.setName(QLatin1String("LM5"));
    location.setCoordinate(QGeoCoordinate(80,20));
    place5.setLocation(location);
    places << &place5;

    QPlace place6;
    place6.setName(QLatin1String("LM6"));
    location.setCoordinate(QGeoCoordinate(60,20));
    place6.setLocation(location);
    places << &place6;

    doSavePlaces(places);

    QPlaceSearchRequest searchRequest;
    QGeoBoundingCircle *circle = new QGeoBoundingCircle(QGeoCoordinate(20,20));
    searchRequest.setSearchArea(circle);
    searchRequest.setRelevanceHint(QPlaceSearchRequest::DistanceHint);
    QList<QPlace> results;
    QVERIFY(doSearch(searchRequest, &results));
    QCOMPARE(results.count(), 6);
    QCOMPARE(results.at(0), place1);
    QCOMPARE(results.at(1), place3);
    QCOMPARE(results.at(2), place4);
    QCOMPARE(results.at(3), place2);
    QCOMPARE(results.at(4), place6);
    QCOMPARE(results.at(5), place5);

    qreal radius = QGeoCoordinate(20,20).distanceTo(QGeoCoordinate(20,50));
    circle->setRadius(radius);

    QVERIFY(doSearch(searchRequest, &results));
    QCOMPARE(results.count(),4);
    QCOMPARE(results.at(0), place1);
    QCOMPARE(results.at(1), place3);
    QCOMPARE(results.at(2), place4);
    QCOMPARE(results.at(3), place2);

    //try radius less than 1
    circle->setRadius(-5);
    QVERIFY(doSearch(searchRequest, &results));
    QCOMPARE(results.count(), 6);
    QCOMPARE(results.at(0), place1);
    QCOMPARE(results.at(1), place3);
    QCOMPARE(results.at(2), place4);
    QCOMPARE(results.at(3), place2);
    QCOMPARE(results.at(4), place6);
    QCOMPARE(results.at(5), place5);

}

void tst_QPlaceManagerJsonDb::searchByCategory()
{
    QString categoryId;
    QPlaceCategory restaurant;
    restaurant.setName("Restaurant");
    QVERIFY(doSaveCategory(restaurant, QPlaceReply::NoError, &categoryId));
    restaurant.setCategoryId(categoryId);

    QString placeId;
    QPlace park;
    park.setName("Park");
    QVERIFY(doSavePlace(park, QPlaceReply::NoError, &placeId));
    park.setPlaceId(placeId);

    QList<QPlaceCategory> categories;
    categories << restaurant;

    QPlace krustyBurger;
    krustyBurger.setName("Krusty burger");
    krustyBurger.setCategories(categories);
    QVERIFY(doSavePlace(krustyBurger, QPlaceReply::NoError, &placeId));
    krustyBurger.setPlaceId(placeId);

    QList<QPlace> places;
    QPlaceSearchRequest request;
    request.setCategory(restaurant);
    QVERIFY(doSearch(request, &places));
    QCOMPARE(places.count(), 1);
    QCOMPARE(places.at(0), krustyBurger);
}

void tst_QPlaceManagerJsonDb::icons()
{
    QPlaceIcon icon;
    icon.setManager(placeManager);
    icon.setFullUrl(QUrl(QLatin1String("/icons/placeicon.jpg")));

    //check that we can get the url from the icon
    QCOMPARE(icon.url(), QUrl(QLatin1String("/icons/placeicon.jpg")));

    //check that we can save and retrieve a place with the icon
    //intact.
    QPlace place;
    place.setIcon(icon);
    QString placeId;
    QVERIFY(doSavePlace(place,QPlaceReply::NoError, &placeId));

    QPlace retrievedPlace;
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QCOMPARE(retrievedPlace.icon().url(),  QUrl(QLatin1String("/icons/placeicon.jpg")));

    QPlaceIcon categoryIcon;
    categoryIcon.setManager(placeManager);
    categoryIcon.setFullUrl(QUrl(QLatin1String("/icons/motel.jpg")));

    QPlaceCategory motel;
    motel.setIcon(categoryIcon);
    QString categoryId;
    QVERIFY(doSaveCategory(motel, "", QPlaceReply::NoError, &categoryId));
    motel.setCategoryId(categoryId);

    QList<QPlaceCategory> categories = placeManager->childCategories();
    QCOMPARE(categories.count(), 1);
    QCOMPARE(categories.at(0).icon().url(), QUrl(QLatin1String("/icons/motel.jpg")));

    //test modification of an icon of an existin category
    categoryIcon.setFullUrl(QUrl(QLatin1String("/icons/motel2.jpg")));
    motel.setIcon(categoryIcon);
    QVERIFY(doSaveCategory(motel, "", QPlaceReply::NoError, &categoryId));

    categories = placeManager->childCategories();
    QCOMPARE(categories.count(), 1);
    QCOMPARE(categories.at(0).icon().url(), QUrl(QLatin1String("/icons/motel2.jpg")));

    //try saving an empty icon to the category
    motel.setIcon(QPlaceIcon());
    QVERIFY(doSaveCategory(motel, "", QPlaceReply::NoError, &categoryId));
    categories = placeManager->childCategories();
    QCOMPARE(categories.count(), 1);
    QCOMPARE(categories.at(0).icon().url(), QUrl());
    QVERIFY(categories.at(0).icon().isEmpty());
}

void tst_QPlaceManagerJsonDb::unsupportedFunctions()
{
    QPlace place;
    place.setPlaceId("id");
    QPlaceContentRequest request;
    request.setContentType(QPlaceContent::ImageType);
    request.setLimit(5);
    request.setOffset(0);
    QPlaceContentReply *contentReply = placeManager->getPlaceContent(place.placeId(), request);
    QSignalSpy contentSpy(contentReply, SIGNAL(finished()));
    QTRY_VERIFY(contentSpy.count() == 1);
    QCOMPARE(contentReply->error(), QPlaceReply::UnsupportedError);

    QPlaceSearchRequest searchRequest;
    QPlaceSearchSuggestionReply *searchSuggestionReply = placeManager->searchSuggestions(searchRequest);
    QSignalSpy searchSuggestionSpy(searchSuggestionReply, SIGNAL(finished()));
    QTRY_VERIFY(searchSuggestionSpy.count() == 1 );
    QCOMPARE(searchSuggestionReply->error(), QPlaceReply::UnsupportedError);
}

void tst_QPlaceManagerJsonDb::supportedFeatures()
{
    QCOMPARE((placeManager->supportedFeatures() & QPlaceManager::SavePlaceFeature),
             QPlaceManager::SavePlaceFeature);
    QCOMPARE((placeManager->supportedFeatures() & QPlaceManager::RemovePlaceFeature),
            QPlaceManager::RemovePlaceFeature);
    QCOMPARE((placeManager->supportedFeatures() & QPlaceManager::SaveCategoryFeature),
             QPlaceManager::SaveCategoryFeature);
    QCOMPARE((placeManager->supportedFeatures() & QPlaceManager::RemoveCategoryFeature),
             QPlaceManager::RemoveCategoryFeature);

    QVERIFY((placeManager->supportedFeatures() & QPlaceManager::RecommendationsFeature) == 0);
    QVERIFY((placeManager->supportedFeatures() & QPlaceManager::SearchSuggestionsFeature) == 0);
    QVERIFY((placeManager->supportedFeatures() & QPlaceManager::CorrectionsFeature) == 0);
    QVERIFY((placeManager->supportedFeatures() & QPlaceManager::LocaleFeature) == 0);
}

void tst_QPlaceManagerJsonDb::categoryFunctions()
{
    QString categoryId;
    QPlaceCategory restaurant;
    restaurant.setName(QLatin1String("Restaurant"));
    QVERIFY(doSaveCategory(restaurant, QPlaceReply::NoError, &categoryId));
    restaurant.setCategoryId(categoryId);

    QPlaceCategory fastFood;
    fastFood.setName(QLatin1String("Fast Food"));
    QVERIFY(doSaveCategory(fastFood, restaurant.categoryId(), QPlaceReply::NoError, &categoryId));
    fastFood.setCategoryId(categoryId);

    QPlaceCategory fineDining;
    fineDining.setName(QLatin1String("Fine dining"));
    QVERIFY(doSaveCategory(fineDining, restaurant.categoryId(), QPlaceReply::NoError, &categoryId));
    fineDining.setCategoryId(categoryId);

    //try find a parent id
    QCOMPARE(placeManager->parentCategoryId(fineDining.categoryId()), restaurant.categoryId());

    //try find the parent id of a top level category
    QCOMPARE(placeManager->parentCategoryId(restaurant.categoryId()), QString());

    //try find the parent id of a non-existent category.
    QCOMPARE(placeManager->parentCategoryId(QLatin1String("does-not-exist")), QString());

    //try find the child ids
    QStringList childIds = placeManager->childrenCategoryIds(restaurant.categoryId());
    QVERIFY(childIds.contains(fastFood.categoryId()));
    QVERIFY(childIds.contains(fineDining.categoryId()));
    QCOMPARE(childIds.count(), 2);

    //try find the child id of a category without a child
    QCOMPARE(placeManager->childrenCategoryIds(fineDining.categoryId()), QStringList());

    //try to find child ids of a non-existent category
    QCOMPARE(placeManager->childrenCategoryIds(QLatin1String("does-not-exist")), QStringList());

    //try to find a category by it's id
    QCOMPARE(placeManager->category(fastFood.categoryId()), fastFood);

    //try to find a category with a non-existent id
    QCOMPARE(placeManager->category(QLatin1String("does-not-exist")), QPlaceCategory());
}

void tst_QPlaceManagerJsonDb::contactDetails()
{
    QFETCH(QString, contactType);
    QPlace place;
    //create a place with a single contact detail of a given type
    place.setName(QLatin1String("place"));
    QPlaceContactDetail detail;
    detail.setLabel(QLatin1String("detailLabel"));
    detail.setValue(QLatin1String("detail"));
    place.appendContactDetail(contactType, detail);

    QString placeId;
    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));
    place.setPlaceId(placeId);

    QPlace retrievedPlace;
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QVERIFY(retrievedPlace.contactTypes().contains(contactType));
    QCOMPARE(retrievedPlace.contactDetails(contactType).count(), 1);
    QCOMPARE(retrievedPlace.contactDetails(contactType).at(0), detail);

    //add multiple details of a given type;
    QPlaceContactDetail detail2;
    detail2.setLabel("detail2Label");
    detail2.setValue(QLatin1String("detail2"));
    QPlaceContactDetail detail3;
    detail3.setLabel("detail3Label");
    detail3.setValue(QLatin1String("detail3"));

    place.appendContactDetail(contactType, detail2);
    place.appendContactDetail(contactType, detail3);
    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QCOMPARE(retrievedPlace.contactDetails(contactType).count(), 3);
    QCOMPARE(retrievedPlace.contactDetails(contactType).at(0), detail);
    QCOMPARE(retrievedPlace.contactDetails(contactType).at(1), detail2);
    QCOMPARE(retrievedPlace.contactDetails(contactType).at(2), detail3);

    //try removing a detail
    QList<QPlaceContactDetail> details;
    details << detail <<detail3;
    place.setContactDetails(contactType, details);
    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QCOMPARE(retrievedPlace.contactDetails(contactType).count(), 2);
    QCOMPARE(retrievedPlace.contactDetails(contactType).at(0), detail);
    QCOMPARE(retrievedPlace.contactDetails(contactType).at(1), detail3);

    //try remove all details
    place.removeContactDetails(contactType);
    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QVERIFY(retrievedPlace.contactTypes().contains(contactType) == false);
    QCOMPARE(retrievedPlace.contactDetails(contactType).count(), 0);

    //try creating a place with multiple contact details of a given type.
    QPlace place2;
    place2.setContactDetails(contactType, details);
    QVERIFY(doSavePlace(place2, QPlaceReply::NoError, &placeId));
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QVERIFY(retrievedPlace.contactTypes().contains(contactType) == true);
    QCOMPARE(retrievedPlace.contactDetails(contactType).count(), 2);
    QCOMPARE(retrievedPlace.contactDetails(contactType).at(0), detail);
    QCOMPARE(retrievedPlace.contactDetails(contactType).at(1), detail3);
}

void tst_QPlaceManagerJsonDb::contactDetails_data()
{
    QTest::addColumn<QString>("contactType");

    QTest::newRow("phone") << QPlaceContactDetail::Phone.latin1();
    QTest::newRow("email") << QPlaceContactDetail::Email.latin1();
    QTest::newRow("website") << QPlaceContactDetail::Website.latin1();
    QTest::newRow("fax") << QPlaceContactDetail::Fax.latin1();
}

void tst_QPlaceManagerJsonDb::mulipleDetailTypes()
{
    //try saving a place with multiple detail types simultaneously.
    QPlace place;
    place.setName("Char");

    QPlaceContactDetail phone;
    phone.setLabel("phone1");
    phone.setValue("555-5555");
    QPlaceContactDetail phone2;
    phone2.setLabel("phone2");
    phone2.setValue("444-4444");
    QList<QPlaceContactDetail> phones;
    place.setContactDetails(QPlaceContactDetail::Phone, phones);

    QPlaceContactDetail fax;
    fax.setLabel("fax1");
    fax.setValue("999-9999");
    QPlaceContactDetail fax2;
    fax2.setLabel("fax2");
    fax2.setValue("999-9999");
    QList<QPlaceContactDetail> faxes;
    place.setContactDetails(QPlaceContactDetail::Fax, faxes);

    QPlaceContactDetail email;
    email.setValue("email@adddress.com");
    QPlaceContactDetail email2;
    email2.setValue("email2@adddress.com");
    place.appendContactDetail(QPlaceContactDetail::Email, email);
    place.appendContactDetail(QPlaceContactDetail::Email, email2);

    QPlaceContactDetail website;
    website.setLabel("website");
    website.setValue("www.example.com");
    QPlaceContactDetail website2;
    website2.setLabel("website2");
    website2.setValue("www.example2.com");
    place.appendContactDetail(QPlaceContactDetail::Website, website);
    place.appendContactDetail(QPlaceContactDetail::Website, website2);

    //Save a place
    QString placeId;
    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));

    //ensure we can retrieve it's details
    QPlace retrievedPlace;
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    place.setPlaceId(placeId);
    QVERIFY(retrievedPlace == place);

    //try adding some more, changing and removing details of different types
    //when updating a place.
    phone2.setValue("222-2222");  //modify detail
    phones.clear();
    phones << phone << phone2;
    place.setContactDetails(QPlaceContactDetail::Phone, phones);

    //remove an entire set of details
    place.removeContactDetails(QPlaceContactDetail::Fax);

    //add remove one detail
    QList<QPlaceContactDetail> emails;
    emails << email2;
    place.setContactDetails(QPlaceContactDetail::Email, emails);

    //add more of a detail
    QPlaceContactDetail website3;
    website3.setLabel("website3");
    website3.setValue("www.example3.com");
    place.appendContactDetail(QPlaceContactDetail::Website, website3);

    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QVERIFY(retrievedPlace == place);
}

void tst_QPlaceManagerJsonDb::placeNotifications()
{
    QSignalSpy createSpy(placeManager, SIGNAL(placeAdded(QString)));
    QSignalSpy updateSpy(placeManager, SIGNAL(placeUpdated(QString)));
    QSignalSpy removeSpy(placeManager, SIGNAL(placeRemoved(QString)));

    //create place
    QPlace place;
    place.setName("Char");
    QString placeId;
    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));
    place.setPlaceId(placeId);

    QTRY_VERIFY(createSpy.count() == 1);
    createSpy.clear();
    QVERIFY(updateSpy.count() == 0);
    QVERIFY(removeSpy.count() == 0);

    //modify place
    QGeoLocation location;
    location.setCoordinate(QGeoCoordinate(10,10));
    place.setLocation(location);
    QVERIFY(doSavePlace(place, QPlaceReply::NoError));
    QTRY_VERIFY(updateSpy.count() == 1);
    updateSpy.clear();
    QVERIFY(createSpy.count() == 0);
    QVERIFY(removeSpy.count() == 0);

    //remove place
    QVERIFY(doRemovePlace(place, QPlaceReply::NoError));
    QTRY_VERIFY(removeSpy.count() == 1);
    removeSpy.clear();
    QVERIFY(createSpy.count() == 0);
    QVERIFY(updateSpy.count() == 0);
}

void tst_QPlaceManagerJsonDb::categoryNotifications()
{
    QSignalSpy createSpy(placeManager, SIGNAL(categoryAdded(QPlaceCategory,QString)));
    QSignalSpy updateSpy(placeManager, SIGNAL(categoryUpdated(QPlaceCategory,QString)));
    QSignalSpy removeSpy(placeManager, SIGNAL(categoryRemoved(QString,QString)));

    //create category
    QString restaurantId;
    QPlaceCategory restaurant;
    restaurant.setName("Restaurant");
    QVERIFY(doSaveCategory(restaurant, QPlaceReply::NoError, &restaurantId));
    restaurant.setCategoryId(restaurantId);
    QTRY_VERIFY(createSpy.count() == 1);
    createSpy.clear();
    QVERIFY(updateSpy.count() == 0);
    QVERIFY(removeSpy.count() == 0);

    //modify category
    restaurant.setName("RESTAURANT");
    QVERIFY(doSaveCategory(restaurant));
    QTRY_VERIFY(updateSpy.count() == 1);
    updateSpy.clear();
    QVERIFY(createSpy.count() == 0);
    QVERIFY(removeSpy.count() == 0);

    QVERIFY(doRemoveCategory(restaurant));
    QTRY_VERIFY(removeSpy.count() == 1);
    removeSpy.clear();
    QVERIFY(createSpy.count() == 0);
    QVERIFY(updateSpy.count() == 0);

    restaurant.setCategoryId(QString());
    QVERIFY(doSaveCategory(restaurant, QPlaceReply::NoError, &restaurantId));
    QTRY_VERIFY(createSpy.count() == 1);
    createSpy.clear();
    QVERIFY(updateSpy.count() == 0);
    QVERIFY(removeSpy.count() == 0);

    //save a category as a child
    QString steakId;
    QPlaceCategory steak;
    steak.setName("Steak");

    QVERIFY(doSaveCategory(steak, restaurantId, QPlaceReply::NoError, &steakId));
    steak.setCategoryId(steakId);
    QTRY_VERIFY(createSpy.count() == 1);
    QVERIFY(createSpy.at(0).at(0).value<QPlaceCategory>() == steak);
    createSpy.clear();
    QVERIFY(updateSpy.count() == 0);
    QVERIFY(removeSpy.count() == 0);
 }

void tst_QPlaceManagerJsonDb::cleanup()
{
    QSignalSpy cleanSpy(dbCleaner, SIGNAL(dbCleaned()));
    dbCleaner->cleanDb();
    QTRY_VERIFY(cleanSpy.count() == 1);
}

bool tst_QPlaceManagerJsonDb::doSavePlace(const QPlace &place,
                                          QPlaceReply::Error expectedError,
                                          QString *placeId)
{
    QPlaceIdReply *saveReply = placeManager->savePlace(place);
    bool isSuccessful = checkSignals(saveReply, expectedError);
    if (placeId != 0) {
        *placeId = saveReply->id();
    }

    if (saveReply->id().isEmpty() && expectedError == QPlaceReply::NoError) {
        qWarning("ID is empty in reply for save operation");
        isSuccessful = false;
    }

    return isSuccessful;
}

void tst_QPlaceManagerJsonDb::doSavePlaces(QList<QPlace> &places)
{
    QPlaceIdReply *saveReply;

    foreach (QPlace place, places) {
        saveReply = placeManager->savePlace(place);
        QSignalSpy saveSpy(saveReply, SIGNAL(finished()));
        QTRY_VERIFY(saveSpy.count() == 1);
        QCOMPARE(saveReply->error(), QPlaceReply::NoError);
        saveSpy.clear();
    }
}

void tst_QPlaceManagerJsonDb::doSavePlaces(const QList<QPlace *> &places)
{
    QPlaceIdReply *saveReply;

    static int count= 0;
    foreach (QPlace *place, places) {
        count++;
        saveReply = placeManager->savePlace(*place);
        QSignalSpy saveSpy(saveReply, SIGNAL(finished()));
        QTRY_VERIFY(saveSpy.count() == 1);
        QCOMPARE(saveReply->error(), QPlaceReply::NoError);
        place->setPlaceId(saveReply->id());
        saveSpy.clear();
    }
}

bool tst_QPlaceManagerJsonDb::doRemovePlace(const QPlace &place,
                                            QPlaceReply::Error expectedError)
{
    QPlaceIdReply *removeReply = placeManager->removePlace(place.placeId());
    bool isSuccessful = false;
    isSuccessful = checkSignals(removeReply, expectedError)
                    && (removeReply->id() == place.placeId());
    return isSuccessful;
}

bool tst_QPlaceManagerJsonDb::doSearch(const QPlaceSearchRequest &request,
                                       QList<QPlaceSearchResult> *results, QPlaceReply::Error expectedError)
{
    QPlaceSearchReply *searchReply= placeManager->search(request);
    bool success = checkSignals(searchReply, expectedError);
    *results = searchReply->results();
    return success;
}

bool tst_QPlaceManagerJsonDb::doSearch(const QPlaceSearchRequest &request,
                                       QList<QPlace> *results, QPlaceReply::Error expectedError)
{
    bool success = false;
    results->clear();
    QList<QPlaceSearchResult> searchResults;
    success = doSearch(request, &searchResults, expectedError);
    foreach (const QPlaceSearchResult &searchResult, searchResults)
        results->append(searchResult.place());
    return success;
}

bool tst_QPlaceManagerJsonDb::doFetchDetails(QString placeId, QPlace *place, QPlaceReply::Error expectedError)
{
    QPlaceDetailsReply *detailsReply = placeManager->getPlaceDetails(placeId);
    bool success = checkSignals(detailsReply, expectedError);
    *place = detailsReply->place();
    return success;
}

bool tst_QPlaceManagerJsonDb::doSaveCategory(const QPlaceCategory &category,
                                          QPlaceReply::Error expectedError,
                                          QString *categoryId)
{
    QPlaceIdReply *saveReply = placeManager->saveCategory(category);
    bool isSuccessful = false;
    isSuccessful = checkSignals(saveReply, expectedError);
    if (categoryId != 0)
        *categoryId = saveReply->id();
    return isSuccessful;
}

bool tst_QPlaceManagerJsonDb::doSaveCategory(const QPlaceCategory &category,
                                             const QString &parentId,
                                          QPlaceReply::Error expectedError,
                                          QString *categoryId)
{
    QPlaceIdReply *idReply = placeManager->saveCategory(category, parentId);
    bool isSuccessful = checkSignals(idReply, expectedError)
                        && (idReply->error() == expectedError);

    if (categoryId != 0)
        *categoryId = idReply->id();
    return isSuccessful;
}

bool tst_QPlaceManagerJsonDb::doRemoveCategory(const QPlaceCategory &category,
                                          QPlaceReply::Error expectedError)
{
    QPlaceIdReply *idReply = placeManager->removeCategory(category.categoryId());

    bool isSuccessful = checkSignals(idReply, expectedError) &&
                        (idReply->error() == expectedError);
    return isSuccessful;
}

bool tst_QPlaceManagerJsonDb::checkSignals(QPlaceReply *reply, QPlaceReply::Error expectedError)
{
    QSignalSpy finishedSpy(reply, SIGNAL(finished()));
    QSignalSpy errorSpy(reply, SIGNAL(error(QPlaceReply::Error,QString)));
    QSignalSpy managerFinishedSpy(placeManager, SIGNAL(finished(QPlaceReply*)));
    QSignalSpy managerErrorSpy(placeManager,SIGNAL(error(QPlaceReply*,QPlaceReply::Error,QString)));

    if (expectedError != QPlaceReply::NoError) {
        //check that we get an error signal from the reply
        WAIT_UNTIL(errorSpy.count() == 1);
        if (errorSpy.count() != 1) {
            qWarning() << "Error signal for search operation not received";
            return false;
        }

        //check that we get the correct error from the reply's signal
        QPlaceReply::Error actualError = qvariant_cast<QPlaceReply::Error>(errorSpy.at(0).at(0));
        if (actualError != expectedError) {
            qWarning() << "Actual error code in reply signal does not match expected error code";
            qWarning() << "Actual error code = " << actualError;
            qWarning() << "Expected error coe =" << expectedError;
            return false;
        }

        //check that we get an error  signal from the manager
        WAIT_UNTIL(managerErrorSpy.count() == 1);
        if (managerErrorSpy.count() !=1) {
           qWarning() << "Error signal from manager for search operation not received";
           return false;
        }

        //check that we get the correct reply instance in the error signal from the manager
        if (qvariant_cast<QPlaceReply*>(managerErrorSpy.at(0).at(0)) != reply)  {
            qWarning() << "Reply instance in error signal from manager is incorrect";
            return false;
        }

        //check that we get the correct error from the signal of the manager
        actualError = qvariant_cast<QPlaceReply::Error>(managerErrorSpy.at(0).at(1));
        if (actualError != expectedError) {
            qWarning() << "Actual error code from manager signal does not match expected error code";
            qWarning() << "Actual error code =" << actualError;
            qWarning() << "Expected error code = " << expectedError;
            return false;
        }
    }

    //check that we get a finished signal
    WAIT_UNTIL(finishedSpy.count() == 1);
    if (finishedSpy.count() !=1) {
        qWarning() << "Finished signal from reply not received";
        return false;
    }

    if (reply->error() != expectedError) {
        qWarning() << "Actual error code does not match expected error code";
        qWarning() << "Actual error code: " << reply->error();
        qWarning() << "Expected error code" << expectedError;
        return false;
    }

    if (expectedError == QPlaceReply::NoError && !reply->errorString().isEmpty()) {
        qWarning() << "Expected error was no error but error string was not empty";
        qWarning() << "Error string=" << reply->errorString();
        return false;
    }

    //check that we get the finished signal from the manager
    WAIT_UNTIL(managerFinishedSpy.count() == 1);
    if (managerFinishedSpy.count() != 1) {
        qWarning() << "Finished signal from manager not received";
        return false;
    }

    //check that the reply instance in the finished signal from the manager is correct
    if (qvariant_cast<QPlaceReply *>(managerFinishedSpy.at(0).at(0)) != reply) {
        qWarning() << "Reply instance in finished signal from manager is incorrect";
        return false;
    }

    return true;
}

bool tst_QPlaceManagerJsonDb::compareResults(const QList<QPlaceSearchResult> &results,
                                             const QList<QPlace> &expectedResults)
{
    QSet<QString> actualPlaceCoords;
    foreach (const QPlaceSearchResult &result, results)
        actualPlaceCoords.insert(result.place().location().coordinate().toString());
    QSet<QString> expectedPlaceCoords;
    foreach (const QPlace &place, expectedResults) {
        expectedPlaceCoords.insert(place.location().coordinate().toString());
    }
    return actualPlaceCoords == expectedPlaceCoords;
}

QTEST_APPLESS_MAIN(tst_QPlaceManagerJsonDb)

#include "tst_qplacemanager_jsondb.moc"
