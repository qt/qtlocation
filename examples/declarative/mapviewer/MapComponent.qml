/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
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

import Qt 4.7
import Qt.location 5.0
import "common" as Common

Map {
    id: map
    plugin : defaultPlugin
    zoomLevel: 9
    focus: true
    width: parent.width
    center: defaultCoordinates
    property int longPressDuration: 1000

    property RouteQuery routeQuery: RouteQuery {}
    property RouteModel routeModel: RouteModel {
                                        plugin : map.plugin;
                                        query: routeQuery
                                        onStatusChanged:{
                                            if (status == RouteModel.Ready){
                                                if (count == 1) showRouteInfo()
                                            }
                                        }
                                    }
    property GeocodeModel geocodeModel: GeocodeModel {
                                            plugin : map.plugin;
                                            onStatusChanged:{
                                                if ((status == GeocodeModel.Ready) || (status == GeocodeModel.Error)) map.geocodeFinished()
                                            }
                                        }

    signal mousePressed() // replace with
                          // signal mousePressed(MouseEvent mouse)
                          // when QTBUG-14550 is fixed
    signal sliderUpdated()
    signal coordinatesCaptured(double latitude, double longitude)
    signal showRouteInfo()
    signal geocodeFinished()
    signal showGeocodeInfo()

    Component {
        id: routeDelegate
        MapRoute {
            route: path
            color: routeMouseArea.containsMouse ? "lime" :"red"
            MapMouseArea {
                id: routeMouseArea
                hoverEnabled: true
                onPressed : {routeTimer.start()}
                onReleased : { if (routeTimer.running) routeTimer.stop() }//SHORT PRESS
                onPositionChanged: { if (routeTimer.running) routeTimer.stop()}

                Timer {
                    id: routeTimer
                    interval: longPressDuration; running: false; repeat: false
                    onTriggered: { //LONG PRESS
                        map.showRouteInfo()
                    }
                }
            }
        }
    }

    Component {
        id: pointDelegate
        MapCircle {
            radius: 1000
            color: circleMouseArea.containsMouse ? "lime" : "#80FF0000"
            center: place.coordinate
            MapMouseArea {
                id: circleMouseArea
                hoverEnabled: true
                onPressed : { circleTimer.start()}
                onReleased : { if (circleTimer.running) circleTimer.stop() }//SHORT PRESS
                onPositionChanged: {
                    if (circleTimer.running) circleTimer.stop()
                    if (mouse.button == Qt.LeftButton) radius = center.distanceTo(mouse.coordinate)
                }

                Timer {
                    id: circleTimer
                    interval: longPressDuration; running: false; repeat: false
                    onTriggered: { //LONG PRESS
                        map.geocodeFinished()
                    }
                }
            }
        }
    }

    MapObjectView {
        model: routeModel
        delegate: routeDelegate
    }

    MapObjectView {
        model: geocodeModel
        delegate: pointDelegate
    }

    Plugin {
        id: defaultPlugin
        name : "nokia"
    }

    Coordinate {
        id: defaultCoordinates
        latitude : -27.575
        longitude : 153.088
    }

    Common.Slider {
        id: zoomSlider;
        parent: map.parent //todo: remove the line
        minimum: map.minimumZoomLevel;
        maximum: map.maximumZoomLevel;
        opacity: 1
        z: map.z + 1
        anchors {
            bottom: parent.bottom;
            bottomMargin: 50; rightMargin: 5; leftMargin: 5
            left: parent.left
        }
        width: parent.width - anchors.rightMargin - anchors.leftMargin
        value: map.zoomLevel
        onValueChanged: {
            map.zoomLevel = value
            map.sliderUpdated()
        }
    }

    Common.Menu {
        id: popupMenu
        orientation: ListView.Vertical
        z: map.z + 1
        opacity: 0

        itemHeight: 30;
        itemWidth: 150
        x: 0
        y: 0

        Component.onCompleted: {
            setModel(["Capture"])
        }
        onClicked: {
            map.coordinatesCaptured(mouseArea.lastCoordinate.latitude, mouseArea.lastCoordinate.longitude)
            map.state = ""
        }
    }

    MapMouseArea {
        id: mouseArea
        property int lastX : -1
        property int lastY : -1
        property Coordinate lastCoordinate: Coordinate { latitude : 0; longitude : 0}

        onPressed : {
            mapTimer.start()
            map.state = ""
            lastX = mouse.x
            lastY = mouse.y
            lastCoordinate = mouse.coordinate
            map.mousePressed()
        }
        onReleased : {
            if (mapTimer.running) { //SHORT PRESS
                mapTimer.stop()
                lastX = -1
                lastY = -1
            }
        }
        onPositionChanged: {
            if (mapTimer.running) mapTimer.stop()
            if ((mouse.button == Qt.LeftButton) & (map.state == "")) {
                if ((lastX != -1) && (lastY != -1)) {
                    var dx = mouse.x - lastX
                    var dy = mouse.y - lastY
                    map.pan(-dx, -dy)
                }
                lastX = mouse.x
                lastY = mouse.y
            }
        }
        onDoubleClicked: {
            map.center = mouse.coordinate
            if (mouse.button == Qt.LeftButton){
                map.zoomLevel += 1
            } else if (mouse.button == Qt.RightButton){
                map.zoomLevel -= 1
            }
            lastX = -1
            lastY = -1
        }
        Timer {
            id: mapTimer
            interval: longPressDuration; running: false; repeat: false
            onTriggered: { //LONG PRESS
                map.state = "PopupMenu"
            }
        }
    }

    Keys.onPressed: {
        if ((event.key == Qt.Key_Plus) || (event.key == Qt.Key_VolumeUp)) {
            map.zoomLevel += 1
        } else if ((event.key == Qt.Key_Minus) || (event.key == Qt.Key_VolumeDown)){
            map.zoomLevel -= 1
        }
    }

    // states of map
    states: [
        State {
            name: "PopupMenu"
            PropertyChanges { target: popupMenu; opacity: 1}
            PropertyChanges { target: popupMenu; x: ((mouseArea.lastX + popupMenu.width > map.width) ? map.width - popupMenu.width : mouseArea.lastX)}
            PropertyChanges { target: popupMenu; y: ((mouseArea.lastY + popupMenu.height > map.height) ? map.height - popupMenu.height : mouseArea.lastY)}
        }
    ]
}
