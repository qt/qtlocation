/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#include "tst_qgeolocation.h"

QTM_USE_NAMESPACE

tst_QGeoLocation::tst_QGeoLocation()
    : m_locationScore(0)
{
}

void tst_QGeoLocation::initTestCase()
{

}

void tst_QGeoLocation::cleanupTestCase()
{

}

void tst_QGeoLocation::init()
{
}

void tst_QGeoLocation::cleanup()
{
}

void tst_QGeoLocation::constructor()
{
    QCOMPARE(m_location.address(), m_address);
    QCOMPARE(m_location.coordinate(), m_coordinate);
    QCOMPARE(m_location.viewport(), m_viewport);
    QCOMPARE(m_location.additionalData().count(), 0);
    QCOMPARE(m_location.additionalData(), m_additionalData );
    QCOMPARE(m_location.alternativeLabels().count(), 0);
    QCOMPARE(m_location.alternativeLabels(), m_alternativeLabels);
    QCOMPARE(m_location.label(), m_label);
    QCOMPARE(m_location.locationId(), m_locationId);
    QCOMPARE(m_location.locationScore(), m_locationScore);
}

void tst_QGeoLocation::copy_constructor()
{
    QGeoLocation *qgeolocationcopy = new QGeoLocation (m_location);
    QCOMPARE(m_location, *qgeolocationcopy);
    delete qgeolocationcopy;
}

void tst_QGeoLocation::destructor()
{
    QGeoLocation *qgeolocationcopy;

    QLocationTestUtils::uheap_mark();
    qgeolocationcopy = new QGeoLocation();
    delete qgeolocationcopy;
    QLocationTestUtils::uheap_mark_end();

    QLocationTestUtils::uheap_mark();
    qgeolocationcopy = new QGeoLocation(m_location);
    delete qgeolocationcopy;
    QLocationTestUtils::uheap_mark_end();
}

void tst_QGeoLocation::address()
{
    m_address.setCity("Berlin");
    m_address.setCountry("Germany");
    m_address.setCountryCode("DEU");
    m_address.setDistrict("Mitte");
    m_address.setPostcode("10115");
    m_address.setStreet("Invalidenstrasse");

    m_location.setAddress(m_address);

    QCOMPARE(m_location.address(),m_address);

    m_address.setPostcode("10125");
    QVERIFY(m_location.address() != m_address);
}

void tst_QGeoLocation::coordinate()
{
    m_coordinate.setLatitude(13.3851);
    m_coordinate.setLongitude(52.5312);
    m_coordinate.setAltitude(134.23);

    m_location.setCoordinate(m_coordinate);

    QCOMPARE(m_location.coordinate(), m_coordinate);

    m_coordinate.setAltitude(0);
    QVERIFY(m_location.coordinate() != m_coordinate);
}

void tst_QGeoLocation::viewport()
{
    m_coordinate.setLatitude(13.3851);
    m_coordinate.setLongitude(52.5312);

    QGeoBoundingBox qgeoboundingboxcopy(m_coordinate, 0.4, 0.4);
    m_location.setViewport(qgeoboundingboxcopy);

    QCOMPARE(m_location.viewport(),qgeoboundingboxcopy);

    qgeoboundingboxcopy.setHeight(1);

    QVERIFY(m_location.viewport() != qgeoboundingboxcopy);
}

void tst_QGeoLocation::additionalData()
{
    m_additionalData.insert("key1", "value1");
    m_additionalData.insert("key2", "value2");

    m_location.setAdditionalData(m_additionalData);
    QCOMPARE(m_location.additionalData().count(), 2);
    QCOMPARE(m_location.additionalData(), m_additionalData);

    m_additionalData.insert("key3", "value3");
    QVERIFY(m_location.additionalData() != m_additionalData);
}

void tst_QGeoLocation::alternativeLabels()
{
    QPlaceAlternativeValue altValue;
    m_alternativeLabels.append(altValue);

    altValue.setKey("testData");
    altValue.setValue("value");
    m_alternativeLabels.append(altValue);

    m_location.setAlternativeLabels(m_alternativeLabels);
    QCOMPARE(m_location.alternativeLabels(), m_alternativeLabels);

    altValue.setKey("newKey");
    m_alternativeLabels.append(altValue);
    QVERIFY(m_location.alternativeLabels() != m_alternativeLabels);
}

void tst_QGeoLocation::navigationPositions()
{
    QGeoCoordinate coordinate1, coordinate2;
    coordinate1.setLatitude(10);
    coordinate1.setLongitude(10);
    coordinate2.setLatitude(2);
    coordinate2.setLongitude(2);
    m_navigationPositions.append(coordinate1);
    m_navigationPositions.append(coordinate2);

    m_location.setNavigationPositions(m_navigationPositions);
    QCOMPARE(m_location.navigationPositions(), m_navigationPositions);

    m_navigationPositions.append(coordinate1);
    QVERIFY(m_location.navigationPositions() != m_navigationPositions);
}

void tst_QGeoLocation::label()
{
    m_label = "testText";
    m_location.setLabel(m_label);
    QCOMPARE(m_location.label(), m_label);

    m_label = "differentText";
    QVERIFY(m_location.label() != m_label);
}

void tst_QGeoLocation::locationId()
{
    m_locationId = "id";
    m_location.setLocationId(m_locationId);

    m_locationId = "new id";
    m_location.setLocationId(m_locationId);
}

void tst_QGeoLocation::locationScore()
{
    m_locationScore = 4.5;
    m_location.setLocationScore(m_locationScore);
    QCOMPARE(m_location.locationScore(), m_locationScore);

    m_locationScore = 2;
    QVERIFY(m_location.locationScore() != m_locationScore);
}

void tst_QGeoLocation::operators()
{
    QGeoAddress qgeoaddresscopy;
    qgeoaddresscopy.setCity("Berlin");
    qgeoaddresscopy.setCountry("Germany");
    qgeoaddresscopy.setCountryCode("DEU");

    QGeoCoordinate qgeocoordinatecopy (32.324 , 41.324 , 24.55);

    m_address.setCity("Madrid");
    m_address.setCountry("Spain");
    m_address.setCountryCode("SPA");

    m_coordinate.setLatitude(21.3434);
    m_coordinate.setLongitude(38.43443);
    m_coordinate.setAltitude(634.21);

    m_location.setAddress(m_address);
    m_location.setCoordinate(m_coordinate);

    //Create a copy and see that they are the same
    QGeoLocation qgeolocationcopy(m_location);
    QVERIFY(m_location == qgeolocationcopy);
    QVERIFY(!(m_location != qgeolocationcopy));

    //Modify one and test if they are different
   qgeolocationcopy.setAddress(qgeoaddresscopy);
   QVERIFY(!(m_location == qgeolocationcopy));
   QVERIFY(m_location != qgeolocationcopy);
   qgeolocationcopy.setCoordinate(qgeocoordinatecopy);
   QVERIFY(!(m_location == qgeolocationcopy));
   QVERIFY(m_location != qgeolocationcopy);

    //delete qgeolocationcopy;
    //Asign and test that they are the same
    qgeolocationcopy = m_location;
    QVERIFY(m_location ==qgeolocationcopy);
    QVERIFY(!(m_location != qgeolocationcopy));
}

void tst_QGeoLocation::comparison()
{
    QFETCH(QString, dataField);

    QGeoLocation location;

    //set address
    QGeoAddress address;
    address.setStreet("21 jump st");
    address.setCountry("USA");
    location.setAddress(address);

    //set coordinate
    location.setCoordinate(QGeoCoordinate(5,10));

    //set viewport
    location.setViewport(QGeoBoundingBox(QGeoCoordinate(5,5),0.4,0.4));

    //set additionalData
    QVariantHash additionalData;
    additionalData.insert("key", "value");
    location.setAdditionalData(additionalData);

    //set alternative labels
    QPlaceAlternativeValue altValue;
    QList<QPlaceAlternativeValue> alternativeLabels;

    altValue.setKey("testData");
    altValue.setValue("value");
    alternativeLabels.append(altValue);

    //set navigation positions
    QList<QGeoCoordinate> navPos;
    navPos.append(QGeoCoordinate(10,2));
    location.setNavigationPositions(navPos);

    //set label
    location.setLabel("label");

    //set locationId
    location.setLocationId("id");

    //set locationScore
    location.setLocationScore(2.5);

    QGeoLocation otherLocation(location);

    if (dataField == "no change") {
        QCOMPARE(location, otherLocation);
    } else {
        if (dataField == "address") {
            QGeoAddress otherAddress;
            otherAddress.setStreet("42 evergreen tce");
            otherAddress.setCountry("USA");
            otherLocation.setAddress(otherAddress);
        } else if (dataField == "coordinate") {
            otherLocation.setCoordinate(QGeoCoordinate(12,13));
        } else if (dataField == "viewport"){
            otherLocation.setViewport(QGeoBoundingBox(QGeoCoordinate(1,2), 0.5,0.5));
        } else if (dataField == "additionalData") {
            additionalData.insert("key", "otherValue");
            otherLocation.setAdditionalData(additionalData);
        } else if (dataField == "alternativeLabels") {
            altValue.setKey("otherKey");
            alternativeLabels.clear();
            alternativeLabels.append(altValue);
            otherLocation.setAlternativeLabels(alternativeLabels);
        } else if (dataField == "navigationPositions") {
            navPos.clear();
            navPos.append(QGeoCoordinate(-5, -5));
            otherLocation.setNavigationPositions(navPos);
        } else if (dataField == "label") {
            otherLocation.setLabel("otherLabel");
        } else if (dataField == "locationId") {
            otherLocation.setLocationId("otherId");
        } else if (dataField == "locationScore") {
            otherLocation.setLocationScore(1);
        }else {
            qFatal("Unknown data field to test");
        }

        QVERIFY(location != otherLocation);
    }
}

void tst_QGeoLocation::comparison_data()
{
    QTest::addColumn<QString> ("dataField");
    QTest::newRow("no change") << "no change";
    QTest::newRow("address") << "address";
    QTest::newRow("coordinate") << "coordinate";
    QTest::newRow("additionalData") << "additionalData";
    QTest::newRow("alternativeLabels") << "alternativeLabels";
    QTest::newRow("navigationPositions") << "navigationPositions";
    QTest::newRow("label") << "label";
    QTest::newRow("locationId") << "locationId";
    QTest::newRow("locationScore") << "locationScore";
}

QTEST_MAIN(tst_QGeoLocation);

