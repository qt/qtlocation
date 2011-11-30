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
     MapMouseArea setup for this test case.
     Map dimensions are 100 * 100
     Item containing map is 120,120

                                         (50,50)
     (0,0)   ---------------------------------------------------- (100,0)
             | no mouse area            | mouse area overlapper |
             |                          |                       |
     (0,20)  ---------------------------------------------------- (100,20)
             | mouse area upper         | mouse area upper,     |
             |                          | mouse area overlapper |
             |                          |                       |
             |                          |                       |
             |                          |                       |
     (0,50)  ---------------------------------------------------- (100,50)
             | mouse area lower         | mouse area lower,     |
             |                          | mouse area overlapper |
             |                          |                       |
             |                          |                       |
             |                          |                       |
             |                          |                       |
             |                          |                       |
             |                          |                       |
     (0,100) ---------------------------------------------------- (100,100)    |
                                                                               |
                                                                               |
                                                                      ----------(120, 120)

     */

Item {
    id: page
    x: 0; y: 0;
    width: 120
    height: 120
    // General-purpose elements for the test:
    Plugin { id: testPlugin; name : "nokia"; PluginParameter {name: "mapping.host"; value: "for.nonexistent"}}
    Coordinate{ id: mapDefaultCenter; latitude: 20; longitude: 20}

    MapMouseEvent{
        id: mapMouseEvent
    }

    Map {
        id: map;
        x: 0; y: 0; width: 100; height: 100
        center: mapDefaultCenter
        plugin: testPlugin;

        property real lastWheelDelta: 0
        onWheel: lastWheelDelta = delta

        MapMouseArea {
            id: mouseUpper
            objectName: "mouseUpper"
            x: 0; y: 20; width: 100; height: 29
            property variant lastMouseEvent: null
            onClicked: lastMouseEvent = mouse
            onDoubleClicked: lastMouseEvent = mouse
            onPressed: lastMouseEvent = mouse
            onReleased: lastMouseEvent = mouse
            onPositionChanged: lastMouseEvent = mouse
            onPressAndHold:  lastMouseEvent = mouse
        }
        MapMouseArea {
            id: mouseLower
            objectName: "mouseLower"
            x: 0; y: 50; width: 100; height: 50
            property variant lastMouseEvent: null
            onClicked: lastMouseEvent = mouse
            onDoubleClicked: lastMouseEvent = mouse
            onPressed: lastMouseEvent = mouse
            onReleased: lastMouseEvent = mouse
            onPositionChanged: lastMouseEvent = mouse
            onPressAndHold:  lastMouseEvent = mouse
        }
        MapMouseArea {
            id: mouseOverlapper
            objectName: "mouseOverlapper"
            x: 50; y: 0; width: 50; height: 100
            property variant lastMouseEvent: null
            onClicked: lastMouseEvent = mouse
            onDoubleClicked: lastMouseEvent = mouse
            onPressed: lastMouseEvent = mouse
            onReleased: { lastMouseEvent = mouse }
            onPositionChanged: lastMouseEvent = mouse
            onPressAndHold: { lastMouseEvent = mouse }
        }
    }

    TestCase {
        name: "MapMouseArea"
        when: windowShown
        SignalSpy {id: mouseUpperClickedSpy; target: mouseUpper; signalName: "clicked"}
        SignalSpy {id: mouseLowerClickedSpy; target: mouseLower; signalName: "clicked"}
        SignalSpy {id: mouseOverlapperClickedSpy; target: mouseOverlapper; signalName: "clicked"}
        SignalSpy {id: mouseUpperDoubleClickedSpy; target: mouseUpper; signalName: "doubleClicked"}
        SignalSpy {id: mouseLowerDoubleClickedSpy; target: mouseLower; signalName: "doubleClicked"}
        SignalSpy {id: mouseOverlapperDoubleClickedSpy; target: mouseOverlapper; signalName: "doubleClicked"}
        SignalSpy {id: mouseUpperPressedSpy; target: mouseUpper; signalName: "onPressed"}
        SignalSpy {id: mouseLowerPressedSpy; target: mouseLower; signalName: "onPressed"}
        SignalSpy {id: mouseOverlapperPressedSpy; target: mouseOverlapper; signalName: "onPressed"}
        SignalSpy {id: mouseUpperReleasedSpy; target: mouseUpper; signalName: "released"}
        SignalSpy {id: mouseLowerReleasedSpy; target: mouseLower; signalName: "released"}
        SignalSpy {id: mouseOverlapperReleasedSpy; target: mouseOverlapper; signalName: "released"}
        SignalSpy {id: mouseUpperPositionChangedSpy; target: mouseUpper; signalName: "positionChanged"}
        SignalSpy {id: mouseLowerPositionChangedSpy; target: mouseLower; signalName: "positionChanged"}
        SignalSpy {id: mouseOverlapperPositionChangedSpy; target: mouseOverlapper; signalName: "positionChanged"}
        SignalSpy {id: mouseUpperPressAndHoldSpy; target: mouseUpper; signalName: "pressAndHold"}
        SignalSpy {id: mouseLowerPressAndHoldSpy; target: mouseLower; signalName: "pressAndHold"}
        SignalSpy {id: mouseOverlapperPressAndHoldSpy; target: mouseOverlapper; signalName: "pressAndHold"}
        SignalSpy {id: mouseUpperEnteredSpy; target: mouseUpper; signalName: "entered"}
        SignalSpy {id: mouseLowerEnteredSpy; target: mouseLower; signalName: "entered"}
        SignalSpy {id: mouseOverlapperEnteredSpy; target: mouseOverlapper; signalName: "entered"}
        SignalSpy {id: mouseUpperExitedSpy; target: mouseUpper; signalName: "exited"}
        SignalSpy {id: mouseLowerExitedSpy; target: mouseLower; signalName: "exited"}
        SignalSpy {id: mouseOverlapperExitedSpy; target: mouseOverlapper; signalName: "exited"}

        SignalSpy {id: mouseUpperEnabledChangedSpy; target: mouseUpper; signalName: "enabledChanged"}
        SignalSpy {id: mouseUpperAcceptedButtonsChangedSpy; target: mouseUpper; signalName: "acceptedButtonsChanged"}
        SignalSpy {id: mouseUpperPressedButtonsChangedSpy; target: mouseUpper; signalName: "pressedChanged"}
        SignalSpy {id: mouseUpperHoveredChangedSpy; target: mouseUpper; signalName: "hoveredChanged"}
        SignalSpy {id: mouseUpperPressedChangedSpy; target: mouseUpper; signalName: "pressedChanged"}

        SignalSpy {id: mouseOverlapperEnabledChangedSpy; target: mouseOverlapper; signalName: "enabledChanged"}

        SignalSpy {id: mapWheelSpy; target: map; signalName: "wheel"}

        function clear_data() {
            map.lastWheelDelta = 0
            mouseUpper.lastMouseEvent = null
            mouseLower.lastMouseEvent = null
            mouseOverlapper.lastMouseEvent = null

            mouseUpperClickedSpy.clear()
            mouseLowerClickedSpy.clear()
            mouseOverlapperClickedSpy.clear()
            mouseUpperDoubleClickedSpy.clear()
            mouseLowerDoubleClickedSpy.clear()
            mouseOverlapperDoubleClickedSpy.clear()
            mouseUpperPressedSpy.clear()
            mouseLowerPressedSpy.clear()
            mouseOverlapperPressedSpy.clear()
            mouseUpperReleasedSpy.clear()
            mouseLowerReleasedSpy.clear()
            mouseOverlapperReleasedSpy.clear()
            mouseUpperPositionChangedSpy.clear()
            mouseLowerPositionChangedSpy.clear()
            mouseOverlapperPositionChangedSpy.clear()
            mouseUpperPressAndHoldSpy.clear()
            mouseLowerPressAndHoldSpy.clear()
            mouseOverlapperPressAndHoldSpy.clear()
            mouseUpperEnteredSpy.clear()
            mouseLowerEnteredSpy.clear()
            mouseOverlapperEnteredSpy.clear()
            mouseUpperExitedSpy.clear()
            mouseLowerExitedSpy.clear()
            mouseOverlapperExitedSpy.clear()

            mouseUpperEnabledChangedSpy.clear()
            mouseUpperAcceptedButtonsChangedSpy.clear()
            mouseUpperPressedButtonsChangedSpy.clear()
            mouseUpperHoveredChangedSpy.clear()
            mouseUpperPressedChangedSpy.clear()
            mouseUpperPositionChangedSpy.clear()

            mouseOverlapperEnabledChangedSpy.clear()
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

        function test_enabled() {
            clear_data()
            // check that disabling overlapping mouse areas let events flow through
            mouseUpper.enabled = false
            compare(mouseUpperEnabledChangedSpy.count, 1)
            compare(mouseUpperClickedSpy.count, 0)
            mouseClick(map, 5, 25)
            compare(mouseUpperClickedSpy.count, 0)
            mouseUpper.enabled = true
            mouseClick(map, 5, 25)
            compare(mouseUpperClickedSpy.count, 1)
            compare(mouseUpperEnabledChangedSpy.count, 2)
            // when overlapping are is disabled, the event should flow through
            compare(mouseOverlapperClickedSpy.count, 0)
            mouseClick(map, 55, 25)
            compare(mouseUpperClickedSpy.count, 1)
            compare(mouseOverlapperClickedSpy.count, 1)
            mouseOverlapper.enabled = false
            compare(mouseOverlapperEnabledChangedSpy.count, 1)
            compare(mouseOverlapper.enabled, false)
            mouseClick(map, 55, 25)
            compare(mouseOverlapperClickedSpy.count, 1)
            compare(mouseUpperClickedSpy.count, 2)
            // re-enable and verify that still works
            mouseOverlapper.enabled = true
            compare(mouseOverlapperEnabledChangedSpy.count, 2)
            compare(mouseOverlapper.enabled, true)
            mouseClick(map, 55, 25)
            compare(mouseOverlapperClickedSpy.count, 2) // should consume again
            compare(mouseUpperClickedSpy.count, 2)
        }

        function test_wheel() {
            clear_data()
            // on map but without mouse area
            mouseWheel(map, 5, 5, 15, Qt.LeftButton, Qt.NoModifiers)
            compare(mapWheelSpy.count, 1)
            compare(map.lastWheelDelta, 15)
            mouseWheel(map, 5, 5, -15, Qt.LeftButton, Qt.NoModifiers)
            compare(mapWheelSpy.count, 2)
            compare(map.lastWheelDelta, -15)
            // on map on top of mouse area
            mouseWheel(map, 55, 75, -30, Qt.LeftButton, Qt.NoModifiers)
            compare(mapWheelSpy.count, 3)
            compare(map.lastWheelDelta, -30)
            // outside of map
            mouseWheel(map, -100, -100, 30, Qt.LeftButton, Qt.NoModifiers)
            compare(mapWheelSpy.count, 3)
            compare(map.lastWheelDelta, -30)
        }

        function test_aaa_basic_properties() // _aaa_ to ensure execution first
        {
            clear_data()
            wait(50)
            // default values
            compare(mouseUpper.containsMouse, false)
            compare(mouseUpper.pressed, false)
            compare(mouseUpper.enabled, true)
            compare(mouseUpper.pressedButtons, 0)
            compare(mouseUpper.acceptedButtons, Qt.LeftButton)
            // accepted buttons
            compare(mouseUpperAcceptedButtonsChangedSpy.count, 0)
            mouseUpper.acceptedButtons = Qt.RightButton  | Qt.MiddleButton
            compare(mouseUpper.acceptedButtons, Qt.RightButton | Qt.MiddleButton)
            compare(mouseUpperAcceptedButtonsChangedSpy.count, 1)
            mouseClick(map, 5, 25)
            compare(mouseUpperClickedSpy.count, 0) // left button not accepted
            mouseUpper.acceptedButtons = Qt.LeftButton
            compare(mouseUpperAcceptedButtonsChangedSpy.count, 2)
            mouseClick(map, 5, 25)
            compare(mouseUpperClickedSpy.count, 1)
        }

        function test_aab_event_basic_properties() {
            compare(mapMouseEvent.accepted, false)
            compare(mapMouseEvent.button, Qt.NoButton)
            compare(mapMouseEvent.modifiers, Qt.NoModifier)
            compare(mapMouseEvent.wasHeld, false)
            compare(mapMouseEvent.x, 0)
            compare(mapMouseEvent.y, 0)
        }

        function test_basic_position_changed() {
            // tests basic position changed/move when button is being pressed
            clear_data();
            mousePress(map, 5, 25)
            compare(mouseUpperPressedSpy.count, 1)
            compare(mouseUpper.lastMouseEvent.accepted, true)
            compare(mouseUpper.lastMouseEvent.button, Qt.LeftButton)
            compare(mouseUpper.lastMouseEvent.buttons, Qt.LeftButton)
            compare(mouseUpper.lastMouseEvent.modifiers, Qt.NoModifier)
            // moves within the mouse area
            mouseMove(map, 5, 26, 0, Qt.LeftButton) // '0' is 'delay'
            wait(1) // mouseMove event goes one extra eventloop round in the test lib
            compare(mouseUpperEnteredSpy.count, 1)
            compare(mouseUpperPositionChangedSpy.count, 1)
            compare(mouseUpper.mouseX, 5)
            compare(mouseUpper.mouseY, 6) // 20 offset, mouseXY is relative to the mouse area
            compare(mouseUpper.lastMouseEvent.accepted, true)
            compare(mouseUpper.lastMouseEvent.button, Qt.NoButton)
            compare(mouseUpper.lastMouseEvent.buttons, Qt.LeftButton) // buttons being pressed whilst movin'
            compare(mouseUpper.lastMouseEvent.modifiers, Qt.NoModifier)
            compare(mouseUpper.lastMouseEvent.wasHeld, false) // testfunction won't take required 800 ms
            compare(mouseUpper.lastMouseEvent.x, 5)
            compare(mouseUpper.lastMouseEvent.y, 6) // remember 20 offset of the mouse area

            mouseMove(map, 6, 27, 0, Qt.LeftButton | Qt.RightButton)
            wait(1)
            compare(mouseUpperEnteredSpy.count, 1) // no re-entry
            compare(mouseUpperPositionChangedSpy.count, 2)
            compare(mouseUpper.mouseX, 6)
            compare(mouseUpper.mouseY, 7)
            compare(mouseUpper.lastMouseEvent.accepted, true)
            compare(mouseUpper.lastMouseEvent.button, Qt.NoButton)
            compare(mouseUpper.lastMouseEvent.buttons, Qt.LeftButton | Qt.RightButton) // buttons being pressed whilst movin'
            compare(mouseUpper.lastMouseEvent.modifiers, Qt.NoModifier)
            compare(mouseUpper.lastMouseEvent.wasHeld, false) // testfunction won't take required 800 ms
            compare(mouseUpper.lastMouseEvent.x, 6)
            compare(mouseUpper.lastMouseEvent.y, 7) // remember 20 offset of the mouse area

            // moves outside of mouse but within map
            mouseMove(map, 2, 2, 0)
            wait(1)
            compare(mouseUpperExitedSpy.count, 1)
            compare(mouseUpperPositionChangedSpy.count, 3)
            compare(mouseUpper.mouseX, 2)
            compare(mouseUpper.mouseY, -18)
            // come back to map
            mouseMove(map, 7, 28, 0)
            wait(1)
            compare(mouseUpperEnteredSpy.count, 2)
            compare(mouseUpperExitedSpy.count, 1)
            compare(mouseUpperPositionChangedSpy.count, 4)
            compare(mouseUpper.mouseX, 7)
            compare(mouseUpper.mouseY, 8)

            // move outside of widget area (left). make sure that other mouse areas won't get the events
            mouseMove(map, -10, 10, 0)
            wait(1)
            compare(mouseUpperPositionChangedSpy.count, 5)
            compare(mouseUpperExitedSpy.count, 2)
            compare(mouseUpper.mouseX, -10)
            compare(mouseUpper.mouseY, -10)

            // back in and then on top of the widget
            mouseMove(map, 5, 25, 0)
            wait(1)
            compare(mouseUpperPositionChangedSpy.count, 6)
            compare(mouseUpperExitedSpy.count, 2)
            compare(mouseUpperEnteredSpy.count, 3)
            compare(mouseUpper.mouseX, 5)
            compare(mouseUpper.mouseY, 5)
            mouseMove(map, 5, -25, 0)
            wait(1)
            compare(mouseUpperPositionChangedSpy.count, 7)
            compare(mouseUpperExitedSpy.count, 3)
            compare(mouseUpperEnteredSpy.count, 3)
            compare(mouseUpper.mouseX, 5)
            compare(mouseUpper.mouseY, -45)

            // back in then float on top of other mouse areas
            mouseMove(map, 5, 25, 0)
            wait(1)
            compare(mouseUpperPositionChangedSpy.count, 8)
            compare(mouseUpperExitedSpy.count, 3)
            compare(mouseUpperEnteredSpy.count, 4)
            compare(mouseUpper.mouseX, 5)
            compare(mouseUpper.mouseY, 5)
            mouseMove(map, 5, 75, 0)
            wait(1)
            compare(mouseUpperPositionChangedSpy.count, 9)
            compare(mouseUpperExitedSpy.count, 4)
            compare(mouseUpperEnteredSpy.count, 4)
            compare(mouseUpper.mouseX, 5)
            compare(mouseUpper.mouseY, 55) // remember the 20 offset of upper mouse area
            mouseMove(map, 75, 75, 0)
            wait(1)
            compare(mouseUpperPositionChangedSpy.count, 10)
            compare(mouseUpperExitedSpy.count, 4)
            compare(mouseUpperEnteredSpy.count, 4)
            compare(mouseUpper.mouseX, 75)
            compare(mouseUpper.mouseY, 55)
            // finally back in
            mouseMove(map, 5, 25, 0)
            wait(1)
            compare(mouseUpperPositionChangedSpy.count, 11)
            compare(mouseUpperExitedSpy.count, 4)
            compare(mouseUpperEnteredSpy.count, 5)
            compare(mouseUpper.mouseX, 5)
            compare(mouseUpper.mouseY, 5)

            // check that these fellas didn't get any stupid ideas
            compare(mouseLowerEnteredSpy.count, 0)
            compare(mouseLowerPositionChangedSpy.count, 0)
            compare(mouseOverlapperEnteredSpy.count, 0)
            compare(mouseOverlapperPositionChangedSpy.count, 0)
            // release mouse
            mouseRelease(map, 5, 25)
            // TODO enable these!
            compare(mouseUpperEnteredSpy.count, 5)
            compare(mouseUpperExitedSpy.count, 5) // release triggers one more exited()
        }

        function test_basic_press_release() {
            clear_data()
            // send to emptiness
            mousePress(map, 5, 5)
            compare(mouseUpperPressedSpy.count, 0)
            compare(mouseLowerPressedSpy.count, 0)
            compare(mouseOverlapperPressedSpy.count, 0)
            mouseRelease(map, 5, 5)
            compare(mouseUpperReleasedSpy.count, 0)
            compare(mouseLowerReleasedSpy.count, 0)
            compare(mouseOverlapperReleasedSpy.count, 0)
            // send to upper mouse area
            mousePress(map, 5, 25)
            compare(mouseUpperPressedSpy.count, 1)
            compare(mouseLowerPressedSpy.count, 0)
            compare(mouseOverlapperPressedSpy.count, 0)

            compare(mouseUpper.lastMouseEvent.accepted, true)
            compare(mouseUpper.lastMouseEvent.button, Qt.LeftButton)
            compare(mouseUpper.lastMouseEvent.modifiers, Qt.NoModifier)
            compare(mouseUpper.lastMouseEvent.wasHeld, false)
            compare(mouseUpper.lastMouseEvent.x, 5)
            compare(mouseUpper.lastMouseEvent.y, 5) // remember 20 offset of the mouse area
            mousePress(map, 5, 26)
            compare(mouseUpperPressedSpy.count, 1)
            compare(mouseLowerPressedSpy.count, 0)
            compare(mouseOverlapperPressedSpy.count, 0)

            mouseRelease(map, 5, 26)
            compare(mouseUpperPressedSpy.count, 1)
            compare(mouseUpperReleasedSpy.count, 1)
            compare(mouseLowerPressedSpy.count, 0)
            compare(mouseLowerReleasedSpy.count, 0)
            compare(mouseUpper.lastMouseEvent.accepted, true)
            compare(mouseUpper.lastMouseEvent.button, Qt.LeftButton)
            compare(mouseUpper.lastMouseEvent.modifiers, Qt.NoModifier)
            compare(mouseUpper.lastMouseEvent.wasHeld, false)
            compare(mouseUpper.lastMouseEvent.x, 5)
            compare(mouseUpper.lastMouseEvent.y, 6) // remember 20 offset of the mouse area

            mousePress(map, 5, 75)
            compare(mouseUpperPressedSpy.count, 1)
            compare(mouseLowerPressedSpy.count, 1)
            compare(mouseOverlapperPressedSpy.count, 0)
            compare(mouseLower.lastMouseEvent.accepted, true)
            compare(mouseLower.lastMouseEvent.button, Qt.LeftButton)
            compare(mouseLower.lastMouseEvent.modifiers, Qt.NoModifier)
            compare(mouseLower.lastMouseEvent.wasHeld, false)
            compare(mouseLower.lastMouseEvent.x, 5)
            compare(mouseLower.lastMouseEvent.y, 25) // remember 50 offset of the mouse area

            mouseRelease(map, 5, 75)
            compare(mouseUpperPressedSpy.count, 1)
            compare(mouseUpperReleasedSpy.count, 1)
            compare(mouseLowerPressedSpy.count, 1)
            compare(mouseLowerReleasedSpy.count, 1)
            mousePress(map, 55, 75)
            compare(mouseUpperPressedSpy.count, 1)
            compare(mouseLowerPressedSpy.count, 1)
            compare(mouseOverlapperPressedSpy.count, 1)
            compare(mouseOverlapperReleasedSpy.count, 0)
            mouseRelease(map, 55, 25)
            compare(mouseUpperPressedSpy.count, 1)
            compare(mouseUpperReleasedSpy.count, 1)
            compare(mouseLowerPressedSpy.count, 1)
            compare(mouseLowerReleasedSpy.count, 1)
            compare(mouseOverlapperReleasedSpy.count, 1)
        }

        function test_basic_click() {
            clear_data();

            mouseClick(map, 5, 5, Qt.RightButton, Qt.AltModifier)
            compare(mouseUpperClickedSpy.count, 0)
            compare(mouseLowerClickedSpy.count, 0)
            compare(mouseOverlapperClickedSpy.count, 0)
            mouseUpper.acceptedButtons = Qt.LeftButton | Qt.RightButton
            mouseClick(map, 5, 25, Qt.RightButton, Qt.AltModifier)
            compare(mouseUpperClickedSpy.count, 1)
            compare(mouseLowerClickedSpy.count, 0)
            compare(mouseOverlapperClickedSpy.count, 0)
            compare(mouseUpper.lastMouseEvent.accepted, true)
            compare(mouseUpper.lastMouseEvent.button, Qt.RightButton)
            compare(mouseUpper.lastMouseEvent.modifiers, Qt.AltModifier)
            compare(mouseUpper.lastMouseEvent.wasHeld, false)
            compare(mouseUpper.lastMouseEvent.x, 5)
            compare(mouseUpper.lastMouseEvent.y, 5) // remember 20 offset of the mouse area
            // check we get valid geocoordinates (would be NaN if something was wrong)
            verify(mouseUpper.lastMouseEvent.coordinate.longitude > -180 && mouseUpper.lastMouseEvent.coordinate.longitude < 180)
            verify(mouseUpper.lastMouseEvent.coordinate.longitude > -90 && mouseUpper.lastMouseEvent.coordinate.latitude < 90)

            // mouse click with unaccepted buttons should not cause click
            mouseUpper.acceptedButtons = Qt.LeftButton
            mouseClick(map, 5, 25, Qt.RightButton, Qt.AltModifier)
            compare(mouseUpperClickedSpy.count, 1)
            compare(mouseLowerClickedSpy.count, 0)
            compare(mouseOverlapperClickedSpy.count, 0)

            mouseClick(map, 5, 25)
            compare(mouseUpperClickedSpy.count, 2)
            compare(mouseLowerClickedSpy.count, 0)
            compare(mouseOverlapperClickedSpy.count, 0)
            compare(mouseUpper.lastMouseEvent.modifiers, Qt.NoModifier)
            compare(mouseUpper.lastMouseEvent.button, Qt.LeftButton)
            mouseClick(map, 5, 55)
            compare(mouseUpperClickedSpy.count, 2)
            compare(mouseLowerClickedSpy.count, 1)
            compare(mouseOverlapperClickedSpy.count, 0)
            mouseClick(map, 5, 55)
            compare(mouseUpperClickedSpy.count, 2)
            compare(mouseLowerClickedSpy.count, 2)
            compare(mouseOverlapperClickedSpy.count, 0)
            // declaration order counts on overlap case; overlapping area
            // declared later will get the events
            mouseClick(map, 55, 25)
            compare(mouseUpperClickedSpy.count, 2)
            compare(mouseLowerClickedSpy.count, 2)
            compare(mouseOverlapperClickedSpy.count, 1)
            mouseClick(map, 55, 75)
            compare(mouseUpperClickedSpy.count, 2)
            compare(mouseLowerClickedSpy.count, 2)
            compare(mouseOverlapperClickedSpy.count, 2)
            real_click(map, 55, 25)
            compare(mouseUpperClickedSpy.count, 2)
            compare(mouseLowerClickedSpy.count, 2)
            compare(mouseOverlapperClickedSpy.count, 3)
            real_click(map, 55, 75)
            compare(mouseUpperClickedSpy.count, 2)
            compare(mouseLowerClickedSpy.count, 2)
            compare(mouseOverlapperClickedSpy.count, 4)
        }

        function test_basic_double_click() {
            clear_data();
            real_double_click(map, 5, 5)

            compare(mouseUpperDoubleClickedSpy.count, 0)
            compare(mouseLowerDoubleClickedSpy.count, 0)
            compare(mouseOverlapperDoubleClickedSpy.count, 0)
            real_double_click(map, 5, 25)
            compare(mouseUpper.lastMouseEvent.accepted, true)
            compare(mouseUpper.lastMouseEvent.button, Qt.LeftButton)
            compare(mouseUpper.lastMouseEvent.modifiers, Qt.NoModifier)
            compare(mouseUpper.lastMouseEvent.wasHeld, false)
            compare(mouseUpper.lastMouseEvent.x, 5)
            compare(mouseUpper.lastMouseEvent.y, 5) // remember 20 offset of the mouse area

            compare(mouseUpperDoubleClickedSpy.count, 1)
            compare(mouseLowerDoubleClickedSpy.count, 0)
            compare(mouseOverlapperDoubleClickedSpy.count, 0)
            real_double_click(map, 5, 25)
            compare(mouseUpperDoubleClickedSpy.count, 2)
            compare(mouseLowerDoubleClickedSpy.count, 0)
            compare(mouseOverlapperDoubleClickedSpy.count, 0)
            real_double_click(map, 5, 55)
            compare(mouseUpperDoubleClickedSpy.count, 2)
            compare(mouseLowerDoubleClickedSpy.count, 1)
            compare(mouseOverlapperDoubleClickedSpy.count, 0)
            real_double_click(map, 5, 55)
            compare(mouseUpperDoubleClickedSpy.count, 2)
            compare(mouseLowerDoubleClickedSpy.count, 2)
            compare(mouseOverlapperDoubleClickedSpy.count, 0)
            // declaration order counts on overlap case; overlapping area declared later will get the events
            real_double_click(map, 55, 25)
            compare(mouseUpperDoubleClickedSpy.count, 2)
            compare(mouseLowerDoubleClickedSpy.count, 2)
            compare(mouseOverlapperDoubleClickedSpy.count, 1)
            compare(mouseOverlapperPressedSpy.count, 1)
            compare(mouseOverlapperReleasedSpy.count, 2)
            real_double_click(map, 55, 75)
            compare(mouseUpperDoubleClickedSpy.count, 2)
            compare(mouseLowerDoubleClickedSpy.count, 2)
            compare(mouseOverlapperDoubleClickedSpy.count, 2)
            compare(mouseOverlapperPressedSpy.count, 2)
            compare(mouseOverlapperReleasedSpy.count, 4)
            // disable overlapping area and check event is delivered to the ones beneath
            mouseOverlapper.enabled = false
            real_double_click(map, 55, 25)
            compare(mouseUpperDoubleClickedSpy.count, 3)
            compare(mouseLowerDoubleClickedSpy.count, 2)
            compare(mouseOverlapperDoubleClickedSpy.count, 2)
            real_double_click(map, 55, 75)
            compare(mouseUpperDoubleClickedSpy.count, 3)
            compare(mouseLowerDoubleClickedSpy.count, 3)
            compare(mouseOverlapperDoubleClickedSpy.count, 2)
            mouseOverlapper.enabled = true
            real_double_click(map, 55, 25)
            compare(mouseUpperDoubleClickedSpy.count, 3)
            compare(mouseLowerDoubleClickedSpy.count, 3)
            compare(mouseOverlapperDoubleClickedSpy.count, 3)
            real_double_click(map, 55, 75)
            compare(mouseUpperDoubleClickedSpy.count, 3)
            compare(mouseLowerDoubleClickedSpy.count, 3)
            compare(mouseOverlapperDoubleClickedSpy.count, 4)
        }

        function test_zzz_basic_press_and_hold() { // _zzz_ to ensure execution last (takes time)
            clear_data();
            real_press_and_hold(map, 5, 5)
            compare(mouseUpperPressAndHoldSpy.count, 0)
            compare(mouseLowerPressAndHoldSpy.count, 0)
            compare(mouseOverlapperPressAndHoldSpy.count, 0)

            mousePress(map,5,25)
            wait(850) // threshold is 800 ms
            compare(mouseUpperPressAndHoldSpy.count, 1)
            compare(mouseLowerPressAndHoldSpy.count, 0)
            compare(mouseOverlapperPressAndHoldSpy.count, 0)
            compare(mouseUpper.lastMouseEvent.accepted, true)
            compare(mouseUpper.lastMouseEvent.button, Qt.LeftButton)
            compare(mouseUpper.lastMouseEvent.modifiers, Qt.NoModifier)
            compare(mouseUpper.lastMouseEvent.wasHeld, true) // notable part
            compare(mouseUpper.lastMouseEvent.x, 5)
            compare(mouseUpper.lastMouseEvent.y, 5) // remember 20 offset of the mouse area
            mouseRelease(map,5,25)
            real_press_and_hold(map, 5, 55)
            compare(mouseUpperPressAndHoldSpy.count, 1)
            compare(mouseLowerPressAndHoldSpy.count, 1)
            compare(mouseOverlapperPressAndHoldSpy.count, 0)
            real_press_and_hold(map, 55, 75)
            compare(mouseUpperPressAndHoldSpy.count, 1)
            compare(mouseLowerPressAndHoldSpy.count, 1)
            compare(mouseOverlapperPressAndHoldSpy.count, 1)
            compare(mouseOverlapper.lastMouseEvent.accepted, true)
            compare(mouseOverlapper.lastMouseEvent.button, Qt.LeftButton)
            compare(mouseOverlapper.lastMouseEvent.modifiers, Qt.NoModifier)
            compare(mouseOverlapper.lastMouseEvent.wasHeld, true)
            compare(mouseOverlapper.lastMouseEvent.x, 5)
            compare(mouseOverlapper.lastMouseEvent.y, 75)
            // make sure that the wasHeld is cleared
            mouseClick(map, 55, 75)
            compare(mouseOverlapper.lastMouseEvent.accepted, true)
            compare(mouseOverlapper.lastMouseEvent.button, Qt.LeftButton)
            compare(mouseOverlapper.lastMouseEvent.modifiers, Qt.NoModifier)
            compare(mouseOverlapper.lastMouseEvent.wasHeld, false)
            compare(mouseOverlapper.lastMouseEvent.x, 5)
            compare(mouseOverlapper.lastMouseEvent.y, 75)
            real_press_and_hold(map, 55, 25)
            compare(mouseUpperPressAndHoldSpy.count, 1)
            compare(mouseLowerPressAndHoldSpy.count, 1)
            compare(mouseOverlapperPressAndHoldSpy.count, 2)
        }
    }
}
