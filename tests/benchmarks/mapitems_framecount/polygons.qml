/****************************************************************************
**
** Copyright (C) 2023 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.15
import QtLocation 5.15
import QtPositioning 5.15

Map {
    width: 1024
    height: 1024

    property double lonPos: 30
    center: QtPositioning.coordinate(0, lonPos)

    NumberAnimation on lonPos
    {
        loops: Animation.Infinite
        from: -180
        to: 180
        duration: 30000
    }

    id: map
    plugin: Plugin {
        name: "osm"
    }
    zoomLevel: 1
    copyrightsVisible: false

    Repeater {
        id: polylines
        property var colors: [
            "red",
            "orange",
            "yellow",
            "green",
            "blue",
            "violet"
        ]
        property int count: polylines.colors.length*10
        model: count*count
        MapPolygon {
            color: polylines.colors[Math.floor(index%polylines.colors.length)]
            autoFadeIn: false
            property double center_longitude: -180+360*(index%polylines.count+0.5)/(polylines.count)
            property double center_latitude: -90+180*(Math.floor(index/polylines.count)+0.5)/(polylines.count)
            property double a: 1
            geoShape: QtPositioning.polygon(
                [
                    QtPositioning.coordinate(a*center_latitude + 1.11, a*center_longitude + -3.13),
                    QtPositioning.coordinate(a*center_latitude + 0.42, a*center_longitude + 10.54),
                    QtPositioning.coordinate(a*center_latitude + 8.36, a*center_longitude + 6.70)
                ],
                [
                    [
                       QtPositioning.coordinate(a*center_latitude + 4.36, a*center_longitude + 3.46),
                       QtPositioning.coordinate(a*center_latitude + 1.91, a*center_longitude + 0.52),
                       QtPositioning.coordinate(a*center_latitude + 1.50, a*center_longitude + 8.25),
                       QtPositioning.coordinate(a*center_latitude + 4.36, a*center_longitude + 6.80)
                   ]
                ]
            )
            opacity: 0.5
            NumberAnimation on a
            {
                loops: Animation.Infinite
                from: 0
                to: 1
                duration: 10000
            }
        }
    }

    Keys.onPressed: (event)=> {
        Qt.quit()
    }
}

