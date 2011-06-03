/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 1.0
import QtTest 1.0
import Qt.location 5.0

Item {
    id: masterItem
    // General-purpose elements for the test:
    Plugin { id: testPlugin; name : "nokia"; PluginParameter {name: "mapping.host"; value: "for.nonexistent"}}
    //Coordinate{ id: mapDefaultCenter; latitude: 10; longitude: 10}
    Coordinate{ id: mapDefaultCenter; latitude: 0; longitude: 0}
    Coordinate{ id: topLeftCoord; latitude: 10; longitude: 11}
    Coordinate{ id: bottomRightCoord; latitude: 12; longitude: 13}
    MapMouseEvent{ id: dummyMouseEvent;}
    Rectangle {id: rektankeli; anchors.top:  parent.top; width:  parent.width; height:  20}
    Map {
        id: mouseMap;
        anchors.top: rektankeli.bottom
        center: mapDefaultCenter
        plugin: testPlugin;
        MapMouseArea {
            id: mapMouse
            enabled: true
            //onClicked: console.log('mouse area clicked mapMouse')
        }
        MapCircle {
            MapMouseArea {
                id: circleMouse
                //onClicked: console.log('mouse area clicked circleMouse')
            }
        }
        MapRectangle {
            topLeft: topLeftCoord
            bottomRight: bottomRightCoord
            MapMouseArea {
                id: rectangleMouse
                enabled: false
                //onClicked: console.log('mouse area clicked rectanglemouse')
            }
        }
    }
    TestCase {
        name: "MapMouse"
        when: windowShown
        SignalSpy {id: rectangleClickedSpy; target: rectangleMouse; signalName: "clicked"}
        SignalSpy {id: rectangleEnabledSpy; target: rectangleMouse; signalName: "enabledChanged"}
        SignalSpy {id: rectangleAcceptedButtonsSpy; target: rectangleMouse; signalName: "acceptedButtonsChanged"}
        function test_signals() {
            // 1. Default values
            compare (circleMouse.mouseX, 0)
            compare (circleMouse.mouseY, 0)
            compare (circleMouse.pressed, false)
            compare (circleMouse.enabled, true)
            // 2. Basic changes
            compare (rectangleEnabledSpy.count, 0)
            compare (rectangleAcceptedButtonsSpy.count, 0)
            rectangleMouse.enabled = true
            rectangleMouse.acceptedButtons = Qt.MiddleButton
            compare (rectangleEnabledSpy.count, 1)
            compare (rectangleAcceptedButtonsSpy.count, 1)
            // 3. Clicking
            // MousePress does not work for some reason. It propagates all the way
            // to the QDeclarativeView but never reaches the graphics geo map.
            // That should be solved to really be able to test the mouse.
            //mousePress(masterItem, 5, 5)
            //mousePress(mouseMap, 20, 20)
            //compare(rectangleClickedSpy.count, 1)
        }
    }
}
