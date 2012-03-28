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
        id: icelandReykjavikBollagata
        latitude: 64.136845
        longitude: -21.918838
    }

    Coordinate {
        id: faroerIslandTorshavnGripsvegur
        latitude: 62.013148
        longitude: -6.769097
    }


    Coordinate {
        id: canadaStJohnCaseySt82
        latitude: 47.5587712
        longitude: -52.7152346
    }


    TestCase {
        name: "No route"

        function test_no_route_within_the_north_sea() {
            var testData = { }
            testData.waypoints = [ northSeaA, northSeaB ]
            testData.expectNoRouteFound = true
            fixture.runTest(this, testData)
        }

        // NOTE North Sea to island / main land tests don't work because positions get snapped

        function test_no_route_from_island_to_island() {
            var testData = { }
            testData.waypoints = [ faroerIslandTorshavnGripsvegur, canadaStJohnCaseySt82 ]
            testData.expectNoRouteFound = true
            fixture.runTest(this, testData)
        }
    }
}




