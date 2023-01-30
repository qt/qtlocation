// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
import QtQuick
import QtLocation

//TODO: remove/refactor me when items are integrated

MapPolyline {

    line.color: "#46a2da"
    line.width: 4
    opacity: 0.25
    smooth: true

    function setGeometry(markers, index){
        for (var i = index; i<markers.length; i++){
            addCoordinate(markers[i].coordinate)
        }
    }
}
