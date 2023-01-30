// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
import QtQuick
import QtLocation

//TODO: remove/refactor me when items are integrated

MapCircle {

    color: "#46a2da"
    border.color: "#190a33"
    border.width: 2
    smooth: true
    opacity: 0.25

    function setGeometry(markers, index){
        center.latitude = markers[index].coordinate.latitude
        center.longitude = markers[index].coordinate.longitude
        radius= center.distanceTo(markers[index + 1].coordinate)
    }
}
