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
import QtLocation 5.0

Item {
    // General-purpose elements for the test:
    Plugin { id: nokiaPlugin; name: "nokia";  PluginParameter {name: "mapping.host"; value: "for.nonexistent"} }
    Plugin { id: nokiaPlugin2; name: "nokia"; }
    Plugin { id: testPlugin; name: "qmlgeo.test.plugin" }
    Coordinate{ id: coordinate1; latitude: 10; longitude: 11}
    Coordinate{ id: coordinate2; latitude: 12; longitude: 13}
    Coordinate{ id: coordinate3; latitude: 50; longitude: 50; altitude: 0}
    Coordinate{ id: coordinate4; latitude: 80; longitude: 80; altitude: 0}
    Coordinate{ id: invalidCoordinate; altitude: 0}
    Coordinate{ id: altitudelessCoordinate; latitude: 50; longitude: 50}
    Map {id: pluginlessMap; width: 100; height: 100}
    Map {id: pluginlessMap2; width: 100; height: 100}
    Map {id: map; plugin: nokiaPlugin; center: coordinate1; width: 100; height: 100}
    Map {id: coordinateMap; plugin: nokiaPlugin2; center: coordinate3; width: 1000; height: 1000; zoomLevel: 15}

    SignalSpy {id: pluginlessMapPluginSpy; target: pluginlessMap; signalName: "pluginChanged" }
    SignalSpy {id: pluginlessMapCenterSpy; target: pluginlessMap; signalName: 'centerChanged'}
    SignalSpy {id: pluginlessMapZoomLevelSpy; target: pluginlessMap; signalName: 'zoomLevelChanged'}
    SignalSpy {id: pluginlessMapMinimumZoomLevelSpy; target: pluginlessMap; signalName: 'minimumZoomLevelChanged'}
    SignalSpy {id: pluginlessMapMaximumZoomLevelSpy; target: pluginlessMap; signalName: 'maximumZoomLevelChanged'}

    SignalSpy {id: mapCenterSpy; target: map; signalName: 'centerChanged'}

    SignalSpy {id: coordinate2LatitudeSpy; target: coordinate2; signalName: 'latitudeChanged'}
    SignalSpy {id: coordinate2LongitudeSpy; target: coordinate2; signalName: 'longitudeChanged'}
    SignalSpy {id: coordinate2AltitudeSpy; target: coordinate2; signalName: 'altitudeChanged'}

    TestCase {
        when: windowShown
        name: "Basic Map properties"

        function clear_data() {
            pluginlessMapMaximumZoomLevelSpy.clear()
            pluginlessMapMinimumZoomLevelSpy.clear()
            pluginlessMapPluginSpy.clear()
            pluginlessMapZoomLevelSpy.clear(0)
            mapCenterSpy.clear()
            pluginlessMapCenterSpy.clear()
            coordinate2AltitudeSpy.clear()
            coordinate2LatitudeSpy.clear()
            coordinate2LongitudeSpy.clear()
        }

        function test_pan() {
            console.log('todo map pan')
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

        function test_aa_map_properties_without_plugin() {
            clear_data()
            compare(pluginlessMap.center.latitude, 0)
            compare(pluginlessMap.center.longitude, 0)
            compare(pluginlessMap.center.altitude, 0)
            compare(pluginlessMap.zoomLevel, 8.0)
            compare(pluginlessMap.minimumZoomLevel, -1.0)
            compare(pluginlessMap.maximumZoomLevel, -1.0)
            compare(pluginlessMap.bearing, 0)
            verify(pluginlessMap.plugin == null)
            verify(pluginlessMap.pinch != null) // props must be settable
            verify(pluginlessMap.flick != null)

            pluginlessMap.zoomLevel = 5.0
            compare(pluginlessMapZoomLevelSpy.count, 1)
            compare(pluginlessMap.zoomLevel, 5.0)
            pluginlessMap.zoomLevel = 5.0
            compare(pluginlessMapZoomLevelSpy.count, 1)
            compare(pluginlessMap.zoomLevel, 5.0)
            // too small, won't be accepted
            pluginlessMap2.zoomLevel = -10
            compare(pluginlessMap2.zoomLevel, 8)
            // too big, should be clipped when setting the plugin
            pluginlessMap2.zoomLevel = 25.0

            // set the plugin and see that values change properly
            pluginlessMap.plugin = nokiaPlugin
            pluginlessMap2.plugin = nokiaPlugin
            compare(pluginlessMapPluginSpy.count, 1)
            tryCompare(pluginlessMapMaximumZoomLevelSpy.count, 1)
            tryCompare(pluginlessMapMinimumZoomLevelSpy.count, 1)
            compare(pluginlessMap.minimumZoomLevel, 0)
            compare(pluginlessMap.maximumZoomLevel, 20)
            compare(pluginlessMap.plugin, nokiaPlugin)
            compare(pluginlessMap.zoomLevel, 5.0)
            tryCompare(pluginlessMap2, "zoomLevel", 20.0)
            compare(pluginlessMap.minimumZoomLevel, 0)
            compare(pluginlessMap.maximumZoomLevel, 20)
            compare(pluginlessMap.center.latitude, 0)
            compare(pluginlessMap.center.longitude, 0)
            compare(pluginlessMap.center.altitude, 0)
            compare(pluginlessMap.bearing, 0)
        }

        function test_map_center() {
            clear_data()
            // coordinate is set at map element declaration
            compare(map.center.latitude, 10)
            compare(map.center.longitude, 11)
            // use the default coordinate (coordinate is not explicitly set)
            pluginlessMap.center.latitude = 5
            compare(pluginlessMapCenterSpy.count, 1)
            pluginlessMap.center.longitude = 10
            compare(pluginlessMapCenterSpy.count, 2)
            pluginlessMap.center.altitude = 15
            compare(pluginlessMapCenterSpy.count, 3)
            compare(pluginlessMap.center.latitude, 5)
            compare(pluginlessMap.center.longitude, 10)
            compare(pluginlessMap.center.altitude, 15)
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
    }
}
