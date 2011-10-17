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
    Plugin { id: testPlugin; name: "qmlgeo.test.plugin" }
    Coordinate{ id: coordinate1; latitude: 10; longitude: 11}
    Coordinate{ id: coordinate2; latitude: 12; longitude: 13}
    Map {id: pluginlessMap; center: coordinate1}
    Map {id: map; plugin: testPlugin;}
    SignalSpy {id: pluginlessMapPluginSpy; target: pluginlessMap; signalName: "pluginChanged" }
    SignalSpy {id: centerSpy; target: map; signalName: 'centerChanged'}
    SignalSpy {id: coordinate2LatitudeSpy; target: coordinate2; signalName: 'latitudeChanged'}
    SignalSpy {id: coordinate2LongitudeSpy; target: coordinate2; signalName: 'longitudeChanged'}
    SignalSpy {id: coordinate2AltitudeSpy; target: coordinate2; signalName: 'altitudeChanged'}

    TestCase {
        when: windowShown
        name: "Basic Map properties"

        function test_aa_map_properties_without_plugin() {
            // TODO
        }
        function test_ab_map_properties_with_plugin() {
            // TODO
        }
        function test_map_center() {
            // coordinate is set at declaration
            compare(pluginlessMap.center.latitude, 10)
            compare(pluginlessMap.center.longitude, 11)
            // default coordinate
            compare(map.center.latitude, -27.5)
            compare(map.center.longitude, 153)
            // change center and its values
            compare(centerSpy.count, 0)
            map.center = coordinate2
            compare(centerSpy.count, 1)
            map.center = coordinate2
            compare(centerSpy.count, 1)
            compare(coordinate2LatitudeSpy.count, 0)
            compare(coordinate2LongitudeSpy.count, 0)
            compare(coordinate2AltitudeSpy.count, 0)
            coordinate2.latitude = 21
            compare(coordinate2LatitudeSpy.count, 1)
            compare(centerSpy.count, 2)
            compare(coordinate2LongitudeSpy.count, 0)
            compare(coordinate2AltitudeSpy.count, 0)
            compare(map.center.latitude, 21)
            coordinate2.longitude = 31
            compare(coordinate2LatitudeSpy.count, 1)
            compare(centerSpy.count, 3)
            compare(coordinate2LongitudeSpy.count, 1)
            compare(coordinate2AltitudeSpy.count, 0)
            compare(map.center.longitude, 31)
            coordinate2.altitude = 41
            compare(coordinate2LatitudeSpy.count, 1)
            compare(centerSpy.count, 4)
            compare(coordinate2LongitudeSpy.count, 1)
            compare(coordinate2AltitudeSpy.count, 1)
            compare(map.center.altitude, 41)
            compare(map.center.longitude, 31)
            compare(map.center.latitude, 21)
        }
    }
}
