// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
import QtQuick
import QtLocation

MapQuickItem {  //to be used inside MapComponent only
    id: imageItem

    anchorPoint.x: testImage.width/2
    anchorPoint.y: testImage.height/2

    function setGeometry(anchorCoordinate) {
        coordinate = anchorCoordinate
    }

    function addGeometry(newCoordinate, changeLast){
        var p1 = view.map.fromCoordinate(coordinate, false);
        var p2 = view.map.fromCoordinate(newCoordinate, false);
        var size = Math.max(Math.abs((p1.x-p2.x)), Math.abs((p1.y-p2.y)));
        testImage.scale = Math.max(0.1, size/Math.max(testImage.height, testImage.width))*2
        return true
    }

    function finishAddGeometry(){
    }

    sourceItem: Image {
        id: testImage
        source: "../resources/icon.png"
        opacity: 0.7
    }
}
