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
    Coordinate{ id: someCoordinate1; latitude: 15; longitude: 15}
    Coordinate{ id: someCoordinate2; latitude: 16; longitude: 16}
    Route { id: someRoute;
        path: [
            Coordinate { latitude: 22; longitude: 15},
            Coordinate { latitude: 21; longitude: 16},
            Coordinate { latitude: 23; longitude: 17}
        ]
    }
    Item { id: someItem }

    MapRectangle {
        id: extMapRect
        color: 'darkcyan'
        topLeft: Coordinate { latitude: 20; longitude: 20}
        bottomRight: Coordinate { latitude: 10; longitude: 30}
        MapMouseArea {
            id: extMapRectMa
            anchors.fill: parent
            drag.target: parent
            SignalSpy { id: extMapRectClicked; target: parent; signalName: "clicked" }
            SignalSpy { id: extMapRectActiveChanged; target: parent.drag; signalName: "activeChanged" }
        }
        SignalSpy {id: extMapRectTopLeftChanged; target: parent; signalName: "topLeftChanged" }
        SignalSpy {id: extMapRectBottomRightChanged; target: parent; signalName: "bottomRightChanged" }
        SignalSpy {id: extMapRectColorChanged; target: parent; signalName: "colorChanged"}
    }
    MapCircle {
        id: extMapCircle
        color: 'darkmagenta'
        center: Coordinate { latitude: 10; longitude: 30}
        radius: 400000
        MapMouseArea {
            id: extMapCircleMa
            anchors.fill: parent
            drag.target: parent
            SignalSpy { id: extMapCircleClicked; target: parent; signalName: "clicked" }
            SignalSpy { id: extMapCircleActiveChanged; target: parent.drag; signalName: "activeChanged" }
        }
        SignalSpy {id: extMapCircleCenterChanged; target: parent; signalName: "centerChanged"}
        SignalSpy {id: extMapCircleColorChanged; target: parent; signalName: "colorChanged"}
        SignalSpy {id: extMapCircleRadiusChanged; target: parent; signalName: "radiusChanged"}
        SignalSpy {id: extMapCircleBorderColorChanged; target: parent.border; signalName: "colorChanged"}
        SignalSpy {id: extMapCircleBorderWidthChanged; target: parent.border; signalName: "widthChanged"}
    }
    MapQuickItem {
        id: extMapQuickItem
        MapMouseArea {
            anchors.fill: parent
            drag.target: parent
            SignalSpy { id: extMapQuickItemClicked; target: parent; signalName: "clicked" }
            SignalSpy { id: extMapQuickItemActiveChanged; target: parent.drag; signalName: "activeChanged" }
        }
        coordinate: Coordinate { latitude: 35; longitude: 3}
        sourceItem: Rectangle {
            color: 'darkgreen'
            width: 20
            height: 20
        }
        SignalSpy { id: extMapQuickItemCoordinateChanged; target: parent; signalName: "coordinateChanged"}
        SignalSpy { id: extMapQuickItemAnchorPointChanged; target: parent; signalName: "anchorPointChanged"}
        SignalSpy { id: extMapQuickItemZoomLevelChanged; target: parent; signalName: "zoomLevelChanged"}
        SignalSpy { id: extMapQuickItemSourceItemChanged; target: parent; signalName: "sourceItemChanged"}
    }
    MapPolygon {
        id: extMapPolygon
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

    MapPolygon {
        id: extMapPolyline0
        color: 'darkgrey'
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
    MapRoute {
        id: extMapRoute
        line.color: 'yellow'
        // don't try this at home - route is not user instantiable
        route: Route {
            path: [
                Coordinate { latitude: 25; longitude: 14},
                Coordinate { latitude: 20; longitude: 18},
                Coordinate { latitude: 15; longitude: 15}
            ]
        }
        SignalSpy {id: extMapRouteRouteChanged; target: parent; signalName: "routeChanged"}
        SignalSpy {id: extMapRouteLineWidthChanged; target: parent.line; signalName: "widthChanged"}
        SignalSpy {id: extMapRouteLineColorChanged; target: parent.line; signalName: "colorChanged"}
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
            visualInspectionPoint("polygon 2 points")
            mouseClick(map, point.x - 5, point.y)
            compare(extMapPolygonClicked.count, 0)
            map.addMapItem(extMapPolygon0) // mustn't crash or ill-behave
            extMapPolygon.addCoordinate(polyCoordinate)
            visualInspectionPoint("polygon 3 points")
            mouseClick(map, point.x - 5, point.y)
            compare(extMapPolygonClicked.count, 1)
            extMapPolygon.removeCoordinate(polyCoordinate)
            visualInspectionPoint("polygon 2 points")
            extMapPolygon.removeCoordinate(extMapPolygon.path[1])
            visualInspectionPoint("polygon 1 points")
            extMapPolygon.removeCoordinate(extMapPolygon.path[0])
            visualInspectionPoint("polygon 0 points")
        }

        function test_polyline() {
            map.clearMapItems()
            clear_data()
            compare (extMapPolyline.line.width, 1.0)
            var point = map.toScreenPosition(extMapPolyline.path[1])
            map.addMapItem(extMapPolyline)
            visualInspectionPoint("polyline 2 points")
            map.addMapItem(extMapPolyline0) // mustn't crash or ill-behave
            extMapPolyline.addCoordinate(polyCoordinate)
            visualInspectionPoint("polyline 3 points")
            extMapPolyline.addCoordinate(extMapPolyline.path[0])
            visualInspectionPoint("polyline 4 points")
            // TODO when line rendering is ready
            //mouseClick(map, point.x - 5, point.y)
            //compare(extMapPolylineClicked.count, 1)
            extMapPolyline.removeCoordinate(extMapPolyline.path[0])
            visualInspectionPoint("polyline 3 points")
            extMapPolyline.removeCoordinate(polyCoordinate)
            visualInspectionPoint("polyline 2 points")
            extMapPolyline.removeCoordinate(extMapPolyline.path[1])
            visualInspectionPoint("polyline 1 points")
            extMapPolyline.removeCoordinate(extMapPolyline.path[0])
            visualInspectionPoint("polyline 0 points")
        }

        function clear_data() {
            extMapRectClicked.clear()
            extMapCircleClicked.clear()
            extMapQuickItemClicked.clear()
            extMapPolygonClicked.clear()
            extMapCircleCenterChanged.clear()
            extMapCircleColorChanged.clear()
            extMapCircleRadiusChanged.clear()
            extMapCircleBorderColorChanged.clear()
            extMapCircleBorderWidthChanged.clear()
            extMapRectTopLeftChanged.clear()
            extMapRectBottomRightChanged.clear()
            extMapRectColorChanged.clear()
            extMapPolylineColorChanged.clear()
            extMapPolylineWidthChanged.clear()
            extMapPolylinePathChanged.clear()
            extMapPolygonPathChanged.clear()
            extMapPolygonColorChanged.clear()
            extMapPolygonBorderColorChanged.clear()
            extMapPolygonBorderWidthChanged.clear()
            extMapRouteRouteChanged.clear()
            extMapRouteLineColorChanged.clear()
            extMapRouteLineWidthChanged.clear()
            extMapQuickItemCoordinateChanged.clear()
            extMapQuickItemAnchorPointChanged.clear()
            extMapQuickItemZoomLevelChanged.clear()
            extMapQuickItemSourceItemChanged.clear()
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
