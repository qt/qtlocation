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
    Plugin { id: testPlugin; name : "qmlgeo.test.plugin";}
    Coordinate{ id: mapDefaultCenter; latitude: 20; longitude: 20}

    function setMouseData(ma, me)
    {
        ma.lastX = me.x
        ma.lastY = me.y
        ma.lastButton = me.button
        ma.lastButtons = me.buttons
        ma.lastModifiers = me.modifiers
        ma.lastWasHeld = me.wasHeld
        ma.lastIsClick = me.isClick
        ma.lastAccepted = me.accepted
    }

    Map {
        id: map;
        x: 20; y: 20; width: 200; height: 200
        zoomLevel: 3
        center: mapDefaultCenter
        plugin: testPlugin;

        MapRectangle {
            id: preMapRect
            color: 'darkcyan'
            topLeft: Coordinate { latitude: 20; longitude: 20}
            bottomRight: Coordinate { latitude: 10; longitude: 30}
            MapMouseArea {
                id: preMapRectMa
                anchors.fill: parent
                SignalSpy { id: preMapRectClicked; target: parent; signalName: "clicked" }
            }
        }
        MapCircle {
            id: preMapCircle
            color: 'darkmagenta'
            center: Coordinate { latitude: 10; longitude: 30}
            radius: 400000
            MapMouseArea {
                id: preMapCircleMa
                anchors.fill: parent
                SignalSpy { id: preMapCircleClicked; target: parent; signalName: "clicked" }
            }
        }
    }

    TestCase {
        name: "MapMouseArea"
        when: windowShown

        function test_aa_items_on_map() { // aa et al. for execution order
            // sanity check that the coordinate conversion works, as
            // rest of the case relies on it. for robustness cut
            // a little slack with fuzzy compare
            wait(10)
            var mapcenter = map.toScreenPosition(map.center)
            verify (fuzzy_compare(mapcenter.x, 100))
            verify (fuzzy_compare(mapcenter.y, 100))

            // precondition
            compare(preMapRectClicked.count, 0)
            compare(preMapCircleClicked.count, 0)

            // click rect
            var point = map.toScreenPosition(preMapRect.topLeft)
            mouseClick(map, point.x + 5, point.y + 5)
            compare(preMapRectClicked.count, 1)
            mouseClick(map, 1, 1) // no item hit
            compare(preMapRectClicked.count, 1)
            compare(preMapCircleClicked.count, 0)
            // click circle, overlaps and is above rect
            pause() // marks good visual inspection point
            point = map.toScreenPosition(preMapCircle.center)
            mouseClick(map, point.x - 5, point.y - 5)
            compare(preMapRectClicked.count, 1)
            compare(preMapCircleClicked.count, 1)
            // click within circle bounding rect but not inside the circle geometry
            console.log('circle x y : ' +  preMapCircle.x + ' ' + preMapCircle.y)
            mouseClick(map, preMapCircle.x + 4, preMapCircle.y + 4)
            compare(preMapRectClicked.count, 2)
            compare(preMapCircleClicked.count, 1)
        }

        function clear_data() {
            preMapRectClicked.clear()
            preMapCircleClicked.clear()
        }

        function fuzzy_compare(val, ref) {
            var tolerance = 2;
            if ((val >= ref - tolerance) && (val <= ref + tolerance))
                return true;
            console.log('map fuzzy cmp returns false for value, ref, tolerance: ' + val + ', ' + ref + ', ' + tolerance)
            return false;
        }

        // call to pause testcase (for dev time visual inspection)
        function pause(time) {
            var waitTime = 1000
            if (time !== undefined)
                waitTime = time
            if (waitTime > 0) {
                console.log('halting for ' + waitTime + ' milliseconds')
                wait (waitTime)
            }
        }
        // these 'real_' prefixed functions do sequences as
        // it would occur on real app (e.g. doubleclick is in fact
        // a sequence of press, release, doubleclick, release).
        // (they were recorded as seen on test app). mouseClick() works ok
        // because testlib internally converts it to mousePress + mouseRelease events
        function real_click (target, x, y) {
            mousePress(target, x,y)
            mouseRelease(target, x, y)
        }
        function real_double_click (target, x, y) {
            mousePress(target, x,y)
            mouseRelease(target, x, y)
            mouseDoubleClick(target, x, y)
            mouseRelease(target, x, y)
        }
        function real_press_and_hold(target, x,y) {
            mousePress(target,x,y)
            wait(850) // threshold is 800 ms
            mouseRelease(target,x, y)
        }
    }
}
