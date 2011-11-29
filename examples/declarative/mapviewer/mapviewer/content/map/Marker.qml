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

import QtQuick 2.0;
import QtLocation 5.0
import "../components"

MapScreenItem {  //to be used inside MapComponent only
    id: marker
    anchorPoint.x: image.width/4
    anchorPoint.y: image.height

    coordinate: Coordinate { latitude : 0; longitude : 0 }

    sourceItem:  Image {
        id: image
        source: markerMouseArea.containsMouse ? (markerMouseArea.pressed  ? "../resources/marker_selected.png" :"../resources/marker_hovered.png") : "../resources/marker.png"
        MapMouseArea  {
            id: markerMouseArea
            anchors.fill: parent
            hoverEnabled : true
            drag.target: marker
            drag.axis: Drag.XandYAxis
            drag.minimumX: -map.toScreenPosition(marker.coordinate).x
            drag.maximumX: map.width + drag.minimumX
            drag.minimumY: -map.toScreenPosition(marker.coordinate).y
            drag.maximumY: map.height + drag.minimumY

            onReleased:{
                console.log("drag: implement me")
            }
        }

        Text{
            id: number
            y: image.height/4
            color: "white"
            font.bold: true
            font.pixelSize: 14
            Component.onCompleted: {
                text = map.counter
                //very pro way of positioning text
                x = map.counter > 9 ? image.width/10 : image.width/5
            }
        }

    }

    Component.onCompleted: {
        coordinate.longitude = mouseArea.lastCoordinate.longitude
        coordinate.latitude = mouseArea.lastCoordinate.latitude
    }
}
