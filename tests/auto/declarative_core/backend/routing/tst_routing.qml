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
        id: germanyBerlinChausseestr98
        latitude: 52.53407
        longitude: 13.37861
    }
    Coordinate {
        id: germanyBerlinInvalidenstr117
        latitude: 52.531049
        longitude: 13.385008
    }
    Coordinate {
        id: germanyBerlinTorstr76
        latitude: 52.529193
        longitude: 13.406822
    }

    Coordinate {
        id: northSeaA
        latitude: 56.452778
        longitude: 3.74414
    }

    Coordinate {
        id: northSeaB
        latitude: 55.168829
        longitude: 3.656249
    }

    Coordinate {
        id: germanyMagdeburgUhlichstr6
        latitude: 52.137289
        longitude: 11.620986
    }

    Coordinate {
        id: germanyLeipzigBurgstr8
        latitude: 51.338323
        longitude: 12.37306
    }

    Coordinate {
        id: italyViaLuigiStembrini17
        latitude: 45.481568
        longitude: 9.205732
    }

    Coordinate {
        id: franceMulhouse43RueDeGuebwiller
        latitude: 47.744766
        longitude: 7.322868
    }

    TestCase {
        name: "Same city routing"

        function test_two_waypoint_route() {
            var testData = { }
            testData.waypoints = [ germanyBerlinChausseestr98, germanyBerlinInvalidenstr117 ]
            testData.expectedMinTravelTime = 30
            testData.expectedMinDistance = 400

            fixture.runTest(this, testData)
        }

        function test_three_waypoint_route() {
            var testData = { }
            testData.waypoints = [ germanyBerlinChausseestr98, germanyBerlinInvalidenstr117, germanyBerlinTorstr76 ]
            testData.expectedMinTravelTime = 240
            testData.expectedMinDistance = 2000

            fixture.runTest(this, testData)
        }
    }

    TestCase {
        name: "Same country routing"

        function test_two_waypoint_route() {
            var testData = { }
            testData.waypoints = [ germanyBerlinInvalidenstr117, germanyMagdeburgUhlichstr6 ]
            testData.expectedMinTravelTime = 3600 + 30 * 60
            testData.expectedMinDistance = 120000

            fixture.runTest(this, testData)
        }

        function test_three_waypoint_route() {
            var testData = { }
            testData.waypoints = [ germanyBerlinInvalidenstr117, germanyMagdeburgUhlichstr6, germanyLeipzigBurgstr8 ]
            testData.expectedMinTravelTime = 3600 * 2 + 40 * 60
            testData.expectedMinDistance = 270000

            fixture.runTest(this, testData)
        }
    }

    TestCase {
        name: "Cross country routing"

        function test_across_the_alps() {
            var testData = { }
            testData.waypoints = [ franceMulhouse43RueDeGuebwiller, italyViaLuigiStembrini17 ]
            testData.expectedMinTravelTime = 3600 * 3.5
            testData.expectedMinDistance = 370000

            fixture.runTest(this, testData)
        }
    }
}




