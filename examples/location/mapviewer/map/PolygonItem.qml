// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
import QtQuick
import QtLocation

MapPolygon {

    color: "#da5546"
    border.color: "#330a0a"
    border.width: 2
    smooth: true
    opacity: 0.75

    function setGeometry(anchorCoordinate){
        addCoordinate(anchorCoordinate)
    }

    function addGeometry(newCoordinate, changeLast){
        if (changeLast && path.length > 0)
            removeCoordinate(path[path.length-1])
        addCoordinate(newCoordinate)
        return false
    }

    function finishAddGeometry(){
        if (path.length > 0)
            removeCoordinate(path[path.length-1])
        color = "#46a2da"
        border.color = "#190a33"
        opacity = 0.25
    }
}
