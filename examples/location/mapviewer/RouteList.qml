/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
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
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.4
import QtQuick.Controls 1.3

ListView {
    property variant routeModel
    property string totalTravelTime
    property string totalDistance

    signal closeForm()

    id: listView
    interactive: true

    model: ListModel { id: routeList }

    header: RouteListHeader {}

    delegate:  RouteListDelegate{
        routeIndex.text: index + 1
        routeInstruction.text: instruction
        routeDistance.text: distance
    }

    footer: Button {
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("Close")
        onClicked: {
            closeForm()
        }
    }

    Component.onCompleted: {
        routeList.clear()
        if (routeModel.count > 0) {
            for (var i = 0; i < routeModel.get(0).segments.length; i++) {
                routeList.append({
                                     "instruction": routeModel.get(0).segments[i].maneuver.instructionText,
                                     "distance": formatDistance(routeModel.get(0).segments[i].maneuver.distanceToNextInstruction)
                                 });
            }
        }
        totalTravelTime = routeModel.count == 0 ? "" : formatTime(routeModel.get(0).travelTime)
        totalDistance = routeModel.count == 0 ? "" : formatDistance(routeModel.get(0).distance)
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
}
