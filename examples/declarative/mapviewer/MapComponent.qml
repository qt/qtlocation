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

import QtQuick 1.1
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

    property list<Marker> markers
    property int counter: 0 // counter for total amount of markers. Resets to 0 when number of markers = 0
    property Marker currentMarker

    property RouteQuery routeQuery: RouteQuery {}
    property RouteModel routeModel: RouteModel {
                                        plugin : map.plugin
                                        query: routeQuery
                                        onStatusChanged:{
                                            if (status == RouteModel.Ready){
                                                if (count == 1) { routeInfoModel.update()}
                                            }
                                            else if (status == RouteModel.Error){
                                                clearAll()
                                                map.routeError()
                                            }
                                        }
                                        function clearAll(){
                                            clear()
                                            routeInfoModel.update()
                                        }
                                    }
    property GeocodeModel geocodeModel: GeocodeModel {
                                            plugin : map.plugin;
                                            onStatusChanged:{
                                                if ((status == GeocodeModel.Ready) || (status == GeocodeModel.Error)) map.geocodeFinished()
                                            }
                                            onPlacesChanged:
                                            {
                                                if (count == 1) map.center = get(0).coordinate
                                            }
                                        }
    property int lastX : -1
    property int lastY : -1

    signal mousePressed() // replace with
                          // signal mousePressed(MouseEvent mouse)
                          // when QTBUG-14550 is fixed
    signal sliderUpdated()
    signal coordinatesCaptured(double latitude, double longitude)
    signal geocodeFinished()
    signal showGeocodeInfo()
    signal moveMarker()
    signal routeError()

    Component.onCompleted: {
        markers = []
    }

    Component {
        id: routeDelegate
        MapGroup {
            MapRoute {
                route: path
                color: routeMouseArea.containsMouse ? "lime" :"red"
                MapMouseArea {
                    id: routeMouseArea

                    hoverEnabled: true
                    onPressed : {
                        routeTimer.start();
                        map.state = ""
                        map.lastX = mouse.x
                        map.lastY = mouse.y
                    }
                    onReleased : {
                        if (routeTimer.running){ //SHORT PRESS
                            routeTimer.stop()
                            map.lastX = -1
                            map.lastY = -1
                        }
                    }
                    onPositionChanged: {
                        if (routeTimer.running) routeTimer.stop()
                        if (map.state == "") {
                            map.lastX = mouse.x
                            map.lastY = mouse.y
                        }
                    }
                    Timer {
                        id: routeTimer
                        interval: longPressDuration; running: false; repeat: false
                        onTriggered: { //LONG PRESS
                        map.state = "RoutePopupMenu"
                        }
                    }
                }
            }
            MapImage {
                source: routeMouseArea.containsMouse ? "resources/node_selected.png" : "resources/node.png"
                coordinate:path.path[0]
                offset.x: -5
                offset.y: -5
            }
            MapImage {
                source: routeMouseArea.containsMouse ? "resources/node_selected.png" : "resources/node.png"
                coordinate:path.path[path.path.length-1]
                offset.x: -5
                offset.y: -5
            }
        }
    }

    Component {
        id: pointDelegate
        MapCircle {
            radius: 1000
            color: circleMouseArea.containsMouse ? "#8000FF00" : "#80FF0000"
            center: place.coordinate
            MapMouseArea {
                id: circleMouseArea
                hoverEnabled: true
                onPressed : {
                    circleTimer.start()
                    map.state = ""
                    map.lastX = mouse.x
                    map.lastY = mouse.y
                }
                onReleased : {
                    if (circleTimer.running) {//SHORT PRESS
                        circleTimer.stop();
                        map.lastX = -1
                        map.lastY = -1
                    }
                }
                onPositionChanged: {
                    if (circleTimer.running) circleTimer.stop()
                    if ((mouse.button == Qt.LeftButton) && (map.state == "")) radius = center.distanceTo(mouse.coordinate)
                    if (map.state == "") {
                        map.lastX = mouse.x
                        map.lastY = mouse.y
                    }
                }
                Timer {
                    id: circleTimer
                    interval: longPressDuration; running: false; repeat: false
                    onTriggered: { //LONG PRESS
                        map.state = "PointPopupMenu"
                    }
                }
            }
        }
    }

    Component {
        id: routeInfoDelegate
        Row {
            spacing: 10
            Text {
                id: indexText
                text: index + 1
                color: "white"
                font.bold: true
                font.pixelSize: 12
            }
            Text {
                text: instruction
                color: "white"
                height: indexText.height*3
                wrapMode: Text.Wrap
                width: textArea.width - indexText.width - distanceText.width - spacing*4
            }
            Text {
                id: distanceText
                text: distance
                color: "white"
                font.bold: true
                font.pixelSize: 12
            }
        }
    }

    Component{
        id: routeInfoHeader
        Item {
            width: textArea.width
            height: travelTime.height + line.anchors.topMargin + line.height
            Text {
                id: travelTime
                text: routeInfoModel.travelTime
                color: "white"
                font.bold: true
                font.pixelSize: 14
                anchors.left: parent.left
            }
            Text {
                id: distance
                text: routeInfoModel.distance
                color: "white"
                font.bold: true
                font.pixelSize: 14
                anchors.right: parent.right
            }
            Rectangle {
                id: line
                color: "white"
                width: parent.width
                height: 2
                anchors.left: parent.left
                anchors.topMargin: 1
                anchors.top: distance.bottom
            }
        }
    }


    ListModel{
        id: routeInfoModel
        property string travelTime
        property string distance

        function update() {
            clear()
            if (routeModel.count > 0){
                for (var i=0; i< routeModel.get(0).segments.length; i++){
                    append({"instruction": routeModel.get(0).segments[i].maneuver.instructionText, "distance": formatDistance(routeModel.get(0).segments[i].maneuver.distanceToNextInstruction)})
                }
            }
            travelTime = routeModel.count == 0 ? "" : formatTime(routeModel.get(0).travelTime)
            distance = routeModel.count == 0 ? "" : formatDistance(routeModel.get(0).distance)
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
        latitude : -27.53
        longitude : 153.088
    }

    Item {
        id: infoTab
        parent: scale.parent
        z: map.z
        height: parent.height - 180
        width: parent.width
        x: -5 - infoRect.width
        y: 60
        visible: (routeInfoModel.count > 0)
        Image {
            id: catchImage
            source: "resources/catch.png"
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (infoTab.x == -5) infoTab.x -= infoRect.width
                    else infoTab.x = -5
                    map.state = ""
                }
            }
        }

        Behavior on x {
            PropertyAnimation { properties: "x"; duration: 300; easing.type: Easing.InOutQuad }
        }

        Rectangle {
            id: infoRect
            width: parent.width - catchImage.sourceSize.width
            height: parent.height
            color: "grey"
            opacity: 0.75
            radius: 5
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
            }
            Item {
                id: textArea
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.top: parent.top
                anchors.topMargin: 10
                width: parent.width -15
                height: parent.height - 20
                ListView {
                    id: routeInfoView
                    model: routeInfoModel
                    delegate: routeInfoDelegate
                    header: routeInfoHeader
                    anchors.fill: parent
                    clip: true
                }
            }
        }
    }

    Common.Slider {
        id: zoomSlider;
        parent: map.parent //todo: remove the line
        minimum: map.minimumZoomLevel;
        maximum: map.maximumZoomLevel;
        opacity: 1
        z: map.z
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

    Item {//scale
        id: scale
        parent: zoomSlider.parent
        z: map.z
        opacity: 0.6
        visible: map.zoomLevel == 0 ? false : true
        anchors {
            bottom: zoomSlider.top;
            bottomMargin: 8;
            leftMargin: 20
            left: zoomSlider.left
        }
        Image {
            id: scaleImage
            source: "resources/scale.png"
            anchors.bottom: parent.bottom
            anchors.left: parent.left
        }
        Text {
            id: scaleText
            color: "black"
            horizontalAlignment: Text.AlignHCenter
            width: scaleImage.sourceSize.width
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.bottomMargin: 3
            Component.onCompleted: {text = calculateScale()}
        }
    }

    onCenterChanged:{
        scaleText.text = calculateScale()
    }

    Common.Menu {
        id: markerMenu
        orientation: ListView.Vertical
        z: map.z + 2
        opacity: 0

        itemHeight: 30;
        itemWidth: 150
        x: 0
        y: 0
        onClicked: {
            map.state = ""
            switch (button) {
                case 0: {//remove marker
                    map.removeMarker(currentMarker)
                    break;
                }
                case 1: {//move marker
                    map.moveMarker()
                    break;
                }
                case 2: {//show marker's coordinates
                    map.coordinatesCaptured(currentMarker.coordinate.latitude, currentMarker.coordinate.longitude)
                    break;
                }
                case 3: {//calculate route
                    map.calculateRoute(currentMarker)
                    break;
                }
            }
        }
    }

    Common.Menu {
        id: popupMenu
        orientation: ListView.Vertical
        z: map.z +2
        opacity: 0

        itemHeight: 30;
        itemWidth: 150
        x: 0
        y: 0

        onClicked: {
            switch (button) {
                case 0: { //add Marker
                    addMarker()
                    break;
                }
                case 1: {//show coordinates
                    map.coordinatesCaptured(mouseArea.lastCoordinate.latitude, mouseArea.lastCoordinate.longitude)
                    break;
                }
                case 2: { //delete all Markers
                    map.deleteAllMarkers()
                    break;
                }
            }
            map.state = ""
        }
    }

    Common.Menu {
        id: routeMenu
        orientation: ListView.Vertical
        z: map.z +2
        opacity: 0

        itemHeight: 30;
        itemWidth: 150
        x: 0
        y: 0

        onClicked: {
            switch (button) {
                case 0: {//delete route
                    routeModel.clear()
                    routeInfoModel.update()
                    break;
                }
            }
            map.state = ""
        }
        Component.onCompleted: {
            setModel(["Delete"])
        }
    }

    Common.Menu {
        id: pointMenu
        orientation: ListView.Vertical
        z: map.z +2
        opacity: 0

        itemHeight: 30;
        itemWidth: 150
        x: 0
        y: 0

        onClicked: {
            switch (button) {
                case 0: {//point info
                    map.showGeocodeInfo()
                    break;
                }
                case 1: {//delete point
                    geocodeModel.clear()
                    break;
                }
            }
            map.state = ""
        }
        Component.onCompleted: {
            setModel(["Info", "Delete"])
        }
    }

    onZoomLevelChanged:{
        zoomSlider.value = map.zoomLevel
        scaleText.text = map.calculateScale()
    }

    MapMouseArea {
        id: mouseArea
        property Coordinate lastCoordinate: Coordinate { latitude : 0; longitude : 0}

        onPressed : {
            mapTimer.start()
            map.state = ""
            map.lastX = mouse.x
            map.lastY = mouse.y
            lastCoordinate = mouse.coordinate
            map.mousePressed()
        }
        onReleased : {
            if (mapTimer.running) {//SHORT PRESS
                mapTimer.stop()
                map.lastX = -1
                map.lastY = -1
            }
        }
        onPositionChanged: {
            if (mapTimer.running) mapTimer.stop()
            if ((mouse.button == Qt.LeftButton) & (map.state == "")) {
                if ((map.lastX != -1) && (map.lastY != -1)) {
                    var dx = mouse.x - map.lastX
                    var dy = mouse.y - map.lastY
                    map.pan(-dx, -dy)
                }
                map.lastX = mouse.x
                map.lastY = mouse.y
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
                if (map.markers.length != 0)  popupMenu.setModel(["Set Marker","Coordinates","Delete all markers"])
                else popupMenu.setModel(["Set Marker","Coordinates"])

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

    function calculateScale(){
        var coord1, coord2, dist, text
        if (map.zoomLevel == 0) { text=""}
        else{
            coord1 = map.toCoordinate(Qt.point(0,scale.y))
            coord2 = map.toCoordinate(Qt.point(0+scaleImage.sourceSize.width,scale.y))
            dist = Math.round(coord1.distanceTo(coord2))
            text = formatDistance(dist)
        }
        return text
    }

    function deleteAllMarkers(){
        var count = map.markers.length
        for (var i = 0; i<count; i++){
            map.removeMapObject(map.markers[i])
            map.markers[i].destroy()
        }
        map.markers = []
        counter = 0
    }

    function addMarker(){

        var marker, myArray
        var count = map.markers.length
        counter++
        marker = Qt.createQmlObject ('Marker {}', map)
        map.addMapObject(marker)

        //update list of markers
        myArray = new Array()
        for (var i = 0; i<count; i++){
            myArray.push(markers[i])
        }
        myArray.push(marker)
        markers = myArray
    }

    function removeMarker(marker){
        //update list of markers
        var myArray = new Array()
        var count = map.markers.length
        for (var i = 0; i<count; i++){
            if (marker != map.markers[i]) myArray.push(map.markers[i])
        }

        map.removeMapObject(marker)
        marker.destroy()
        map.markers = myArray
        if (markers.length == 0) counter = 0
    }

    function markerLongPress(){
        var array
        var count = map.markers.length

        if (currentMarker == markers[count-1]) array = ["Delete", "Move to", "Coordinates"]
        else if (count > 2){
            if (currentMarker == markers[count-2]) array = ["Delete", "Move to", "Coordinates", "Route to next point"]
            else array = ["Delete", "Move to", "Coordinates", "Route to next points"]
        }
        else array = ["Delete", "Move to", "Coordinates", "Route to next point"]

        markerMenu.setModel(array)
        map.state = "MarkerPopupMenu"
    }

    function calculateRoute(marker){
        routeQuery.clearWaypoints();
        var startPointFound = false
        var count = map.markers.length
        for (var i = 0; i< count; i++){
            if (startPointFound != true){
                if (markers[i] == marker){
                    startPointFound = true
                    routeQuery.addWaypoint(marker.coordinate)
                }
            }
            else routeQuery.addWaypoint(markers[i].coordinate)
        }
        routeQuery.travelModes = RouteQuery.CarTravel
        routeQuery.routeOptimizations = RouteQuery.ShortestRoute
        for (i=0; i<9; i++) {
            routeQuery.setFeatureWeight(i, 0)
        }
        routeModel.update();
    }

    function roundNumber(number, digits) {
        var multiple = Math.pow(10, digits);
        return Math.round(number * multiple) / multiple;
    }

    function formatTime(sec){
        var value = sec
        var seconds = value % 60
        value /= 60
        value = (value > 1) ? Math.round(value) : 0
        var minutes = value % 60
        value /= 60
        value = (value > 1) ? Math.round(value) : 0
        var hours = value
        if (hours > 0) value = hours + "h:"+ minutes + "m"
        else value = minutes + "min"
        return value
    }

    function formatDistance(meters)
    {
         var dist = Math.round(meters)
         if (dist > 1000 ){
             if (dist > 100000){
                 dist = Math.round(dist / 1000)
             }
             else{
                 dist = Math.round(dist / 100)
                 dist = dist / 10
             }
             dist = dist + " km"
         }
         else{
             dist = dist + " m"
         }
         return dist
    }

    // states of map
    states: [
        State {
            name: "PopupMenu"
            PropertyChanges { target: popupMenu; opacity: 1}
            PropertyChanges { target: popupMenu; x: ((map.lastX + popupMenu.width > map.width) ? map.width - popupMenu.width : map.lastX)}
            PropertyChanges { target: popupMenu; y: ((map.lastY + popupMenu.height > map.height) ? map.height - popupMenu.height : map.lastY)}
        },
        State {
            name: "MarkerPopupMenu"
            PropertyChanges { target: markerMenu; opacity: 1}
            PropertyChanges { target: markerMenu; x: ((currentMarker.lastMouseX + markerMenu.width > map.width) ? map.width - markerMenu.width : currentMarker.lastMouseX )}
            PropertyChanges { target: markerMenu; y: ((currentMarker.lastMouseY + markerMenu.height > map.height) ? map.height - markerMenu.height : currentMarker.lastMouseY)}
        },
        State {
            name: "RoutePopupMenu"
            PropertyChanges { target: routeMenu; opacity: 1}
            PropertyChanges { target: routeMenu; x: ((map.lastX + routeMenu.width > map.width) ? map.width - routeMenu.width : map.lastX)}
            PropertyChanges { target: routeMenu; y: ((map.lastY + routeMenu.height > map.height) ? map.height - routeMenu.height : map.lastY)}
        },
        State {
            name: "PointPopupMenu"
            PropertyChanges { target: pointMenu; opacity: 1}
            PropertyChanges { target: pointMenu; x: ((map.lastX + pointMenu.width > map.width) ? map.width - pointMenu.width : map.lastX)}
            PropertyChanges { target: pointMenu; y: ((map.lastY + pointMenu.height > map.height) ? map.height - pointMenu.height : map.lastY)}
        }
    ]
}
