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
import QtLocation.test 5.0

Item {
    // General-purpose elements for the test:
    id: page
    width: 100
    height: 100
    //Plugin { id: testPlugin; name: "qmlgeo.test.plugin" }
    Plugin { id: nokiaPlugin; name: "nokia";  PluginParameter {name: "mapping.host"; value: "for.nonexistent"} }
    Coordinate{ id: coordinate1; latitude: 10; longitude: 11}
    Coordinate{ id: coordinate2; latitude: 12; longitude: 13}
    Map {
        id: map
        plugin: nokiaPlugin
        center: coordinate1;
        zoomLevel: 9;
        anchors.fill: page
        x:0; y:0
    }

    SignalSpy {id: centerSpy; target: map; signalName: 'centerChanged'}
    SignalSpy {id: coordinate2LatitudeSpy; target: coordinate2; signalName: 'latitudeChanged'}
    SignalSpy {id: coordinate2LongitudeSpy; target: coordinate2; signalName: 'longitudeChanged'}
    SignalSpy {id: coordinate2AltitudeSpy; target: coordinate2; signalName: 'altitudeChanged'}
    SignalSpy {id: pinchStartedSpy; target: map.pinch; signalName: 'pinchStarted'}
    SignalSpy {id: pinchUpdatedSpy; target: map.pinch; signalName: 'pinchUpdated'}
    SignalSpy {id: pinchFinishedSpy; target: map.pinch; signalName: 'pinchFinished'}
    SignalSpy {id: pinchMaximumZoomLevelChangeSpy; target: map.pinch; signalName: 'maximumZoomLevelChangeChanged'}
    SignalSpy {id: pinchMinimumZoomLevelSpy; target: map.pinch; signalName: 'minimumZoomLevelChanged'}
    SignalSpy {id: pinchMaximumZoomLevelSpy; target: map.pinch; signalName: 'maximumZoomLevelChanged'}
    SignalSpy {id: pinchMinimumRotationSpy; target: map.pinch; signalName: 'minimumRotationChanged'}
    SignalSpy {id: pinchMaximumRotationSpy; target: map.pinch; signalName: 'maximumRotationChanged'}
    SignalSpy {id: pinchRotationSpeedSpy; target: map.pinch; signalName: 'rotationSpeedChanged'}
    SignalSpy {id: pinchEnabledSpy; target: map.pinch; signalName: 'enabledChanged'}
    SignalSpy {id: pinchActiveSpy; target: map.pinch; signalName: 'activeChanged'}
    SignalSpy {id: pinchActiveGesturesSpy; target: map.pinch; signalName: 'activeGesturesChanged'}

    // From QtLocation.test plugin
    PinchGenerator {
        id: pinchGenerator
        anchors.fill: parent
        target: map
        enabled: false
    }

    TestCase {
        when: windowShown
        name: "Map pinch"

        function clear_data() {
            centerSpy.clear()
            coordinate2AltitudeSpy.clear()
            coordinate2LatitudeSpy.clear()
            coordinate2LongitudeSpy.clear()
            pinchStartedSpy.clear()
            pinchUpdatedSpy.clear()
            pinchFinishedSpy.clear()
            pinchMaximumZoomLevelChangeSpy.clear()
            pinchMinimumZoomLevelSpy.clear()
            pinchMaximumZoomLevelSpy.clear()
            pinchMinimumRotationSpy.clear()
            pinchMaximumRotationSpy.clear()
            pinchRotationSpeedSpy.clear()
            pinchEnabledSpy.clear()
            pinchActiveSpy.clear()
            pinchActiveGesturesSpy.clear()
        }

        function test_basic_properties() {
            clear_data()

            // pinch
            compare(map.pinch.enabled, true)
            map.pinch.enabled = false
            compare(pinchEnabledSpy.count, 1)
            compare(map.pinch.enabled, false)
            map.pinch.enabled = false
            compare(pinchEnabledSpy.count, 1)
            compare(map.pinch.enabled, false)
            map.pinch.enabled = true
            compare(pinchEnabledSpy.count, 2)
            compare(map.pinch.enabled, true)

            compare(map.pinch.active, false)

            verify(map.pinch.activeGestures & MapPinch.ZoomGesture)
            verify(map.pinch.activeGestures & MapPinch.RotationGesture)
            verify(!(map.pinch.activeGestures & MapPinch.TiltGesture))
            map.pinch.activeGestures = MapPinch.NoGesture
            compare(map.pinch.activeGestures, MapPinch.NoGesture)
            compare(pinchActiveGesturesSpy.count, 1)
            map.pinch.activeGestures = MapPinch.NoGesture
            compare(map.pinch.activeGestures, MapPinch.NoGesture)
            compare(pinchActiveGesturesSpy.count, 1)
            map.pinch.activeGestures = MapPinch.ZoomGesture | MapPinch.RotationGesture
            compare(map.pinch.activeGestures, MapPinch.ZoomGesture | MapPinch.RotationGesture)
            compare(pinchActiveGesturesSpy.count, 2)
            map.pinch.activeGestures = MapPinch.RotationGesture
            compare(map.pinch.activeGestures, MapPinch.RotationGesture)
            compare(pinchActiveGesturesSpy.count, 3)
            map.pinch.activeGestures = MapPinch.ZoomGesture
            compare(map.pinch.activeGestures, MapPinch.ZoomGesture)
            compare(pinchActiveGesturesSpy.count, 4)

            compare(map.pinch.minimumZoomLevel, map.minimumZoomLevel)
            map.pinch.minimumZoomLevel = 5
            compare(pinchMinimumZoomLevelSpy.count, 1)
            compare(map.pinch.minimumZoomLevel, 5)
            map.pinch.minimumZoomLevel = -1 // too small
            map.pinch.minimumZoomLevel = 492 // too big
            compare(pinchMinimumZoomLevelSpy.count, 1)
            compare(map.pinch.minimumZoomLevel, 5)
            map.pinch.minimumZoomLevel = map.minimumZoomLevel
            compare(pinchMinimumZoomLevelSpy.count, 2)
            compare(map.pinch.minimumZoomLevel, map.minimumZoomLevel)

            compare(map.pinch.maximumZoomLevel, map.maximumZoomLevel)
            map.pinch.maximumZoomLevel = 9
            compare (pinchMaximumZoomLevelSpy.count, 1)
            compare(map.pinch.maximumZoomLevel, 9)
            map.pinch.maximumZoomLevel = -1 // too small
            map.pinch.maximumZoomLevel = 3234 // too big
            compare(pinchMaximumZoomLevelSpy.count, 1)
            compare(map.pinch.maximumZoomLevel, 9)
            map.pinch.maximumZoomLevel = map.maximumZoomLevel
            compare(pinchMaximumZoomLevelSpy.count, 2)
            compare(map.pinch.maximumZoomLevel, map.maximumZoomLevel)

            clear_data()
            map.pinch.minimumZoomLevel = 5  // ok
            map.pinch.maximumZoomLevel = 9  // ok
            map.pinch.minimumZoomLevel = 10 // bigger than max
            map.pinch.maximumZoomLevel = 4  // smaller than min
            compare (pinchMaximumZoomLevelSpy.count, 1)
            compare (pinchMinimumZoomLevelSpy.count, 1)
            compare(map.pinch.maximumZoomLevel, 9)
            compare(map.pinch.minimumZoomLevel, 5)
            map.pinch.minimumZoomLevel = map.minimumZoomLevel
            map.pinch.maximumZoomLevel = map.maximumZoomLevel

            compare(map.pinch.maximumZoomLevelChange, 2)
            map.pinch.maximumZoomLevelChange = 4
            compare(pinchMaximumZoomLevelChangeSpy.count, 1)
            compare (map.pinch.maximumZoomLevelChange, 4)
            map.pinch.maximumZoomLevelChange = 4
            compare(pinchMaximumZoomLevelChangeSpy.count, 1)
            compare (map.pinch.maximumZoomLevelChange, 4)
            map.pinch.maximumZoomLevelChange = 11   // too big
            map.pinch.maximumZoomLevelChange = 0.01 // too small
            map.pinch.maximumZoomLevelChange = -1   // too small
            compare(pinchMaximumZoomLevelChangeSpy.count, 1)
            compare (map.pinch.maximumZoomLevelChange, 4)
            map.pinch.maximumZoomLevelChange = 2
            compare(pinchMaximumZoomLevelChangeSpy.count, 2)
            compare (map.pinch.maximumZoomLevelChange, 2)

            compare(map.pinch.minimumRotation, 0)
            map.pinch.minimumRotation = 10
            compare(pinchMinimumRotationSpy.count, 1)
            compare(map.pinch.minimumRotation, 10)
            map.pinch.minimumRotation = 10
            compare(pinchMinimumRotationSpy.count, 1)
            compare(map.pinch.minimumRotation, 10)
            map.pinch.minimumRotation = -1  // too small
            map.pinch.minimumRotation = 361 // too big
            compare(pinchMinimumRotationSpy.count, 1)
            compare(map.pinch.minimumRotation, 10)
            map.pinch.minimumRotation = 0
            compare(pinchMinimumRotationSpy.count, 2)
            compare(map.pinch.minimumRotation, 0)

            compare(map.pinch.maximumRotation, 45)
            map.pinch.maximumRotation = 55
            compare(pinchMaximumRotationSpy.count,1)
            compare(map.pinch.maximumRotation, 55)
            map.pinch.maximumRotation = 55
            compare(pinchMaximumRotationSpy.count,1)
            compare(map.pinch.maximumRotation, 55)
            map.pinch.maximumRotation = -1  // too small
            map.pinch.maximumRotation = 362 // too big
            compare(pinchMaximumRotationSpy.count,1)
            compare(map.pinch.maximumRotation, 55)
            map.pinch.maximumRotation = 45
            compare(pinchMaximumRotationSpy.count,2)
            compare(map.pinch.maximumRotation, 45)

            compare(map.pinch.rotationSpeed, 1)
            map.pinch.rotationSpeed = 2
            compare(pinchRotationSpeedSpy.count, 1)
            compare(map.pinch.rotationSpeed, 2)
            map.pinch.rotationSpeed = 2
            compare(pinchRotationSpeedSpy.count, 1)
            compare(map.pinch.rotationSpeed, 2)
            map.pinch.rotationSpeed = -1 // too small
            map.pinch.rotationSpeed = 11  // too big
            compare(pinchRotationSpeedSpy.count, 1)
            compare(map.pinch.rotationSpeed, 2)
            map.pinch.rotationSpeed = 1
            compare(pinchRotationSpeedSpy.count, 2)
            compare(map.pinch.rotationSpeed, 1)

            compare(map.pinch.maximumTilt, 90)
            compare(map.pinch.minimumTilt, 0)
            compare(map.pinch.maximumTiltChange, 20)

            // flick


        }

        function test_pinch_rotation() {
            map.pinch.activeGestures = MapPinch.RotationGesture
            clear_data()
            // todo

        }


        function test_pinch_zoom() {
            map.pinch.activeGestures = MapPinch.ZoomGesture
            clear_data()
            // 1. typical zoom in
            compare(map.zoomLevel, 9)
            map.pinch.maximumZoomLevelChange = 2
            compare(map.pinch.active, false)
            pinchGenerator.pinch(
                        Qt.point(0,50),   // point1From
                        Qt.point(50,50),  // point1To
                        Qt.point(100,50), // point2From
                        Qt.point(50,50),  // point2To
                        20,               // interval between touch events (swipe1), default 20ms
                        20,               // interval between touch events (swipe2), default 20ms
                        10,               // number of touchevents in point1from -> point1to, default 10
                        10);              // number of touchevents in point2from -> point2to, default 10
            tryCompare(pinchStartedSpy, "count", 1);
            wait(50);
            compare(pinchActiveSpy.count,1) // check that pinch is active
            compare(map.pinch.active, true)
            wait(200);
            verify(pinchUpdatedSpy.count >= 5); // verify 'sane' number of updates received
            tryCompare(pinchFinishedSpy, "count", 1);
            compare(pinchActiveSpy.count,2)
            compare(map.pinch.active, false)
            compare(map.zoomLevel, 8)
            // 2. typical zoom out
            clear_data();
            map.pinch.maximumZoomLevelChange = 2
            pinchGenerator.pinch(Qt.point(50,50), Qt.point(0,50),Qt.point(50,50), Qt.point(100,50));
            tryCompare(pinchStartedSpy, "count", 1);
            wait(250);
            verify(pinchUpdatedSpy.count >= 5); // verify 'sane' number of updates received
            tryCompare(pinchFinishedSpy, "count", 1);
            compare(map.zoomLevel, 9)
            // 3. zoom in and back out (direction change during same pinch)
            clear_data();
            pinchGenerator.pinch(Qt.point(0,50), Qt.point(100,50), Qt.point(100,50),Qt.point(0,50));
            tryCompare(pinchStartedSpy, "count", 1);
            wait(250);
            verify(pinchUpdatedSpy.count >= 5); // verify 'sane' number of updates received
            tryCompare(pinchFinishedSpy, "count", 1);
            compare(map.zoomLevel, 9) // should remain the same
            // 4. typical zoom in with different change level
            clear_data();
            map.pinch.maximumZoomLevelChange = 4
            compare (map.pinch.maximumZoomLevelChange, 4)
            pinchGenerator.pinch(Qt.point(0,50),Qt.point(50,50),Qt.point(100,50),Qt.point(50,50));
            wait(250);
            tryCompare(pinchFinishedSpy, "count", 1);
            compare(map.zoomLevel, 7)
            // 5. typical zoom out with different change level
            clear_data();
            map.pinch.maximumZoomLevelChange = 1
            compare (map.pinch.maximumZoomLevelChange, 1)
            pinchGenerator.pinch(Qt.point(50,50), Qt.point(0,50),Qt.point(50,50), Qt.point(100,50));
            wait(250);
            tryCompare(pinchFinishedSpy, "count", 1);
            compare(map.zoomLevel, 7.5)

            // 6. try to zoom in below minimum zoom level
            clear_data()
            map.pinch.maximumZoomLevelChange = 4
            map.pinch.minimumZoomLevel = 7
            pinchGenerator.pinch(Qt.point(0,50),Qt.point(50,50),Qt.point(100,50),Qt.point(50,50));
            wait(250);
            tryCompare(pinchFinishedSpy, "count", 1);
            compare(map.zoomLevel, 7) // would go to 5.5

            // 7. try to zoom out above maximum zoom level
            clear_data()
            map.pinch.maximumZoomLevelChange = 4
            map.pinch.maximumZoomLevel = 8
            pinchGenerator.pinch(Qt.point(50,50), Qt.point(0,50),Qt.point(50,50), Qt.point(100,50));
            wait(250);
            tryCompare(pinchFinishedSpy, "count", 1);
            compare(map.zoomLevel, 8) // would go to 9

            // 8. pinch when max and min are same
            clear_data()
            map.pinch.maximumZoomLevel = 8
            map.pinch.minimumZoomLevel = 8
            compare(map.pinch.maximumZoomLevel, 8)
            compare(map.pinch.minimumZoomLevel, 8)
            pinchGenerator.pinch(Qt.point(0,50),Qt.point(50,50),Qt.point(100,50),Qt.point(50,50));
            wait(250);
            tryCompare(pinchFinishedSpy, "count", 1);
            compare(map.zoomLevel, 8)

            // 9. pinch when max..min is not where map zoomLevel currently is
            clear_data()
            map.pinch.maximumZoomLevelChange = 4
            map.pinch.minimumZoomLevel = 4
            map.pinch.maximumZoomLevel = 6
            // first when above the zoom range
            map.zoomLevel = 10
            pinchGenerator.pinch(Qt.point(50,50), Qt.point(0,50),Qt.point(50,50), Qt.point(100,50)); // zoom out
            wait(250);
            tryCompare(pinchFinishedSpy, "count", 1);
            compare(map.zoomLevel, 6)
            map.zoomLevel = 10
            pinchGenerator.pinch(Qt.point(0,50),Qt.point(50,50),Qt.point(100,50),Qt.point(50,50)); // zoom in
            wait(250);
            tryCompare(pinchFinishedSpy, "count", 2);
            compare(map.zoomLevel, 6)
            console.log('the zoomLevel after pinching outside of range: ' + map.zoomLevel)
            pinchGenerator.pinch(Qt.point(0,50),Qt.point(50,50),Qt.point(100,50),Qt.point(50,50)); // zoom in
            wait(250);
            tryCompare(pinchFinishedSpy, "count", 3);
            compare(map.zoomLevel, 4)
            // when below the zoom range
            map.zoomLevel = 1
            pinchGenerator.pinch(Qt.point(50,50), Qt.point(0,50),Qt.point(50,50), Qt.point(100,50)); // zoom out
            wait(250);
            tryCompare(pinchFinishedSpy, "count", 4);
            compare(map.zoomLevel, 4)
            map.zoomLevel = 1
            pinchGenerator.pinch(Qt.point(0,50),Qt.point(50,50),Qt.point(100,50),Qt.point(50,50)); // zoom in
            wait(250);
            tryCompare(pinchFinishedSpy, "count", 5);
            compare(map.zoomLevel, 4)
            console.log('the zoomLevel after pinching outside of range: ' + map.zoomLevel)
            pinchGenerator.pinch(Qt.point(50,50), Qt.point(0,50),Qt.point(50,50), Qt.point(100,50)); // zoom out
            wait(250);
            tryCompare(pinchFinishedSpy, "count", 6);
            compare(map.zoomLevel, 6)
            map.pinch.minimumZoomLevel = map.minimumZoomLevel
            map.pinch.maximumZoomLevel = map.maximumZoomLevel

            // 10. pinch while pinch area is disabled
            clear_data()
            map.pinch.enabled = false
            map.pinch.maximumZoomLevelChange = 2
            pinchGenerator.pinch(Qt.point(50,50), Qt.point(0,50),Qt.point(50,50), Qt.point(100,50));
            wait(100);
            compare(pinchActiveSpy.count, 0)
            compare(map.pinch.active, false)
            compare(pinchStartedSpy.count, 0)
            compare(pinchUpdatedSpy.count, 0);
            compare(pinchFinishedSpy.count, 0);
            compare(map.zoomLevel, 6)
            pinchGenerator.stop()
            map.pinch.enabled = true

            // 11. pinch disabling during pinching
            clear_data()
            pinchGenerator.pinch(Qt.point(50,50), Qt.point(0,50),Qt.point(50,50), Qt.point(100,50));
            wait(250)
            map.pinch.enabled = false
            // check that pinch is active. then disable the pinch. pinch area should still process
            // as long as it is active
            compare(pinchActiveSpy.count,1)
            compare(map.pinch.active, true)
            compare(pinchStartedSpy.count, 1)
            compare(pinchFinishedSpy.count, 0)
            var pinchupdates = pinchUpdatedSpy.count
            console.log('this many pinches there were ------ ' + pinchupdates)
            verify(pinchupdates > 0)
            tryCompare(pinchFinishedSpy, "count", 1)
            compare(pinchActiveSpy.count,2)
            compare(map.pinch.active, false)
            map.pinch.enabled = true

            // 12. check nuthin happens if no active gestures
            clear_data()
            map.pinch.activeGestures = MapPinch.NoGesture
            pinchGenerator.pinch(Qt.point(50,50), Qt.point(0,50),Qt.point(50,50), Qt.point(100,50));
            tryCompare(pinchStartedSpy, "count", 0);
            wait(250);
            compare(pinchUpdatedSpy.count, 0);
            compare(pinchStartedSpy.count, 0);
            compare(map.zoomLevel, 7)
            map.pinch.activeGestures = MapPinch.ZoomGesture
        }
    }
}
