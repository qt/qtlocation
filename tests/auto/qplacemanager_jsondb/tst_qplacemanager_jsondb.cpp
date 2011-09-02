#include <QCoreApplication>
#include <QString>
#include <QtTest/QtTest>

#include <qgeoboundingbox.h>
#include <qgeoboundingcircle.h>
#include <qgeoserviceprovider.h>
#include <qplacemanager.h>
#include <qplacecontentreply.h>
#include <qplacetextpredictionreply.h>
#include <qplacesearchrequest.h>
#include <qplacesearchresult.h>
#include <qplacesearchreply.h>

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

    void saveAndRemove();
    void updatePlace();
    void searchByName();
    void searchByBox();
    void searchByCircle();
    void unsupportedFunctions();

private:
    bool doSavePlace(const QGeoPlace &place,
                QPlaceReply::Error expectedError = QPlaceReply::NoError,
                QString *placeId = 0,
                QPlaceManager::VisibilityScope = QPlaceManager::NoScope);
    void doSavePlaces(QList<QGeoPlace> &places);

    bool doRemovePlace(const QGeoPlace &place,
                       QPlaceReply::Error expectedError = QPlaceReply::NoError);

    bool doSearch(const QPlaceSearchRequest &request,
                  QList<QPlaceSearchResult> *results,
             QPlaceReply::Error expectedError = QPlaceReply::NoError);

    bool doSearch(const QPlaceSearchRequest &request,
                  QList<QGeoPlace> *results,
             QPlaceReply::Error expectedError = QPlaceReply::NoError);

    bool doFetchDetails(QString placeId,
                        QGeoPlace *place,
                        QPlaceReply::Error expectedError = QPlaceReply::NoError);

    bool checkSignals(QPlaceReply *reply, QPlaceReply::Error expectedError);


    bool compareResults(const QList<QPlaceSearchResult> &results, const QList<QGeoPlace> &expectedResults);

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

    QVERIFY(placeManager->connectivityMode() == QPlaceManager::OfflineMode);

    cleanup();
}

void tst_QPlaceManagerJsonDb::saveAndRemove()
{
    QGeoPlace place;
    place.setName("Char");
    QGeoLocation location;

    location.setCoordinate(QGeoCoordinate(10,20,30));

    QGeoAddress address;
    address.setStreet("1 test street");
    address.setCity("New york");
    address.setCountry("USA");
    location.setAddress(address);

    place.setLocation(location);

    //Save a place
    QString placeId;
    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));

    //ensure we can retrieve it's details
    QGeoPlace retrievedPlace;
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    place.setPlaceId(placeId);
    QVERIFY(retrievedPlace == place);

    //try removing a place
    QVERIFY(doRemovePlace(place, QPlaceReply::NoError));

    //ensure it is actually deleted
    QVERIFY(doFetchDetails(placeId, &retrievedPlace, QPlaceReply::PlaceDoesNotExistError));
    QCOMPARE(retrievedPlace, QGeoPlace());

    //try removing a place that does not exist;
    QVERIFY(doRemovePlace(place, QPlaceReply::PlaceDoesNotExistError));

    QVERIFY(doSavePlace(place, QPlaceReply::UnsupportedError,0, QPlaceManager::PublicScope));
}

void tst_QPlaceManagerJsonDb::updatePlace()
{
    //save a place and check that we can retrieve it
    QGeoPlace place;
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

    QGeoPlace retrievedPlace;
    QVERIFY(doSavePlace(place, QPlaceReply::NoError));
    QVERIFY(doFetchDetails(place.placeId(), &retrievedPlace));
    QCOMPARE(retrievedPlace.name(), QLatin1String("Brisbane"));
    QCOMPARE(retrievedPlace.location().address().street(), QLatin1String("new street"));
    QCOMPARE(retrievedPlace.location().address().country(), QLatin1String("Australia"));

    //try updating a non-existent place
    place.setPlaceId("Non-existent id");
    QVERIFY(doSavePlace(place, QPlaceReply::PlaceDoesNotExistError));
}

void tst_QPlaceManagerJsonDb::searchByName()
{
    QGeoPlace adelaide, adel, ad, brisbane;
    adelaide.setName("Adelaide");
    adel.setName("adel");
    ad.setName("ad");
    brisbane.setName("brisbane");

    QList<QGeoPlace> places;
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

    QList<QGeoPlace> places;
    for (int i = 0; i < coords.size(); ++i) {
        QGeoPlace place;
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
    QList<QGeoPlace> places1;
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
    QList<QGeoPlace> places2;
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
    QList<QGeoPlace> places3;
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
    QList<QGeoPlace> places4;
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
    QList<QGeoPlace> places5;
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
            QGeoPlace place;
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
    QList<QGeoPlace> places;
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
}

void tst_QPlaceManagerJsonDb::unsupportedFunctions()
{
    QGeoPlace place;
    place.setPlaceId("id");
    QPlaceContentRequest request;
    request.setContentType(QPlaceContent::ImageType);
    request.setLimit(5);
    request.setOffset(0);
    QPlaceContentReply *contentReply = placeManager->getContent(place, request);
    QSignalSpy contentSpy(contentReply, SIGNAL(finished()));
    QTRY_VERIFY(contentSpy.count() == 1);
    QCOMPARE(contentReply->error(), QPlaceReply::UnsupportedError);

    QPlaceSearchRequest searchRequest;
    QPlaceTextPredictionReply *textPredictionReply = placeManager->textPredictions(searchRequest);
    QSignalSpy textPredictionSpy(textPredictionReply, SIGNAL(finished()));
    QTRY_VERIFY(textPredictionSpy.count() == 1 );
    QCOMPARE(textPredictionReply->error(), QPlaceReply::UnsupportedError);
}


void tst_QPlaceManagerJsonDb::cleanup()
{
    QSignalSpy cleanSpy(dbCleaner, SIGNAL(dbCleaned()));
    dbCleaner->cleanDb();
    QTRY_VERIFY(cleanSpy.count() == 1);
}

bool tst_QPlaceManagerJsonDb::doSavePlace(const QGeoPlace &place,
                                          QPlaceReply::Error expectedError,
                                          QString *placeId,
                                          QPlaceManager::VisibilityScope scope)
{
    QPlaceIdReply *saveReply = placeManager->savePlace(place,scope);
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

void tst_QPlaceManagerJsonDb::doSavePlaces(QList<QGeoPlace> &places)
{
    QPlaceIdReply *saveReply;

    foreach (QGeoPlace place, places) {
        saveReply = placeManager->savePlace(place);
        QSignalSpy saveSpy(saveReply, SIGNAL(finished()));
        QTRY_VERIFY(saveSpy.count() == 1);
        QCOMPARE(saveReply->error(), QPlaceReply::NoError);
        saveSpy.clear();
    }
}

bool tst_QPlaceManagerJsonDb::doRemovePlace(const QGeoPlace &place,
                                            QPlaceReply::Error expectedError)
{
    QPlaceIdReply *removeReply = placeManager->removePlace(place);
    bool isSuccessful = false;
    isSuccessful = checkSignals(removeReply, expectedError)
                    && (removeReply->id() == place.placeId());
    return isSuccessful;
}

bool tst_QPlaceManagerJsonDb::doSearch(const QPlaceSearchRequest &request,
                                       QList<QPlaceSearchResult> *results, QPlaceReply::Error expectedError)
{
    QPlaceSearchReply *searchReply= placeManager->searchForPlaces(request);
    bool success = checkSignals(searchReply, expectedError);
    *results = searchReply->results();
    return success;
}

bool tst_QPlaceManagerJsonDb::doSearch(const QPlaceSearchRequest &request,
                                       QList<QGeoPlace> *results, QPlaceReply::Error expectedError)
{
    bool success = false;
    results->clear();
    QList<QPlaceSearchResult> searchResults;
    success = doSearch(request, &searchResults, expectedError);
    foreach (const QPlaceSearchResult &searchResult, searchResults)
        results->append(searchResult.place());
    return success;
}

bool tst_QPlaceManagerJsonDb::doFetchDetails(QString placeId, QGeoPlace *place, QPlaceReply::Error expectedError)
{
    QPlaceDetailsReply *detailsReply = placeManager->getPlaceDetails(placeId);
    bool success = checkSignals(detailsReply, expectedError);
    *place = detailsReply->result();
    return success;
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
                                             const QList<QGeoPlace> &expectedResults)
{
    QSet<QString> actualPlaceCoords;
    foreach (const QPlaceSearchResult &result, results)
        actualPlaceCoords.insert(result.place().location().coordinate().toString());
    QSet<QString> expectedPlaceCoords;
    foreach (const QGeoPlace &place, expectedResults) {
        expectedPlaceCoords.insert(place.location().coordinate().toString());
    }
    return actualPlaceCoords == expectedPlaceCoords;
}

QTEST_APPLESS_MAIN(tst_QPlaceManagerJsonDb)

#include "tst_qplacemanager_jsondb.moc"
