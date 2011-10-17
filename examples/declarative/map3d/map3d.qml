/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import QtLocation 5.0
import QtLocation.test 5.0
import "common" as Common

Item {
    objectName: "The page."
    width:  1140     //360
    height: 1085    // 640
    //width:  360
    //height: 640
    id: page

    //Rectangle {
    //    id: bottleAnimation
    //    width: animation.width; height: animation.height + 8
        // }

    // From location.test plugin
    TestModel {
          id: testModel
          datatype: 'coordinate'
          datacount: 8
          delay: 0
          crazyMode:  false  // generate arbitrarily updates. interval is set below, and the number of items is varied between 0..datacount
          crazyLevel: 2000 // the update interval varies between 3...crazyLevel (ms)
      }

    Item {
        visible: false
        id: shaderItem
        width: 256
        height: 256
        x: 100
        y: 100
        Rectangle {
            radius: 20
            id: shaderRect
            width: parent.width
            height: parent.height/4
            color: 'red'
            z: 1
            Text {text: "Wicked!"}

            SequentialAnimation on color {
                loops: Animation.Infinite
                ColorAnimation { from: "DeepSkyBlue"; to: "red"; duration: 2000 }
                ColorAnimation { from: "red"; to: "DeepSkyBlue"; duration: 2000 }
            }
        }
        Rectangle {
            id: shaderRect2
            opacity: 0.5
            //width: parent.width
            anchors.fill: shaderRect
            //height: parent.height/4
            color: 'green'
            Text {text: "Sick!"}
        }
        Rectangle {
            id: shaderRect3
            width: parent.width
            anchors.top: shaderRect2.bottom
            height: parent.height/4
            color: 'blue'
            Text {text: "Sick!"}
        }
        Rectangle {
            width: parent.width
            anchors.top: shaderRect3.bottom
            height: parent.height/4
            color: 'yellow'
            Text {text: "Sick!"}
        }
    }

    Column {
        id: buttonColumn
        anchors.top: page.top
        anchors.left:  map.right
        spacing: 2

        Rectangle {color: "lightblue"; width: 80; height: 80;
            Text {text: "Crazy mode:\n" + testModel.crazyMode + "\nclick to\ntoggle."}
            MouseArea{ anchors.fill: parent;
                onClicked: testModel.crazyMode = !testModel.crazyMode
                onDoubleClicked: map.removeMapItem(mapItem1)
            }
        }
        AnimatedImage {
            MouseArea { anchors.fill: parent; onClicked: mapItem2.source = parent }
            width: 80
            height: 80
            playing: testModel.crazyMode
            source: "blinky.gif"
        }
        Rectangle {color: "lightblue"; width: 80; height: 80;
            Text {text: "Click:\nadd item1\nDouble-click:\nrm item1"}
            MouseArea{ anchors.fill: parent;
                onClicked: {console.log('----------------adding item 1'); map.addMapItem(externalStaticMapItem1);}
                onDoubleClicked: {console.log('+++++++++++++++ removing item 1'); map.removeMapItem(externalStaticMapItem1);}
            }
        }
        Rectangle {color: "lightblue"; width: 80; height: 80;
            Text {text: "Click:\nadd item2\nDouble-click:\nrm item2"}
            MouseArea{ anchors.fill: parent;
                onClicked: {console.log('adding item 2'); map.addMapItem(externalStaticMapItem2);}
                onDoubleClicked: {console.log('removing item 2'); map.removeMapItem(externalStaticMapItem2);}
            }
        }
        Rectangle {color: "lightblue"; width: 80; height: 40;
            Text {text: "Click:\nlat++"}
            MouseArea{ anchors.fill: parent;
                onClicked: { mapCenterCoordinate.latitude += 1}
            }
        }
        Rectangle {color: "lightblue"; width: 80; height: 40;
            Text {text: "Click:\nlat--"}
            MouseArea{ anchors.fill: parent;
                onClicked: { mapCenterCoordinate.latitude -= 1}
            }
        }
        Rectangle {color: "lightblue"; width: 80; height: 40;
            Text {text: "Click:\nlon++"}
            MouseArea{ anchors.fill: parent;
                onClicked: { mapCenterCoordinate.longitude += 1}
            }
        }
        Rectangle {color: "lightblue"; width: 80; height: 40;
            Text {text: "Click:\nlon--"}
            MouseArea{ anchors.fill: parent;
                onClicked: { mapCenterCoordinate.longitude -= 1}
            }
        }
        Rectangle {color: "lightblue"; width: 80; height: 40;
            Text {text: "Click:\nalt++"}
            MouseArea{ anchors.fill: parent;
                onClicked: { mapCenterCoordinate.altitude += 1}
            }
        }
        Rectangle {color: "lightblue"; width: 80; height: 40;
            Text {text: "Click:\nalt--"}
            MouseArea{ anchors.fill: parent;
                onClicked: { mapCenterCoordinate.altitude -= 1}
            }
        }
    }

    /*
    MapItem {
        id: externalStaticMapItem1
        objectName: "externalStaticMapItem1"
        coordinate: brisbaneCoordinate
        zoomLevel: 5.0
        source: Rectangle {
            color: "gray"
            width: 140
            height: 20
            Text {font.pixelSize: 15;text: "ext map item 1"; font.bold: true; color: 'red'}
        }
    }

    MapItem {
        id: externalStaticMapItem2
        objectName: "externalStaticMapItem2"
        coordinate: brisbaneCoordinate2
        zoomLevel: 5.0
        source: Rectangle {
            color: "gray"
            width: 140
            height: 20
            Text {font.pixelSize: 15;text: "ext map item 2"; font.bold: true; color: 'red'}
        }
    }
    */

    /*
    MapItem {
        id: mapItem1
        source: AnimatedImage {width: 80; height: 80; playing: true; source: "walk.gif"}
    }
    */

    //AnimatedImage {width: 80; height: 80; playing: true; source: "walk.gif"}
    //MapItem {id: mapItem2 }
    //MapItem {id: mapItem3 }
    //MapItem {id: mapItem4 }
    //MapItem {id: mapItem5 }
    //MapItem {id: mapItem6 }
    //MapItem {id: mapItem7 }
    //MapItem {id: mapItem8 }

    Coordinate {
        id: brisbaneCoordinate
        latitude: -27.5
        longitude: 140
    }

    Coordinate {
        id: brisbaneCoordinate2
        latitude: -30.5
        longitude: 140
    }

    Map {
        id: map

        /*
        MapMouseArea {
            id: mapMouseArea
            onDoubleClicked: console.log('mapmousearea got clicked')
            anchors.fill: parent
        }
        */

        /*
        MapItem {
            id: externalStaticMapItem1
            visible: true
            objectName: "externalStaticMapItem1"
            coordinate: brisbaneCoordinate
            zoomLevel: 5.0
            source: Item {
                width: 640
                height: 20
                Rectangle {
                    color: "gray"
                    width: 640
                    height: 20
                    Text {font.pixelSize: 15;text: "ext map item 1"; font.bold: true; color: 'red'}
                }
            }
        }
        */

        /*
        MapItem {
            objectName: 'blinky static item'
            zoomLevel: 7 // at which map's  zoom level the width and height are '1-to-1'
            coordinate: brisbaneCoordinate
            source: AnimatedImage {
                width: 80
                height: 80
                playing: true
                source: "blinky.gif"
            }
        }
        */


        //MapItem {
        //    source: Rectangle { width: 40; height: 40; color: 'chocolate'
        //    }
        // }
        /*
        MapObjectView {
            id: theObjectView
            model: testModel
            delegate: Component {
                MapItem {
                    objectName: 'one of many items from model'
                    visible: true
                    live: true
                    recursive: true
                    source: Rectangle {
                        width: 300; height: 300; color: 'green'
                        Component.onCompleted: {
                            var num = (Math.floor(4 * Math.random()));
                            switch (num % 4) {
                            case 0:
                                color = "#ff0000";
                                break;
                            case 1:
                                color = "#0000ff";
                                break;
                            case 2:
                                color = "#00ffff";
                                break;
                            case 3:
                                color = "#00ff00";
                                break;
                            }
                        }

                    }
                    coordinate: Coordinate {
                        latitude: modeldata.coordinate.latitude;
                        longitude: modeldata.coordinate.longitude;
                    }
                }
            }
        }
        */

        // From location.test plugin
        PinchGenerator {
            id: pinchGenerator
            anchors.fill: parent
            target: map
            enabled: false
            focus: true           // enables keyboard control for convinience
            replaySpeedFactor: 1.1 // replay with 1.1 times the recording speed to better see what happens
            Text {
                text: "PinchArea state: " + pinchGenerator.state + "\n"
                      + "Swipes recorded: " + pinchGenerator.count + "\n"
                      + "Replay speed factor: " + pinchGenerator.replaySpeedFactor
            }
        }
        /*
        Keys.onPressed: {
            if (event.key == Qt.Key_A) {
                console.log('Key A was pressed');
                //event.accepted = true;
            }
        }
        */

        Row {
            id: textRow1
            spacing: 15
            y: 1050
            Text {id: firstText; text: "Map zoom level: " + map.zoomLevel; color: 'red'; font.bold: true}
            Text {text: "Pinch zoom sensitivity: " + map.pinch.maximumZoomLevelChange; color: firstText.color; font.bold: true}
            Text {text: "Pinch rotation sensitivity: " + map.pinch.rotationSpeed; color: firstText.color; font.bold: true}
        }

        Row {
            spacing: 15
            anchors.top: textRow1.bottom
            Text {text: "Pinch tilt sensitivity: " + map.pinch.maximumTiltChange; color: firstText.color; font.bold: true}
            Text {text: "Flick deceleration: " + map.flick.deceleration; color: firstText.color; font.bold: true}
            Text {text: "Weather: Sunny, mild, late showers."; color: firstText.color; font.bold: true}
        }
        plugin : Plugin {name : "nokia"}
        // commented features are checked to work at least somehow
        x: 0
        y: 0
        //size.width: 100
        //size.height: 100
        //anchors.left: parent.left
        //anchors.bottom: parent.bottom
        //anchors.leftMargin: 70
        //scale: 2
        //visible: false
        //transform: Translate {y: 200}
        //anchors.fill: page
        width: page.width - 80
        height: 1000
        zoomLevel: 5.1

        // pinch.activeGestures: MapPinch.ZoomGesture | RotationGesture
        pinch.activeGestures: MapPinch.NoGesture

        pinch.enabled: true
        pinch.maximumZoomLevel: 20        // biggest zoomlevel allowed
        pinch.minimumZoomLevel: 1         // smallest zoomlevel allowed
        pinch.maximumZoomLevelChange: 1.0 // maximum zoomlevel changes per pinch
        pinch.maximumRotation: 0 // unlimited
        pinch.minimumRotation: 0 // unlimited
        pinch.rotationSpeed: 1.0 // default ~follows angle between fingers
        pinch.maximumTilt: 90
        pinch.minimumTilt: 0
        pinch.maximumTiltChange: 35

        // Flicking
        flick.enabled: true
        flick.deceleration: 3000
        flick.onFlickStarted: {console.log   ('flick started signal                      F Start ++++++++++++++++++ ') }
        flick.onFlickEnded: {console.log     ('flick ended signal                        F Stop ------------------ ') }
        flick.onMovementStarted: {console.log('movement started signal                   M Start ++++++++++++++++++ ') }
        flick.onMovementEnded: {console.log  ('movement ended signal                     M Stop ------------------ ') }


        onWheel: {
            console.log('map wheel event, rotation in degrees: ' + delta/8);
            if (delta > 0) map.zoomLevel += 0.25
            else map.zoomLevel -= 0.25
        }

        pinch.onPinchStarted: {
            console.log('Map element pinch started---------+++++++++++++++++++++++++++++++++++++')
            pinchRect1.x = pinch.point1.x; pinchRect1.y = pinch.point1.y;
            pinchRect2.x = pinch.point2.x; pinchRect2.y = pinch.point2.y;
            pinchRect1.visible = true; pinchRect2.visible = true;
            //console.log('Point 1 x: ' + pinch.point1.x + ' Point2 x' + pinch.point2.x)
            //console.log('Center x: ' + pinch.center.x + ' Point1 y: ' + pinch.point1.y)
        }
        pinch.onPinchUpdated: {
            console.log('Map element pinch updated---------+++++++++++++++++++++++++++++++++++++')
            pinchRect1.x = pinch.point1.x; pinchRect1.y = pinch.point1.y;
            pinchRect2.x = pinch.point2.x; pinchRect2.y = pinch.point2.y;
        }
        pinch.onPinchFinished: {
            console.log('Map element pinch finished ---------+++++++++++++++++++++++++++++++++++++')
            pinchRect1.visible = false; pinchRect2.visible = false;
            //map.pinch.minimumZoomLevel =  map.zoomLevel - 2
            //map.pinch.maximumZoomLevel = map.zoomLevel + 2
        }

        center: Coordinate {
            id: mapCenterCoordinate
            latitude: 51.5
            longitude: -0.11
        }

        // <unsupported so far>
        //rotation: 10 // strangely impacts the size of the map element though
        //transform: Scale { origin.x: 25; origin.y: 25; xScale: 3}          // weirdly translates the item
        //transform: Rotation { origin.y: 25; origin.x: 25; angle: 45}       // weirdly translates the item
        //z: 4 // map will always be under everything, will not be supported
        //opacity: 0.4 // doesn't probably make sense
        //clip: true // not implemented, not sure if very useful either
        // </unsupported so far>
    }



    Row {
        id: buttonRow
        anchors.leftMargin: 2
        anchors.topMargin: 2
        anchors.top: map.bottom;
        spacing: 2
        Rectangle { id: rowRect1; width: 80; height: 45; color: 'peru';
            MouseArea { anchors.fill: parent; onClicked: { map.pinch.maximumZoomLevelChange += 0.1}
                Text {text: "Pinch zoom\nsensitivity+"}
            }
        }
        Rectangle { width: rowRect1.width; height: rowRect1.height; color: rowRect1.color;
            MouseArea { anchors.fill: parent; onClicked: {map.pinch.maximumZoomLevelChange -= 0.1}
                Text {text: "Pinch zoom\nsensitivity-"}
            }
        }
        Rectangle { width: rowRect1.width; height: rowRect1.height; color: rowRect1.color;
            MouseArea { anchors.fill: parent; onClicked: {map.pinch.rotationSpeed += 0.1}
                Text {text: "Pinch rotation\nsensitivity+"}
            }
        }
        Rectangle { width: rowRect1.width; height: rowRect1.height; color: rowRect1.color;
            MouseArea { anchors.fill: parent; onClicked: {map.pinch.rotationSpeed -= 0.1}
                Text {text: "Pinch rotation\nsensitivity-"}
            }
        }
        Rectangle { width: rowRect1.width; height: rowRect1.height; color: rowRect1.color;
            MouseArea { anchors.fill: parent; onClicked: {map.pinch.maximumTiltChange += 1}
                Text {text: "Pinch tilt\nsensitivity+"}
            }
        }
        Rectangle { width: rowRect1.width; height: rowRect1.height; color: rowRect1.color;
            MouseArea { anchors.fill: parent; onClicked: {map.pinch.maximumTiltChange -= 1}
                Text {text: "Pinch tilt\nsensitivity-"}
            }
        }
        Rectangle { id: rowRectPinchGen; width: rowRect1.width; height: rowRect1.height; color: 'lightsteelblue';
            Text { text: "Pinch\nzoom:\n"  + ((map.pinch.activeGestures & MapPinch.ZoomGesture) > 0? "Yes":"No")}
            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    console.log('map pinch active gestures' + map.pinch.activeGestures);
                    if (map.pinch.activeGestures & MapPinch.ZoomGesture)
                        map.pinch.activeGestures &= ~MapPinch.ZoomGesture
                    else
                        map.pinch.activeGestures += MapPinch.ZoomGesture
                }
            }
        }
        Rectangle { width: rowRect1.width; height: rowRect1.height; color: rowRectPinchGen.color;
            Text { text: "Pinch\nrotation:\n"  + ((map.pinch.activeGestures & MapPinch.RotationGesture) > 0? "Yes":"No")}
            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    console.log('map pinch active gestures' + map.pinch.activeGestures);
                    if (map.pinch.activeGestures & MapPinch.RotationGesture)
                        map.pinch.activeGestures &= ~MapPinch.RotationGesture
                    else
                        map.pinch.activeGestures += MapPinch.RotationGesture
                }
            }
        }

        Rectangle { width: rowRect1.width; height: rowRect1.height; color: rowRectPinchGen.color;
            Text { text: "Pinch\ntilt:\n"  + ((map.pinch.activeGestures & MapPinch.TiltGesture) > 0? "Yes":"No")}
            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    console.log('map pinch active gestures' + map.pinch.activeGestures);
                    if (map.pinch.activeGestures & MapPinch.TiltGesture)
                        map.pinch.activeGestures &= ~MapPinch.TiltGesture
                    else
                        map.pinch.activeGestures += MapPinch.TiltGesture
                }
            }

        }
        Rectangle { width: rowRect1.width; height: rowRect1.height; color: rowRectPinchGen.color;
            Text { id: generatorEnabledText; text: pinchGenerator.enabled? "Pinch Gen\nEnabled" : "Pinch Gen\nDisabled"; font.bold: true}
            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    if (pinchGenerator.focus == true) {
                        pinchGenerator.focus = false;
                        pinchGenerator.enabled = false;
                        pinchGenerator.z = -1
                        map.focus = true;
                    } else {
                        pinchGenerator.focus = true
                        pinchGenerator.enabled = true;
                        pinchGenerator.z = 10
                        map.focus = false
                    }
                }
            }
        }
        Rectangle { width: rowRect1.width; height: rowRect1.height; color: rowRect1.color;
            Text {text: map.flick.enabled? "Flick\nEnabled":"Flick\nDisabled"; font.bold: true}
            MouseArea { anchors.fill: parent; onClicked: {map.flick.enabled = !map.flick.enabled} }
        }
        Rectangle { width: rowRect1.width; height: rowRect1.height; color: rowRect1.color;
            MouseArea { anchors.fill: parent; onClicked: {map.flick.deceleration += 200}
                Text {text: "Flick\ndeceleration+"}
            }
        }
        Rectangle { width: rowRect1.width; height: rowRect1.height; color: rowRect1.color;
            MouseArea { anchors.fill: parent; onClicked: {map.flick.deceleration -= 200}
                Text {text: "Flick\ndeceleration-"}
            }
        }
    } // Row

    Rectangle {
        id: pinchRect1
        color: 'red'
        visible: false
        z: 10
        width: 5
        height: 5
    }
    Rectangle {
        id: pinchRect2
        color: 'red'
        visible: false
        z: 10
        width: 5
        height: 5
    }

    Repeater {
        id: swipeView1
        model: pinchGenerator.swipe1
        delegate: Component {
            Rectangle {
                Text {id: touchPointText}
                Component.onCompleted: {
                    if (modelData.touchState == 1)  {    // Qt.TouchPointPressed
                        color = "pink"; width = 15; height = 15
                        touchPointText.text = 'From'
                    }
                    else if (modelData.touchState == 2) { // Qt.TouchPointMoved
                        color = 'yellow'; width = 5; height = 5
                    }
                    else if (modelData.touchState == 8) { // Qt.TouchPointReleased
                        color = 'red'; width = 15; height = 15
                        touchPointText.text = 'To'
                    }
                }
                x: modelData.targetX; y: modelData.targetY
            }
        }
    }

    Repeater {
        id: swipeView2
        model: pinchGenerator.swipe2
        delegate: Component {
                Rectangle {
                Text {id: touchPoint2Text}
                Component.onCompleted: {
                    if (modelData.touchState == 1)  {    // Qt.TouchPointPressed
                        color = "green"; width = 15; height = 15
                        touchPoint2Text.text = 'From'
                    }
                    else if (modelData.touchState == 2) { // Qt.TouchPointMoved
                        color = 'yellow'; width = 5; height = 5
                    }
                    else if (modelData.touchState == 8) { // Qt.TouchPointReleased
                        color = 'blue'; width = 15; height = 15
                        touchPoint2Text.text = 'To'
                    }
                }
                x: modelData.targetX; y: modelData.targetY
            }
        }
    }
}
