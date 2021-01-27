/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import QtTest 1.0
import QtPositioning 5.2
import QtLocation 5.5

Item {
    id: testCase

    MapPolyline {
        id: mapPolyline
        path: [
                { latitude: -27, longitude: 153.0 },
                { latitude: -27, longitude: 154.1 },
                { latitude: -28, longitude: 153.5 },
                { latitude: -29, longitude: 153.5 }
            ]
    }

    MapPolyline {
        id: mapPolylineGeopath
    }

    TestCase {
        name: "MapPolyline path"
        function test_path_operations() {
            compare(mapPolyline.path[1].latitude, -27)
            compare(mapPolyline.path[1].longitude, 154.1)
            compare(mapPolyline.coordinateAt(1), QtPositioning.coordinate(-27, 154.1))
            compare(mapPolyline.path.length, mapPolyline.pathLength())

            mapPolyline.removeCoordinate(1);
            compare(mapPolyline.path[1].latitude, -28)
            compare(mapPolyline.path[1].longitude, 153.5)
            compare(mapPolyline.coordinateAt(1), QtPositioning.coordinate(-28, 153.5))
            compare(mapPolyline.path.length, mapPolyline.pathLength())

            mapPolyline.addCoordinate(QtPositioning.coordinate(30, 153.1))
            compare(mapPolyline.path[mapPolyline.path.length-1].latitude, 30)
            compare(mapPolyline.path[mapPolyline.path.length-1].longitude, 153.1)
            compare(mapPolyline.containsCoordinate(QtPositioning.coordinate(30, 153.1)), true)
            compare(mapPolyline.path.length, mapPolyline.pathLength())

            mapPolyline.removeCoordinate(QtPositioning.coordinate(30, 153.1))
            compare(mapPolyline.path[mapPolyline.path.length-1].latitude, -29)
            compare(mapPolyline.path[mapPolyline.path.length-1].longitude, 153.5)
            compare(mapPolyline.containsCoordinate(QtPositioning.coordinate(30, 153.1)), false)
            compare(mapPolyline.path.length, mapPolyline.pathLength())

            mapPolyline.insertCoordinate(2, QtPositioning.coordinate(35, 153.1))
            compare(mapPolyline.path[2].latitude, 35)
            compare(mapPolyline.path[2].longitude, 153.1)
            compare(mapPolyline.containsCoordinate(QtPositioning.coordinate(35, 153.1)), true)
            compare(mapPolyline.path.length, mapPolyline.pathLength())

            mapPolyline.replaceCoordinate(2, QtPositioning.coordinate(45, 150.1))
            compare(mapPolyline.path[2].latitude, 45)
            compare(mapPolyline.path[2].longitude, 150.1)
            compare(mapPolyline.containsCoordinate(QtPositioning.coordinate(35, 153.1)), false)
            compare(mapPolyline.containsCoordinate(QtPositioning.coordinate(45, 150.1)), true)
            compare(mapPolyline.path.length, mapPolyline.pathLength())

            mapPolyline.insertCoordinate(2, QtPositioning.coordinate(35, 153.1))
            compare(mapPolyline.coordinateAt(2).latitude, 35)
            compare(mapPolyline.coordinateAt(2).longitude, 153.1)
            compare(mapPolyline.containsCoordinate(QtPositioning.coordinate(35, 153.1)), true)
            compare(mapPolyline.path.length, mapPolyline.pathLength())
        }
    }

    TestCase {
        name: "GeoPath path MapPolyLine"
        function test_qgeopath_path_operations() {
            var geopath = QtPositioning.path()

            geopath.path = mapPolyline.path
            compare(geopath.path.length, mapPolyline.pathLength())
            compare(geopath.boundingGeoRectangle(), mapPolyline.geoShape.boundingGeoRectangle())

            mapPolylineGeopath.geoShape = geopath
            compare(mapPolylineGeopath.pathLength(), mapPolyline.pathLength())
            compare(mapPolylineGeopath.geoShape.boundingGeoRectangle(), mapPolyline.geoShape.boundingGeoRectangle())
        }
    }

    TestCase {
        name: "GeoPolygon path MapPolyLine"
        function test_qgeopolygon_path_operations() {
            var geopolygon = QtPositioning.polygon()

            geopolygon.perimeter = mapPolyline.path
            compare(geopolygon.perimeter.length, mapPolyline.pathLength())
            compare(geopolygon.boundingGeoRectangle(), mapPolyline.geoShape.boundingGeoRectangle())
        }
    }
}
