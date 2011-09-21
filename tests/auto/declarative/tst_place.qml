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

import QtQuick 2.0
import QtTest 1.0
import Qt.location 5.0

TestCase {
    id: testCase

    name: "Place"

    Plugin {
        id: testPlugin
        name: "qmlgeo.test.plugin"
    }

    Place { id: emptyPlace }

    Place { id: emptyPlace2 }

    Place { id: testPlace }

    Place {
        id: savePlace

        name: "Test place"

        primaryPhone: "11111111"
        primaryFax: "22222222"
        primaryEmail: "test-place@example.com"
        primaryUrl: "http://www.example.com/test-place"

        location: Location {
            locationId: "test-place-location-id"

            address: Address {
                country: "country"
                countryCode: "cc"
                state: "state"
                county: "county"
                city: "city"
                district: "district"
                street: "123 Fake Street"
                postcode: "1234"
            }

            coordinate: Coordinate {
                latitude: 10
                longitude: 10
                altitude: 100
            }

            viewport: BoundingBox {
                center: Coordinate {
                    latitude: 10
                    longitude: 10
                    altitude: 100
                }
                width: 100
                height: 100
            }
        }

        rating: Rating {
            value: 3.5
            count: 10
        }

        suppliers: [
            Supplier {
                name: "Supplier 1"
                supplierId: "supplier-id-1"
                url: "http://www.example.com/supplier-id-1/"
                supplierIconUrl: "http://www.example.com/supplier-id-1/icon"
            },
            Supplier {
                name: "Supplier 2"
                supplierId: "supplier-id-2"
                url: "http://www.example.com/supplier-id-2/"
                supplierIconUrl: "http://www.example.com/supplier-id-2/icon"
            }
        ]

        categories: [
            Category {
                name: "Category 1"
                categoryId: "category-id-1"
            },
            Category {
                name: "Category 2"
                categoryId: "category-id-2"
            }
        ]

        // fixme, how is this meant to work?
        /*extendedAttributes: ExtendedAttributes {
            foo: PlaceAttribute {
                text: "Attribute 1"
                label: "Attribute Label 1"
            }
            other: PlaceAttribute {
                text: "Attribute 2"
                label: "Attribute Label 2"
            }
        }*/
    }

    // compares two coordinates property by property
    function compare_coordinate(coord1, coord2) {
        return coord1.latitude === coord2.latitude &&
               coord1.longitude === coord2.longitude &&
               coord1.altitude === coord2.altitude;
    }

    // compares two places property by property
    function compare_place(place1, place2) {
        // check simple properties
        var simpleProperties = ["name", "placeId", "primaryPhone", "primaryFax", "primaryEmail",
                                "primaryUrl"];
        for (x in simpleProperties) {
            if (place1[simpleProperties[x]] !== place2[simpleProperties[x]])
                return false;
        }

        // check categories
        if (place1.categories.length !== place2.categories.length)
            return false;
        for (var i = 0; i < place1.categories.length; ++i) {
            // fixme, what if the order of the two lists are not the same
            if (place1.categories[i].categoryId !== place2.categories[i].categoryId)
                return false;
            if (place1.categories[i].name !== place2.categories[i].name)
                return false;
        }

        // check suppliers
        if (place1.suppliers.length !== place2.suppliers.length)
            return false;
        for (i = 0; i < place1.suppliers.length; ++i) {
            // fixme, what if the order of the two lists are not the same
            if (place1.suppliers[i].supplierId !== place2.suppliers[i].supplierId)
                return false;
            if (place1.suppliers[i].name !== place2.suppliers[i].name)
                return false;
            if (place1.suppliers[i].url !== place2.suppliers[i].url)
                return false;
            if (place1.suppliers[i].supplierIconUrl !== place2.suppliers[i].supplierIconUrl)
                return false;
        }

        // check rating
        if (place1.rating === null && place2.rating !== null)
            return false;
        if (place1.rating !== null && place2.rating === null)
            return false;
        if (place1.rating !== null && place2.rating !== null) {
            if (place1.rating.value !== place2.rating.value)
                return false;
            if (place1.rating.count !== place2.rating.count)
                return false;
        }

        // check location
        if (place1.location === null && place2.location !== null)
            return false;
        if (place1.location !== null && place2.location === null)
            return false;
        if (place1.location !== null && place2.location !== null) {
            if (place1.location.locationId !== place2.location.locationId)
                return false;
            if (place1.location.address.country !== place2.location.address.country)
                return false;
            if (place1.location.address.countryCode !== place2.location.address.countryCode)
                return false;
            if (place1.location.address.state !== place2.location.address.state)
                return false;
            if (place1.location.address.county !== place2.location.address.county)
                return false;
            if (place1.location.address.city !== place2.location.address.city)
                return false;
            if (place1.location.address.district !== place2.location.address.district)
                return false;
            if (place1.location.address.street !== place2.location.address.street)
                return false;
            if (place1.location.address.postcode !== place2.location.address.postcode)
                return false;

            /*
            if (!compare_coordinate(place1.location.coordinate, place2.location.coordinate))
                return false;
            console.log("location.coordinate is equal");
            if (!compare_coordinate(place1.location.viewport.bottomLeft, place2.location.viewport.bottomLeft))
                return false;
            console.log("location.viewport.bottomLeft is equal");
            if (!compare_coordinate(place1.location.viewport.bottomRight, place2.location.viewport.bottomRight))
                return false;
            console.log("location.viewport.bottomRight is equal");
            if (!compare_coordinate(place1.location.viewport.topLeft, place2.location.viewport.topLeft))
                return false;
            console.log("location.viewport.topLeft is equal");
            if (!compare_coordinate(place1.location.viewport.topRight, place2.location.viewport.topRight))
                return false;
            console.log("location.viewport.topRight is equal");
            if (!compare_coordinate(place1.location.viewport.center, place2.location.viewport.center))
                return false;
            console.log("location.viewport.center is equal");
            console.log(place1.location.viewport.height + " eq " + place2.location.viewport.height);
            if (place1.location.viewport.height !== place2.location.viewport.height)
                return false;
            console.log(place1.location.viewport.width + " eq " + place2.location.viewport.width);
            if (place1.location.viewport.width !== place2.location.viewport.width)
                return false;
            console.log("location.viewport is equal");
            */
        }

        // check extended attributes

        return true;
    }

    function test_emptyPlace() {
        // basic properties
        compare(emptyPlace.plugin, null);
        compare(emptyPlace.categories.length, 0);
        compare(emptyPlace.name, "");
        compare(emptyPlace.placeId, "");
        compare(emptyPlace.detailsFetched, false);
        compare(emptyPlace.status, Place.Ready);
        compare(emptyPlace.primaryPhone, "");
        compare(emptyPlace.primaryFax, "");
        compare(emptyPlace.primaryEmail, "");
        compare(emptyPlace.primaryUrl, "");

        // complex properties
        compare(emptyPlace.rating, null);
        compare(emptyPlace.location, null);
        compare(emptyPlace.suppliers.length, 0);
        compare(emptyPlace.reviewModel.totalCount, -1);
        compare(emptyPlace.imageModel.totalCount, -1);
        compare(emptyPlace.editorialModel.totalCount, -1);
        compare(emptyPlace.categories.length, 0);

        verify(compare_place(emptyPlace, emptyPlace));
        verify(compare_place(emptyPlace, emptyPlace2));
    }

    function test_basicProperties_data() {
        return [
            { tag: "name", property: "name", signal: "nameChanged", value: "Test Place", reset: "" },
            { tag: "placeId", property: "placeId", signal: "placeIdChanged", value: "test-place-id-1", reset: "" },
            { tag: "primaryPhone", property: "primaryPhone", signal: "primaryPhoneChanged", value: "11111111", reset: "" },
            { tag: "primaryFax", property: "primaryFax", signal: "primaryFaxChanged", value: "22222222", reset: "" },
            { tag: "primaryEmail", property: "primaryEmail", signal: "primaryEmailChanged", value: "test@example.com", reset: "" },
            { tag: "primaryUrl", property: "primaryUrl", signal: "primaryUrlChanged", value: "http://www.example.com/test-place-id-1", reset: "" },
        ];
    }

    function test_basicProperties(data) {
        var signalSpy = Qt.createQmlObject('import QtTest 1.0; SignalSpy {}', testCase, "SignalSpy");
        signalSpy.target = testPlace;
        signalSpy.signalName = data.signal;

        // set it property to something new
        testPlace[data.property] = data.value;
        compare(testPlace[data.property], data.value);
        compare(signalSpy.count, 1);

        // set it to the same (signal spy should not increase)
        testPlace[data.property] = data.value;
        compare(testPlace[data.property], data.value);
        compare(signalSpy.count, 1);

        // reset it
        testPlace[data.property] = data.reset;
        compare(testPlace[data.property], data.reset);
        compare(signalSpy.count, 2);

        signalSpy.destroy();
    }

    function test_categories() {
        var categories = new Array(2);
        categories[0] = Qt.createQmlObject('import Qt.location 5.0; Category { categoryId: "cat-id-1"; name: "Category 1" }', testCase, "Category1");
        categories[1] = Qt.createQmlObject('import Qt.location 5.0; Category { categoryId: "cat-id-2"; name: "Category 2" }', testCase, "Category2");

        var signalSpy = Qt.createQmlObject('import QtTest 1.0; SignalSpy {}', testCase, "SignalSpy");
        signalSpy.target = testPlace;
        signalSpy.signalName = "categoriesChanged";

        // set categories to something new
        testPlace.categories = categories;
        compare(testPlace.categories.length, categories.length);

        for (var i = 0; i < categories.length; ++i) {
            compare(testPlace.categories[i].categoryId, categories[i].categoryId);
            compare(testPlace.categories[i].name, categories[i].name);
        }

        compare(signalSpy.count, 2);

        // set categories to the same (signal spy should not increase?)
        testPlace.categories = categories;
        compare(testPlace.categories.length, categories.length);

        for (var i = 0; i < categories.length; ++i) {
            compare(testPlace.categories[i].categoryId, categories[i].categoryId);
            compare(testPlace.categories[i].name, categories[i].name);
        }

        compare(signalSpy.count, 5);    // clear + append + append

        // reset by assignment
        testPlace.categories = new Array(0);
        compare(testPlace.categories.length, 0);
        compare(signalSpy.count, 6);

        signalSpy.destroy();
    }

    function test_suppliers() {
        var suppliers = new Array(2);
        suppliers[0] = Qt.createQmlObject('import Qt.location 5.0; Supplier { supplierId: "sup-id-1"; name: "Category 1" }', testCase, "Supplier1");
        suppliers[1] = Qt.createQmlObject('import Qt.location 5.0; Supplier { supplierId: "sup-id-2"; name: "Category 2" }', testCase, "Supplier2");

        var signalSpy = Qt.createQmlObject('import QtTest 1.0; SignalSpy {}', testCase, "SignalSpy");
        signalSpy.target = testPlace;
        signalSpy.signalName = "suppliersChanged";

        // set suppliers to something new
        testPlace.suppliers = suppliers;
        compare(testPlace.suppliers.length, suppliers.length);

        for (var i = 0; i < suppliers.length; ++i) {
            compare(testPlace.suppliers[i].supplierId, suppliers[i].supplierId);
            compare(testPlace.suppliers[i].name, suppliers[i].name);
        }

        compare(signalSpy.count, 2);

        // set suppliers to the same (signal spy should not increase?)
        testPlace.suppliers = suppliers;
        compare(testPlace.suppliers.length, suppliers.length);

        for (var i = 0; i < suppliers.length; ++i) {
            compare(testPlace.suppliers[i].supplierId, suppliers[i].supplierId);
            compare(testPlace.suppliers[i].name, suppliers[i].name);
        }

        compare(signalSpy.count, 5);    // clear + append + append

        // reset by assignment
        testPlace.suppliers = new Array(0);
        compare(testPlace.suppliers.length, 0);
        compare(signalSpy.count, 6);

        signalSpy.destroy();
    }

    function test_location() {
        var location = Qt.createQmlObject('import Qt.location 5.0; Location { locationId: "loc-id-1" }', testCase, "Location1");

        var signalSpy = Qt.createQmlObject('import QtTest 1.0; SignalSpy {}', testCase, "SignalSpy");
        signalSpy.target = testPlace;
        signalSpy.signalName = "locationChanged";

        testPlace.location = location;
        compare(testPlace.location.locationId, "loc-id-1");
        compare(signalSpy.count, 1);

        testPlace.location = location;
        compare(testPlace.location.locationId, "loc-id-1");
        compare(signalSpy.count, 1);

        testPlace.location = null;
        compare(testPlace.location, null);
        compare(signalSpy.count, 2);

        location.destroy();
        signalSpy.destroy();
    }

    function test_rating() {
        var rating = Qt.createQmlObject('import Qt.location 5.0; Rating { value: 3; count: 100 }', testCase, "Rating1");

        var signalSpy = Qt.createQmlObject('import QtTest 1.0; SignalSpy {}', testCase, "SignalSpy");
        signalSpy.target = testPlace;
        signalSpy.signalName = "ratingChanged";

        testPlace.rating = rating;
        compare(testPlace.rating.value, 3);
        compare(testPlace.rating.count, 100);
        compare(signalSpy.count, 1);

        testPlace.rating = rating;
        compare(testPlace.rating.value, 3);
        compare(testPlace.rating.count, 100);
        compare(signalSpy.count, 1);

        testPlace.rating = null;
        compare(testPlace.rating, null);
        compare(signalSpy.count, 2);

        rating.destroy();
        signalSpy.destroy();
    }

    function test_extendedAttributes() {
        var signalSpy = Qt.createQmlObject('import QtTest 1.0; SignalSpy {}', testCase, "SignalSpy");
        signalSpy.target = testPlace;
        signalSpy.signalName = "extendedAttributesChanged";

        verify(testPlace.extendedAttributes);

        testPlace.extendedAttributes["foo"] = Qt.createQmlObject('import Qt.location 5.0; PlaceAttribute { text: "Foo"; label: "Foo label" }', testCase, 'PlaceAttribute');

        verify(testPlace.extendedAttributes.foo);
        compare(testPlace.extendedAttributes.foo.text, "Foo");
        compare(testPlace.extendedAttributes.foo.label, "Foo label");

        testPlace.extendedAttributes = Qt.createQmlObject('import Qt.location 5.0; ExtendedAttributes { }', testPlace, 'ExtendedAttributes1');

        compare(signalSpy.count, 1);
        verify(testPlace.extendedAttributes);
        compare(testPlace.extendedAttributes.foo, undefined);

        testPlace.extendedAttributes = testPlace.extendedAttributes
        compare(signalSpy.count, 1);

        signalSpy.destroy();
    }

    function test_saveload() {
        // Save a place
        var signalSpy = Qt.createQmlObject('import QtTest 1.0; SignalSpy {}', testCase, "SignalSpy");
        signalSpy.target = savePlace;
        signalSpy.signalName = "statusChanged";

        savePlace.plugin = testPlugin;
        savePlace.placeId = "invalid-place-id";

        savePlace.savePlace();

        compare(savePlace.status, Place.Saving);

        // SignalSpy.wait() doesn't seem to work here
        //signalSpy.wait();
        wait(0);

        compare(savePlace.status, Place.Error);

        // try again without an invalid placeId
        savePlace.placeId = "";
        savePlace.savePlace();

        compare(savePlace.status, Place.Saving);

        wait(0);

        compare(savePlace.status, Place.Ready);

        verify(savePlace.placeId !== "");

        signalSpy.destroy();


        // Read a place
        var readPlace = Qt.createQmlObject('import Qt.location 5.0; Place { }', testCase, "test_saveload");

        signalSpy = Qt.createQmlObject('import QtTest 1.0; SignalSpy {}', testCase, "SignalSpy");
        signalSpy.target = readPlace;
        signalSpy.signalName = "statusChanged";

        readPlace.plugin = testPlugin;

        readPlace.getDetails();

        compare(readPlace.status, Place.Fetching);

        wait(0);

        compare(readPlace.status, Place.Error);

        readPlace.placeId = "invalid-id";

        readPlace.getDetails();

        compare(readPlace.status, Place.Fetching);

        wait(0);

        compare(readPlace.status, Place.Error);

        readPlace.placeId = savePlace.placeId;

        // verify that read place is not currently the same as what we saved
        verify(!compare_place(readPlace, savePlace));

        readPlace.getDetails();

        compare(readPlace.status, Place.Fetching);

        wait(0);

        compare(readPlace.status, Place.Ready);

        // verify that read place is the same as what we saved
        verify(compare_place(readPlace, savePlace));

        signalSpy.destroy();


        // Remove a place
        var removePlace = Qt.createQmlObject('import Qt.location 5.0; Place { }', testCase, "test_saveload");

        signalSpy = Qt.createQmlObject('import QtTest 1.0; SignalSpy {}', testCase, "SignalSpy");
        signalSpy.target = removePlace;
        signalSpy.signalName = "statusChanged";

        removePlace.plugin = testPlugin;

        removePlace.removePlace();

        compare(removePlace.status, Place.Removing);

        wait(0);

        compare(removePlace.status, Place.Error);

        removePlace.placeId = "invalid-id";

        removePlace.removePlace();

        compare(removePlace.status, Place.Removing);

        wait(0);

        compare(removePlace.status, Place.Error);

        removePlace.placeId = savePlace.placeId;

        removePlace.removePlace();

        compare(removePlace.status, Place.Removing);

        wait(0);

        compare(removePlace.status, Place.Ready);

        verify(removePlace.placeId === "");

        removePlace.placeId = savePlace.placeId;

        removePlace.getDetails();

        compare(removePlace.status, Place.Fetching);

        wait(0);

        compare(removePlace.status, Place.Error);

        signalSpy.destroy();
    }
}
