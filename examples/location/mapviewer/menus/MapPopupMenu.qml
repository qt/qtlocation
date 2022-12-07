// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Controls

Menu {
    property variant coordinate
    property int markersCount
    property int mapItemsCount
    signal itemClicked(string item)

    MenuItem {
        text: qsTr("Add Marker")
        onTriggered: itemClicked("addMarker")
    }
    MenuItem {
        text: qsTr("Get coordinate")
        onTriggered: itemClicked("getCoordinate")
    }
    MenuItem {
        text: qsTr("Fit Viewport To Map Items")
        onTriggered: itemClicked("fitViewport")
    }
    MenuItem {
        text: qsTr("Delete all markers")
        enabled: markersCount > 0
        onTriggered: itemClicked("deleteMarkers")
    }
    MenuItem {
        text: qsTr("Delete all items")
        enabled: mapItemsCount > 0
        onTriggered: itemClicked("deleteItems")
    }

    Menu {
        title: qsTr("Draw...")

        MenuItem {
            text: qsTr("Image")
            onTriggered: itemClicked("drawImage")
        }
        MenuItem {
            text: qsTr("Rectangle")
            onTriggered: itemClicked("drawRectangle")
        }
        MenuItem {
            text: qsTr("Circle")
            onTriggered: itemClicked("drawCircle")
        }
        MenuItem {
            text: qsTr("Polyline")
            onTriggered: itemClicked("drawPolyline")
        }
        MenuItem {
            text: qsTr("Polygon")
            onTriggered: itemClicked("drawPolygonMenu")
        }
    }
}
