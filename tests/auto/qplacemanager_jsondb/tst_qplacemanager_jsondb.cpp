/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/QCoreApplication>
#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/qnumeric.h>
#include <QtGui/QImageReader>
#include <QtJsonDb/QJsonDbCreateRequest>
#include <QtJsonDb/QJsonDbReadRequest>
#include <QtLocation/QGeoCircle>
#include <QtLocation/QGeoRectangle>

#include <qgeoserviceprovider.h>
#include <qplacemanager.h>
#include <qplacecategory.h>
#include <qplacecontentreply.h>
#include <qplacematchreply.h>
#include <qplaceresult.h>
#include <qplacesearchsuggestionreply.h>
#include <qplacesearchrequest.h>
#include <qplacesearchresult.h>
#include <qplacesearchreply.h>
#include <qplacecontactdetail.h>
#include <qplaceimage.h>
#include <qplacesupplier.h>

#include "jsondbutils.h"
#include "../placemanager_utils/placemanager_utils.h"

Q_DECLARE_METATYPE(QPlaceIdReply *)
Q_DECLARE_METATYPE(QJsonObject)

QT_USE_NAMESPACE

class tst_QPlaceManagerJsonDb : public PlaceManagerUtils
{
    Q_OBJECT
    enum UrlCorrectnes {
        ValidUrl,
        NoSchemeUrl
    };

    enum DestinationExists {
        Exists,
        DoesNotExist
    };

public:
    tst_QPlaceManagerJsonDb();

private Q_SLOTS:
    void initTestCase();
    void cleanup();

    void saveAndRemovePlace();
    void simpleUpdatePlace();
    void updatePlace();
    void updatePlace_data();
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
    void unsupportedFunctions();
    void supportedFeatures();
    void categoryFunctions();
    void contactDetails();
    void contactDetails_data();
    void mulipleDetailTypes();
    void placeNotifications();
    void categoryNotifications();
    void compatiblePlace();
    void extendedAttribute();
    void matchingPlaces();
    void matchingPlaces_data();
    void iconSourceDestination();
    void iconSourceDestination_data();
    void iconSourceOnly();
    void iconSourceOnly_data();
    void iconDestinationOnly();
    void iconDestinationOnly_data();
    void iconSavedFromDifferentManager();
    void iconFormats();
    void iconFormats_data();
    void iconUrls();
    void iconUrls_data();
#ifdef REMOTE_TEST
    void iconDownload();
    void iconDownload_data();
#endif
    void constructIconUrl();
    void providerIcons();
    void specifiedPartition();
    void validateIndexes();

private:
    QImage dataUrlToImage(const QUrl &url);

    static const QString SmallSource;
    static const QString SmallDestination;
    static const QString SmallDestinationSize;
    static const QString MediumSource;
    static const QString MediumDestination;
    static const QString MediumDestinationSize;
    static const QString LargeSource;
    static const QString LargeDestination;
    static const QString LargeDestinationSize;
    static const QString FullscreenSource;
    static const QString FullscreenDestination;
    static const QString FullscreenDestinationSize;

    static const QString Nokiaicon;
    static const QString IconPrefix;
    static const QString NokiaIconGenerated;

    static const QSize SmallSize;
    static const QSize MediumSize;
    static const QSize LargeSize;
    static const QSize FullscreenSize;
    static const QString Provider;

    static const QString CustomIcons;

    QGeoServiceProvider *provider;
    QCoreApplication *coreApp;
    JsonDbUtils *dbUtils ;
    QTemporaryDir tempDir;
};

//These constants are equivalent to those from the jsondb plugin icon class
const QString tst_QPlaceManagerJsonDb::SmallSource("smallSourceUrl");
const QString tst_QPlaceManagerJsonDb::SmallDestination("smallUrl");
const QString tst_QPlaceManagerJsonDb::SmallDestinationSize("smallSize");

const QString tst_QPlaceManagerJsonDb::MediumSource("mediumSourceUrl");
const QString tst_QPlaceManagerJsonDb::MediumDestination("mediumUrl");
const QString tst_QPlaceManagerJsonDb::MediumDestinationSize("mediumSize");

const QString tst_QPlaceManagerJsonDb::LargeSource("largeSourceUrl");
const QString tst_QPlaceManagerJsonDb::LargeDestination("largeUrl");
const QString tst_QPlaceManagerJsonDb::LargeDestinationSize("largeSize");

const QString tst_QPlaceManagerJsonDb::FullscreenSource("fullscreenSourceUrl");
const QString tst_QPlaceManagerJsonDb::FullscreenDestination("fullscreenUrl");
const QString tst_QPlaceManagerJsonDb::FullscreenDestinationSize("fullscreenSize");

const QString tst_QPlaceManagerJsonDb::Nokiaicon("nokiaIcon");
const QString tst_QPlaceManagerJsonDb::IconPrefix("iconPrefix");
const QString tst_QPlaceManagerJsonDb::NokiaIconGenerated("nokiaIconGenerated");

const QSize tst_QPlaceManagerJsonDb::SmallSize = QSize(20,20);
const QSize tst_QPlaceManagerJsonDb::MediumSize = QSize(30,30);
const QSize tst_QPlaceManagerJsonDb::LargeSize = QSize(50, 50);
const QSize tst_QPlaceManagerJsonDb::FullscreenSize = QSize(320, 480);

//This constant is key for the place attribute that signifies which provider
//a place originates from
const QString tst_QPlaceManagerJsonDb::Provider("x_provider");

const QString tst_QPlaceManagerJsonDb::CustomIcons("places.icons.custom");

tst_QPlaceManagerJsonDb::tst_QPlaceManagerJsonDb()
{
    //We need a QCoreApplication instance for
    //event dispatching
    int argc;
    char **argv=0;
    coreApp = new QCoreApplication(argc, argv);

    dbUtils = new JsonDbUtils(this);
}

void tst_QPlaceManagerJsonDb::initTestCase()
{
    QVERIFY(dbUtils->hasJsonDbConnection());

    qRegisterMetaType<QPlaceIdReply *>();
    qRegisterMetaType<QJsonObject>();

    QStringList providers = QGeoServiceProvider::availableServiceProviders();
    QVERIFY(providers.contains(QLatin1String("places_jsondb")));

    //create dummy icon file
    QString catIconPath = tempDir.path() + "/icons/categories/";
    QDir::root().mkpath(catIconPath);
    QFile iconFile(catIconPath + "01.icon");
    iconFile.open(QIODevice::WriteOnly);
    iconFile.close();

    iconFile.setFileName(catIconPath + "02.icon");
    iconFile.open(QIODevice::WriteOnly);
    iconFile.close();

    iconFile.setFileName(catIconPath + "06.icon");//default icon
    iconFile.open(QIODevice::WriteOnly);
    iconFile.close();

    QString offlinePath = tempDir.path() + QStringLiteral("/offline/");
    QDir::root().mkpath(offlinePath);
    QFile mappingFile(QStringLiteral(":/resources/offline-mapping.json"));
    mappingFile.copy(offlinePath + QStringLiteral("offline-mapping.json"));

    provider = new QGeoServiceProvider(QLatin1String("places_jsondb"));

    placeManager = provider->placeManager();
    QVERIFY(placeManager);

    QPlaceReply * catInitReply = placeManager->initializeCategories();
    QVERIFY(checkSignals(catInitReply, QPlaceReply::NoError));
    catInitReply->deleteLater();

    cleanup();
}

void tst_QPlaceManagerJsonDb::saveAndRemovePlace()
{
    QPlace place;
    place.setName(QLatin1String("Char"));
    QGeoLocation location;

    location.setCoordinate(QGeoCoordinate(10,20));

    QGeoAddress address;
    address.setStreet(QLatin1String("1 test street"));
    address.setCity(QLatin1String("New york"));
    address.setCountry(QLatin1String("USA"));
    location.setAddress(address);
    place.setLocation(location);

    QPlaceContactDetail phone;
    phone.setValue(QLatin1String("555-5555"));
    place.appendContactDetail(QPlaceContactDetail::Phone, phone);

    QPlaceContactDetail fax;
    fax.setValue(QLatin1String("999-9999"));
    place.appendContactDetail(QPlaceContactDetail::Fax, fax);

    QPlaceContactDetail email;
    email.setValue(QLatin1String("email@adddresss.com"));
    place.appendContactDetail(QPlaceContactDetail::Email, email);

    QPlaceContactDetail website;
    website.setValue(QLatin1String("www.example.com"));
    place.appendContactDetail(QPlaceContactDetail::Website, website);

    //Save a place
    QString placeId;
    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));
    place.setPlaceId(placeId);
    place.setVisibility(QLocation::DeviceVisibility);

    //ensure we can retrieve it's details
    QPlace retrievedPlace;
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QVERIFY(retrievedPlace == place);

    //try removing a place
    QVERIFY(doRemovePlace(place, QPlaceReply::NoError));

    //ensure it is actually deleted
    QVERIFY(doFetchDetails(placeId, &retrievedPlace, QPlaceReply::PlaceDoesNotExistError));
    QCOMPARE(retrievedPlace, QPlace());

    //try removing a place that does not exist;
    QVERIFY(doRemovePlace(place, QPlaceReply::PlaceDoesNotExistError));

    place.setVisibility(QLocation::PublicVisibility);
    QVERIFY(doSavePlace(place, QPlaceReply::UnsupportedError, 0));

    place.setVisibility(QLocation::PrivateVisibility);
    QVERIFY(doSavePlace(place, QPlaceReply::UnsupportedError, 0));
}

void tst_QPlaceManagerJsonDb::simpleUpdatePlace()
{
    //save a place and check that we can retrieve it
    QPlace place;
    place.setName(QLatin1String("Sydney"));
    QGeoAddress address;
    address.setStreet(QLatin1String("original street"));
    QGeoLocation location;
    location.setAddress(address);
    place.setLocation(location);

    QString placeId;
    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));

    //check that created and modified date time has been set
    QDateTime currentDateTime = QDateTime::currentDateTime();
    dbUtils->fetchPlaceJson(placeId);
    QSignalSpy spy(dbUtils, SIGNAL(placeFetched(QJsonObject)));
    QTRY_VERIFY_WITH_TIMEOUT(spy.count() == 1, Timeout);
    QJsonObject placeJson = spy.at(0).at(0).value<QJsonObject>();
    spy.clear();

    QVERIFY(placeJson.contains(JsonDbUtils::CreatedDateTime));
    QVERIFY(placeJson.contains(JsonDbUtils::ModifiedDateTime));
    QDateTime createdDateTime = placeJson.value(JsonDbUtils::CreatedDateTime).toVariant().toDateTime();
    QDateTime modifiedDateTime = placeJson.value(JsonDbUtils::ModifiedDateTime).toVariant().toDateTime();
    QCOMPARE(createdDateTime, modifiedDateTime);
    QVERIFY(createdDateTime.secsTo(currentDateTime) < 10);
    QTest::qWait(1100); //stabilize test by waiting before updating to ensure modification
                        //time will change.  The DateTime is stored in ISO format which only
                        //has a resolution of seconds

    QVERIFY(doFetchDetails(placeId, &place));

    //update the place again with some changes
    place.setName(QLatin1String("Brisbane"));
    address.setStreet(QLatin1String("new street"));
    address.setCountry(QLatin1String("Australia"));
    location.setAddress(address);
    place.setLocation(location);

    QPlace retrievedPlace;
    QVERIFY(doSavePlace(place, QPlaceReply::NoError));

    //check that the created datetime is the same, while the modified date time has changed.
    currentDateTime = QDateTime::currentDateTime();
    dbUtils->fetchPlaceJson(placeId);
    QTRY_VERIFY_WITH_TIMEOUT(spy.count() == 1, Timeout);
    placeJson = spy.at(0).at(0).value<QJsonObject>();
    spy.clear();

    QVERIFY(placeJson.contains(JsonDbUtils::CreatedDateTime));
    QVERIFY(placeJson.contains(JsonDbUtils::ModifiedDateTime));
    QCOMPARE(placeJson.value(JsonDbUtils::CreatedDateTime).toVariant().toDateTime(), createdDateTime);
    QVERIFY(placeJson.value(JsonDbUtils::ModifiedDateTime).toVariant().toDateTime() != modifiedDateTime);
    QVERIFY(placeJson.value(JsonDbUtils::ModifiedDateTime).toVariant().toDateTime().secsTo(currentDateTime) < 10);

    QVERIFY(doFetchDetails(place.placeId(), &retrievedPlace));
    QCOMPARE(retrievedPlace.name(), QLatin1String("Brisbane"));
    QCOMPARE(retrievedPlace.location().address().street(), QLatin1String("new street"));
    QCOMPARE(retrievedPlace.location().address().country(), QLatin1String("Australia"));

    //try updating a non-existent place
    place.setPlaceId("Non-existent id");
    QVERIFY(doSavePlace(place, QPlaceReply::PlaceDoesNotExistError));
}

void tst_QPlaceManagerJsonDb::updatePlace()
{
    const QUrl IconUrl(QLatin1String("file://opt/myicon.png"));
    const QSize IconSize(20,20);

    QPlaceAttribute smoking;
    smoking.setLabel(QLatin1String("Smoking"));
    smoking.setText(QLatin1String("yes"));

    QPlaceAttribute noSmoking;
    noSmoking.setLabel(QLatin1String("Smoking"));
    noSmoking.setText(QLatin1String("no"));

    QPlaceAttribute bus;
    bus.setLabel(QLatin1String("Bus stop"));
    bus.setText(QLatin1String("yes"));

    QPlaceContactDetail contactDetail;
    contactDetail.setLabel(QLatin1String("contact label"));
    contactDetail.setValue(QLatin1String("contact value"));

    QFETCH(QString, field);
    qDebug(); //this simply provides visual output on what is being tested

    QString categoryId;
    QPlaceCategory pizza;
    pizza.setName(QLatin1String("pizza"));
    QVERIFY(doSaveCategory(pizza, QPlaceReply::NoError, &categoryId));
    pizza.setCategoryId(categoryId);

    QPlaceCategory burgers;
    burgers.setName(QLatin1String("burgers"));
    QVERIFY(doSaveCategory(burgers, QPlaceReply::NoError, &categoryId));
    burgers.setCategoryId(categoryId);

    //save minimal place
    QPlace place;
    place.setName(QLatin1String("place1"));

    QGeoAddress address;
    address.setCountryCode(QLatin1String("USA"));

    QGeoLocation location;
    location.setCoordinate(QGeoCoordinate(10,10));
    location.setAddress(address);
    place.setLocation(location);

    QPlaceIcon icon;
    QVariantMap iconParameters;
    iconParameters.insert(QLatin1String("fullscreenUrl"), QUrl("file://opt/icon.png"));
    iconParameters.insert(QLatin1String("fullscreenSize"), QSize(320, 480));

    place.setVisibility(QLocation::DeviceVisibility);
    QString placeId;
    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));
    place.setPlaceId(placeId);

    dbUtils->fetchPlaceJson(placeId);
    QSignalSpy spy(dbUtils, SIGNAL(placeFetched(QJsonObject)));
    QTRY_VERIFY_WITH_TIMEOUT(spy.count() == 1, Timeout);
    QJsonObject placeJson = spy.at(0).at(0).value<QJsonObject>();
    spy.clear();

    //we need to verify that other properties are not overwritten
    placeJson.insert(QLatin1String("description"), QLatin1String("simpson residence"));
    QJsonObject locationJson = placeJson.value(JsonDbUtils::Location).toObject();
    locationJson.insert(QLatin1String("launchUrl"), QLatin1String("http://www.example.com"));
    placeJson.insert(JsonDbUtils::Location, locationJson);

    dbUtils->savePlaceJson(placeJson);
    QSignalSpy saveSpy(dbUtils, SIGNAL(placeSaved()));
    QTRY_VERIFY_WITH_TIMEOUT(saveSpy.count() == 1, Timeout);

    //modify all fields
    if (field == QLatin1String("category")) {
        place.setCategory(pizza);
    } else if (field == QLatin1String("coordinate")) {
        location.setCoordinate(QGeoCoordinate(20,20));
        place.setLocation(location);
    } else if (field == QLatin1String("no coord with address")) {
        location.setCoordinate(QGeoCoordinate());
        address.setStreet(QLatin1String("42 Evergreen Terrace"));
        location.setAddress(address);
        place.setLocation(location);
    } else if (field == QLatin1String("street")) {
        address.setStreet(QLatin1String("42 Evergreen Terrace"));
        location.setAddress(address);
        place.setLocation(location);
    } else if (field == QLatin1String("district")) {
        address.setDistrict(QLatin1String("Henderson"));;
        location.setAddress(address);
        place.setLocation(location);
    } else if (field == QLatin1String("city")) {
        address.setCity("Springfield");
        location.setAddress(address);
        place.setLocation(location);
    } else if (field == QLatin1String("county")) {
        address.setCounty(QLatin1String("Lane County"));
        location.setAddress(address);
        place.setLocation(location);
    } else if (field == QLatin1String("state")) {
        address.setState(QLatin1String("Oregon"));
        location.setAddress(address);
        place.setLocation(location);
    } else if (field == QLatin1String("country")) {
        address.setCountry(QLatin1String("United States of America"));
        location.setAddress(address);
        place.setLocation(location);
    } else if (field == QLatin1String("country code")) {
        address.setCountryCode("AUS");
        location.setAddress(address);
        place.setLocation(location);
    } else if (field == QLatin1String("clear address")) {
        location.setAddress(QGeoAddress());
        place.setLocation(location);
    } else if (field == QLatin1String("small icon")) {
        iconParameters.insert(JsonDbUtils::SmallIconParam, IconUrl);
        iconParameters.insert(JsonDbUtils::SmallIconSizeParam, IconSize);
        icon.setParameters(iconParameters);
        place.setIcon(icon);
    } else if (field == QLatin1String("medium icon")) {
        iconParameters.insert(JsonDbUtils::MediumIconParam, IconUrl);
        iconParameters.insert(JsonDbUtils::MediumIconSizeParam, IconSize);
        icon.setParameters(iconParameters);
        place.setIcon(icon);
    } else if (field == QLatin1String("large icon")) {
        iconParameters.insert(JsonDbUtils::LargeIconParam, IconUrl);
        iconParameters.insert(JsonDbUtils::LargeIconSizeParam, IconSize);
        icon.setParameters(iconParameters);
        place.setIcon(icon);
    } else if (field == QLatin1String("fullscreen icon")) {
        iconParameters.insert(JsonDbUtils::FullscreenIconParam, IconUrl);
        iconParameters.insert(JsonDbUtils::FullscreenIconSizeParam, IconSize);
        icon.setParameters(iconParameters);
        place.setIcon(icon);
    }  else if (field == QLatin1String("name")) {
        place.setName(QLatin1String("home"));
    } else if (field == QLatin1String("extended attribute") || field == QLatin1String("clear extended attributes")) {
        place.setExtendedAttribute("smoking", smoking);
        place.setExtendedAttribute("bus", bus);
    } else if (field == QLatin1String("phone")) {
        place.appendContactDetail(QPlaceContactDetail::Phone, contactDetail);
    } else if (field == QLatin1String("fax")) {
        place.appendContactDetail(QPlaceContactDetail::Fax, contactDetail);
    } else if (field == QLatin1String("email")) {
        place.appendContactDetail(QPlaceContactDetail::Email, contactDetail);
    } else if (field == QLatin1String("website")) {
        place.appendContactDetail(QPlaceContactDetail::Website, contactDetail);
    }

    QVERIFY(doSavePlace(place, QPlaceReply::NoError));
    QPlace retrievedPlace;
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));

    dbUtils->fetchPlaceJson(placeId);
    QTRY_VERIFY_WITH_TIMEOUT(spy.count() == 1, Timeout);
    placeJson = spy.at(0).at(0).value<QJsonObject>();
    spy.clear();

    locationJson = placeJson.value(JsonDbUtils::Location).toObject();
    QJsonObject addressJson = locationJson.value(JsonDbUtils::Address).toObject();
    QJsonObject thumbnailsJson = placeJson.value(JsonDbUtils::Thumbnails).toObject();
    QJsonObject attributesJson = placeJson.value(JsonDbUtils::ExtendedAttributes).toObject();

    QCOMPARE(placeJson.value(QLatin1String("description")).toString(), QLatin1String("simpson residence"));
    QCOMPARE(locationJson.value(QLatin1String("launchUrl")).toString(), QLatin1String("http://www.example.com"));

    if (field == QLatin1String("category")) {
        QCOMPARE(retrievedPlace.categories().count(), 1);
        QVERIFY(retrievedPlace.categories().contains(pizza));
        QVERIFY(placeJson.contains(JsonDbUtils::CategoryUuids));

        place.setCategories(QList<QPlaceCategory>());
    } else if (field == QLatin1String("coordinate")) {        iconParameters.insert(JsonDbUtils::MediumIconParam, IconUrl);
        iconParameters.insert(JsonDbUtils::MediumIconSizeParam, IconSize);
        icon.setParameters(iconParameters);
        place.setIcon(icon);
        QCOMPARE(retrievedPlace.location().coordinate(), QGeoCoordinate(20,20));
    } else if (field == QLatin1String("no coord with address")) {
        QCOMPARE(retrievedPlace.location().coordinate(), QGeoCoordinate());
        QCOMPARE(retrievedPlace.location().address().street(), QLatin1String("42 Evergreen Terrace"));
        QVERIFY(!locationJson.contains(JsonDbUtils::Coordinate));
        QVERIFY(locationJson.contains(JsonDbUtils::Address));
    } else if (field == QLatin1String("street")) {
        QCOMPARE(retrievedPlace.location().address().street(), QLatin1String("42 Evergreen Terrace"));
        QCOMPARE(addressJson.keys().count(), 2);
        QVERIFY(addressJson.keys().contains(JsonDbUtils::Street));

        address.setStreet(QString());
        location.setAddress(address);
        place.setLocation(location);
    } else if (field == QLatin1String("district")) {
        QCOMPARE(retrievedPlace.location().address().district(), QLatin1String("Henderson"));
        QCOMPARE(addressJson.keys().count(), 2);
        QVERIFY(addressJson.keys().contains(JsonDbUtils::District));

        address.setDistrict(QString());
        location.setAddress(address);
        place.setLocation(location);
    } else if (field == QLatin1String("city")) {
        QCOMPARE(retrievedPlace.location().address().city(), QLatin1String("Springfield"));
        QCOMPARE(addressJson.keys().count(), 2);
        QVERIFY(addressJson.keys().contains(JsonDbUtils::City));

        address.setCity(QString());
        location.setAddress(address);
        place.setLocation(location);
    } else if (field == QLatin1String("county")) {
        QCOMPARE(retrievedPlace.location().address().county(), QLatin1String("Lane County"));
        QCOMPARE(addressJson.keys().count(), 2);
        QVERIFY(addressJson.keys().contains(JsonDbUtils::County));

        address.setCounty(QString());
        location.setAddress(address);
        place.setLocation(location);
    } else if (field == QLatin1String("state")) {
        QCOMPARE(retrievedPlace.location().address().state(), QLatin1String("Oregon"));
        QCOMPARE(addressJson.keys().count(), 2);
        QVERIFY(addressJson.keys().contains(JsonDbUtils::State));

        address.setState(QString());
        location.setAddress(address);
        place.setLocation(location);
    } else if (field == QLatin1String("country")) {
        QCOMPARE(retrievedPlace.location().address().country(), QLatin1String("United States of America"));
        QCOMPARE(addressJson.keys().count(), 2);
        QVERIFY(addressJson.keys().contains(JsonDbUtils::Country));

        address.setCountry(QString());
        location.setAddress(address);
        place.setLocation(location);
    } else if (field == QLatin1String("country code")) {
        QCOMPARE(retrievedPlace.location().address().countryCode(), QLatin1String("AUS"));
        QCOMPARE(addressJson.keys().count(), 1);
        QVERIFY(addressJson.keys().contains(JsonDbUtils::CountryCode));

        address.setStreet(QLatin1String("742 Evergreen Tce"));//this is assigned so that the entire address object
                                               //is not removed in jsondb
        address.setCountryCode(QString());
        location.setAddress(address);
        place.setLocation(location);
    }  else if (field == QLatin1String("clear address")) {
        QVERIFY(retrievedPlace.location().address().isEmpty());
        QVERIFY(!locationJson.keys().contains(JsonDbUtils::Address));
    } else if (field == QLatin1String("small icon")) {
        QCOMPARE(retrievedPlace.icon().parameters().value(JsonDbUtils::SmallIconParam).toUrl(), IconUrl);
        QCOMPARE(retrievedPlace.icon().parameters().value(JsonDbUtils::SmallIconSizeParam).toSize(), IconSize);

        iconParameters.remove(JsonDbUtils::SmallIconParam);
        iconParameters.remove(JsonDbUtils::SmallIconSizeParam);
        icon.setParameters(iconParameters);
        place.setIcon(icon);
    } else if (field == QLatin1String("medium icon")) {
        QCOMPARE(retrievedPlace.icon().parameters().value(JsonDbUtils::MediumIconParam).toUrl(), IconUrl);
        QCOMPARE(retrievedPlace.icon().parameters().value(JsonDbUtils::MediumIconSizeParam).toSize(), IconSize);

        iconParameters.remove(JsonDbUtils::MediumIconParam);
        iconParameters.remove(JsonDbUtils::MediumIconSizeParam);
        icon.setParameters(iconParameters);
        place.setIcon(icon);
    } else if (field == QLatin1String("large icon")) {
        QCOMPARE(retrievedPlace.icon().parameters().value(JsonDbUtils::LargeIconParam).toUrl(), IconUrl);
        QCOMPARE(retrievedPlace.icon().parameters().value(JsonDbUtils::LargeIconSizeParam).toSize(), IconSize);

        iconParameters.remove(JsonDbUtils::LargeIconParam);
        iconParameters.remove(JsonDbUtils::LargeIconSizeParam);
        icon.setParameters(iconParameters);
        place.setIcon(icon);
    } else if (field == QLatin1String("fullscreen icon")) {
        QCOMPARE(retrievedPlace.icon().parameters().value(JsonDbUtils::FullscreenIconParam).toUrl(), IconUrl);
        QCOMPARE(retrievedPlace.icon().parameters().value(JsonDbUtils::FullscreenIconSizeParam).toSize(), IconSize);

        iconParameters.remove(JsonDbUtils::FullscreenIconParam);
        iconParameters.remove(JsonDbUtils::FullscreenIconSizeParam);

        iconParameters.insert(JsonDbUtils::SmallIconParam, IconUrl);      //this is assigned so the entire
        iconParameters.insert(JsonDbUtils::SmallIconSizeParam, IconSize);  //thumbnails jsondb object is not removed.

        icon.setParameters(iconParameters);
        place.setIcon(icon);
    } else if (field == QLatin1String("name")) {
        QCOMPARE(retrievedPlace.name(), QLatin1String("home"));
    } else if (field == QLatin1String("extended attribute")) {
        QCOMPARE(retrievedPlace.extendedAttributeTypes().count(), 2);
        QCOMPARE(retrievedPlace.extendedAttribute(QLatin1String("smoking")), smoking);
        QCOMPARE(retrievedPlace.extendedAttribute("bus"), bus);

        QVERIFY(attributesJson.keys().contains(QLatin1String("smoking")));
        QVERIFY(attributesJson.keys().contains(QLatin1String("bus")));

        place.setExtendedAttribute(QLatin1String("smoking"), noSmoking);
    } else if (field == QLatin1String("clear extended attributes")) {
        QCOMPARE(retrievedPlace.extendedAttributeTypes().count(), 2);
        QCOMPARE(retrievedPlace.extendedAttribute(QLatin1String("smoking")), smoking);
        QCOMPARE(retrievedPlace.extendedAttribute(QLatin1String("bus")), bus);

        QVERIFY(attributesJson.keys().contains(QLatin1String("smoking")));
        QVERIFY(attributesJson.keys().contains(QLatin1String("bus")));

        place.removeExtendedAttribute(QLatin1String("smoking"));
        place.removeExtendedAttribute(QLatin1String("bus"));
    } else if (field == QLatin1String("phone")) {
        QVERIFY(place.contactTypes().contains(QPlaceContactDetail::Phone));
        QCOMPARE(place.contactDetails(QPlaceContactDetail::Phone).first(), contactDetail);
        QVERIFY(placeJson.contains(JsonDbUtils::Phones));

        place.removeContactDetails(QPlaceContactDetail::Phone);
    } else if (field == QLatin1String("fax")) {
        QVERIFY(place.contactTypes().contains(QPlaceContactDetail::Fax));
        QCOMPARE(place.contactDetails(QPlaceContactDetail::Fax).first(), contactDetail);
        QVERIFY(placeJson.contains(JsonDbUtils::Phones));

        place.removeContactDetails(QPlaceContactDetail::Fax);
    } else if (field == QLatin1String("email")) {
        QVERIFY(place.contactTypes().contains(QPlaceContactDetail::Email));
        QCOMPARE(place.contactDetails(QPlaceContactDetail::Email).first(), contactDetail);
        QVERIFY(placeJson.contains(JsonDbUtils::Emails));

        place.removeContactDetails(QPlaceContactDetail::Email);
    } else if (field == QLatin1String("website")) {
        QVERIFY(place.contactTypes().contains(QPlaceContactDetail::Website));
        QCOMPARE(place.contactDetails(QPlaceContactDetail::Website).first(), contactDetail);
        QVERIFY(placeJson.contains(JsonDbUtils::Websites));

        place.removeContactDetails(QPlaceContactDetail::Website);
    } else {
        QFAIL("Unknown test case");
    }

    QVERIFY(doSavePlace(place, QPlaceReply::NoError));

    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    dbUtils->fetchPlaceJson(placeId);
    QTRY_VERIFY_WITH_TIMEOUT(spy.count() == 1, Timeout);
    placeJson = spy.at(0).at(0).value<QJsonObject>();
    locationJson = placeJson.value(JsonDbUtils::Location).toObject();
    addressJson = locationJson.value(JsonDbUtils::Address).toObject();
    thumbnailsJson = placeJson.value(JsonDbUtils::Thumbnails).toObject();
    attributesJson = placeJson.value(JsonDbUtils::ExtendedAttributes).toObject();
    spy.clear();

    QCOMPARE(placeJson.value(QLatin1String("description")).toString(), QLatin1String("simpson residence"));
    QCOMPARE(locationJson.value(QLatin1String("launchUrl")).toString(), QLatin1String("http://www.example.com"));

    if (field == QLatin1String("category")) {
        QCOMPARE(retrievedPlace.categories().count(), 0);
        QVERIFY(!placeJson.contains(JsonDbUtils::CategoryUuids));
    } else if (field == QLatin1String("street")) {
        QCOMPARE(retrievedPlace.location().address().street(), QString());
        QVERIFY(!addressJson.contains(JsonDbUtils::Street));
    } else if (field == QLatin1String("district")) {
        QCOMPARE(retrievedPlace.location().address().district(), QString());
        QVERIFY(!addressJson.contains(JsonDbUtils::District));
    } else if (field == QLatin1String("city")) {
        QCOMPARE(retrievedPlace.location().address().city(), QString());
        QVERIFY(!addressJson.contains(JsonDbUtils::City));
    } else if (field == QLatin1String("county")) {
        QCOMPARE(retrievedPlace.location().address().county(), QString());
        QVERIFY(!addressJson.contains(JsonDbUtils::County));
    } else if (field == QLatin1String("state")) {
        QCOMPARE(retrievedPlace.location().address().state(), QString());
        QVERIFY(!addressJson.contains(JsonDbUtils::State));
    } else if (field == QLatin1String("country")) {
        QCOMPARE(retrievedPlace.location().address().country(), QString());
        QVERIFY(!addressJson.contains(JsonDbUtils::Country));
    } else if (field == QLatin1String("country code")) {
        QCOMPARE(retrievedPlace.location().address().countryCode(), QString());
        QVERIFY(!addressJson.contains(JsonDbUtils::CountryCode));
    } else if (field == QLatin1String("small icon")) {
        QVERIFY(!retrievedPlace.icon().parameters().contains(JsonDbUtils::SmallIconParam));
        QVERIFY(!retrievedPlace.icon().parameters().contains(JsonDbUtils::SmallIconSizeParam));
        QVERIFY(!thumbnailsJson.contains(JsonDbUtils::Small));
    } else if (field == QLatin1String("medium icon")) {
        QVERIFY(!retrievedPlace.icon().parameters().contains(JsonDbUtils::MediumIconParam));
        QVERIFY(!retrievedPlace.icon().parameters().contains(JsonDbUtils::MediumIconSizeParam));
        QVERIFY(!thumbnailsJson.contains(JsonDbUtils::Medium));
    } else if (field == QLatin1String("large icon")) {
        QVERIFY(!retrievedPlace.icon().parameters().contains(JsonDbUtils::LargeIconParam));
        QVERIFY(!retrievedPlace.icon().parameters().contains(JsonDbUtils::LargeIconSizeParam));
        QVERIFY(!thumbnailsJson.contains(JsonDbUtils::Large));
    } else if (field == QLatin1String("fullscreen icon")) {
        QVERIFY(!retrievedPlace.icon().parameters().contains(JsonDbUtils::FullscreenIconParam));
        QVERIFY(!retrievedPlace.icon().parameters().contains(JsonDbUtils::FullscreenIconParam));
        QVERIFY(!thumbnailsJson.contains(JsonDbUtils::Fullscreen));
    } else if (field == QLatin1String("extended attribute")) {
        QCOMPARE(retrievedPlace.extendedAttribute("smoking"), noSmoking);
        QCOMPARE(retrievedPlace.extendedAttribute("bus"), bus);

        QVERIFY(attributesJson.keys().contains(QLatin1String("smoking")));
        QVERIFY(attributesJson.keys().contains(QLatin1String("bus")));
    } else if (field == QLatin1String("clear extended attributes")) {
        QVERIFY(retrievedPlace.extendedAttributeTypes().isEmpty());
        QVERIFY(!placeJson.keys().contains(JsonDbUtils::ExtendedAttributes));
    } else if (field == QLatin1String("phone")) {
        QVERIFY(retrievedPlace.contactTypes().isEmpty());
        QVERIFY(!placeJson.contains(JsonDbUtils::Phones));
    } else if (field == QLatin1String("fax")) {
        QVERIFY(retrievedPlace.contactTypes().isEmpty());
        QVERIFY(!placeJson.contains(JsonDbUtils::Phones));
    } else if (field == QLatin1String("email")) {
        QVERIFY(retrievedPlace.contactTypes().isEmpty());
        QVERIFY(!placeJson.contains(JsonDbUtils::Emails));
    } else if (field == QLatin1String("website")) {
        QVERIFY(retrievedPlace.contactTypes().isEmpty());
        QVERIFY(!placeJson.contains(JsonDbUtils::Websites));
    } else if (field == QLatin1String("coordinate") || field == QLatin1String("no coord with address")
               || field == QLatin1String("clear address") || field == QLatin1String("name")) {
        //no further tests required for these cases
    } else {
        QFAIL("Unknown test case");
    }
}

void tst_QPlaceManagerJsonDb::updatePlace_data()
{
    QTest::addColumn<QString>("field");

    QTest::newRow("category") << "category";
    QTest::newRow("coordinate") << "coordinate";
    QTest::newRow("no coord with address") << "no coord with address";
    QTest::newRow("street") << "street";
    QTest::newRow("district") << "district";
    QTest::newRow("city") << "city";
    QTest::newRow("county") << "county";
    QTest::newRow("state") << "state";
    QTest::newRow("country") << "country";
    QTest::newRow("country code") << "country code";
    QTest::newRow("clear address") << "clear address";
    QTest::newRow("small icon") << "small icon";
    QTest::newRow("medium icon") << "medium icon";
    QTest::newRow("large icon") << "large icon";
    QTest::newRow("fullscreen icon") << "fullscreen icon";
    QTest::newRow("name") << "name";
    QTest::newRow("extended attribute") << "extended attribute"; //initially set two attributes, modify then remove one, verify other is unchanged
    QTest::newRow("clear extended attributes") << "clear extended attributes"; //initially set two attributes clear them,
                                                                            //ensure attribute property does not exist in jsondb
    QTest::newRow("phone") << "phone";
    QTest::newRow("fax") << "fax";
    QTest::newRow("email") << "email";
    QTest::newRow("website") << "website";
}

void tst_QPlaceManagerJsonDb::simpleSaveAndRemoveCategory()
{
    QString categoryId;
    QPlaceCategory restaurant;
    restaurant.setName(QLatin1String("Restaurant"));

    //check saving of category
    QVERIFY(doSaveCategory(restaurant, QPlaceReply::NoError, &categoryId));
    restaurant.setCategoryId(categoryId);
    QPlaceReply * catInitReply = placeManager->initializeCategories();
    QVERIFY(checkSignals(catInitReply, QPlaceReply::NoError));

    QList<QPlaceCategory> categories = placeManager->childCategories();
    QCOMPARE(categories.count(), 1);

    QPlaceCategory retrievedCategory = placeManager->category(categoryId);
    QCOMPARE(retrievedCategory, restaurant);

    //check removal of category
    QVERIFY(doRemoveCategory(restaurant));
    catInitReply = placeManager->initializeCategories();
    QVERIFY(checkSignals(catInitReply, QPlaceReply::NoError));

    QCOMPARE(placeManager->category(categoryId), QPlaceCategory());
    categories = placeManager->childCategories();
    QCOMPARE(categories.count(), 0);
}

void tst_QPlaceManagerJsonDb::saveAndRemoveCategory()
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

    QPlaceCategory pancakes;
    pancakes.setName(QLatin1String("Pancakes"));
    QVERIFY(doSaveCategory(pancakes, restaurant.categoryId(), QPlaceReply::NoError, &categoryId));
    pancakes.setCategoryId(categoryId);

    QPlaceCategory pizza;
    pizza.setName(QLatin1String("Pizza"));
    QVERIFY(doSaveCategory(pizza, fastFood.categoryId(), QPlaceReply::NoError, &categoryId));
    pizza.setCategoryId(categoryId);

    QPlaceCategory burgers;
    burgers.setName(QLatin1String("burgers"));
    QVERIFY(doSaveCategory(burgers, fastFood.categoryId(), QPlaceReply::NoError, &categoryId));
    burgers.setCategoryId(categoryId);

    QPlaceCategory accommodation;
    accommodation.setName(QLatin1String("Accommodation"));
    QVERIFY(doSaveCategory(accommodation, QPlaceReply::NoError,&categoryId));
    accommodation.setCategoryId(categoryId);

    QPlaceCategory hotel;
    hotel.setName(QLatin1String("Hotel"));
    QVERIFY(doSaveCategory(hotel, accommodation.categoryId(), QPlaceReply::NoError, &categoryId));
    hotel.setCategoryId(categoryId);

    QPlaceCategory motel;
    motel.setName(QLatin1String("Motel"));
    QVERIFY(doSaveCategory(motel, accommodation.categoryId(), QPlaceReply::NoError, &categoryId));
    motel.setCategoryId(categoryId);
    QPlaceReply *catInitReply = placeManager->initializeCategories();
    QVERIFY(checkSignals(catInitReply, QPlaceReply::NoError));

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
    catInitReply = placeManager->initializeCategories();
    QVERIFY(checkSignals(catInitReply, QPlaceReply::NoError));

    categories = placeManager->childCategories(restaurant.categoryId());
    QVERIFY(categories.contains(fineDining));
    QVERIFY(categories.contains(pancakes));
    QCOMPARE(categories.count(), 2);

    QVERIFY(placeManager->category(pizza.categoryId()) == QPlaceCategory());
    QVERIFY(placeManager->category(burgers.categoryId()) == QPlaceCategory());

    QVERIFY(doRemoveCategory(accommodation));
    catInitReply = placeManager->initializeCategories();
    QVERIFY(checkSignals(catInitReply, QPlaceReply::NoError));

    categories = placeManager->childCategories();
    QVERIFY(categories.contains(restaurant));
    QCOMPARE(categories.count(), 1);
}

void tst_QPlaceManagerJsonDb::updateCategory()
{
    //Test updating a category name
    QPlaceCategory category;
    category.setName(QLatin1String("Foood"));
    QString categoryId;
    QVERIFY(doSaveCategory(category, QPlaceReply::NoError, &categoryId));
    category.setCategoryId(categoryId);
    QPlaceReply *catInitReply = placeManager->initializeCategories();
    QVERIFY(checkSignals(catInitReply, QPlaceReply::NoError));

    QList<QPlaceCategory> categories;
    categories = placeManager->childCategories();
    QVERIFY(categories.contains(category));

    category.setName(QLatin1String("Food"));
    QVERIFY(doSaveCategory(category, QPlaceReply::NoError, &categoryId));
    catInitReply = placeManager->initializeCategories();
    QVERIFY(checkSignals(catInitReply, QPlaceReply::NoError));

    categories = placeManager->childCategories();
    QVERIFY(categories.contains(category));
    QCOMPARE(categories.count(), 1);

    //Test updating a category's parent
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

    QPlaceCategory pancakes;
    pancakes.setName(QLatin1String("Pancakes"));
    QVERIFY(doSaveCategory(pancakes, restaurant.categoryId(), QPlaceReply::NoError, &categoryId));
    pancakes.setCategoryId(categoryId);

    QPlaceCategory pizza;
    pizza.setName(QLatin1String("Pizza"));
    QVERIFY(doSaveCategory(pizza, fastFood.categoryId(), QPlaceReply::NoError, &categoryId));
    pizza.setCategoryId(categoryId);

    QPlaceCategory burgers;
    burgers.setName(QLatin1String("burgers"));
    QVERIFY(doSaveCategory(burgers, fastFood.categoryId(), QPlaceReply::NoError, &categoryId));
    burgers.setCategoryId(categoryId);

    //resave pizza as a child of fine dining
    QVERIFY(doSaveCategory(pizza, fineDining.categoryId()));
    catInitReply = placeManager->initializeCategories();
    QVERIFY(checkSignals(catInitReply, QPlaceReply::NoError));

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
    restaurant.setName(QLatin1String("Restaurant"));
    QVERIFY(doSaveCategory(restaurant, QPlaceReply::NoError, &categoryId));
    restaurant.setCategoryId(categoryId);

    QString placeId;
    QPlace monolithBurgers;
    monolithBurgers.setName(QLatin1String("Monolith burgers"));
    monolithBurgers.setCategory(restaurant);
    QVERIFY(doSavePlace(monolithBurgers,QPlaceReply::NoError, &placeId));
    monolithBurgers.setPlaceId(placeId);

    QPlace retrievedPlace;
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QCOMPARE(retrievedPlace.categories().count(), 1);
    QCOMPARE(retrievedPlace.categories().first(), restaurant);

    //update place with multiple categories
    QPlaceCategory facilities;
    facilities.setName(QLatin1String("facilities"));
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
    QPlace adelaide, adel, ad, brisbane, bradel;
    adelaide.setName(QLatin1String("Adelaide"));
    adel.setName(QLatin1String("adel"));
    ad.setName(QLatin1String("ad"));
    brisbane.setName(QLatin1String("brisbane"));
    bradel.setName(QLatin1String("bradel"));

    QList<QPlace *> places;
    places << &adelaide << &adel << &ad << &brisbane << &bradel;
    setVisibility(places, QLocation::DeviceVisibility);
    doSavePlaces(places);

    //test that search has exhibits substring behaviour
    //and is case insensitive
    QPlaceSearchRequest request;
    request.setSearchTerm(QLatin1String("adel"));
    QList<QPlace> results;
    QVERIFY(doSearch(request, &results));
    QList<QPlace> expectedPlaces;
    expectedPlaces << adelaide << adel << bradel;
    QVERIFY(PlaceManagerUtils::compare(results, expectedPlaces));

    //Search for a non-exisent place
    request.setSearchTerm(QLatin1String("Nowhere"));
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
    request.setSearchArea(QGeoRectangle(QGeoCoordinate(5.0, -5.0), QGeoCoordinate(-5.0, 5.0)));

    QList<QPlace> places1;
    doSearch(request, &places1);
    QCOMPARE(places1.size(), inBox1.size());

    QSet<QString> testSet1;
    for (int i = 0; i < places1.size(); ++i)
        testSet1.insert(places1.at(i).location().coordinate().toString());

    QSet<QString> inBoxSet1;
    for (int i = 0; i < inBox1.size(); ++i)
        inBoxSet1.insert(inBox1.at(i).toString());

    QCOMPARE(testSet1, inBoxSet1);

    request.setSearchArea(QGeoRectangle(QGeoCoordinate(5.0, 10.0), QGeoCoordinate(-5.0, 20.0)));
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

    request.setSearchArea(QGeoRectangle(QGeoCoordinate(20.0, -5.0), QGeoCoordinate(10.0, 5.0)));
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

    request.setSearchArea(QGeoRectangle(QGeoCoordinate(20.0, 10.0), QGeoCoordinate(10.0, 20.0)));
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

    request.setSearchArea(QGeoRectangle(QGeoCoordinate(5.0, 175.0), QGeoCoordinate(-5.0, -175.0)));
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
    request.setSearchArea(QGeoRectangle(QGeoCoordinate(-70,-70), QGeoCoordinate(-80,-60)));
    QVERIFY(doSearch(request, &places));
    QCOMPARE(places.count(), 0);

    //--- Test error conditions
    //bottom right latitude > top left latitude
    request.setSearchArea(QGeoRectangle(QGeoCoordinate(20,20), QGeoCoordinate(50,30)));
    QVERIFY(doSearch(request,&places, QPlaceReply::BadArgumentError));

    //try an invalid coordinate for one of the corners
    request.setSearchArea(QGeoRectangle(QGeoCoordinate(qQNaN(),20),QGeoCoordinate(10,30)));
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

    for (int i = 0; i < testSets.size(); ++i) {
        QList<QGeoCoordinate> filterCoords = testSets.at(i).first;
        QList<QGeoCoordinate> plCoords = testSets.at(i).second;

        QPlaceSearchRequest request;
        for (int j = 0; j < filterCoords.size(); ++j) {
            QList<QPlace> places;
            request.setSearchArea(QGeoCircle(filterCoords.at(j),dist));

            if (i ==2 || i ==3)
                continue; //TODO: Testing poles, ignore for now
            else
                doSearch(request, &places);

            if (places.size() != plCoords.size()) {
                for (int k = 0; k < places.size(); ++k)
                    qWarning() << QLatin1String("pl") << places.at(k).location().coordinate().toString(QGeoCoordinate::Degrees);
                for (int k = 0; k < plCoords.size(); ++k)
                    qWarning() << QLatin1String("plCoords") << plCoords.at(k).toString(QGeoCoordinate::Degrees);
            }

            QCOMPARE(places.size(), plCoords.size());

            for (int k = 0; k < places.size(); ++k) {
                QVERIFY(plCoords.contains(places.at(k).location().coordinate()));
            }
        }
    }

    QList<QPlace> places;
    //--- Test error conditions and edge cases
    //try a circle that covers the north pole
    QPlaceSearchRequest request;
    request.setSearchArea(QGeoCircle(QGeoCoordinate(89.91,0),11000));
    QVERIFY(doSearch(request,&places, QPlaceReply::BadArgumentError));
    QCOMPARE(places.count(), 0);

    //try a circle that's close to but does not cover the north pole
    request.setSearchArea(QGeoCircle(QGeoCoordinate(89.91,0),9000));
    QVERIFY(doSearch(request,&places, QPlaceReply::NoError));

    //try a circle that covers the south pole
    request.setSearchArea(QGeoCircle(QGeoCoordinate(-89.91,180),11000));
    QVERIFY(doSearch(request, &places, QPlaceReply::BadArgumentError));
    QCOMPARE(places.count(), 0);

    //try a circle that's close to but does not cover the south pole
    request.setSearchArea(QGeoCircle(QGeoCoordinate(-89.91,180),9000));
    QVERIFY(doSearch(request, &places, QPlaceReply::NoError));

    //try a radius of 0
    request.setSearchArea(QGeoCircle(QGeoCoordinate(11.0,11.0), 0));
    QVERIFY(doSearch(request, &places));
    QCOMPARE(places.count(), 1);
    QCOMPARE(places.at(0).location().coordinate().latitude(), 11.0);
    QCOMPARE(places.at(0).location().coordinate().longitude(), 11.0);

    //try an invalid center
    request.setSearchArea(QGeoCircle(QGeoCoordinate(), 5000));
    QVERIFY(doSearch(request, &places, QPlaceReply::BadArgumentError));
    QVERIFY(places.isEmpty());

    //try an invalid latitude for the center
    request.setSearchArea(QGeoCircle(QGeoCoordinate(qQNaN(),50), 5000));
    QVERIFY(doSearch(request, &places, QPlaceReply::BadArgumentError));
    QVERIFY(places.isEmpty());

    //try a proximity filter with an out of range latitude
    request.setSearchArea(QGeoCircle(QGeoCoordinate(90,10), 5000));
    QVERIFY(doSearch(request, &places, QPlaceReply::BadArgumentError));
    QVERIFY(places.isEmpty());

    request.setSearchArea(QGeoCircle(QGeoCoordinate(150,10), 5000));
    QVERIFY(doSearch(request, &places, QPlaceReply::BadArgumentError));
    QVERIFY(places.isEmpty());

    //try a proximity filter with an out of range longitude
    request.setSearchArea(QGeoCircle(QGeoCoordinate(-12,180.1),5000));
    QVERIFY(doSearch(request, &places, QPlaceReply::BadArgumentError));
    QVERIFY(places.isEmpty());

    request.setSearchArea(QGeoCircle(QGeoCoordinate(-12,-180.1),5000));
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
    setVisibility(places, QLocation::DeviceVisibility);

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
    setVisibility(places, QLocation::DeviceVisibility);

    QPlaceSearchRequest searchRequest;
    QGeoCircle circle(QGeoCoordinate(20,20));
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
    circle.setRadius(radius);
    searchRequest.setSearchArea(circle);
    QVERIFY(doSearch(searchRequest, &results));

    QCOMPARE(results.count(),4);
    QCOMPARE(results.at(0), place1);
    QCOMPARE(results.at(1), place3);
    QCOMPARE(results.at(2), place4);
    QCOMPARE(results.at(3), place2);

    //try radius less than 1
    circle.setRadius(-5);
    searchRequest.setSearchArea(circle);
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
    restaurant.setName(QLatin1String("Restaurant"));
    QVERIFY(doSaveCategory(restaurant, QPlaceReply::NoError, &categoryId));
    restaurant.setCategoryId(categoryId);

    QString placeId;
    QPlace park;
    park.setName(QLatin1String("Park"));
    QVERIFY(doSavePlace(park, QPlaceReply::NoError, &placeId));
    park.setPlaceId(placeId);

    QList<QPlaceCategory> categories;
    categories << restaurant;

    QPlace krustyBurger;
    krustyBurger.setName(QLatin1String("Krusty burger"));
    krustyBurger.setCategories(categories);
    QVERIFY(doSavePlace(krustyBurger, QPlaceReply::NoError, &placeId));
    krustyBurger.setPlaceId(placeId);
    krustyBurger.setVisibility(QLocation::DeviceVisibility);

    QList<QPlace> places;
    QPlaceSearchRequest request;
    request.setCategory(restaurant);
    QVERIFY(doSearch(request, &places));
    QCOMPARE(places.count(), 1);
    QCOMPARE(places.at(0), krustyBurger);
}

void tst_QPlaceManagerJsonDb::unsupportedFunctions()
{
    QPlace place;
    place.setPlaceId(QLatin1String("id"));
    QPlaceContentRequest request;
    request.setContentType(QPlaceContent::ImageType);
    request.setLimit(5);
    request.setOffset(0);
    QPlaceContentReply *contentReply = placeManager->getPlaceContent(place.placeId(), request);
    QSignalSpy contentSpy(contentReply, SIGNAL(finished()));
    QTRY_VERIFY_WITH_TIMEOUT(contentSpy.count() == 1, Timeout);
    QCOMPARE(contentReply->error(), QPlaceReply::UnsupportedError);

    QPlaceSearchRequest searchRequest;
    QPlaceSearchSuggestionReply *searchSuggestionReply = placeManager->searchSuggestions(searchRequest);
    QSignalSpy searchSuggestionSpy(searchSuggestionReply, SIGNAL(finished()));
    QTRY_VERIFY_WITH_TIMEOUT(searchSuggestionSpy.count() == 1, Timeout);
    QCOMPARE(searchSuggestionReply->error(), QPlaceReply::UnsupportedError);
}

void tst_QPlaceManagerJsonDb::supportedFeatures()
{
    QGeoServiceProvider::PlacesFeatures f = QGeoServiceProvider::NoPlacesFeatures;

    f |= QGeoServiceProvider::OfflinePlacesFeature;
    f |= QGeoServiceProvider::SavePlaceFeature;
    f |= QGeoServiceProvider::RemovePlaceFeature;
    f |= QGeoServiceProvider::SaveCategoryFeature;
    f |= QGeoServiceProvider::RemoveCategoryFeature;
    f |= QGeoServiceProvider::PlaceMatchingFeature;

    QCOMPARE(provider->placesFeatures(), f);
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
    QPlaceReply *reply = placeManager->initializeCategories();
    QVERIFY(checkSignals(reply, QPlaceReply::NoError));

    //try find a parent id
    QCOMPARE(placeManager->parentCategoryId(fineDining.categoryId()), restaurant.categoryId());

    //try find the parent id of a top level category
    QCOMPARE(placeManager->parentCategoryId(restaurant.categoryId()), QString());

    //try find the parent id of a non-existent category.
    QCOMPARE(placeManager->parentCategoryId(QLatin1String("does-not-exist")), QString());

    //try find the child ids
    QStringList childIds = placeManager->childCategoryIds(restaurant.categoryId());
    QVERIFY(childIds.contains(fastFood.categoryId()));
    QVERIFY(childIds.contains(fineDining.categoryId()));
    QCOMPARE(childIds.count(), 2);

    //try find the child id of a category without a child
    QCOMPARE(placeManager->childCategoryIds(fineDining.categoryId()), QStringList());

    //try to find child ids of a non-existent category
    QCOMPARE(placeManager->childCategoryIds(QLatin1String("does-not-exist")), QStringList());

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
    detail2.setLabel(QLatin1String("detail2Label"));
    detail2.setValue(QLatin1String("detail2"));
    QPlaceContactDetail detail3;
    detail3.setLabel(QLatin1String("detail3Label"));
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

    QTest::newRow("phone") << QPlaceContactDetail::Phone;
    QTest::newRow("email") << QPlaceContactDetail::Email;
    QTest::newRow("website") << QPlaceContactDetail::Website;
    QTest::newRow("fax") << QPlaceContactDetail::Fax;
}

void tst_QPlaceManagerJsonDb::mulipleDetailTypes()
{
    //try saving a place with multiple detail types simultaneously.
    QPlace place;
    place.setName(QLatin1String("Char"));

    QPlaceContactDetail phone;
    phone.setLabel(QLatin1String("phone1"));
    phone.setValue(QLatin1String("555-5555"));
    QPlaceContactDetail phone2;
    phone2.setLabel(QLatin1String("phone2"));
    phone2.setValue(QLatin1String("444-4444"));
    QList<QPlaceContactDetail> phones;
    place.setContactDetails(QPlaceContactDetail::Phone, phones);

    QPlaceContactDetail fax;
    fax.setLabel(QLatin1String("fax1"));
    fax.setValue(QLatin1String("999-9999"));
    QPlaceContactDetail fax2;
    fax2.setLabel(QLatin1String("fax2"));
    fax2.setValue(QLatin1String("999-9999"));
    QList<QPlaceContactDetail> faxes;
    place.setContactDetails(QPlaceContactDetail::Fax, faxes);

    QPlaceContactDetail email;
    email.setValue(QLatin1String("email@adddress.com"));
    QPlaceContactDetail email2;
    email2.setValue(QLatin1String("email2@adddress.com"));
    place.appendContactDetail(QPlaceContactDetail::Email, email);
    place.appendContactDetail(QPlaceContactDetail::Email, email2);

    QPlaceContactDetail website;
    website.setLabel(QLatin1String("website"));
    website.setValue(QLatin1String("www.example.com"));
    QPlaceContactDetail website2;
    website2.setLabel(QLatin1String("website2"));
    website2.setValue(QLatin1String("www.example2.com"));
    place.appendContactDetail(QPlaceContactDetail::Website, website);
    place.appendContactDetail(QPlaceContactDetail::Website, website2);

    //Save a place
    QString placeId;
    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));
    place.setVisibility(QLocation::DeviceVisibility);

    //ensure we can retrieve it's details
    QPlace retrievedPlace;
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    place.setPlaceId(placeId);
    QVERIFY(retrievedPlace == place);

    //try adding some more, changing and removing details of different types
    //when updating a place.
    phone2.setValue(QLatin1String("222-2222"));  //modify detail
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
    website3.setLabel(QLatin1String("website3"));
    website3.setValue(QLatin1String("www.example3.com"));
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
    place.setName(QLatin1String("Char"));
    QString placeId;
    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));
    place.setPlaceId(placeId);

    QTRY_VERIFY_WITH_TIMEOUT(createSpy.count() == 1, Timeout);
    createSpy.clear();
    QVERIFY(updateSpy.count() == 0);
    QVERIFY(removeSpy.count() == 0);

    //modify place
    QGeoLocation location;
    location.setCoordinate(QGeoCoordinate(10,10));
    place.setLocation(location);
    QVERIFY(doSavePlace(place, QPlaceReply::NoError));
    QTRY_VERIFY_WITH_TIMEOUT(updateSpy.count() == 1, Timeout);
    updateSpy.clear();
    QVERIFY(createSpy.count() == 0);
    QVERIFY(removeSpy.count() == 0);

    //remove place
    QVERIFY(doRemovePlace(place, QPlaceReply::NoError));
    QTRY_VERIFY_WITH_TIMEOUT(removeSpy.count() == 1, Timeout);
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
    restaurant.setName(QLatin1String("Restaurant"));
    QVERIFY(doSaveCategory(restaurant, QPlaceReply::NoError, &restaurantId));
    restaurant.setCategoryId(restaurantId);
    QTRY_VERIFY_WITH_TIMEOUT(createSpy.count() == 1, Timeout);
    createSpy.clear();
    QVERIFY(updateSpy.count() == 0);
    QVERIFY(removeSpy.count() == 0);

    //modify category
    restaurant.setName(QLatin1String("RESTAURANT"));
    QVERIFY(doSaveCategory(restaurant));
    QTRY_VERIFY_WITH_TIMEOUT(updateSpy.count() == 1, Timeout);
    updateSpy.clear();
    QVERIFY(createSpy.count() == 0);
    QVERIFY(removeSpy.count() == 0);

    QVERIFY(doRemoveCategory(restaurant));
    QTRY_VERIFY_WITH_TIMEOUT(removeSpy.count() == 1, Timeout);
    removeSpy.clear();
    QVERIFY(createSpy.count() == 0);
    QVERIFY(updateSpy.count() == 0);

    restaurant.setCategoryId(QString());
    QVERIFY(doSaveCategory(restaurant, QPlaceReply::NoError, &restaurantId));
    QTRY_VERIFY_WITH_TIMEOUT(createSpy.count() == 1, Timeout);
    createSpy.clear();
    QVERIFY(updateSpy.count() == 0);
    QVERIFY(removeSpy.count() == 0);

    //save a category as a child
    QString steakId;
    QPlaceCategory steak;
    steak.setName(QLatin1String("Steak"));

    QVERIFY(doSaveCategory(steak, restaurantId, QPlaceReply::NoError, &steakId));
    steak.setCategoryId(steakId);
    QTRY_VERIFY_WITH_TIMEOUT(createSpy.count() == 1, Timeout);
    QVERIFY(createSpy.at(0).at(0).value<QPlaceCategory>() == steak);
    createSpy.clear();
    QVERIFY(updateSpy.count() == 0);
    QVERIFY(removeSpy.count() == 0);
 }

void tst_QPlaceManagerJsonDb::compatiblePlace()
{
    QGeoServiceProvider geoTest(QLatin1String("qmlgeo.test.plugin"));
    geoTest.setAllowExperimental(true);
    QPlaceManager *geoTestManager = geoTest.placeManager();
    QVERIFY(geoTestManager);

    QPlace place;
    place.setPlaceId(QLatin1String("123"));
    place.setName(QLatin1String("Moe's Tavern"));

    QGeoAddress address;
    address.setStreet(QLatin1String("93 Brewing Ave"));
    address.setDistrict(QLatin1String("Maine"));
    address.setCity(QLatin1String("Springfield"));
    address.setCounty(QLatin1String("Jackson"));
    address.setState(QLatin1String("Minnesota"));
    address.setCountry(QLatin1String("Unisted Statess"));
    address.setCountryCode(QLatin1String("USA"));
    QGeoCoordinate coord(56,34,5);

    QGeoLocation location;
    location.setAddress(address);
    location.setCoordinate(coord);
    location.setBoundingBox(QGeoRectangle(QGeoCoordinate(20,20), QGeoCoordinate(10,30)));
    place.setLocation(location);

    QPlaceContactDetail phone;
    phone.setLabel(QLatin1String("Phone"));
    phone.setValue(QLatin1String("555-1793"));
    place.appendContactDetail(QPlaceContactDetail::Phone, phone);

    QPlaceSupplier supplier;
    supplier.setName(QLatin1String("Springfield brewery"));
    supplier.setSupplierId(QLatin1String("ID"));

    place.setSupplier(supplier);

    QPlaceImage image;
    image.setImageId(QLatin1String("798"));
    image.setUrl(QUrl("http://image.com/"));
    QPlaceContent::Collection imageCollection;
    imageCollection.insert(0,image);
    place.insertContent(QPlaceContent::ImageType, imageCollection);
    place.setTotalContentCount(QPlaceContent::ImageType, 1);

    QPlaceAttribute attribute;
    attribute.setLabel(QLatin1String("Smoking"));
    attribute.setText(QLatin1String("Yes"));

    QPlaceIcon icon;
    QVariantMap iconParams;
    iconParams.insert(QLatin1String("s"), QLatin1String("www.example.com/small.png"));
    iconParams.insert(QLatin1String("m"), QLatin1String("www.example.com/medium.png"));
    iconParams.insert(QLatin1String("l"), QLatin1String("www.example.com/large.png"));
    icon.setParameters(iconParams);
    icon.setManager(geoTestManager);
    place.setIcon(icon);

    place.setExtendedAttribute(QLatin1String("Smoking"), attribute);

    place.setVisibility(QLocation::PublicVisibility);

    QPlace compatPlace = placeManager->compatiblePlace(place);

    QVERIFY(compatPlace.placeId().isEmpty());
    QCOMPARE(compatPlace.name(),QLatin1String("Moe's Tavern"));
    QCOMPARE(compatPlace.location().address(), address);
    QVERIFY(compatPlace.location().coordinate() != location.coordinate());
    location.setCoordinate(QGeoCoordinate(56,34));
    QVERIFY(compatPlace.location().coordinate() == location.coordinate());
    QVERIFY(compatPlace.location().boundingBox().isEmpty());

    QCOMPARE(compatPlace.contactTypes().count(), 1);
    QVERIFY(compatPlace.contactTypes().contains(QPlaceContactDetail::Phone));
    QCOMPARE(compatPlace.contactDetails(QPlaceContactDetail::Phone).count(), 1);
    QCOMPARE(compatPlace.contactDetails(QPlaceContactDetail::Phone).at(0), phone);

    QCOMPARE(compatPlace.supplier(), QPlaceSupplier());
    QVERIFY(compatPlace.content(QPlaceContent::ImageType).isEmpty());
    QVERIFY(compatPlace.content(QPlaceContent::ReviewType).isEmpty());
    QVERIFY(compatPlace.content(QPlaceContent::EditorialType).isEmpty());
    QCOMPARE(compatPlace.totalContentCount(QPlaceContent::ImageType), 0);
    QCOMPARE(compatPlace.totalContentCount(QPlaceContent::ReviewType), 0);
    QCOMPARE(compatPlace.totalContentCount(QPlaceContent::EditorialType), 0);

    QCOMPARE(compatPlace.icon().parameters().value(SmallSource).toString(),  QLatin1String("www.example.com/small.png"));
    QCOMPARE(compatPlace.icon().parameters().value(MediumSource).toString(),  QLatin1String("www.example.com/medium.png"));
    QCOMPARE(compatPlace.icon().parameters().value(LargeSource).toString(),  QLatin1String("www.example.com/large.png"));

    QVERIFY(compatPlace.extendedAttributeTypes().contains(QLatin1String("Smoking")));
    QCOMPARE(compatPlace.extendedAttribute(QLatin1String("Smoking")), attribute);

    QCOMPARE(compatPlace.visibility(), QLocation::UnspecifiedVisibility);
}

void tst_QPlaceManagerJsonDb::extendedAttribute()
{
    QPlaceAttribute attribute;
    attribute.setLabel(QLatin1String("x_id_nokia"));
    attribute.setText(QLatin1String("ae562"));
    QPlace place;
    place.setExtendedAttribute(QLatin1String("x_id_nokia"), attribute);

    QString placeId;
    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));

    QPlace retrievedPlace;
    QVERIFY(doFetchDetails(placeId,&retrievedPlace));
    QVERIFY(retrievedPlace.extendedAttributeTypes().contains(QLatin1String("x_id_nokia")));
}

void tst_QPlaceManagerJsonDb::matchingPlaces()
{
    QFETCH(QString, matchType);

    QString categoryId;
    QPlaceCategory restaurant;
    restaurant.setName(QLatin1String("Restaurant"));
    QVERIFY(doSaveCategory(restaurant, QPlaceReply::NoError, &categoryId));
    restaurant.setCategoryId(categoryId);

    QPlaceCategory park;
    park.setName(QLatin1String("Park"));
    QVERIFY(doSaveCategory(park, QPlaceReply::NoError, &categoryId));
    park.setCategoryId(categoryId);

    QPlaceCategory travel;
    travel.setName(QLatin1String("Travel"));
    QVERIFY(doSaveCategory(travel, QPlaceReply::NoError, &categoryId));
    travel.setCategoryId(categoryId);

    QPlace place1;
    place1.setPlaceId(QLatin1String("abcd"));
    place1.setName(QLatin1String("place1"));
    QGeoLocation location;
    location.setCoordinate(QGeoCoordinate(1,1));
    place1.setLocation(location);

    if (matchType == QPlaceMatchRequest::AlternativeId) {
        QPlaceAttribute origin1;
        origin1.setText(QLatin1String("nokia"));
        place1.setExtendedAttribute(QLatin1String("x_provider"), origin1);
    }

    QPlace place1Saved;
    place1Saved = placeManager->compatiblePlace(place1);

    QString placeId;
    QVERIFY(doSavePlace(place1Saved, QPlaceReply::NoError, &placeId));
    place1Saved.setPlaceId(placeId);
    place1Saved.setVisibility(QLocation::DeviceVisibility);

    QPlaceResult result1;
    result1.setPlace(place1);

    QList<QPlaceSearchResult> results;
    results << result1;

    QPlaceMatchRequest matchRequest;
    QVariantMap parameters;
    if (matchType == QPlaceMatchRequest::AlternativeId)
        parameters.insert(QPlaceMatchRequest::AlternativeId, QLatin1String("x_id_nokia"));
    else
        parameters.insert(QLatin1String("proximityRange"), 0);
    matchRequest.setParameters(parameters);
    matchRequest.setResults(results);
    QList<QPlace> places;
    QVERIFY(doMatch(matchRequest,&places));
    QCOMPARE(places.count(), 1);
    QCOMPARE(place1Saved, places.at(0));

    //try matching multiple places
    QPlace nonMatchingPlace;
    nonMatchingPlace.setName(QLatin1String("Non matching"));
    nonMatchingPlace.setPlaceId(QLatin1String("1234"));
    QPlaceAttribute originNonMatch;
    originNonMatch.setText(QLatin1String("nokia"));
    nonMatchingPlace.setExtendedAttribute(QLatin1String("x_provider"),originNonMatch);
    QPlaceResult nonMatchingResult;
    nonMatchingResult.setPlace(nonMatchingPlace);
    results.insert(1, nonMatchingResult);

    QPlace place2;
    place2.setName(QLatin1String("place2"));
    place2.setPlaceId(QLatin1String("efgh"));
    location.setCoordinate(QGeoCoordinate(2,2));
    place2.setLocation(location);

    if (matchType == QPlaceMatchRequest::AlternativeId) {
        QPlaceAttribute origin2;
        origin2.setText(QLatin1String("nokia"));
        place2.setExtendedAttribute(QLatin1String("x_provider"), origin2);
    }

    QPlace place2Saved = placeManager->compatiblePlace(place2);
    QVERIFY(doSavePlace(place2Saved, QPlaceReply::NoError, &placeId));
    place2Saved.setPlaceId(placeId);
    place2Saved.setVisibility(QLocation::DeviceVisibility);

    QPlaceResult result2;
    result2.setPlace(place2);
    results.clear();
    results << result1 << nonMatchingResult << result2;

    matchRequest.setResults(results);
    QVERIFY(doMatch(matchRequest, &places));
    QCOMPARE(places.count(), 3);
    QCOMPARE(places.at(0), place1Saved);

    QCOMPARE(places.at(1), QPlace());
    QCOMPARE(places.at(2), place2Saved);

    //check that if we have assigned categories, the results with return those
    //categories in a match
    QList<QPlaceCategory> categories;
    categories << restaurant << travel;
    place2Saved.setCategories(categories);
    QVERIFY(doSavePlace(place2Saved, QPlaceReply::NoError, &placeId));

    QVERIFY(doMatch(matchRequest, &places));
    QCOMPARE(places.count(), 3);
    QCOMPARE(places.at(0), place1Saved);
    QCOMPARE(places.at(1), QPlace());
    QCOMPARE(places.at(2), place2Saved);
}

void tst_QPlaceManagerJsonDb::matchingPlaces_data()
{
    QTest::addColumn<QString>("matchType");
    QTest::newRow("Alternative Id") << QPlaceMatchRequest::AlternativeId;
    QTest::newRow("Proximity") << "proximity";
}

void tst_QPlaceManagerJsonDb::iconSourceDestination()
{
    //We test that an input source icon is correctly copied to it's specified destination
    //Source and Destination urls are provided
    QFETCH(QString, sourceIconResource);
    QFETCH(QSize, iconSize);
    QFETCH(QString, iconType);
    QFETCH(QString, source);
    QFETCH(QString, specifiedDestination);

    for (int destUrlCorrectness = ValidUrl; destUrlCorrectness != (NoSchemeUrl +1); ++destUrlCorrectness) {
        for (int destFileExists = Exists; destFileExists != (DoesNotExist + 1); ++destFileExists) {
            QPlace place;
            place.setName("place");

            QPlaceCategory category;
            category.setName("category");

            QPlaceIcon icon;
            QVariantMap iconParams;

            QTemporaryFile sourceIconFile;

            if (iconType.startsWith(QLatin1String("file"))) {
                QImage sourceIconImage(sourceIconResource);
                sourceIconFile.open();
                sourceIconImage.save(sourceIconFile.fileName(), QImageReader::imageFormat(sourceIconResource));
                if (iconType == QLatin1String("file"))
                    iconParams.insert(source, QUrl::fromLocalFile(sourceIconFile.fileName()));
                else if (iconType == QLatin1String("file_improperUrl"))
                    iconParams.insert(source, QUrl(sourceIconFile.fileName()));
                else
                    qFatal("Unknown icon type");
            } else if (iconType == QLatin1String("dataUrl")) {
                QFile sourceIcon(sourceIconResource);
                sourceIcon.open(QIODevice::ReadOnly);
                QString mimeType;
                if (QImageReader::imageFormat(sourceIconResource) == "png")
                    mimeType = QLatin1String("image/png");
                QUrl dataUrl(QString::fromLatin1("data:") + mimeType + QLatin1String(";base64,") + sourceIcon.readAll().toBase64());
                iconParams.insert(source, dataUrl);
            }

            QTemporaryDir tempDir;
            QVERIFY(tempDir.isValid());
            QString destIconFileName = tempDir.path() + QLatin1String("/tempFile");
            if (destFileExists == Exists) {
                QFile destFile(destIconFileName);
                destFile.open(QIODevice::ReadWrite);
                destFile.close();
            }//else must be creating a new destination file

            if (destUrlCorrectness == ValidUrl)
                iconParams.insert(specifiedDestination, QUrl::fromLocalFile(destIconFileName));
            else  //must be no scheme url
                iconParams.insert(specifiedDestination, QUrl(destIconFileName));

            icon.setParameters(iconParams);
            place.setIcon(icon);
            category.setIcon(icon);

            QString placeId;
            QString categoryId;
            QVERIFY(doSavePlace(place,QPlaceReply::NoError, &placeId));
            QVERIFY(doSaveCategory(category,QPlaceReply::NoError, &categoryId));

            QPlace retrievedPlace;
            QPlaceCategory retrievedCategory;
            QVERIFY(doFetchDetails(placeId, &retrievedPlace));
            QVERIFY(doFetchCategory(categoryId, &retrievedCategory));

            QVERIFY(retrievedPlace.icon().parameters().contains(specifiedDestination));
            QVERIFY(retrievedCategory.icon().parameters().contains(specifiedDestination));

            QUrl retrievedUrl = retrievedPlace.icon().parameters().value(specifiedDestination).toUrl();
            QCOMPARE(retrievedCategory.icon().parameters().value(specifiedDestination).toUrl(), retrievedUrl);

            QImage retrievedImage;

            QVERIFY(retrievedUrl.scheme().compare(QLatin1String("file"), Qt::CaseInsensitive) == 0);
            retrievedImage = QImage(retrievedUrl.toLocalFile());

            QCOMPARE(retrievedImage.size(), iconSize);
        }
    }
}

void tst_QPlaceManagerJsonDb::iconSourceDestination_data()
{
    QTest::addColumn<QString>("sourceIconResource");
    QTest::addColumn<QSize>("iconSize");
    QTest::addColumn<QString>("iconType");
    QTest::addColumn<QString>("source");
    QTest::addColumn<QString>("specifiedDestination");

    //Specify source and destionation, expect file to be copied into destination file
    QTest::newRow("source destination small file") << ":/resources/icon_small.png" << QSize(20,20) << "file" << SmallSource << SmallDestination;
    QTest::newRow("source destination medium file") << ":/resources/icon_medium.png" << QSize(30,30) << "file" << MediumSource << MediumDestination;
    QTest::newRow("source destination large file") << ":/resources/icon_large.png" << QSize(50,50) << "file" << LargeSource << LargeDestination;
    QTest::newRow("source destination fullscreen file") << ":/resources/icon_fullscreen.png" << QSize(320,480) << "file" << FullscreenSource << FullscreenDestination;

    //Specify source and destination, expect data url to be copied into destination url
    QTest::newRow("source destination small dataUrl") << ":/resources/icon_small.png" << QSize(20,20) << "dataUrl" << SmallSource << SmallDestination;
    QTest::newRow("source destination medium dataUrl") << ":/resources/icon_medium.png" << QSize(30,30) << "dataUrl" << MediumSource << MediumDestination;
    QTest::newRow("source destination large dataUrl") << ":/resources/icon_large.png" << QSize(50,50) << "dataUrl" << LargeSource << LargeDestination;
    QTest::newRow("source destination fullscreen dataUrl") << ":/resources/icon_fullscreen.png" << QSize(320,480) << "dataUrl" << FullscreenSource << FullscreenDestination;

    //try using improper source file urls, e.g. /some/path rather than file:///some/path
    QTest::newRow("source destination small file_improperUrl") << ":/resources/icon_small.png" << QSize(20,20) << "file_improperUrl" << SmallSource << SmallDestination;
    QTest::newRow("source destination medium file_improperUrl") << ":/resources/icon_medium.png" << QSize(30,30) << "file_improperUrl" << MediumSource << MediumDestination;
    QTest::newRow("source destination large file_improperUrl") << ":/resources/icon_large.png" << QSize(50,50) << "file_improperUrl" << LargeSource << LargeDestination;
    QTest::newRow("source destination fullscreen file_improperUrl") << ":/resources/icon_fullscreen.png" << QSize(320,480) << "file_improperUrl" << FullscreenSource << FullscreenDestination;
}

void tst_QPlaceManagerJsonDb::iconSourceOnly()
{
    //We test that a dataUrl is generated from the given input icon
    //Soucre urls are provided while destination urls
    QFETCH(QString, sourceIconResource);
    QFETCH(QSize, iconSize);
    QFETCH(QString, iconType);
    QFETCH(QString, source);
    QFETCH(QString, expectedDestination);

    QPlace place;
    place.setName("place");

    QPlaceCategory category;
    category.setName("category");

    QPlaceIcon icon;
    QVariantMap iconParams;

    QTemporaryFile sourceIconFile;
    if (iconType.startsWith(QLatin1String("file"))) {
        QImage sourceIconImage(sourceIconResource);
        sourceIconFile.open();
        sourceIconImage.save(sourceIconFile.fileName(), QImageReader::imageFormat(sourceIconResource));
        if (iconType == QLatin1String("file"))
            iconParams.insert(source, QUrl::fromLocalFile(sourceIconFile.fileName()));
        else if (iconType == QLatin1String("file_improperUrl"))
            iconParams.insert(source, QUrl(sourceIconFile.fileName()));
        else
            qFatal("Unknown iconType");
    } else if (iconType == QLatin1String("dataUrl")) {
        QFile sourceIcon(sourceIconResource);
        sourceIcon.open(QIODevice::ReadOnly);
        QString mimeType;
        if (QImageReader::imageFormat(sourceIconResource) == "png")
            mimeType = QLatin1String("image/png");
        else
            qFatal("Unexpected image format");
        QUrl dataUrl(QString::fromLatin1("data:") + mimeType + QLatin1String(";base64,") + sourceIcon.readAll().toBase64());
        iconParams.insert(source, dataUrl);
    } else if (iconType == QLatin1String("accessible_incorrectWebUrl") || iconType == QLatin1String("unaccessible_webUrl")) {
        iconParams.insert(source, QUrl(sourceIconResource));
    }

    icon.setParameters(iconParams);
    place.setIcon(icon);
    category.setIcon(icon);

    QString placeId;
    QString categoryId;

    if (iconType == QLatin1String("accessible_incorrectWebUrl")) {
        QVERIFY(doSavePlace(place,QPlaceReply::UnsupportedError));
        QVERIFY(doSaveCategory(category,QPlaceReply::UnsupportedError));
    } else if (iconType == QLatin1String("unaccessible_webUrl")){
        QVERIFY(doSavePlace(place,QPlaceReply::CommunicationError));
        QVERIFY(doSaveCategory(category,QPlaceReply::CommunicationError));
    } else {
        QVERIFY(doSavePlace(place,QPlaceReply::NoError, &placeId));
        QVERIFY(doSaveCategory(category,QPlaceReply::NoError, &categoryId));

        QPlace retrievedPlace;
        QPlaceCategory retrievedCategory;
        QVERIFY(doFetchDetails(placeId, &retrievedPlace));
        QVERIFY(doFetchCategory(categoryId, &retrievedCategory));
        if (expectedDestination != QString()) {
            QVERIFY(retrievedPlace.icon().parameters().contains(expectedDestination));
            QVERIFY(retrievedCategory.icon().parameters().contains(expectedDestination));

            QUrl retrievedUrl = retrievedPlace.icon().parameters().value(expectedDestination).toUrl();
            QCOMPARE(retrievedUrl, retrievedCategory.icon().parameters().value(expectedDestination).toUrl());
            QImage retrievedImage;
            if (expectedDestination != QString())
                QVERIFY(retrievedUrl.scheme().compare(QLatin1String("data"), Qt::CaseInsensitive) == 0);
            retrievedImage = dataUrlToImage(retrievedUrl);
            QCOMPARE(retrievedImage.size(), iconSize);
        }
    }
}

void tst_QPlaceManagerJsonDb::iconSourceOnly_data()
{
    QTest::addColumn<QString>("sourceIconResource");
    QTest::addColumn<QSize>("iconSize");
    QTest::addColumn<QString>("iconType");
    QTest::addColumn<QString>("source");
    QTest::addColumn<QString>("expectedDestination");

    //input icon is a file
    QTest::newRow("source small file") << ":/resources/icon_small.png" << SmallSize << "file" << SmallSource <<  SmallDestination;
    QTest::newRow("source medium file") << ":/resources/icon_medium.png" << MediumSize << "file" << MediumSource <<  MediumDestination;
    QTest::newRow("source large file") << ":/resources/icon_large.png" << LargeSize << "file" << LargeSource <<  LargeDestination;
    QTest::newRow("source fullscreen file") << ":/resources/icon_fullscreen.png" << FullscreenSize << "file" << FullscreenSource << ""; //fullscreen icons shouldn't be saved as data urls

    //input is a file but an improper url (e.g of the format /some/path instead of file:///some/path)
    QTest::newRow("source small file_improperUrl") << ":/resources/icon_small.png" << SmallSize << "file_improperUrl" << SmallSource <<  SmallDestination;
    QTest::newRow("source medium file_improperUrl") << ":/resources/icon_medium.png" << MediumSize << "file_improperUrl" << MediumSource <<  MediumDestination;
    QTest::newRow("source large file_improperUrl") << ":/resources/icon_large.png" << LargeSize << "file_improperUrl" << LargeSource <<  LargeDestination;
    QTest::newRow("source fullscreen file_improperUrl") << ":/resources/icon_fullscreen.png" << FullscreenSize << "file_improperUrl" << FullscreenSource << ""; //fullscreen icons shouldn't be saved as data urls

    //input icon is a data url
    QTest::newRow("source small dataUrl") << ":/resources/icon_small.png" << SmallSize << "dataUrl" << SmallSource << SmallDestination;
    QTest::newRow("source medium dataUrl") << ":/resources/icon_medium.png" << MediumSize << "dataUrl" << MediumSource << MediumDestination;
    QTest::newRow("source large dataUrl") << ":/resources/icon_large.png" << LargeSize << "dataUrl" << LargeSource << LargeDestination;
    QTest::newRow("source fullscreen dataUrl") << ":/resources/icon_fullscreen.png" << FullscreenSize << "dataUrl" << FullscreenSource << ""; //fullscreen icons shouldn't be saved as data urls

#ifdef REMOTE_TEST
    //input icon is a reachable url, but does not reference an image
    QTest::newRow("source small accessible_incorrectWebUrl") << "www.example.com/icon_small.png" << SmallSize << "accessible_incorrectWebUrl" << SmallSource << SmallDestination;
    QTest::newRow("source medium accessible_incorrectWebUrl") << "www.example.com/icon_medium.png" << MediumSize << "accessible_incorrectWebUrl" << MediumSource << MediumDestination;
    QTest::newRow("source large accessible_incorrectWebUrl") << "www.example.com/icon_large.png" << LargeSize << "accessible_incorrectWebUrl" << LargeSource << LargeDestination;
    QTest::newRow("source fullscreen accessible_incorrectWebUrl") << "www.example.com/icon_fullscreen.png" << FullscreenSize << "accessible_incorrectWebUrl" << FullscreenSource << ""; //fullscreen icons shouldn't be saved as data urls
#endif

    //input icon is an unnaccessible url
    QTest::newRow("source small unaccessible_webUrl") << "www.non-existetent-example.com/icon_small.png" << SmallSize << "unaccessible_webUrl" << SmallSource << SmallDestination;
    QTest::newRow("source medium unaccessible_webUrl") << "www.non-existetent-example.com/icon_medium.png" << MediumSize << "unaccessible_webUrl" << MediumSource << MediumDestination;
    QTest::newRow("source large unaccessible_webUrl") << "www.non-existetent-example.com/icon_large.png" << LargeSize << "unaccessible_webUrl" << LargeSource << LargeDestination;
    QTest::newRow("source fullscreen unaccessible_webUrl") << "www.non-existetent-example.com/icon_fullscreen.png" << FullscreenSize << "unaccessible_webUrl" << FullscreenSource << ""; //fullscreen icons shouldn't be saved as data urls
}

void tst_QPlaceManagerJsonDb::iconDestinationOnly()
{
    //We test when a destination url is provided, that it is correctly saved
    //No soure urls are provided while destination urls are.
    QFETCH(QString, destination);
    QFETCH(QUrl, destinationUrl);
    QFETCH(QSize, specifiedSize);
    QFETCH(QString, iconResource); //empty reosurce indicates icon is not reachable/accessible

    QString destinationSize = destination;
    destinationSize.replace(QLatin1String("Url"), QLatin1String("Size"));

    QVariantMap iconParams;

    QTemporaryFile destinationFile;
    QSize size;
    if (destinationUrl.toString().startsWith("_autogenerated_file_")) {
        QImage iconImage(iconResource);
        destinationFile.open();
        iconImage.save(destinationFile.fileName(), QImageReader::imageFormat(iconResource));
        size = iconImage.size();

        if (destinationUrl == QUrl("_autogenerated_file_no_scheme_url_"))
            destinationUrl = QUrl::fromLocalFile(destinationFile.fileName());
        else
            destinationUrl = QUrl::fromLocalFile(destinationFile.fileName());
    } else if (destinationUrl == QUrl("_data_url_")) {
        QFile iconFile(iconResource);
        iconFile.open(QIODevice::ReadOnly);

        QString mimeType;
        if (QImageReader::imageFormat(iconResource) == "png")
            mimeType = QLatin1String("image/png");

        QImage iconImage(iconResource);
        size = iconImage.size();

        destinationUrl = QUrl(QString::fromLatin1("data:") + mimeType + QLatin1String(";base64,") + iconFile.readAll().toBase64());
    }

    iconParams.insert(destination, destinationUrl);

    if (!specifiedSize.isEmpty())
        iconParams.insert(destinationSize, specifiedSize);

    QPlace place;
    place.setName("Place");
    QPlaceIcon icon;
    icon.setParameters(iconParams);
    place.setIcon(icon);
    QString placeId;

    QPlaceCategory category;
    category.setName("Cat");
    category.setIcon(icon);
    QString categoryId;

    if (specifiedSize.isEmpty() && iconResource.isEmpty()) {
        QVERIFY(doSavePlace(place, QPlaceReply::BadArgumentError));
        QVERIFY(doSaveCategory(category, QPlaceReply::BadArgumentError));
        return;
    } else {
        QVERIFY(doSavePlace(place,QPlaceReply::NoError, &placeId));
        QVERIFY(doSaveCategory(category,QPlaceReply::NoError, &categoryId));
    }

    QPlace retrievedPlace;
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));

    QPlaceCategory retrievedCategory;
    QVERIFY(doFetchCategory(categoryId, &retrievedCategory));


    QCOMPARE(retrievedPlace.icon().parameters().value(destination).toUrl(), destinationUrl);
    QCOMPARE(retrievedCategory.icon().parameters().value(destination).toUrl(), destinationUrl);

    if (!specifiedSize.isEmpty() && iconResource.isEmpty()) {
        QCOMPARE(retrievedPlace.icon().parameters().value(destinationSize).toSize(), specifiedSize);
        QCOMPARE(retrievedCategory.icon().parameters().value(destinationSize).toSize(), specifiedSize);
    } else {
        QCOMPARE(retrievedPlace.icon().parameters().value(destinationSize).toSize(), size);
        QCOMPARE(retrievedCategory.icon().parameters().value(destinationSize).toSize(), size);
    }
}

void tst_QPlaceManagerJsonDb::iconDestinationOnly_data()
{
    QTest::addColumn<QString>("destination");
    QTest::addColumn<QUrl>("destinationUrl");
    QTest::addColumn<QSize>("specifiedSize");
    QTest::addColumn<QString>("iconResource");

    //destination url is local file, no size is specified
    QTest::newRow("destination small file no_size_provided") << SmallDestination  << QUrl("_autogenerated_file_") << QSize() << ":/resources/icon_small.png";
    QTest::newRow("destination medium file no_size_provided") << MediumDestination  << QUrl("_autogenerated_file_") << QSize() << ":/resources/icon_medium.png";
    QTest::newRow("destination large file no_size_provided") << LargeDestination  << QUrl("_autogenerated_file_") << QSize() << ":/resources/icon_large.png";
    QTest::newRow("destination fullscreen file no_size_provided") << FullscreenDestination  << QUrl("_autogenerated_file_") << QSize() << ":/resources/icon_fullscreen.png";

    //destination url is local file, no size is specified, the url has no scheme
    QTest::newRow("destination small file no_size_provided") << SmallDestination  << QUrl("_autogenerated_file_no_scheme_url_") << QSize() << ":/resources/icon_small.png";
    QTest::newRow("destination medium file no_size_provided") << MediumDestination  << QUrl("_autogenerated_file_no_scheme_url_") << QSize() << ":/resources/icon_medium.png";
    QTest::newRow("destination large file no_size_provided") << LargeDestination  << QUrl("_autogenerated_file_no_scheme_url_") << QSize() << ":/resources/icon_large.png";
    QTest::newRow("destination fullscreen file no_size_provided") << FullscreenDestination  << QUrl("_autogenerated_file_no_scheme_url_") << QSize() << ":/resources/icon_fullscreen.png";

    //destination url is local file and an (inaccurate) size is specified, (the actual size of file should take precedence)
    QTest::newRow("destination small file size_provided") << SmallDestination  << QUrl("_autogenerated_file_") << QSize(86,99) << ":/resources/icon_small.png";
    QTest::newRow("destination medium file size_provided") << MediumDestination  << QUrl("_autogenerated_file_") << QSize(86, 99) << ":/resources/icon_medium.png";
    QTest::newRow("destination large file size_provided") << LargeDestination  << QUrl("_autogenerated_file_") << QSize(86, 99) << ":/resources/icon_large.png";
    QTest::newRow("destination fulscreen file size_provided") << FullscreenDestination  << QUrl("_autogenerated_file_") << QSize(86, 99) << ":/resources/icon_fullscreen.png";

    //destination url is a data url, no size is specified
    QTest::newRow("destination small dataurl no_size_provided") << SmallDestination  << QUrl("_data_url_") << QSize() << ":/resources/icon_small.png";
    QTest::newRow("destination medium dataurl no_size_provided") << MediumDestination  << QUrl("_data_url_") << QSize() << ":/resources/icon_medium.png";
    QTest::newRow("destination large dataurl no_size_provided") << LargeDestination  << QUrl("_data_url_") << QSize() << ":/resources/icon_large.png";
    QTest::newRow("destination fullscreen dataurl no_size_provided") << FullscreenDestination  << QUrl("_data_url_") << QSize() << ":/resources/icon_fullscreen.png";

    //destination url is a data url, and an (inaccurate) size is specified, (the actual size of the icon should take precedence)
    QTest::newRow("destination small dataurl size_provided") << SmallDestination  << QUrl("_data_url_") << QSize(86,99) << ":/resources/icon_small.png";
    QTest::newRow("destination medium dataurl size_provided") << MediumDestination  << QUrl("_data_url_") << QSize(86, 99) << ":/resources/icon_medium.png";
    QTest::newRow("destination large dataurl size_provided") << LargeDestination  << QUrl("_data_url_") << QSize(86, 99) << ":/resources/icon_large.png";
    QTest::newRow("destination fullscreen dataurl size_provided") << FullscreenDestination  << QUrl("_data_url_") << QSize(86, 99) << ":/resources/icon_fullscreen.png";

    //destination url is unaccessible, and size is specified
    QTest::newRow("destination small unaccessible_webUrl size_provided") << SmallDestination << QUrl("http://www.non-existetent-example.com/iconS.png") << SmallSize << "";
    QTest::newRow("destination medium unaccessible_webUrl size_provided") << MediumDestination << QUrl("http://www.non-existetent-example.com/iconM.png") << MediumSize << "" ;
    QTest::newRow("destination large unaccessible_webUrl size_provided") << LargeDestination << QUrl("http://www.non-existetent-example.com/iconL.png") << LargeSize << "";
    QTest::newRow("destination fullscreen unaccessible_webUrl size_provided") << FullscreenDestination << QUrl("http://www.non-existetent-example.com/iconF.png") << FullscreenSize << "";

    //destination url is unaccessible and no size is provided, expect failure to save
    QTest::newRow("destination small unaccessible_webUrl no_size_provided") << SmallDestination << QUrl("http://www.non-existetent-example.com/iconS.png") << QSize() << "";
    QTest::newRow("destination medium unaccessible_webUrl no_size_provided") << MediumDestination << QUrl("http://www.non-existetent-example.com/iconM.png") << QSize() << "" ;
    QTest::newRow("destination large unaccessible_webUrl no_size_provided") << LargeDestination << QUrl("http://www.non-existetent-example.com/iconL.png") << QSize() << "";
    QTest::newRow("destination fullscreen unaccessible_webUrl no_size_provided") << FullscreenDestination << QUrl("http://www.non-existetent-example.com/iconL.png") << QSize() << "";
}

void tst_QPlaceManagerJsonDb::iconSavedFromDifferentManager()
{
    QGeoServiceProvider geoTest("qmlgeo.test.plugin");
    geoTest.setAllowExperimental(true);
    QPlaceManager *geoTestManager = geoTest.placeManager();

    QPlaceIcon icon;
    icon.setManager(geoTestManager);
    QVariantMap iconParams;

    QTemporaryFile sourceIconFile;
    QImage sourceIconImage(":/resources/icon_small.png");
    sourceIconFile.open();
    sourceIconImage.save(sourceIconFile.fileName(), "png");

    //try an icon from another manager which different icons for each size variant
    //check that we get correctly generated data urls
    iconParams.insert("s", QUrl::fromLocalFile(":/resources/icon_small.png"));
    iconParams.insert("m", QUrl::fromLocalFile(":/resources/icon_medium.png"));
    iconParams.insert("l", QUrl::fromLocalFile(":/resources/icon_large.png"));
    iconParams.insert("f", QUrl::fromLocalFile(":/resources/icon_fullscreen.png"));
    icon.setParameters(iconParams);

    QPlace place;
    place.setName("Place");
    place.setIcon(icon);

    QPlace compatiblePlace = placeManager->compatiblePlace(place);
    QString placeId;
    QVERIFY(doSavePlace(compatiblePlace,QPlaceReply::NoError, &placeId));

    QPlace retrievedPlace;
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));

    QVERIFY(retrievedPlace.icon().parameters().keys().contains(SmallDestination));
    QVERIFY(retrievedPlace.icon().parameters().keys().contains(SmallDestinationSize));
    QVERIFY(retrievedPlace.icon().parameters().keys().contains(MediumDestination));
    QVERIFY(retrievedPlace.icon().parameters().keys().contains(MediumDestinationSize));
    QVERIFY(retrievedPlace.icon().parameters().keys().contains(LargeDestination));
    QVERIFY(retrievedPlace.icon().parameters().keys().contains(LargeDestinationSize));

    //we don't expect fullscreen icon urls to be download into data urls
    //data urls are only meant for small icons
    QVERIFY(!retrievedPlace.icon().parameters().keys().contains(FullscreenDestination));

    QImage smallImage = dataUrlToImage(retrievedPlace.icon().parameters().value(SmallDestination).toUrl());
    QImage mediumImage = dataUrlToImage(retrievedPlace.icon().parameters().value(MediumDestination).toUrl());
    QImage largeImage = dataUrlToImage(retrievedPlace.icon().parameters().value(LargeDestination).toUrl());

    QCOMPARE(smallImage.size(), SmallSize);
    QCOMPARE(mediumImage.size(), MediumSize);
    QCOMPARE(largeImage.size(), LargeSize);

    QCOMPARE(retrievedPlace.icon().parameters().value(SmallDestinationSize).toSize(), SmallSize);
    QCOMPARE(retrievedPlace.icon().parameters().value(MediumDestinationSize).toSize(), MediumSize);
    QCOMPARE(retrievedPlace.icon().parameters().value(LargeDestinationSize).toSize(), LargeSize);

    //try an icon from another manaager which has only a single size variant
    //check that we get only one size variant
    iconParams.clear();
    iconParams.insert("s", QUrl::fromLocalFile(":/resources/icon_small.png"));
    icon.setParameters(iconParams);
    place.setIcon(icon);
    compatiblePlace = placeManager->compatiblePlace(place);
    QVERIFY(doSavePlace(compatiblePlace,QPlaceReply::NoError, &placeId));
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));

    QCOMPARE(retrievedPlace.icon().parameters().keys().count(), 2);
    QVERIFY(retrievedPlace.icon().parameters().keys().contains(SmallDestination));
    QVERIFY(retrievedPlace.icon().parameters().keys().contains(SmallDestinationSize));
    smallImage = dataUrlToImage(retrievedPlace.icon().parameters().value(SmallDestination).toUrl());
    QCOMPARE(smallImage.size(), SmallSize);
    QSize size = retrievedPlace.icon().parameters().value(SmallDestinationSize).toSize();
    QCOMPARE(size, SmallSize);

    //try an icon from a manager which has sizes which don't exactly match the standard expected icons
    //sizes, check that dataUrls are generated for the appropriate size variant
    iconParams.clear();
    iconParams.insert("m", QUrl::fromLocalFile(":/resources/icon_24x24.png"));
    iconParams.insert("l", QUrl::fromLocalFile(":/resources/icon_40x40.png"));
    icon.setParameters(iconParams);
    place.setIcon(icon);
    compatiblePlace = placeManager->compatiblePlace(place);
    QVERIFY(doSavePlace(compatiblePlace,QPlaceReply::NoError, &placeId));
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QCOMPARE(retrievedPlace.icon().parameters().keys().count(), 4);
    QVERIFY(retrievedPlace.icon().parameters().keys().contains(SmallDestination));
    QVERIFY(retrievedPlace.icon().parameters().keys().contains(SmallDestinationSize));
    smallImage = dataUrlToImage(retrievedPlace.icon().parameters().value(SmallDestination).toUrl());
    QCOMPARE(smallImage.size(), QSize(24,24));
    size = retrievedPlace.icon().parameters().value(SmallDestinationSize).toSize();
    QCOMPARE(size, QSize(24,24));

    QVERIFY(retrievedPlace.icon().parameters().keys().contains(LargeDestination));
    QVERIFY(retrievedPlace.icon().parameters().keys().contains(LargeDestinationSize));
    smallImage = dataUrlToImage(retrievedPlace.icon().parameters().value(LargeDestination).toUrl());
    QCOMPARE(smallImage.size(), QSize(40,40));
    size = retrievedPlace.icon().parameters().value(LargeDestinationSize).toSize();
    QCOMPARE(size, QSize(40,40));
}

void tst_QPlaceManagerJsonDb::iconFormats()
{
    QFETCH(QByteArray, imageFormat);
    QFETCH(QString, mimeType);

    if (imageFormat == "svg")
        QSKIP("There is currently a crash issue with QImage and svg, see QTBUG-24468, so skip for now");

    if (imageFormat == "tiff")
        QSKIP("This test currently fails, investigate");

    QTemporaryFile sourceIconFile;
    sourceIconFile.open();
    QImage sourceIconImage;

    if (imageFormat == "svg") {
        QFile file(":/resources/icon_small.svg");
        file.open(QIODevice::ReadOnly);
        sourceIconFile.write(file.readAll());
        sourceIconFile.close();
    } else {
        QVERIFY(sourceIconImage.load(":/resources/icon_small.png"));
        QVERIFY(sourceIconImage.save(&sourceIconFile, imageFormat));
    }

    QVariantMap iconParams;
    iconParams.insert(SmallSource, QUrl::fromLocalFile(sourceIconFile.fileName()));

    QPlace place;
    place.setName("place");

    QPlaceCategory category;
    category.setName("category");

    QPlaceIcon icon;
    icon.setParameters(iconParams);

    place.setIcon(icon);
    category.setIcon(icon);


    QString placeId;
    QString categoryId;

    if (imageFormat == "svg" && !QImageReader::supportedImageFormats().contains("svg")) {
        //svg format is not supported therefore saving of the icon should fail
        //in this instance we are creating a data url and therefore need to be
        //able to detect the image format and mime type.
        QVERIFY(doSavePlace(place,QPlaceReply::BadArgumentError));
        QVERIFY(doSaveCategory(category,QPlaceReply::BadArgumentError));
    } else {
        QVERIFY(doSavePlace(place,QPlaceReply::NoError, &placeId));
        QVERIFY(doSaveCategory(category,QPlaceReply::NoError, &categoryId));

        QPlace retrievedPlace;
        QVERIFY(doFetchDetails(placeId, &retrievedPlace));

        QPlaceCategory retrievedCategory;
        QVERIFY(doFetchCategory(categoryId, &retrievedCategory));
        QCOMPARE(retrievedPlace.icon(), retrievedCategory.icon());

        QRegExp regExp("^data:(.*);.*$");
        regExp.indexIn(retrievedPlace.icon().parameters().value(SmallDestination).toUrl().toString());
        QCOMPARE(regExp.cap(1), mimeType);
        QCOMPARE(dataUrlToImage(retrievedPlace.icon().parameters().value(SmallDestination).toUrl()).size(), SmallSize);
    }
}

void tst_QPlaceManagerJsonDb::iconFormats_data()
{
    QTest::addColumn<QByteArray>("imageFormat");
    QTest::addColumn<QString>("mimeType");

    QTest::newRow("bmp") << QByteArray("bmp") << "image/bmp";
    QTest::newRow("jpg") << QByteArray("jpg") << "image/jpeg";
    QTest::newRow("jpeg") << QByteArray("jpeg") << "image/jpeg";
    QTest::newRow("png") << QByteArray("png") << "image/png";
    QTest::newRow("pbm") << QByteArray("pbm") << "image/x-portable-bitmap";
    QTest::newRow("pgm") << QByteArray("pgm") << "image/x-portable-graymap";
    QTest::newRow("ppm") << QByteArray("ppm") << "image/x-portable-pixmap";
    QTest::newRow("tiff") << QByteArray("tiff") << "image/tiff";
    QTest::newRow("xbm") << QByteArray("xbm") << "image/x-xbitmap";
    QTest::newRow("xpm") << QByteArray("xpm") << "image/x-xpixmap";
    QTest::newRow("svg") << QByteArray("svg") << "image/svg+xml";
}

void tst_QPlaceManagerJsonDb::iconUrls()
{
    QFETCH(QString, sizeType);
    QFETCH(QSize, size);

    QString source = sizeType + QLatin1String("SourceUrl");
    QString destination = sizeType + QLatin1String("Url");
    QString destinationSize = sizeType + QLatin1String("Size");

    QPlace place;
    place.setName("place");

    QPlaceCategory category;
    category.setName("category");

    QPlaceIcon icon;
    QVariantMap iconParams;

    //test conversion to valid url, in this care using a valid file scheme
    iconParams.insert(destination, QUrl("/home/user/icon.png"));
    iconParams.insert(destinationSize, size);
    icon.setParameters(iconParams);
    place.setIcon(icon);
    category.setIcon(icon);

    QString placeId;
    QString categoryId;
    QVERIFY(doSavePlace(place,QPlaceReply::NoError, &placeId));
    QVERIFY(doSaveCategory(category,QPlaceReply::NoError, &categoryId));

    QPlace retrievedPlace;
    QPlaceCategory retrievedCategory;
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QVERIFY(doFetchCategory(categoryId, &retrievedCategory));
    QCOMPARE(retrievedPlace.icon(), retrievedCategory.icon());
    QCOMPARE(retrievedPlace.icon().parameters().value(destination).toUrl(), QUrl("file:///home/user/icon.png"));

    //test conversion to valid url,
    iconParams.insert(destination, QUrl("qrc:/home/user/icon.png"));
    icon.setParameters(iconParams);
    place.setIcon(icon);
    category.setIcon(icon);
    QVERIFY(doSavePlace(place,QPlaceReply::NoError, &placeId));
    QVERIFY(doSaveCategory(category,QPlaceReply::NoError, &categoryId));
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QVERIFY(doFetchCategory(categoryId, &retrievedCategory));
    QCOMPARE(retrievedPlace.icon(), retrievedCategory.icon());
    QCOMPARE(retrievedPlace.icon().parameters().value(destination).toUrl(), QUrl("qrc:///home/user/icon.png"));

    iconParams.insert(destination, QUrl("qrc:///home/user/icon.png"));
    icon.setParameters(iconParams);
    place.setIcon(icon);
    category.setIcon(icon);

    QVERIFY(doSavePlace(place,QPlaceReply::NoError, &placeId));
    QVERIFY(doSaveCategory(category,QPlaceReply::NoError, &categoryId));
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QVERIFY(doFetchCategory(categoryId, &retrievedCategory));
    QCOMPARE(retrievedPlace.icon(), retrievedCategory.icon());
    QCOMPARE(retrievedPlace.icon().parameters().value(destination).toUrl(), QUrl("qrc:///home/user/icon.png"));

    //test urls that are non-encoded and encoded
    iconParams.insert(destination, QUrl("qrc:///home/user/i con.png"));
    icon.setParameters(iconParams);
    place.setIcon(icon);
    category.setIcon(icon);
    QVERIFY(doSavePlace(place,QPlaceReply::NoError, &placeId));
    QVERIFY(doSaveCategory(category,QPlaceReply::NoError, &categoryId));
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QVERIFY(doFetchCategory(categoryId, &retrievedCategory));
    QCOMPARE(retrievedPlace.icon(), retrievedCategory.icon());
    QCOMPARE(retrievedPlace.icon().parameters().value(destination).toUrl(), QUrl("qrc:///home/user/i con.png"));

    iconParams.insert(destination, QUrl("qrc:///home/user/ico%20n.png"));
    icon.setParameters(iconParams);
    place.setIcon(icon);
    category.setIcon(icon);
    QVERIFY(doSavePlace(place,QPlaceReply::NoError, &placeId));
    QVERIFY(doSaveCategory(category,QPlaceReply::NoError, &categoryId));
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QVERIFY(doFetchCategory(categoryId, &retrievedCategory));
    QCOMPARE(retrievedPlace.icon(), retrievedCategory.icon());
    QCOMPARE(retrievedPlace.icon().parameters().value(destination).toUrl(), QUrl("qrc:///home/user/ico n.png"));

    //try using a QString instead of a url in the parameters
    iconParams.insert(destination, "www.example.com");
    icon.setParameters(iconParams);
    place.setIcon(icon);
    category.setIcon(icon);
    QVERIFY(doSavePlace(place,QPlaceReply::BadArgumentError, &placeId));
    QVERIFY(doSaveCategory(category,QPlaceReply::BadArgumentError, &categoryId));

    //try using a QString instead of a url in the parameters
    iconParams.insert(source, "www.example.com");
    icon.setParameters(iconParams);
    place.setIcon(icon);
    category.setIcon(icon);
    QVERIFY(doSavePlace(place,QPlaceReply::BadArgumentError, &placeId));
    QVERIFY(doSaveCategory(category,QPlaceReply::BadArgumentError, &categoryId));
}

void tst_QPlaceManagerJsonDb::iconUrls_data()
{
    QTest::addColumn<QString>("sizeType");
    QTest::addColumn<QSize>("size");

    QTest::newRow("small") << "small" << SmallSize;
    QTest::newRow("medium") << "medium" << MediumSize;
    QTest::newRow("large") << "large" << LargeSize;
    QTest::newRow("fullscreen") << "fullscreen" << FullscreenSize;
}

#ifdef REMOTE_TEST
void tst_QPlaceManagerJsonDb::iconDownload()
{
    QFETCH(QString, sizeType);
    QFETCH(QString, destinationType);
    QFETCH(QString, errorCondition);

    QPlace place;
    place.setName("place");
    QGeoCoordinate coord(10,10);
    QGeoLocation location;
    location.setCoordinate((coord));
    place.setLocation(location);

    QPlaceCategory category;
    category.setName("category");

    QPlaceIcon icon;
    QVariantMap parameters;

    QString source = sizeType + QLatin1String("SourceUrl");
    if (errorCondition == "no error")
        parameters.insert(source, QUrl("http://download.ch1.qa.vcdn.nokia.com/p/d/places2/icons/categories/01.icon"));
    else if (errorCondition == "unreachable host")
        parameters.insert(source, QUrl("http://doesnotexist_aef32.com/01.icon"));
    else
        QFAIL("Unknown error condition");

    QTemporaryDir tempDir;
    if (destinationType.startsWith(QLatin1String("file"))) {
        QVERIFY(tempDir.isValid());
        QString destIconFileName = tempDir.path() + QLatin1String("/tempFile");

        if (destinationType == QLatin1String("file existing")) {
            QFile destFile(destIconFileName);
            destFile.open(QIODevice::ReadWrite);
            destFile.close();
        }
        parameters.insert(sizeType + QLatin1String("Url"), QUrl::fromLocalFile(destIconFileName));
    }

    icon.setParameters(parameters);
    place.setIcon(icon);
    category.setIcon(icon);

    QString placeId;
    QString categoryId;

    if (errorCondition == "no error") {
        QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));
        QVERIFY(doSaveCategory(category, QPlaceReply::NoError, &categoryId));

        QPlace retrievedPlace;
        QPlaceCategory retrievedCategory;

        QVERIFY(doFetchDetails(placeId, &retrievedPlace));
        QVERIFY(doFetchCategory(categoryId, &retrievedCategory));
        QCOMPARE(retrievedPlace.icon().url(), retrievedCategory.icon().url());

        QImage image;
        if (destinationType == "dataUrl")
            image = dataUrlToImage(retrievedPlace.icon().url());
        else if (destinationType.startsWith("file"))
            image = QImage(retrievedPlace.icon().url().toLocalFile());

        QCOMPARE(image.size(), QSize(60, 65));
    } else if (errorCondition == "unreachable host") {
        QVERIFY(doSavePlace(place, QPlaceReply::CommunicationError));
        QVERIFY(doSaveCategory(category, QPlaceReply::CommunicationError));
    }

    //Hardening Extension: Potentially there could be more thorough tests here to handle
    //different icon sizes coming from the server
}

void tst_QPlaceManagerJsonDb::iconDownload_data()
{
    QTest::addColumn<QString>("sizeType");
    QTest::addColumn<QString>("destinationType");
    QTest::addColumn<QString>("errorCondition");

    QStringList sizeTypes;
    sizeTypes << JsonDbUtils::Small << JsonDbUtils::Medium << JsonDbUtils::Large;

    QStringList destinationTypes;
    destinationTypes << QLatin1String("dataUrl") << QLatin1String("file new") << QLatin1String("file existing");

    QStringList errorConditions;
    errorConditions << QLatin1String("no error") << QLatin1String("unreachable host");

    foreach (const QString &sizeType, sizeTypes) {
        foreach (const QString &destinationType, destinationTypes) {
            foreach (const QString &errorCondition, errorConditions) {
                QByteArray dataTag = (sizeType + QLatin1String(" ") + destinationType + QLatin1String(" ") + errorCondition).toLatin1();
                QTest::newRow(dataTag.constData()) << sizeType << destinationType << errorCondition;
            }
        }
    }
}
#endif

void tst_QPlaceManagerJsonDb::constructIconUrl()
{
    QPlaceIcon icon;
    QVariantMap iconParams;

    //try with all possible size buckets set
    iconParams.insert(SmallDestination, QUrl("http://www.example.com/icon_small.png"));
    iconParams.insert(SmallDestinationSize, SmallSize);
    iconParams.insert(MediumDestination, QUrl("http://www.example.com/icon_medium.png"));
    iconParams.insert(MediumDestinationSize, MediumSize);
    iconParams.insert(LargeDestination, QUrl("http://www.example.com/icon_large.png"));
    iconParams.insert(LargeDestinationSize, LargeSize);
    iconParams.insert(FullscreenDestination, QUrl("http://www.example.com/icon_fullscreen.png"));
    iconParams.insert(FullscreenDestinationSize, FullscreenSize);

    icon.setParameters(iconParams);
    icon.setManager(placeManager);

    QCOMPARE(icon.url(SmallSize), QUrl("http://www.example.com/icon_small.png"));
    QCOMPARE(icon.url(MediumSize), QUrl("http://www.example.com/icon_medium.png"));
    QCOMPARE(icon.url(LargeSize), QUrl("http://www.example.com/icon_large.png"));
    QCOMPARE(icon.url(FullscreenSize), QUrl("http://www.example.com/icon_fullscreen.png"));

    //try with only a single destination icon set
    iconParams.clear();
    iconParams.insert(LargeDestination, QUrl("http://www.example.com/icon_large.png"));
    iconParams.insert(LargeDestinationSize, LargeSize);
    icon.setParameters(iconParams);

    QCOMPARE(icon.url(SmallSize), QUrl("http://www.example.com/icon_large.png"));
    QCOMPARE(icon.url(MediumSize), QUrl("http://www.example.com/icon_large.png"));
    QCOMPARE(icon.url(LargeSize), QUrl("http://www.example.com/icon_large.png"));
    QCOMPARE(icon.url(FullscreenSize), QUrl("http://www.example.com/icon_large.png"));

    //try requesting non-standard sizes and don't populate all size buckets
    iconParams.clear();
    iconParams.insert(SmallDestination, QUrl("http://www.example.com/icon_small.png"));
    iconParams.insert(SmallDestinationSize, SmallSize);
    iconParams.insert(LargeDestination, QUrl("http://www.example.com/icon_large.png"));
    iconParams.insert(LargeDestinationSize, LargeSize);
    icon.setParameters(iconParams);

    QCOMPARE(icon.url(QSize(10,10)), QUrl("http://www.example.com/icon_small.png"));
    QCOMPARE(icon.url(QSize(34,34)), QUrl("http://www.example.com/icon_small.png"));
    QCOMPARE(icon.url(QSize(35,35)), QUrl("http://www.example.com/icon_large.png"));
    QCOMPARE(icon.url(QSize(60,60)), QUrl("http://www.example.com/icon_large.png"));

    //try the icons themselves haveing non standard sizes
    iconParams.clear();
    iconParams.insert(SmallDestination, QUrl("http://www.example.com/icon_small.png"));
    iconParams.insert(SmallDestinationSize, QSize(25,25));
    iconParams.insert(LargeDestination, QUrl("http://www.example.com/icon_large.png"));
    iconParams.insert(LargeDestinationSize, QSize(35,35));
    icon.setParameters(iconParams);

    QCOMPARE(icon.url(QSize(24,24)), QUrl("http://www.example.com/icon_small.png"));
    QCOMPARE(icon.url(QSize(29,29)), QUrl("http://www.example.com/icon_small.png"));
    QCOMPARE(icon.url(QSize(30,30)), QUrl("http://www.example.com/icon_large.png"));
    QCOMPARE(icon.url(QSize(60,60)), QUrl("http://www.example.com/icon_large.png"));

    //TODO: edge case testing for all combinations
}

void tst_QPlaceManagerJsonDb::providerIcons()
{
    QGeoServiceProvider *oldProvider = provider;
    QPlaceManager *oldManager = placeManager;

    QVariantMap providerParams;
    providerParams.insert(CustomIcons, false);
    providerParams.insert(QLatin1String("places.local_data_path"), tempDir.path());

    provider = new QGeoServiceProvider(QLatin1String("places_jsondb"), providerParams);
    placeManager = provider->placeManager();

    QPlaceReply * catInitReply = placeManager->initializeCategories();
    QVERIFY(checkSignals(catInitReply, QPlaceReply::NoError));

    QMap<QString,QVariant> credentials;
    credentials.insert("app_id", "stub");
    credentials.insert("token", "stub");
    QGeoServiceProvider *nokiaProvider = new QGeoServiceProvider(QLatin1String("nokia"), credentials);
    QPlaceManager *nokiaManager = nokiaProvider->placeManager();
    Q_ASSERT(nokiaManager);

    QPlaceIcon icon;
    QVariantMap params;
    QPlaceCategory accommodation;
    accommodation.setCategoryId("accommodation");
    params.insert(Nokiaicon, QLatin1String("/icons/categories/01.icon"));
    params.insert(IconPrefix, QLatin1String("www.server.com/foo"));
    icon.setParameters(params);
    icon.setManager(nokiaManager);
    accommodation.setIcon(icon);

    QPlaceCategory businessIndustry;
    businessIndustry.setCategoryId("business-industry");
    params.insert(Nokiaicon, QLatin1String("/icons/categories/02.icon"));
    params.insert(IconPrefix, QLatin1String("www.server.com/foo"));
    icon.setParameters(params);
    icon.setManager(nokiaManager);
    businessIndustry.setIcon(icon);

    //Check the compatible place for an icon that was generated from its categories
    QPlace place;
    place.setName(QStringLiteral("Gotham City"));
    QGeoCoordinate coord(31.516, 74.316);
    QGeoLocation location;
    location.setCoordinate(coord);
    place.setLocation(location);
    place.setCategory(accommodation);

    QPlaceAttribute providerAttr;
    providerAttr.setText("nokia");
    place.setExtendedAttribute(Provider, providerAttr);

    params.clear();
    params.insert(Nokiaicon, QLatin1String("/icons/categories/01.icon"));
    params.insert(IconPrefix, QLatin1String("www.server.com/foo"));
    params.insert(NokiaIconGenerated, true);
    icon.setParameters(params);

    place.setIcon(icon);

    QPlace compatiblePlace = placeManager->compatiblePlace(place);
    QVariantMap compatParams = compatiblePlace.icon().parameters();
    QCOMPARE(compatParams.value(Nokiaicon).toString(),
            QLatin1String("/icons/categories/01.icon"));

    QCOMPARE(compatParams.value(NokiaIconGenerated).toBool(), true);

    QString placeId;
    QVERIFY(doSavePlace(compatiblePlace, QPlaceReply::NoError, &placeId));
    compatiblePlace.setPlaceId(placeId);
    QPlace retrievedPlace;
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));

    QCOMPARE(retrievedPlace.icon().url(),
             QUrl(QString::fromLatin1("file://")
                  + tempDir.path() + QLatin1String("/icons/categories/01.icon")));
    QCOMPARE(retrievedPlace.icon().parameters().value(NokiaIconGenerated).toBool(), true);

    //Try explicitly setting an icon that is not generated.
    place.setIcon(businessIndustry.icon());
    compatiblePlace = placeManager->compatiblePlace(place);
    compatParams = compatiblePlace.icon().parameters();
    QCOMPARE(compatParams.value(Nokiaicon).toString(),
            QLatin1String("/icons/categories/02.icon"));

    QVERIFY(doSavePlace(compatiblePlace, QPlaceReply::NoError, &placeId));

    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QCOMPARE(retrievedPlace.icon().url(),
             QUrl(QString::fromLatin1("file://")
                  + tempDir.path() + QLatin1String("/icons/categories/02.icon")));
    QCOMPARE(retrievedPlace.icon().parameters().value(Nokiaicon).toString(),
            QLatin1String("/icons/categories/02.icon"));
    QVERIFY(!retrievedPlace.icon().parameters().contains(NokiaIconGenerated));


    QStringList sizes;
    sizes << QLatin1String("small") << QLatin1String("medium")
          << QLatin1String("large");
    foreach (const QString &size, sizes) {
        //save an icon with a custom field populated
        //ensure a generated provider icon is returned.
        params.clear();
        params.insert(size + QLatin1String("Url"), QUrl("file:///opt/icon.png"));
        params.insert(size + QLatin1String("Size"), QSize(40,40));
        icon.setParameters(params);
        icon.setManager(placeManager);
        compatiblePlace.setIcon(icon);
        QVERIFY(doSavePlace(compatiblePlace, QPlaceReply::NoError, &placeId));
        QVERIFY(doFetchDetails(placeId, &retrievedPlace));
        QCOMPARE(retrievedPlace.icon().url(),
                 QUrl(QString::fromLatin1("file://")
                      + tempDir.path() + QLatin1String("/icons/categories/01.icon")));
        QCOMPARE(retrievedPlace.icon().parameters().value(NokiaIconGenerated).toBool(), true);

        //verify that retriving the same place via a plugin configured to return custom
        //icons will return the custom icon
        QVERIFY(doFetchDetails(oldManager, placeId, &retrievedPlace));
        QCOMPARE(retrievedPlace.icon().url(), QUrl("file:///opt/icon.png"));
        QCOMPARE(retrievedPlace.icon().parameters()
                                             .value(size + QLatin1String("Url")).toUrl(),
                 QUrl("file:///opt/icon.png"));

        //save an icon with a custom icon populated and a provider icon explicitly populated
        //ensure the explicitly set provider icon is returned.
        params.clear();
        params.insert(size + QLatin1String("Url"), QUrl("file:///opt/icon.png"));
        params.insert(size + QLatin1String("Size"), QSize(40,40));
        params.insert(Nokiaicon, QLatin1String("/icons/categories/02.icon"));

        icon.setParameters(params);
        icon.setManager(placeManager);
        compatiblePlace.setIcon(icon);

        QVERIFY(doSavePlace(compatiblePlace, QPlaceReply::NoError, &placeId));
        QVERIFY(doFetchDetails(placeId, &retrievedPlace));
        QCOMPARE(retrievedPlace.icon().url(),
                 QUrl(QString::fromLatin1("file://")
                      + tempDir.path() + QLatin1String("/icons/categories/02.icon")));
        QCOMPARE(retrievedPlace.icon().parameters().value(NokiaIconGenerated).toBool(),
                 false);

        //verify that retriving the same place via a plugin configured to return custom
        //icons will return the custom icon
        QVERIFY(doFetchDetails(oldManager, placeId, &retrievedPlace));
        QCOMPARE(retrievedPlace.icon().url(), QUrl("file:///opt/icon.png"));
        QCOMPARE(retrievedPlace.icon().parameters()
                                             .value(size + QLatin1String("Url")).toUrl(),
                 QUrl("file:///opt/icon.png"));
    }

    //try saving a place with no icon set and no provider category information either
    //we expect that a default icon is returned
    place.setIcon(QPlaceIcon());
    place.setCategories(QList<QPlaceCategory>());
    compatiblePlace = placeManager->compatiblePlace(place);
    QVERIFY(compatiblePlace.icon().isEmpty());
    QVERIFY(!compatiblePlace.extendedAttributeTypes()
            .contains(QLatin1String("x_nokia_category_ids")));

    QVERIFY(doSavePlace(compatiblePlace, QPlaceReply::NoError, &placeId));
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));

    QCOMPARE(retrievedPlace.icon().url(),
             QUrl(QString::fromLatin1("file://")
                  + tempDir.path() + QLatin1String("/icons/categories/06.icon")));
    QCOMPARE(retrievedPlace.icon().parameters().value(NokiaIconGenerated).toBool(),
             true);

    //restore old state
    delete provider;

    provider = oldProvider;
    placeManager = oldManager;
}

void tst_QPlaceManagerJsonDb::specifiedPartition()
{
    QSKIP("The behavior for creating partitions has changed, test needs to be updated");

    const QLatin1String ArbitraryPartition("com.nokia.arbitraryPartition");
    dbUtils->setupPartition(ArbitraryPartition);
    dbUtils->setCurrentPartition(ArbitraryPartition);
    QSignalSpy spy(dbUtils, SIGNAL(partitionSetupDone()));
    QTRY_VERIFY_WITH_TIMEOUT(spy.count() ==1, 10000);

    QGeoServiceProvider *oldProvider = provider;
    QPlaceManager *oldManager = placeManager;

    QVariantMap parameters;
    parameters.insert(QLatin1String("partition"), ArbitraryPartition);
    provider = new QGeoServiceProvider(QLatin1String("places_jsondb"), parameters);
    placeManager = provider->placeManager();

    QSignalSpy createPlaceSpy(placeManager, SIGNAL(placeAdded(QString)));
    QSignalSpy updatePlaceSpy(placeManager, SIGNAL(placeUpdated(QString)));
    QSignalSpy removePlaceSpy(placeManager, SIGNAL(placeRemoved(QString)));

    QSignalSpy createCategorySpy(placeManager, SIGNAL(categoryAdded(QPlaceCategory,QString)));
    QSignalSpy updateCategorySpy(placeManager, SIGNAL(categoryUpdated(QPlaceCategory,QString)));
    QSignalSpy removeCategorySpy(placeManager, SIGNAL(categoryRemoved(QString,QString)));

    //Test saving, updating and removing a place
    QPlace place;
    place.setName(QLatin1String("Place"));
    QGeoLocation location;
    location.setCoordinate(QGeoCoordinate(10,20));

    QString placeId;
    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));
    QCOMPARE(createPlaceSpy.count(), 1);
    place.setPlaceId(placeId);
    place.setVisibility(QLocation::DeviceVisibility);

    QPlace retrievedPlace;
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QVERIFY(retrievedPlace == place);

    place.setName(QLatin1String("place2"));
    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId));
    QCOMPARE(updatePlaceSpy.count(), 1);
    QVERIFY(doFetchDetails(placeId, &retrievedPlace));
    QVERIFY(retrievedPlace == place);

    QVERIFY(doRemovePlace(place, QPlaceReply::NoError));
    QCOMPARE(removePlaceSpy.count(), 1);

    //test saving, updating and removing a category
    QString categoryId;
    QPlaceCategory restaurant;
    restaurant.setName(QLatin1String("Restaurant"));

    QVERIFY(doSaveCategory(restaurant, QPlaceReply::NoError, &categoryId));
    QCOMPARE(createCategorySpy.count(), 1);
    restaurant.setCategoryId(categoryId);
    QPlaceReply * catInitReply = placeManager->initializeCategories();
    QVERIFY(checkSignals(catInitReply, QPlaceReply::NoError));

    QList<QPlaceCategory> categories = placeManager->childCategories();
    QCOMPARE(categories.count(), 1);

    restaurant.setName(QLatin1String("Restaurant2"));
    QVERIFY(doSaveCategory(restaurant, QPlaceReply::NoError, &categoryId));
    QCOMPARE(updateCategorySpy.count(), 1);
    catInitReply = placeManager->initializeCategories();
    QVERIFY(checkSignals(catInitReply, QPlaceReply::NoError));

    categories = placeManager->childCategories();
    QCOMPARE(categories.count(), 1);

    QVERIFY(doRemoveCategory(restaurant));
    QCOMPARE(removeCategorySpy.count(), 1);

    QSignalSpy cleanSpy(dbUtils, SIGNAL(dbCleaned()));
    dbUtils->cleanDb();
    QTRY_VERIFY(cleanSpy.count() == 1);

    //restore old state
    delete provider;

    provider = oldProvider;
    placeManager = oldManager;
    dbUtils->setCurrentPartition(JsonDbUtils::DefaultPartition);
}

void tst_QPlaceManagerJsonDb::validateIndexes()
{
    QPlace place;
    place.setName(QLatin1String("Winterfell"));
    QGeoLocation location;

    location.setCoordinate(QGeoCoordinate(1,-30));
    place.setLocation(location);

    QString placeId1;
    QVERIFY(doSavePlace(place, QPlaceReply::NoError, &placeId1));

    QPlace place2;
    place2.setName(QLatin1String("Casterly Rock"));
    location.setCoordinate(QGeoCoordinate(11,-20));
    place2.setLocation(location);

    QString placeId2;
    QVERIFY(doSavePlace(place2, QPlaceReply::NoError, &placeId2));

    QPlace place3;
    place3.setName(QLatin1String("Qarth"));
    location.setCoordinate(QGeoCoordinate(5,5));
    place3.setLocation(location);

    QString placeId3;
    QVERIFY(doSavePlace(place3, QPlaceReply::NoError, &placeId3));

    QPlace place4;
    place4.setName(QLatin1String("Wall"));
    location.setCoordinate(QGeoCoordinate(7,20));
    place4.setLocation(location);

    QString placeId4;
    QVERIFY(doSavePlace(place4, QPlaceReply::NoError, &placeId4));

    //validate latitude index
    QJsonDbReadRequest *request = new QJsonDbReadRequest();
    request->setQuery(QStringLiteral("[?_type=%type][?placeLatitudeIndex < 10]"));
    request->bindValue(QStringLiteral("type"), JsonDbUtils::PlaceType);
    dbUtils->sendRequest(request);

    QSignalSpy readSpy(request, SIGNAL(finished()));
    WAIT_UNTIL(readSpy.count() == 1);
    readSpy.clear();
    QCOMPARE(request->takeResults().count(), 3);
    QCOMPARE(request->sortKey(), JsonDbUtils::LatitudeIndex);

    //validate longitude index
    request->setQuery(QStringLiteral("[?_type=%type][?placeLongitudeIndex < -25]"));
    request->bindValue(QStringLiteral("type"), JsonDbUtils::PlaceType);
    dbUtils->sendRequest(request);

    WAIT_UNTIL(readSpy.count() == 1);
    readSpy.clear();
    QCOMPARE(request->takeResults().count(), 1);
    QCOMPARE(request->sortKey(), JsonDbUtils::LongitudeIndex);

    //verify place display name index
    request->setQuery(QStringLiteral("[?_type=%type][/placeDisplayNameIndex]"));
    request->bindValue(QStringLiteral("type"), JsonDbUtils::PlaceType);
    dbUtils->sendRequest(request);

    WAIT_UNTIL(readSpy.count() == 1);
    QList<QJsonObject> results= request->takeResults();
    QCOMPARE(results.count(), 4);
    QCOMPARE(results.at(0).value(JsonDbUtils::Uuid).toString(), placeId2);
    QCOMPARE(results.at(1).value(JsonDbUtils::Uuid).toString(), placeId3);
    QCOMPARE(results.at(2).value(JsonDbUtils::Uuid).toString(), placeId4);
    QCOMPARE(results.at(3).value(JsonDbUtils::Uuid).toString(), placeId1);

    QCOMPARE(request->sortKey(), JsonDbUtils::PlaceNameIndex);
}

void tst_QPlaceManagerJsonDb::cleanup()
{
    QSignalSpy cleanSpy(dbUtils, SIGNAL(dbCleaned()));
    dbUtils->cleanDb();
    QTRY_VERIFY_WITH_TIMEOUT(cleanSpy.count() == 1, Timeout);
}

QImage tst_QPlaceManagerJsonDb::dataUrlToImage(const QUrl &url)
{
    QByteArray data = QByteArray::fromPercentEncoding(url.toEncoded());
    data.remove(0,5);
    int pos = data.indexOf(',');
    if (pos != -1) {
        QByteArray payload = QByteArray::fromBase64(data.mid(pos + 1));
        data.truncate(pos);

        if (!data.endsWith(";base64")) {
            qWarning() << "Data url payload not base64 encoded";
            return QImage();
        }

         return QImage::fromData(payload);
    }

    return QImage();
}

QTEST_APPLESS_MAIN(tst_QPlaceManagerJsonDb)

#include "tst_qplacemanager_jsondb.moc"
