// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

import QtQuick
import QtLocation
import QtPositioning

Map {
    width: 512
    height: 512

    center: QtPositioning.coordinate(0, 20)

    id: map
    plugin: Plugin {
        name: "osm"
    }
    zoomLevel: 1
    copyrightsVisible: false

    Repeater {
        id: c
        property int count: 4
        model: count*count
        MapCircle
        {
            center
            {
                longitude: -180+360*(index%c.count+0.5)/(c.count)
                latitude: -90+180*(Math.floor(index/c.count)+0.5)/(c.count)
            }
            radius: 4000*1000
            color: 'green'
            border.width: 1
            autoFadeIn: false
            opacity: 0.3
        }
    }
}
