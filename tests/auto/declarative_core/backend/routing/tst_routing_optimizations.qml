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
        name: "Route Optimizations"

        function test_optimizations_dont_fail_for_any_supported_travel_mode() {
            var travelModes = fixture.supportTravelModes()
            var optimizations = [RouteQuery.FastestRoute, RouteQuery.ShortestRoute, RouteQuery.MostEconomicRoute, RouteQuery.MostScenicRoute]

            for (var m = 0; m < travelModes.length; ++m) {
                for (var o = 0; o < optimizations.length; ++o) {
                    var testData = { }
                    testData.waypoints = [ germanyBerlinInvalidenstr27, germanyBerlinOranienburgerStr73 ]
                    testData.travelModes = travelModes[m]
                    testData.routeOptimizations  = optimizations[o]
                    testData.segmentTolerance = 50
                    fixture.runTest(this, testData)
                }
            }
        }
    }

}




