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

#include <QtTest/QtTest>
#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeComponent>
#include <QtLocation/QGeoCoordinate>
#include <QtLocation/QGeoAddress>
#include <QtLocation/QGeoBoundingBox>
#include <QtLocation/QGeoBoundingCircle>
#include <QtLocation/QGeoLocation>
#include <QtLocation/QPlaceCategory>
#include <QtLocation/QPlace>
#include <QtLocation/QPlaceIcon>
#include <QtLocation/QPlaceRatings>
#include <QtLocation/QPlaceSupplier>
#include <QtLocation/QPlaceUser>
#include <QtLocation/QPlaceAttribute>
#include <QtLocation/QPlaceContactDetail>

class tst_qmlinterface : public QObject
{
    Q_OBJECT

public:
    tst_qmlinterface();

private Q_SLOTS:
    void testCoordinate();
    void testAddress();
    void testBoundingBox();
    void testBoundingCircle();
    void testLocation();
    void testCategory();
    void testIcon();
    void testRatings();
    void testSupplier();
    void testUser();
    void testPlaceAttribute();
    void testContactDetail();
    void testPlace();

private:
    QGeoCoordinate m_coordinate;
    QGeoAddress m_address;
    QGeoBoundingBox m_boundingBox;
    QGeoBoundingCircle m_boundingCircle;
    QGeoLocation m_location;
    QPlaceCategory m_category;
    QPlaceIcon m_icon;
    QPlaceRatings m_ratings;
    QPlaceSupplier m_supplier;
    QPlaceUser m_user;
    QPlaceAttribute m_placeAttribute;
    QPlaceContactDetail m_contactDetail;
    QList<QPlaceCategory> m_categories;
    QPlace m_place;
};

tst_qmlinterface::tst_qmlinterface()
{
    m_coordinate.setLongitude(10.0);
    m_coordinate.setLatitude(20.0);
    m_coordinate.setAltitude(30.0);

    m_address.setCity(QLatin1String("Brisbane"));
    m_address.setCountry(QLatin1String("Australia"));
    m_address.setCountryCode(QLatin1String("AU"));
    m_address.setPostcode(QLatin1String("4000"));
    m_address.setState(QLatin1String("Queensland"));
    m_address.setStreet(QLatin1String("123 Fake Street"));

    m_boundingBox.setCenter(m_coordinate);
    m_boundingBox.setHeight(30.0);
    m_boundingBox.setWidth(40.0);

    m_boundingCircle.setCenter(m_coordinate);
    m_boundingCircle.setRadius(30.0);

    m_location.setAddress(m_address);
    m_location.setBoundingBox(m_boundingBox);
    m_location.setCoordinate(m_coordinate);

    m_category.setName(QLatin1String("Test category"));
    m_category.setCategoryId(QLatin1String("test-category-id"));

    m_icon.setFullUrl(QUrl(QLatin1String("http://www.example.com/test-icon.png")));

    m_ratings.setValue(3.5);
    m_ratings.setMaximum(5.0);
    m_ratings.setCount(10);

    m_supplier.setName(QLatin1String("Test supplier"));
    m_supplier.setUrl(QUrl(QLatin1String("http://www.example.com/test-supplier")));
    m_supplier.setSupplierId(QLatin1String("test-supplier-id"));
    m_supplier.setIcon(m_icon);

    m_user.setName(QLatin1String("Test User"));
    m_user.setUserId(QLatin1String("test-user-id"));

    m_placeAttribute.setLabel(QLatin1String("Test Attribute"));
    m_placeAttribute.setText(QLatin1String("Test attribute text"));

    m_contactDetail.setLabel(QLatin1String("Test Contact Detail"));
    m_contactDetail.setValue(QLatin1String("Test contact detail value"));

    QPlaceCategory category;
    category.setName(QLatin1String("Test category 1"));
    category.setCategoryId(QLatin1String("test-category-id-1"));
    m_categories.append(category);
    category.setName(QLatin1String("Test category 2"));
    category.setCategoryId(QLatin1String("test-category-id-2"));
    m_categories.append(category);

    m_place.setName(QLatin1String("Test Place"));
    m_place.setPlaceId(QLatin1String("test-place-id"));
    m_place.setAttribution(QLatin1String("Place data by Foo"));
    m_place.setCategories(m_categories);
    m_place.setLocation(m_location);
    m_place.setRatings(m_ratings);
    m_place.setIcon(m_icon);
    m_place.setSupplier(m_supplier);
    m_place.setVisibility(QtLocation::PrivateVisibility);
}

void tst_qmlinterface::testCoordinate()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, SRCDIR "data/TestCoordinate.qml");
    QVERIFY(component.isReady());
    QObject *qmlObject = component.create();

    QGeoCoordinate coordinate = qmlObject->property("coordinate").value<QGeoCoordinate>();

    QCOMPARE(coordinate, m_coordinate);
}

void tst_qmlinterface::testAddress()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, SRCDIR "data/TestAddress.qml");
    QVERIFY(component.isReady());
    QObject *qmlObject = component.create();

    QGeoAddress address = qmlObject->property("address").value<QGeoAddress>();

    QCOMPARE(address, m_address);
}

void tst_qmlinterface::testBoundingBox()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, SRCDIR "data/TestBoundingBox.qml");
    QVERIFY(component.isReady());
    QObject *qmlObject = component.create();

    QGeoBoundingBox boundingBox = qmlObject->property("box").value<QGeoBoundingBox>();

    QCOMPARE(boundingBox, m_boundingBox);
}

void tst_qmlinterface::testBoundingCircle()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, SRCDIR "data/TestBoundingCircle.qml");
    QVERIFY(component.isReady());
    QObject *qmlObject = component.create();

    QGeoBoundingCircle boundingCircle = qmlObject->property("circle").value<QGeoBoundingCircle>();

    QCOMPARE(boundingCircle, m_boundingCircle);
}

void tst_qmlinterface::testLocation()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, SRCDIR "data/TestLocation.qml");
    QVERIFY(component.isReady());
    QObject *qmlObject = component.create();

    QGeoLocation location = qmlObject->property("location").value<QGeoLocation>();

    QCOMPARE(location, m_location);
}

void tst_qmlinterface::testCategory()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, SRCDIR "data/TestCategory.qml");
    QVERIFY(component.isReady());
    QObject *qmlObject = component.create();

    QPlaceCategory category = qmlObject->property("category").value<QPlaceCategory>();

    QCOMPARE(category, m_category);
}

void tst_qmlinterface::testIcon()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, SRCDIR "data/TestIcon.qml");
    QVERIFY(component.isReady());
    QObject *qmlObject = component.create();

    QPlaceIcon icon = qmlObject->property("icon").value<QPlaceIcon>();

    QCOMPARE(icon, m_icon);
}

void tst_qmlinterface::testRatings()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, SRCDIR "data/TestRatings.qml");
    QVERIFY(component.isReady());
    QObject *qmlObject = component.create();

    QPlaceRatings ratings = qmlObject->property("ratings").value<QPlaceRatings>();

    QCOMPARE(ratings, m_ratings);
}

void tst_qmlinterface::testSupplier()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, SRCDIR "data/TestSupplier.qml");
    QVERIFY(component.isReady());
    QObject *qmlObject = component.create();

    QPlaceSupplier supplier = qmlObject->property("supplier").value<QPlaceSupplier>();

    QCOMPARE(supplier, m_supplier);
}

void tst_qmlinterface::testUser()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, SRCDIR "data/TestUser.qml");
    QVERIFY(component.isReady());
    QObject *qmlObject = component.create();

    QPlaceUser user = qmlObject->property("user").value<QPlaceUser>();

    QCOMPARE(user, m_user);
}

void tst_qmlinterface::testPlaceAttribute()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, SRCDIR "data/TestPlaceAttribute.qml");
    QVERIFY(component.isReady());
    QObject *qmlObject = component.create();

    QPlaceAttribute placeAttribute = qmlObject->property("attribute").value<QPlaceAttribute>();

    QCOMPARE(placeAttribute, m_placeAttribute);
}

void tst_qmlinterface::testContactDetail()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, SRCDIR "data/TestContactDetail.qml");
    QVERIFY(component.isReady());
    QObject *qmlObject = component.create();

    QPlaceContactDetail contactDetail = qmlObject->property("contactDetail").value<QPlaceContactDetail>();

    QCOMPARE(contactDetail, m_contactDetail);
}

void tst_qmlinterface::testPlace()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, SRCDIR "data/TestPlace.qml");
    QVERIFY(component.isReady());
    QObject *qmlObject = component.create();

    QPlace place = qmlObject->property("place").value<QPlace>();

    QCOMPARE(place, m_place);
}

QTEST_MAIN(tst_qmlinterface)

#include "tst_qmlinterface.moc"
