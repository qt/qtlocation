/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

Item {
    // General-purpose elements for the test:
    Plugin { id: nokiaPlugin; name: "nokia";  PluginParameter {name: "mapping.host"; value: "for.nonexistent"} }
    Coordinate{ id: coordinate1; latitude: 10; longitude: 11}
    Coordinate{ id: coordinate2; latitude: 12; longitude: 13}
    TestCase {
        name: "MapProperties"
        Map {id: emptyMap}
        SignalSpy {id: emptyMapPluginSpy; target: emptyMap; signalName: "pluginChanged" }
        function test_a_map_properties_without_plugin() {
            compare (emptyMap.minimumZoomLevel, -1,  "Minimum zoom level.")
            compare (emptyMap.maximumZoomLevel, -1,  "Maximum zoom level.")
            compare (emptyMap.zoomLevel, 8, "Zoom level.")
            compare (emptyMap.mapType, Map.NoMap, "Map type")
            compare (emptyMap.connectivityMode, Map.NoConnectivity, "Connectivity mode.")
        }
        function test_b_map_properties_with_plugin() {
            // Set plugin and check that appropriate changes are reflected
            emptyMap.plugin = nokiaPlugin
            compare(emptyMapPluginSpy.count, 1, "Plugin changed signal received.")
            compare (emptyMap.plugin, nokiaPlugin,  "Plugin set correctly.")
            verify(emptyMap.maximumZoomLevel != -1, "Maximum zoom level.")
            verify(emptyMap.minimumZoomLevel != -1, "Minimum zoom level.")
            verify(emptyMap.zoomLevel != -1, "Zoom level")
            // Should not change:
            compare (emptyMap.mapType, Map.NoMap, "Default map type.")
            compare (emptyMap.connectivityMode, Map.NoConnectivity, "Connectivity mode.")
        }
    }
    TestCase {
        name: "MapPolyObjects"
        Map {
            id: polygonMap;
            plugin: nokiaPlugin;
            MapPolygon  {id: polygon1}
            MapPolygon  {id: polygon2; Coordinate{latitude: 0;longitude:1} Coordinate{latitude: 2;longitude:3} }
            MapPolyline {id:polyline1}
            MapPolyline {id: polyline2; Coordinate{latitude: 0;longitude:1} Coordinate{latitude: 2;longitude:3} }
        }
        SignalSpy {id: polygon2PathSpy; target: polygon2; signalName: "pathChanged"}
        function test_polygons() {
            compare(polygon1.path.length, 0)
            compare(polygon2.path.length, 2)
            compare(polygon2PathSpy.count, 0)
            polygon2.addCoordinate(coordinate1)
            polygon2.addCoordinate(coordinate2)
            compare(polygon2PathSpy.count, 2)
            // Check the values are correct
            compare(polygon2.path[0].latitude, 0)
            compare(polygon2.path[1].latitude, 2)
            compare(polygon2.path[2].latitude, 10)
            compare(polygon2.path[3].latitude, 12)
            // Remove a coordinate from the middle
            polygon2.removeCoordinate(coordinate1)
            compare(polygon2PathSpy.count, 3)
            compare(polygon2.path[0].latitude, 0)
            compare(polygon2.path[1].latitude, 2)
            compare(polygon2.path[2].latitude, 12)
            // Add same coordinate twice and remove it, latest should be removed
            polygon2.addCoordinate(coordinate1)
            polygon2.addCoordinate(coordinate2)
            compare(polygon2PathSpy.count, 5)
            compare(polygon2.path[2].latitude, 12)
            compare(polygon2.path[3].latitude, 10)
            compare(polygon2.path[4].latitude, 12)
            polygon2.removeCoordinate(coordinate2)
            compare(polygon2.path[2].latitude, 12)
            compare(polygon2.path[3].latitude, 10)
            compare(polygon2.path.length, 4)
            // Clear coordinates
            compare(polygon2PathSpy.count, 6)
            polygon2.clearCoordinates()
            compare(polygon2.path.length, 0)
            compare(polygon2PathSpy.count, 7)
        }
        SignalSpy {id: polyline2PathSpy; target: polyline2; signalName: "pathChanged"}
        function test_polylines() {
            compare(polyline1.path.length, 0)
            compare(polyline2.path.length, 2)
            compare(polyline2PathSpy.count, 0)
            polyline2.addCoordinate(coordinate1)
            polyline2.addCoordinate(coordinate2)
            compare(polyline2PathSpy.count, 2)
            // Check the values are correct
            compare(polyline2.path[0].latitude, 0)
            compare(polyline2.path[1].latitude, 2)
            compare(polyline2.path[2].latitude, 10)
            compare(polyline2.path[3].latitude, 12)
            // Remove a coordinate from the middle
            polyline2.removeCoordinate(coordinate1)
            compare(polyline2PathSpy.count, 3)
            compare(polyline2.path[0].latitude, 0)
            compare(polyline2.path[1].latitude, 2)
            compare(polyline2.path[2].latitude, 12)
            // Add same coordinate twice and remove it, latest should be removed
            polyline2.addCoordinate(coordinate1)
            polyline2.addCoordinate(coordinate2)
            compare(polyline2PathSpy.count, 5)
            compare(polyline2.path[2].latitude, 12)
            compare(polyline2.path[3].latitude, 10)
            compare(polyline2.path[4].latitude, 12)
            polyline2.removeCoordinate(coordinate2)
            compare(polyline2.path[2].latitude, 12)
            compare(polyline2.path[3].latitude, 10)
            compare(polyline2.path.length, 4)
            // Clear coordinates
            compare(polyline2PathSpy.count, 6)
            polyline2.clearCoordinates()
            compare(polyline2.path.length, 0)
            compare(polyline2PathSpy.count, 7)
        }
    }
}



