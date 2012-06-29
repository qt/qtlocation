/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import QtTest 1.0
import QtLocation 5.0

TestCase {
    id: testCase

    name: "PlaceSearchModel"

    PlaceSearchModel {
        id: testModel
    }

    Plugin {
        id: testPlugin
        name: "qmlgeo.test.plugin"
        allowExperimental: true
        parameters: [
            PluginParameter {
                name: "initializePlaceData"
                value: true
            }
        ]
    }

    Plugin {
        id: favoritePlugin
        name: "foo"
    }

    GeoCircle {
        id: testSearchArea
        center: Coordinate {
            latitude: 10
            longitude: 20
        }
        radius: 5000
    }

    Category {
        id: testCategory1
        categoryId: "da3606c1-3448-43b3-a4a3-ca24b12dd94a"
        name: "Test Category 1"
    }

    Category {
        id: testCategory2
        categoryId: "bb8ead84-ec2a-48a9-9c8f-d4ffd3134b21"
        name: "Test Category 2"
    }

    function compareArray(a, b) {
        if (a.length !== b.length)
            return false;

        for (var i = 0; i < a.length; ++i) {
            if (b.indexOf(a[i]) < 0)
                return false;
        }

        return true;
    }

    function test_setAndGet_data() {
        return [
            { tag: "plugin", property: "plugin", signal: "pluginChanged", value: testPlugin },
            { tag: "searchArea", property: "searchArea", signal: "searchAreaChanged", value: testSearchArea },
            { tag: "offset", property: "offset", signal: "offsetChanged", value: 10, reset: 0 },
            { tag: "limit", property: "limit", signal: "limitChanged", value: 10, reset: -1 },

            { tag: "searchTerm", property: "searchTerm", signal: "searchTermChanged", value: "Test term", reset: "" },
            { tag: "recommendationId", property: "recommendationId", signal: "recommendationIdChanged", value: "Test-place-id", reset: "" },
            { tag: "relevanceHint", property: "relevanceHint", signal: "relevanceHintChanged", value: PlaceSearchModel.DistanceHint, reset: PlaceSearchModel.UnspecifiedHint },
            { tag: "visibilityScope", property: "visibilityScope", signal: "visibilityScopeChanged", value: Place.DeviceVisibility, reset: Place.UnspecifiedVisibility },
            { tag: "favoritesPlugin", property: "favoritesPlugin", signal: "favoritesPluginChanged", value: favoritePlugin },
            { tag: "category", property: "categories", signal: "categoriesChanged", value: testCategory1, expectedValue: [ testCategory1 ], reset: [], array: true },
            { tag: "categories", property: "categories", signal: "categoriesChanged", value: [ testCategory1, testCategory2 ], reset: [], array: true },
        ];
    }

    function test_setAndGet(data) {
        var signalSpy = Qt.createQmlObject('import QtTest 1.0; SignalSpy {}', testCase, "SignalSpy");
        signalSpy.target = testModel;
        signalSpy.signalName = data.signal;

        // set property to something new
        testModel[data.property] = data.value;
        if (data.array) {
            if (data.expectedValue) {
                verify(compareArray(testModel[data.property], data.expectedValue));
                compare(signalSpy.count, 1 + data.expectedValue.length);
            } else {
                verify(compareArray(testModel[data.property], data.value));
                compare(signalSpy.count, 1 + data.value.length);
            }

        } else {
            compare(testModel[data.property], data.value);
            compare(signalSpy.count, 1);
        }

        signalSpy.clear();

        // set property to the same value (signal spy should not increase)
        testModel[data.property] = data.value;
        if (data.array) {
            if (data.expectedValue) {
                verify(compareArray(testModel[data.property], data.expectedValue));
                compare(signalSpy.count, 1 + data.expectedValue.length);
            } else {
                verify(compareArray(testModel[data.property], data.value));
                compare(signalSpy.count, 1 + data.value.length);
            }

        } else {
            compare(testModel[data.property], data.value);
            compare(signalSpy.count, 0);
        }

        signalSpy.clear();

        // reset property
        if (data.reset === undefined) {
            testModel[data.property] = null;
            compare(testModel[data.property], null);
        } else {
            testModel[data.property] = data.reset;
            if (data.array)
                verify(compareArray(testModel[data.property], data.reset));
            else
                compare(testModel[data.property], data.reset);
        }
        compare(signalSpy.count, 1);

        signalSpy.destroy();
    }

    function test_search_data() {
        var park = Qt.createQmlObject('import QtLocation 5.0; Category {name: "Park"; categoryId: "c2e1252c-b997-44fc-8165-e53dd00f66a7"}', testCase, "Category");
        return [
            {
                tag: "searchTerm, multiple results",
                property: "searchTerm",
                value: "view",
                reset: "",
                places: [
                    "4dcc74ce-fdeb-443e-827c-367438017cf1",
                    "8f72057a-54b2-4e95-a7bb-97b4d2b5721e"
                ]
            },
            {
                tag: "searchTerm, single result",
                property: "searchTerm",
                value: "park",
                reset: "",
                places: [
                    "4dcc74ce-fdeb-443e-827c-367438017cf1"
                ]
            },
            {
                tag: "categories, single result",
                property: "categories",
                value: [ park ],
                places: [
                    "dacb2181-3f67-4e6a-bd4d-635e99ad5b03"
                ]
            },
        ];
    }

    function test_search(data) {
        testModel.plugin = testPlugin;

        var statusChangedSpy = Qt.createQmlObject('import QtTest 1.0; SignalSpy {}', testCase, "SignalSpy");
        statusChangedSpy.target = testModel;
        statusChangedSpy.signalName = "statusChanged";

        var countChangedSpy = Qt.createQmlObject('import QtTest 1.0; SignalSpy {}', testCase, "SignalSpy");
        countChangedSpy.target = testModel;
        countChangedSpy.signalName = "rowCountChanged";

        compare(testModel.status, PlaceSearchModel.Null);

        testModel[data.property] = data.value;
        testModel.update();

        compare(testModel.status, PlaceSearchModel.Loading);
        compare(statusChangedSpy.count, 1);

        tryCompare(testModel, "status", PlaceSearchModel.Ready);
        compare(statusChangedSpy.count, 2);

        compare(countChangedSpy.count, 1);

        for (var i = 0; i < testModel.count; ++i) {
            compare(testModel.data(i, "type"), PlaceSearchModel.PlaceResult);

            var place = testModel.data(i, "place");

            verify(data.places.indexOf(place.placeId) >= 0);
        }

        testModel.reset();

        compare(statusChangedSpy.count, 3);
        compare(testModel.status, PlaceSearchModel.Null);
        compare(countChangedSpy.count, 2);
        compare(testModel.count, 0);

        testModel.update();

        compare(testModel.status, PlaceSearchModel.Loading);
        compare(statusChangedSpy.count, 4);

        testModel.cancel();

        compare(statusChangedSpy.count, 5);
        compare(testModel.status, PlaceSearchModel.Null);

        countChangedSpy.destroy();
        statusChangedSpy.destroy();

        if (data.reset === undefined) {
            testModel[data.property] = null;
        } else {
            testModel[data.property] = data.reset;
        }

        testModel.plugin = null;
    }
}
