// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
import QtQuick
import QtLocation

MapRectangle {
    id: mapRectangle

    color: "#da5546"
    border.color: "#330a0a"
    border.width: 2
    smooth: true
    opacity: 0.75

    function setGeometry(anchorCoordinate) {
        topLeft = anchorCoordinate
    }

    function addGeometry(newCoordinate, changeLast){
        bottomRight = newCoordinate
        return true //finished
    }

    function finishAddGeometry(){
        color = "#46a2da"
        border.color = "#190a33"
        opacity = 0.25
    }
}
