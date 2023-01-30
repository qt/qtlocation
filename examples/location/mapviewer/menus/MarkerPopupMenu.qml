// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Controls

Menu {
    property int currentMarker
    property int markersCount
    signal itemClicked(string item)

    MenuItem {
        text: qsTr("Delete")
        onTriggered: itemClicked("deleteMarker")
    }
    MenuItem {
        text: qsTr("Coordinates")
        onTriggered: itemClicked("getMarkerCoordinate")
    }
    MenuItem {
        text: qsTr("Move to")
        onTriggered: itemClicked("moveMarkerTo")
    }
    MenuItem {
        text: currentMarker < markersCount-2 ? qsTr("Route to next points")
                                             : qsTr("Route to next point")
        enabled: currentMarker <= markersCount - 2
        onTriggered: currentMarker < markersCount-2 ? itemClicked("routeToNextPoints")
                                                    : itemClicked("routeToNextPoint")
    }
    MenuItem {
        text: currentMarker < markersCount-2 ? qsTr("Distance to next points")
                                             : qsTr("Distance to next point")
        enabled: currentMarker <= markersCount - 2
        onTriggered: currentMarker < markersCount-2 ? itemClicked("distanceToNextPoints")
                                                    : itemClicked("distanceToNextPoint")
    }
    Menu {
        title: qsTr("Draw...")

        MenuItem {
            text: qsTr("Image")
            onTriggered: itemClicked("drawImage")
        }
        MenuItem {
            text: qsTr("Rectangle")
            enabled: currentMarker <= markersCount - 2
            onTriggered: itemClicked("drawRectangle")
        }
        MenuItem {
            text: qsTr("Circle")
            enabled: currentMarker <= markersCount - 2
            onTriggered: itemClicked("drawCircle")
        }
        MenuItem {
            text: qsTr("Polyline")
            enabled: currentMarker <= markersCount - 2
            onTriggered: itemClicked("drawPolyline")
        }
        MenuItem {
            text: qsTr("Polygon")
            enabled: currentMarker < markersCount-2
            onTriggered: itemClicked("drawPolygonMenu")
        }
    }
}
