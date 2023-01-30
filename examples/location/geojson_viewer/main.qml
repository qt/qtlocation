// Copyright (C) 2019 Julian Sherollari <jdotsh@gmail.com>
// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls
import QtPositioning
import QtLocation
import QtCore
import Qt.GeoJson

ApplicationWindow {
    id: win
    visible: true
    width: 1024
    height: 1024
    menuBar: mainMenu
    title: qsTr("GeoJSON Viewer: ") + view.map.center +
           " zoom " + view.map.zoomLevel.toFixed(3)
           + " min " + view.map.minimumZoomLevel +
           " max " + view.map.maximumZoomLevel

    FileDialog {
        visible: false
        id: fileDialog
        title: "Choose a GeoJSON file"
        fileMode: FileDialog.OpenFile
        currentFolder: dataPath
        nameFilters: ["GeoJSON files (*.geojson *.json)"]
        onAccepted: {
            geoJsoner.load(fileDialog.selectedFile)
        }
    }

    FileDialog {
        visible: false
        id: fileWriteDialog
        title: "Write your geometry to a file"
        fileMode: FileDialog.SaveFile
        currentFolder: StandardPaths.writableLocation(StandardPaths.TempLocation)
        nameFilters: ["GeoJSON files (*.geojson *.json)"]
        onAccepted: {
            geoJsoner.dumpGeoJSON(geoJsoner.toGeoJson(miv), fileWriteDialog.selectedFile);
        }
    }

    FileDialog {
        visible: false
        id: debugWriteDialog
        title: "Write Qvariant debug view"
        fileMode: FileDialog.SaveFile
        currentFolder: StandardPaths.writableLocation(StandardPaths.TempLocation)
        nameFilters: ["GeoJSON files (*.geojson *.json)"]
        onAccepted: {
            geoJsoner.writeDebug(geoJsoner.toGeoJson(miv), debugWriteDialog.selectedFile);
        }
    }

    MenuBar {
        id: mainMenu

        Menu {
            title: "&File"
            id : geoJsonMenu
            MenuItem {
                text: "&Open"
                onTriggered: {
                    fileDialog.open()
                }
            }
            MenuItem {
                text: "&Export"
                onTriggered: {
                    fileWriteDialog.open()
                }
            }
            MenuItem {
                text: "E&xit"
                onTriggered: Qt.quit()
            }
        }
        Menu {
            title: "&Debug"
            id : debugMenu
            MenuItem {
                text: "Print debug data to &file"
                onTriggered: {
                    debugWriteDialog.open()
                }
            }
            MenuItem {
                text: "&Print debug data"
                onTriggered: {
                    geoJsoner.print(miv)
                }
            }
        }
    }

    GeoJsoner {
        id: geoJsoner
    }

    Shortcut {
        enabled: view.map.zoomLevel < view.map.maximumZoomLevel
        sequence: StandardKey.ZoomIn
        onActivated: view.map.zoomLevel = Math.round(view.map.zoomLevel + 1)
    }
    Shortcut {
        enabled: view.map.zoomLevel > view.map.minimumZoomLevel
        sequence: StandardKey.ZoomOut
        onActivated: view.map.zoomLevel = Math.round(view.map.zoomLevel - 1)
    }

    MapView {
        id: view
        anchors.fill: parent
        map.center: QtPositioning.coordinate(43.59, 13.50) // Ancona, Italy
        map.plugin: Plugin { name: "osm" }
        map.zoomLevel: 4

        MapItemView {
            id: miv
            parent: view.map
            model: geoJsoner.model
            delegate: GeoJsonDelegate {
            }
        }
    }
}
