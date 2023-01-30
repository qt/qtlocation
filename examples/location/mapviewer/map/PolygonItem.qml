// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
import QtQuick
import QtLocation

//TODO: remove me when items are integrated

MapPolygon {

    color: "#46a2da"
    border.color: "#190a33"
    border.width: 2
    smooth: true
    opacity: 0.25

    function setGeometry(markers, index){
        for (var i = index; i<markers.length; i++){
            addCoordinate(markers[i].coordinate)
        }
    }
    MouseArea {
        anchors.fill:parent
        id: mousearea
        drag.target: parent
    }
}
