// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
import QtQuick
import QtLocation

MapQuickItem {  //to be used inside MapComponent only
    id: imageItem

    MouseArea {
        anchors.fill: parent
        drag.target: parent
    }

    function setGeometry(markers, index) {
        coordinate.latitude = markers[index].coordinate.latitude
        coordinate.longitude = markers[index].coordinate.longitude
    }

    sourceItem: Image {
        id: testImage
        source: "../resources/icon.png"
        opacity: 0.7
    }
}
