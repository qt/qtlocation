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

Item {
    id: root

    property Fixture fixture: Qt.createQmlObject('Fixture {}', root)


    Coordinate {
        id: denmarkRonneKrikestraede19
        latitude: 55.098085
        longitude: 14.698746
    }

    Coordinate {
        id: germanySassnitzMrkranerStr9a
        latitude: 54.511519
        longitude: 13.610657
    }

    Coordinate {
        id: germanyBerlinRungestr3
        latitude: 52.512612
        longitude: 13.413884
    }

    Coordinate {
        id: germanyBerlinWallstr57
        latitude: 52.513451
        longitude: 13.413643
    }

    Coordinate {
        id: germanyHamburgFinkenwerderStr
        latitude: 53.515277
        longitude: 9.919929
    }

    Coordinate {
        id: germanyHamburgBeringStr
        latitude: 53.556991
        longitude: 9.900167
    }

    Coordinate {
        id: germanyBerlinInvalidenstr54
        latitude: 52.526269
        longitude: 13.36822
    }

    Coordinate {
        id: germanyBerlinHildebrandstrasse1
        latitude: 52.509722
        longitude: 13.360193
    }

    Coordinate {
        id: germanyBerlinSpanndauerdamm89
        latitude: 52.518683
        longitude: 13.283677
    }

    Coordinate {
        id: germanyBerlinJungfernheideweg80
        latitude: 52.548582
        longitude: 13.27042
    }

    Coordinate {
        id: franceReims32RuedeMars
        latitude: 49.259397
        longitude: 4.0316
    }

    Coordinate {
        id: franceLaon30RueStJean
        latitude: 49.564549
        longitude: 3.618283
    }

    Coordinate {
        id: germanyBerlinInvalidenstr27
        latitude: 52.531543
        longitude: 13.387532
    }

    Coordinate {
        id: germanyBerlinOranienburgerStr73
        latitude: 52.524646
        longitude: 13.394128
    }


    TestCase {
        name: "Feature ferry"

        function test_router_finds_route_with_ferry_enabled() {
            var testData = { }
            testData.waypoints = [ denmarkRonneKrikestraede19, germanySassnitzMrkranerStr9a ]
            testData.segmentTolerance = 15
            fixture.runTest(this, testData)
        }

        function test_router_doesnt_find_route_with_ferry_disabled() {
            var testData = { }
            testData.waypoints = [ denmarkRonneKrikestraede19, germanySassnitzMrkranerStr9a ]
            testData.expectNoRouteFound = true
            testData.weights = [ RouteQuery.FerryFeature, RouteQuery.DisallowFeatureWeight ]
            fixture.runTest(this, testData)
        }


    }

    TestCase {
        name: "Feature tunnel"

        function test_router_doesnt_use_tunnels_if_so_configured() {
            var testData = { }
            testData.waypoints = [ germanyBerlinInvalidenstr54, germanyBerlinHildebrandstrasse1 ]
            testData.weights = [ RouteQuery.TunnelFeature, RouteQuery.DisallowFeatureWeight ]
            testData.noGrep = ["tunnel"]
            fixture.runTest(this, testData)
        }

        function test_router_avoids_tunnels_if_so_configured() {
            var testData = { }
            testData.waypoints = [ germanyBerlinInvalidenstr54, germanyBerlinHildebrandstrasse1 ]
            testData.weights = [ RouteQuery.TunnelFeature, RouteQuery.AvoidFeatureWeight ]
            testData.noGrep = ["tunnel"]
            testData.flaky = true
            fixture.runTest(this, testData)
        }

        function test_router_prefers_tunnels_if_so_configured() {
            var testData = { }
            testData.waypoints = [ germanyBerlinInvalidenstr54, germanyBerlinHildebrandstrasse1 ]
            testData.weights = [ RouteQuery.TunnelFeature, RouteQuery.PreferFeatureWeight ]
            testData.grep = ["tunnel"]
            testData.flaky = true
            fixture.runTest(this, testData)
        }
    }

    TestCase {
        name: "Feature highway"

        function test_router_uses_the_autobahn() {
            var testData = { }
            testData.waypoints = [ germanyHamburgFinkenwerderStr, germanyHamburgBeringStr ]
            testData.expectedMinTravelTime = 240
            testData.expectedMinDistance = 6000
            fixture.runTest(this, testData)
        }

        function test_router_doesnt_use_autobahn_if_highways_are_disabled() {
            var testData = { }
            testData.waypoints = [ germanyHamburgFinkenwerderStr, germanyHamburgBeringStr ]
            testData.weights = [ RouteQuery.HighwayFeature, RouteQuery.DisallowFeatureWeight ]
            testData.expectedMinDistance = 15000
            fixture.runTest(this, testData)
        }

        function test_router_avoids_highways_if_so_configured() {
            var testData = { }
            testData.waypoints = [ germanyBerlinSpanndauerdamm89, germanyBerlinJungfernheideweg80 ]
            testData.weights = [ RouteQuery.HighwayFeature, RouteQuery.AvoidFeatureWeight ]
            testData.noGrep = ["111"]
            testData.flaky = true
            fixture.runTest(this, testData)
        }

        function test_router_prefers_highways_if_so_configured() {
            var testData = { }
            testData.waypoints = [ germanyBerlinSpanndauerdamm89, germanyBerlinJungfernheideweg80 ]
            testData.weights = [ RouteQuery.HighwayFeature, RouteQuery.PreferFeatureWeight ]
            testData.grep = ["111"]
            testData.flaky = true
            fixture.runTest(this, testData)
        }
    }

    TestCase {
        name: "Feature tollroad"

        function test_router_doesnt_use_tollroad_if_so_configured() {
            var testData = { }
            testData.waypoints = [ franceReims32RuedeMars, franceLaon30RueStJean ]
            testData.weights = [ RouteQuery.TollFeature, RouteQuery.DisallowFeatureWeight ]
            testData.noGrep = ["A26"]
            fixture.runTest(this, testData)
        }

        function test_router_avoids_highways_if_so_configured() {
            var testData = { }
            testData.waypoints = [ franceReims32RuedeMars, franceLaon30RueStJean ]
            testData.weights = [ RouteQuery.TollFeature, RouteQuery.AvoidFeatureWeight ]
            testData.noGrep = ["A26"]
            testData.flaky = true
            fixture.runTest(this, testData)
        }

        function test_router_prefers_highways_if_so_configured() {
            var testData = { }
            testData.waypoints = [ franceReims32RuedeMars, franceLaon30RueStJean ]
            testData.weights = [ RouteQuery.TollFeature, RouteQuery.PreferFeatureWeight ]
            testData.grep = ["A26"]
            testData.flaky = true
            fixture.runTest(this, testData)
        }
    }

    TestCase {
        name: "Feature dirt road"

        function test_router_excludes_dirt_roads_if_so_configured() {
            skip("No know testcase for dirt road")
            var testData = { }
            testData.waypoints = [ germanyBerlinInvalidenstr27, germanyBerlinOranienburgerStr73 ]
            testData.weights = [ RouteQuery.DirtRoadFeature, RouteQuery.DisallowFeatureWeight ]
            testData.travelModes = RouteQuery.CarTravel
            fixture.runTest(this, testData)
        }
    }


    TestCase {
        name: "Unsupported features"

        function test_router_doesnt_support_public_transport_feature() {
            var testData = { }
            testData.waypoints = [ germanyBerlinInvalidenstr27, germanyBerlinOranienburgerStr73 ]
            testData.weights = [ RouteQuery.PublicTransitFeature, RouteQuery.PreferFeatureWeight ]
            testData.travelModes = RouteQuery.PedestrianTravel
            testData.grep = ["S1"]
            testData.noGrep = ["Tor"]
            testData.expectError = RouteModel.UnsupportedOptionError
            fixture.runTest(this, testData)
        }

        function test_router_doesnt_support_park_feature() {
            var testData = { }
            testData.waypoints = [ germanyBerlinInvalidenstr27, germanyBerlinOranienburgerStr73 ]
            testData.weights = [ RouteQuery.ParksFeature, RouteQuery.PreferFeatureWeight ]
            testData.travelModes = RouteQuery.PedestrianTravel
            testData.expectError = RouteModel.UnsupportedOptionError
            fixture.runTest(this, testData)
        }

        function test_router_doesnt_support_motorpool_lane_feature() {
            var testData = { }
            testData.waypoints = [ germanyBerlinInvalidenstr27, germanyBerlinOranienburgerStr73 ]
            testData.weights = [ RouteQuery.MotorPoolLaneFeature, RouteQuery.PreferFeatureWeight ]
            testData.travelModes = RouteQuery.CarTravel
            testData.expectError = RouteModel.UnsupportedOptionError
            fixture.runTest(this, testData)
        }
    }
}




