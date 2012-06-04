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
import QtLocation.test 5.0

    /*

     (0,0)   ---------------------------------------------------- (240,0)
             | no map                                           |
             |    (20,20)                                       |
     (0,20)  |    ------------------------------------------    | (240,20)
             |    |                                        |    |
             |    |  map                                   |    |
             |    |                                        |    |
             |    |                                        |    |
             |    |                                        |    |
             |    |                   (lat 20, lon 20)     |    |
             |    |                     x                  |    |
             |    |                                        |    |
             |    |                                        |    |
             |    |                                        |    |
             |    |                                        |    |
             |    |                                        |    |
             |    ------------------------------------------    |
             |                                                  |
     (0,240) ---------------------------------------------------- (240,240)

     */

Item {
    id: page
    x: 0; y: 0;
    width: 240
    height: 240
    Plugin { id: testPlugin; name : "qmlgeo.test.plugin"; allowExperimental: true }
    Coordinate{ id: mapDefaultCenter; latitude: 20; longitude: 20}

    Coordinate{ id: datelineCoordinate; latitude: 20; longitude: 180}
    Coordinate{ id: datelineCoordinateLeft; latitude: 20; longitude: 170}
    Coordinate{ id: datelineCoordinateRight; latitude: 20; longitude: -170}

    MapPolygon {
        id: extMapPolygon
        color: 'darkgrey'
        path: [
            Coordinate { latitude: 25; longitude: 5},
            Coordinate { latitude: 20; longitude: 10}
        ]
        MapMouseArea {
            anchors.fill: parent
            drag.target: parent
            SignalSpy { id: extMapPolygonClicked; target: parent; signalName: "clicked" }
        }
        SignalSpy {id: extMapPolygonPathChanged; target: parent; signalName: "pathChanged"}
        SignalSpy {id: extMapPolygonColorChanged; target: parent; signalName: "colorChanged"}
        SignalSpy {id: extMapPolygonBorderWidthChanged; target: parent.border; signalName: "widthChanged"}
        SignalSpy {id: extMapPolygonBorderColorChanged; target: parent.border; signalName: "colorChanged"}
    }
    Coordinate { id: polyCoordinate; latitude: 15; longitude: 6}

    MapPolygon {
        id: extMapPolygon0
        color: 'darkgrey'
    }

    MapPolyline {
        id: extMapPolyline0
    }

    MapPolyline {
        id: extMapPolyline
        path: [
            Coordinate { latitude: 25; longitude: 5},
            Coordinate { latitude: 20; longitude: 10}
        ]
        SignalSpy {id: extMapPolylineColorChanged; target: parent.line; signalName: "colorChanged"}
        SignalSpy {id: extMapPolylineWidthChanged; target: parent.line; signalName: "widthChanged"}
        SignalSpy {id: extMapPolylinePathChanged; target: parent; signalName: "pathChanged"}
    }

    MapRectangle {
        id: extMapRectDateline
        color: 'darkcyan'
        topLeft: Coordinate { latitude: 20; longitude: 175}
        bottomRight: Coordinate { latitude: 10; longitude: -175}
        MapMouseArea {
            anchors.fill: parent
            drag.target: parent
        }
    }

    MapCircle {
        id: extMapCircleDateline
        color: 'darkmagenta'
        center: Coordinate { latitude: 20; longitude: 180}
        radius: 400000
        MapMouseArea {
            anchors.fill: parent
            drag.target: parent
        }
    }

    MapQuickItem {
        id: extMapQuickItemDateline
        MapMouseArea {
            anchors.fill: parent
            drag.target: parent
        }
        coordinate: Coordinate { latitude: 20; longitude: 175}
        sourceItem: Rectangle {
            color: 'darkgreen'
            width: 20
            height: 20
        }
    }

    MapPolygon {
        id: extMapPolygonDateline
        color: 'darkmagenta'
        path: [
            Coordinate { latitude: 20; longitude: 175},
            Coordinate { latitude: 20; longitude: -175},
            Coordinate { latitude: 10; longitude: -175},
            Coordinate { latitude: 10; longitude: 175}
        ]
        MapMouseArea {
            anchors.fill: parent
            drag.target: parent
        }
    }

    MapPolyline {
        id: extMapPolylineDateline
        line.width : 3
        path: [
            Coordinate { latitude: 20; longitude: 175},
            Coordinate { latitude: 25; longitude: -175}
        ]
        MapMouseArea {
            anchors.fill: parent
            drag.target: parent
        }
    }

    MapRoute {
        id: extMapRouteDateline
        line.color: 'yellow'
        route: Route {
            path: [
                Coordinate { latitude: 25; longitude: 175},
                Coordinate { latitude: 20; longitude: -175}
            ]
        }
    }

    Map {
        id: map;
        x: 20; y: 20; width: 200; height: 200
        zoomLevel: 3
        center: mapDefaultCenter
        plugin: testPlugin;
    }

    Text {id: progressText}

    TestCase {
        name: "Map Item 2"
        when: windowShown

        function test_aa_precondition() {
            wait(10)
            // sanity check that the coordinate conversion works
            var mapcenter = map.toScreenPosition(map.center)
            verify (fuzzy_compare(mapcenter.x, 100, 2))
            verify (fuzzy_compare(mapcenter.y, 100, 2))
        }

        function test_polygon() {
            map.clearMapItems()
            clear_data()
            compare (extMapPolygon.border.width, 1.0)
            compare (extMapPolygonClicked.count, 0)
            var point = map.toScreenPosition(extMapPolygon.path[1])
            map.addMapItem(extMapPolygon)
            verify(extMapPolygon.path.length == 2)
            mouseClick(map, point.x - 5, point.y)
            compare(extMapPolygonClicked.count, 0)
            map.addMapItem(extMapPolygon0) // mustn't crash or ill-behave
            verify(extMapPolygon0.path.length == 0)
            extMapPolygon.addCoordinate(polyCoordinate)
            verify(extMapPolygon.path.length == 3)
            mouseClick(map, point.x - 5, point.y)
            compare(extMapPolygonClicked.count, 1)

            extMapPolygon.path[0].latitude = 10
            verify(extMapPolygon.path[0].latitude, 10)
            extMapPolygon.path[0].latitude = polyCoordinate.latitude
            verify(extMapPolygon.path[0].latitude, 15)
            extMapPolygon.path[0].longitude = 2
            verify(extMapPolygon.path[0].longitude, 2)
            extMapPolygon.path[0].longitude = polyCoordinate.longitude
            verify(extMapPolygon.path[0].longitude, 6)

            extMapPolygon.removeCoordinate(polyCoordinate)
            verify(extMapPolygon.path.length == 2)
            extMapPolygon.removeCoordinate(extMapPolygon.path[1])
            verify(extMapPolygon.path.length == 1)
            extMapPolygon.removeCoordinate(extMapPolygon.path[0])
            verify(extMapPolygon.path.length == 0)
        }

        function test_polyline() {
            map.clearMapItems()
            clear_data()
            compare (extMapPolyline.line.width, 1.0)
            var point = map.toScreenPosition(extMapPolyline.path[1])
            map.addMapItem(extMapPolyline0) // mustn't crash or ill-behave
            verify(extMapPolyline0.path.length == 0)
            map.addMapItem(extMapPolyline)
            verify(extMapPolyline.path.length == 2)
            extMapPolyline.addCoordinate(polyCoordinate)
            verify(extMapPolyline.path.length == 3)
            extMapPolyline.addCoordinate(extMapPolyline.path[0])
            verify(extMapPolyline.path.length == 4)

            extMapPolyline.path[0].latitude = 10
            verify(extMapPolyline.path[0].latitude, 10)
            extMapPolyline.path[0].latitude = polyCoordinate.latitude
            verify(extMapPolyline.path[0].latitude, 15)
            extMapPolyline.path[0].longitude = 2
            verify(extMapPolyline.path[0].longitude, 2)
            extMapPolyline.path[0].longitude = polyCoordinate.longitude
            verify(extMapPolyline.path[0].longitude, 6)

            // TODO when line rendering is ready
            //mouseClick(map, point.x - 5, point.y)
            //compare(extMapPolylineClicked.count, 1)
            extMapPolyline.removeCoordinate(extMapPolyline.path[0])
            verify(extMapPolyline.path.length == 3)
            extMapPolyline.removeCoordinate(polyCoordinate)
            verify(extMapPolyline.path.length == 2)
            extMapPolyline.removeCoordinate(extMapPolyline.path[1])
            verify(extMapPolyline.path.length == 1)
            extMapPolyline.removeCoordinate(extMapPolyline.path[0])
            verify(extMapPolyline.path.length == 0)
        }

        function test_zz_dateline() {
            map.clearMapItems()
            clear_data()
            map.center = datelineCoordinate
            map.zoomLevel = 2.2

            // rectangle
            // item spanning across dateline
            map.addMapItem(extMapRectDateline)
            verify(extMapRectDateline.topLeft.longitude == 175)
            verify(extMapRectDateline.bottomRight.longitude == -175)
            var point = map.toScreenPosition(extMapRectDateline.topLeft)
            verify(point.x < map.width / 2.0)
            point = map.toScreenPosition(extMapRectDateline.bottomRight)
            verify(point.x > map.width / 2.0)
            // move item away from dataline by directly setting its coords
            extMapRectDateline.bottomRight.longitude = datelineCoordinateRight.longitude
            point = map.toScreenPosition(extMapRectDateline.bottomRight)
            verify(point.x > map.width / 2.0)
            extMapRectDateline.topLeft.longitude = datelineCoordinate.longitude
            point = map.toScreenPosition(extMapRectDateline.topLeft)
            verify(point.x == map.width / 2.0)
            // drag item back onto dateline
            mousePress(map, point.x + 5, point.y + 5)
            var i
            for (i=0; i < 20; i += 2) {
                wait(1)
                mouseMove(map, point.x + 5 - i, point.y + 5 )
            }
            mouseRelease(map, point.x + 5 - i, point.y + 5)
            point = map.toScreenPosition(extMapRectDateline.topLeft)
            verify(point.x < map.width / 2.0)
            point = map.toScreenPosition(extMapRectDateline.bottomRight)
            verify(point.x > map.width / 2.0)
            map.removeMapItem(extMapRectDateline)

            // circle
            map.addMapItem(extMapCircleDateline)
            verify(extMapCircleDateline.center.longitude == 180)
            var point = map.toScreenPosition(extMapCircleDateline.center)
            verify(point.x == map.width / 2.0)
            extMapCircleDateline.center.longitude = datelineCoordinateRight.longitude
            point = map.toScreenPosition(extMapCircleDateline.center)
            verify(point.x > map.width / 2.0)
            mousePress(map, point.x, point.y)
            for (i=0; i < 40; i += 4) {
                wait(1)
                mouseMove(map, point.x - i, point.y)
            }
            mouseRelease(map, point.x - i, point.y)
            point = map.toScreenPosition(extMapCircleDateline.center)
            verify(point.x < map.width / 2.0)
            map.removeMapItem(extMapCircleDateline)

            // quickitem
            map.addMapItem(extMapQuickItemDateline)
            verify(extMapQuickItemDateline.coordinate.longitude == 175)
            point = map.toScreenPosition(extMapQuickItemDateline.coordinate)
            verify(point.x < map.width / 2.0)
            extMapQuickItemDateline.coordinate.longitude = datelineCoordinateRight.longitude
            point = map.toScreenPosition(extMapQuickItemDateline.coordinate)
            verify(point.x > map.width / 2.0)
            mousePress(map, point.x + 5, point.y + 5)
            for (i=0; i < 50; i += 5) {
                wait(1)
                mouseMove(map, point.x + 5 - i, point.y + 5 )
            }
            mouseRelease(map, point.x + 5 - i, point.y + 5)
            point = map.toScreenPosition(extMapQuickItemDateline.coordinate)
            verify(point.x < map.width / 2.0)
            map.removeMapItem(extMapQuickItemDateline)

            // polygon
            map.addMapItem(extMapPolygonDateline)
            verify(extMapPolygonDateline.path[0].longitude == 175)
            verify(extMapPolygonDateline.path[1].longitude == -175)
            verify(extMapPolygonDateline.path[2].longitude == -175)
            verify(extMapPolygonDateline.path[3].longitude == 175)
            point = map.toScreenPosition(extMapPolygonDateline.path[0])
            verify(point.x < map.width / 2.0)
            point = map.toScreenPosition(extMapPolygonDateline.path[1])
            verify(point.x > map.width / 2.0)
            point = map.toScreenPosition(extMapPolygonDateline.path[2])
            verify(point.x > map.width / 2.0)
            point = map.toScreenPosition(extMapPolygonDateline.path[3])
            verify(point.x < map.width / 2.0)
            extMapPolygonDateline.path[1].longitude = datelineCoordinateRight.longitude
            point = map.toScreenPosition(extMapPolygonDateline.path[1])
            verify(point.x > map.width / 2.0)
            extMapPolygonDateline.path[2].longitude = datelineCoordinateRight.longitude
            point = map.toScreenPosition(extMapPolygonDateline.path[2])
            verify(point.x > map.width / 2.0)
            extMapPolygonDateline.path[0].longitude = datelineCoordinate.longitude
            point = map.toScreenPosition(extMapPolygonDateline.path[0])
            verify(point.x == map.width / 2.0)
            extMapPolygonDateline.path[3].longitude = datelineCoordinate.longitude
            point = map.toScreenPosition(extMapPolygonDateline.path[3])
            verify(point.x == map.width / 2.0)
            mousePress(map, point.x + 5, point.y - 5)
            for (i=0; i < 20; i += 2) {
                wait(1)
                mouseMove(map, point.x + 5 - i, point.y - 5 )
            }
            mouseRelease(map, point.x + 5 - i, point.y - 5)
            point = map.toScreenPosition(extMapPolygonDateline.path[0])
            verify(point.x < map.width / 2.0)
            point = map.toScreenPosition(extMapPolygonDateline.path[1])
            verify(point.x > map.width / 2.0)
            point = map.toScreenPosition(extMapPolygonDateline.path[2])
            verify(point.x > map.width / 2.0)
            point = map.toScreenPosition(extMapPolygonDateline.path[3])
            verify(point.x < map.width / 2.0)
            map.removeMapItem(extMapPolygonDateline)

            // polyline
            map.addMapItem(extMapPolylineDateline)
            verify(extMapPolylineDateline.path[0].longitude == 175)
            verify(extMapPolylineDateline.path[1].longitude == -175)
            point = map.toScreenPosition(extMapPolylineDateline.path[0])
            verify(point.x < map.width / 2.0)
            point = map.toScreenPosition(extMapPolylineDateline.path[1])
            verify(point.x > map.width / 2.0)
            extMapPolylineDateline.path[1].longitude = datelineCoordinateRight.longitude
            point = map.toScreenPosition(extMapPolylineDateline.path[1])
            verify(point.x > map.width / 2.0)
            extMapPolylineDateline.path[0].longitude = datelineCoordinate.longitude
            point = map.toScreenPosition(extMapPolylineDateline.path[0])
            verify(point.x == map.width / 2.0)
            map.removeMapItem(extMapPolylineDateline)

            // map route
            // (does not support setting of path coords)
            map.addMapItem(extMapRouteDateline)
            verify(extMapRouteDateline.route.path[0].longitude == 175)
            verify(extMapRouteDateline.route.path[1].longitude == -175)
            point = map.toScreenPosition(extMapRouteDateline.route.path[0])
            verify(point.x < map.width / 2.0)
            point = map.toScreenPosition(extMapRouteDateline.route.path[1])
            verify(point.x > map.width / 2.0)
            map.removeMapItem(extMapRouteDateline)
        }


        function clear_data() {
            extMapPolygonClicked.clear()
            extMapPolylineColorChanged.clear()
            extMapPolylineWidthChanged.clear()
            extMapPolylinePathChanged.clear()
            extMapPolygonPathChanged.clear()
            extMapPolygonColorChanged.clear()
            extMapPolygonBorderColorChanged.clear()
            extMapPolygonBorderWidthChanged.clear()
        }

        function fuzzy_compare(val, ref, tol) {
            var tolerance = 2
            if (tol !== undefined)
                tolerance = tol
            if ((val >= ref - tolerance) && (val <= ref + tolerance))
                return true;
            console.log('map fuzzy cmp returns false for value, ref, tolerance: ' + val + ', ' + ref + ', ' + tolerance)
            return false;
        }

        // call to visualInspectionPoint testcase (only for development time visual inspection)
        function visualInspectionPoint(text) {
            progressText.text = text
            //wait (500)
        }
    }
}
