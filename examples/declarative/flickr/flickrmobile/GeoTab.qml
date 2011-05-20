/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
import QtMobility.location 1.1


Rectangle {
    id: container
    property int maxX: parent.width; property int maxY: parent.height
    property double latitude
    property double longitude
    latitude: positionSource.position.coordinate.latitude
    longitude: positionSource.position.coordinate.longitude

    width: 300; height: 130
    color: "blue"
    opacity: 0.7
    border.color: "black"
    border.width: 1
    radius: 5
    gradient: Gradient {
        GradientStop {position: 0.0; color: "grey"}
        GradientStop {position: 1.0; color: "black"}
    }
    MouseArea {
        anchors.fill: parent
        drag.target: parent
        drag.axis: Drag.XandYAxis
        drag.minimumX: -(parent.width * (2/3)); drag.maximumX: parent.maxX - (parent.width/3)
        drag.minimumY: -(parent.height/2); drag.maximumY: parent.maxY - (parent.height/2)
    }
    Button {
        id: locateButton
        text: "Locate & update"
        anchors {left: parent.left; leftMargin: 5}
        y: 3; height: 32; width: parent.width - 10
        onClicked: {
            if (positionSource.positioningMethod == PositionSource.NoPositioningMethod) {
                positionSource.nmeaSource = "nmealog.txt";
                sourceText.text = "(filesource): " + printableMethod(positionSource.positioningMethod);
            }
            positionSource.update();
        }
    }
    PositionSource {
        id: positionSource
        //nmeaSource: "nmealog.txt"
        onPositionChanged: {planet.source = "images/sun.png";}
    }
    function printableMethod(method) {
        if (method == PositionSource.SatellitePositioningMethod)
            return "Satellite";
        else if (method == PositionSource.NoPositioningMethod)
            return "Not available"
        else if (method == PositionSource.NonSatellitePositioningMethod)
            return "Non-satellite"
        else if (method == PositionSource.AllPositioningMethods)
            return "Multiple"
        return "source error";
    }

    Grid {
        id: locationGrid
        columns: 2
        anchors {left: parent.left; leftMargin: 5; top: locateButton.bottom; topMargin: 5}
        spacing: 5
        Text {color: "white"; font.bold: true
            text: "Lat:"; style: Text.Raised; styleColor: "black"
        }
        Text {id: latitudeValue; color: "white"; font.bold: true
            text: positionSource.position.coordinate.latitude; style: Text.Raised; styleColor: "black";
        }
        Text {color: "white"; font.bold: true
            text: "Lon:"; style: Text.Raised; styleColor: "black"
        }
        Text {id: longitudeValue; color: "white"; font.bold: true
            text: positionSource.position.coordinate.longitude; style: Text.Raised; styleColor: "black"
        }
    }
    Image {
        id: planet
        anchors {top: locationGrid.bottom; left: parent.left; leftMargin: locationGrid.anchors.leftMargin}
        source: "images/moon.png"
        width: 30; height: 30
    }
    Text {id: sourceText; color: "white"; font.bold: true;
        anchors {left: planet.right; leftMargin: 5; verticalCenter: planet.verticalCenter}
        text: "Source: " + printableMethod(positionSource.positioningMethod); style: Text.Raised; styleColor: "black";
    }
}
