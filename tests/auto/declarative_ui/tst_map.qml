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
    width:100
    height:100
    // General-purpose elements for the test:
    Plugin { id: testPlugin; name: "qmlgeo.test.plugin"; allowExperimental: true }
    Plugin { id: testPlugin2; name: "gmlgeo.test.plugin"; allowExperimental: true }
    Plugin { id: nokiaPlugin; name: "nokia"; }
    Coordinate{ id: coordinate1; latitude: 10; longitude: 11}
    Coordinate{ id: coordinate2; latitude: 12; longitude: 13}
    Coordinate{ id: coordinate3; latitude: 50; longitude: 50; altitude: 0}
    Coordinate{ id: coordinate4; latitude: 80; longitude: 80; altitude: 0}
    Coordinate{ id: invalidCoordinate; altitude: 0}
    Coordinate{ id: altitudelessCoordinate; latitude: 50; longitude: 50}
    Map {id: map; plugin: testPlugin; center: coordinate1; width: 100; height: 100}
    Map {id: coordinateMap; plugin: nokiaPlugin; center: coordinate3; width: 1000; height: 1000; zoomLevel: 15}

    SignalSpy {id: mapCenterSpy; target: map; signalName: 'centerChanged'}
    SignalSpy {id: coordinate2LatitudeSpy; target: coordinate2; signalName: 'latitudeChanged'}
    SignalSpy {id: coordinate2LongitudeSpy; target: coordinate2; signalName: 'longitudeChanged'}
    SignalSpy {id: coordinate2AltitudeSpy; target: coordinate2; signalName: 'altitudeChanged'}

    TestCase {
        when: windowShown
        name: "Basic Map properties"

        function fuzzy_compare(val, ref) {
            var tolerance = 0.01;
            if ((val > ref - tolerance) && (val < ref + tolerance))
                return true;
            console.log('map fuzzy cmp returns false for value, ref: ' + val + ', ' + ref)
            return false;
        }

        function clear_data() {
            mapCenterSpy.clear()
            coordinate2AltitudeSpy.clear()
            coordinate2LatitudeSpy.clear()
            coordinate2LongitudeSpy.clear()
        }

        function test_map_center() {
            clear_data()
            // coordinate is set at map element declaration
            compare(map.center.latitude, 10)
            compare(map.center.longitude, 11)

            // change center and its values
            mapCenterSpy.clear();
            compare(mapCenterSpy.count, 0)
            map.center = coordinate2
            compare(mapCenterSpy.count, 1)
            map.center = coordinate2
            compare(mapCenterSpy.count, 1)
            compare(coordinate2LatitudeSpy.count, 0)
            compare(coordinate2LongitudeSpy.count, 0)
            compare(coordinate2AltitudeSpy.count, 0)
            coordinate2.latitude = 21
            compare(coordinate2LatitudeSpy.count, 1)
            compare(mapCenterSpy.count, 2)
            compare(coordinate2LongitudeSpy.count, 0)
            compare(coordinate2AltitudeSpy.count, 0)
            compare(map.center.latitude, 21)
            coordinate2.longitude = 31
            compare(coordinate2LatitudeSpy.count, 1)
            compare(mapCenterSpy.count, 3)
            compare(coordinate2LongitudeSpy.count, 1)
            compare(coordinate2AltitudeSpy.count, 0)
            compare(map.center.longitude, 31)
            coordinate2.altitude = 41
            compare(coordinate2LatitudeSpy.count, 1)
            compare(mapCenterSpy.count, 4)
            compare(coordinate2LongitudeSpy.count, 1)
            compare(coordinate2AltitudeSpy.count, 1)
            compare(map.center.altitude, 41)
            compare(map.center.longitude, 31)
            compare(map.center.latitude, 21)
        }

        function test_pan() {
            map.center.latitude = 30
            map.center.longitude = 60
            map.zoomLevel = 4
            clear_data()

            // up left
            tryCompare(mapCenterSpy, "count", 0)
            map.pan(-20,-20)
            tryCompare(mapCenterSpy, "count", 1)
            verify(map.center.latitude > 30)
            verify(map.center.longitude < 60)
            map.center.latitude = 30
            map.center.longitude = 60
            mapCenterSpy.clear()
            // up
            map.pan(0,-20)
            tryCompare(mapCenterSpy, "count", 1)
            verify(map.center.latitude > 30)
            compare(map.center.longitude, 60)
            map.center.latitude = 30
            map.center.longitude = 60
            mapCenterSpy.clear()
            // up right
            tryCompare(mapCenterSpy, "count", 0)
            map.pan(20,-20)
            tryCompare(mapCenterSpy, "count", 1)
            verify(map.center.latitude > 30)
            verify(map.center.longitude > 60)
            map.center.latitude = 30
            map.center.longitude = 60
            mapCenterSpy.clear()
            // left
            map.pan(-20,0)
            tryCompare(mapCenterSpy, "count", 1)
            verify (fuzzy_compare(map.center.latitude, 30))
            verify(map.center.longitude < 60)
            map.center.latitude = 30
            map.center.longitude = 60
            mapCenterSpy.clear()
            // center
            map.pan(0,0)
            tryCompare(mapCenterSpy, "count", 0)
            compare(map.center.latitude, 30)
            compare(map.center.longitude, 60)
            map.center.latitude = 30
            map.center.longitude = 60
            mapCenterSpy.clear()
            // right
            map.pan(20,0)
            tryCompare(mapCenterSpy, "count", 1)
            verify (fuzzy_compare(map.center.latitude, 30))
            verify(map.center.longitude > 60)
            map.center.latitude = 30
            map.center.longitude = 60
            mapCenterSpy.clear()
            // down left
            map.pan(-20,20)
            tryCompare(mapCenterSpy, "count", 1)
            verify (map.center.latitude < 30 )
            verify (map.center.longitude < 60 )
            map.center.latitude = 30
            map.center.longitude = 60
            mapCenterSpy.clear()
            // down
            map.pan(0,20)
            tryCompare(mapCenterSpy, "count", 1)
            verify (map.center.latitude < 30 )
            verify (fuzzy_compare(map.center.longitude, 60))
            map.center.latitude = 30
            map.center.longitude = 60
            mapCenterSpy.clear()
            // down right
            map.pan(20,20)
            tryCompare(mapCenterSpy, "count", 1)
            verify (map.center.latitude < 30 )
            verify (map.center.longitude > 60 )
            map.center.latitude = 30
            map.center.longitude = 60
            mapCenterSpy.clear()
        }

        function test_coordinate_conversion() {
            wait(1000)
            clear_data()
            compare(coordinateMap.center.latitude, 50)
            compare(coordinateMap.center.longitude, 50)
            // valid to screen position
            var point = coordinateMap.toScreenPosition(coordinateMap.center)
            verify (point.x > 495 && point.x < 505)
            verify (point.y > 495 && point.y < 505)
            // valid coordinate without altitude
            point = coordinateMap.toScreenPosition(altitudelessCoordinate)
            verify (point.x > 495 && point.x < 505)
            verify (point.y > 495 && point.y < 505)
            // out of map area
            point = coordinateMap.toScreenPosition(coordinate4)
            verify(isNaN(point.x))
            verify(isNaN(point.y))
            // invalid coordinates
            point = coordinateMap.toScreenPosition(invalidCoordinate)
            verify(isNaN(point.x))
            verify(isNaN(point.y))
            point = coordinateMap.toScreenPosition(null)
            verify(isNaN(point.x))
            verify(isNaN(point.y))
            // valid point to coordinate
            var coord = coordinateMap.toCoordinate(Qt.point(500,500))
            verify(coord.latitude > 49 && coord.latitude < 51)
            verify(coord.longitude > 49 && coord.longitude < 51)
            // beyond
            coord = coordinateMap.toCoordinate(Qt.point(2000, 2000))
            verify(isNaN(coord.latitude))
            verify(isNaN(coord.longitde))
            // invalid
            coord = coordinateMap.toCoordinate(Qt.point(-5, -6))
            verify(isNaN(coord.latitude))
            verify(isNaN(coord.longitde))
        }
    }
}
