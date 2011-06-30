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

import Qt 4.7;
import Qt.location 5.0
import "common" as Common

    MapImage { //to be used inside MapComponent only
        id: marker
        source: markerMouseArea.pressed ? "resources/marker_selected.png" : "resources/marker.png" //TODO replace with following lane when QTBUG-20096 fixed
//        source: markerMouseArea.containsMouse ? (markerMouseArea.pressed  ? "resources/marker_selected.png" :"resources/marker_hovered.png") : "resources/marker.png"
        coordinate: Coordinate { latitude : 0; longitude : 0 }
        offset.x: -13
        offset.y: -32
        property alias lastMouseX: markerMouseArea.lastX
        property alias lastMouseY: markerMouseArea.lastY
        property alias text: markerIndex.text

        Component.onCompleted: {
            coordinate = mouseArea.lastCoordinate
        }

        MapMouseArea {
            id: markerMouseArea
            property int dX: 0
            property int dY: 0
            property int lastX: -1
            property int lastY: -1
            hoverEnabled: true
            onPressed: {
                marker.z++
                var newX, newY, oldX, oldY
                newX = map.toScreenPosition(mouse.coordinate).x
                newY = map.toScreenPosition(mouse.coordinate).y
                oldX = map.toScreenPosition(marker.coordinate).x
                oldY = map.toScreenPosition(marker.coordinate).y
                dX = oldX - newX
                dY = oldY - newY
                lastX = mouse.x
                lastY = mouse.y

                markerTimer.start()
                map.currentMarker = marker
                map.state = ""
            }
            onReleased: {
                if (markerTimer.running) markerTimer.stop();
                marker.z--
            }

            onPositionChanged: {
                var newX, newY
                if (markerTimer.running) markerTimer.stop();
                if (mouse.button == Qt.LeftButton){
                    lastX = mouse.x
                    lastY = mouse.y
                    newX = map.toScreenPosition(mouse.coordinate).x + dX
                    newY = map.toScreenPosition(mouse.coordinate).y + dY
                    marker.coordinate = map.toCoordinate(Qt.point(newX,newY))
                    textArea.x = map.toScreenPosition(marker.coordinate).x + offset.x
                    textArea.y = map.toScreenPosition(marker.coordinate).y + offset.y
                }
            }

            Timer {
                id: markerTimer
                interval: 1000
                running: false
                repeat: false
                onTriggered: {
                    map.markerLongPress()
                }
            }
        }

        Item {
            id: textArea
            width: 26
            height: 32
            x: map.toScreenPosition(marker.coordinate).x + offset.x
            y: map.toScreenPosition(marker.coordinate).y + offset.y
            Text {
                id: markerIndex
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                z: marker.z
                color: "white"
                font.bold: true
                Component.onCompleted: {
                    text = map.counter
                }
            }
        }

        function update(){
            textArea.x = map.toScreenPosition(marker.coordinate).x + marker.offset.x
            textArea.y = map.toScreenPosition(marker.coordinate).y + marker.offset.y
        }
    }
