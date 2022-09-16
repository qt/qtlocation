/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick
import QtQuick.Controls
import QtLocation

MenuBar {
    id: menuBar
    property variant providerMenu: providerMenu
    property variant mapTypeMenu: mapTypeMenu
    property variant toolsMenu: toolsMenu
    property variant plugin
    property alias isFollowMe: toolsMenu.isFollowMe
    property alias isMiniMap: toolsMenu.isMiniMap

    signal selectProvider(string providerName)
    signal selectMapType(variant mapType)
    signal selectTool(string tool);
    signal toggleMapState(string state)

    function clearMenu(menu)
    {
        while (menu.count)
            menu.removeItem(menu.itemAt(0))
    }

    Menu {
        id: providerMenu
        title: qsTr("Provider")

        function createMenu(plugins)
        {
            clearMenu(providerMenu)
            for (var i = 0; i < plugins.length; i++) {
                createProviderMenuItem(plugins[i]);
            }
        }

        function createProviderMenuItem(provider)
        {
            var action = Qt.createQmlObject('import QtQuick.Controls; Action{ text: "' + provider + '"; checkable: true; onTriggered: function(){selectProvider("' + provider + '")} }', providerMenu)
            addAction(action)
        }
    }

    Menu {
        id: mapTypeMenu
        title: qsTr("MapType")

        Component {
            id: mapTypeMenuActionComponent
            Action {

            }
        }
        function createMenu(map)
        {
            clearMenu(mapTypeMenu)
            for (var i = 0; i<map.supportedMapTypes.length; i++) {
                createMapTypeMenuItem(map.supportedMapTypes[i], map.activeMapType === map.supportedMapTypes[i]);
            }
        }

        function createMapTypeMenuItem(mapType, checked)
        {
            var action = mapTypeMenuActionComponent.createObject(mapTypeMenu, { text: mapType.name, checkable: true, checked: checked })
            action.triggered.connect(function(){selectMapType(mapType)})
            addAction(action)
        }
    }

    Menu {
        id: toolsMenu
        property bool isFollowMe: false;
        property bool isMiniMap: false;
        property variant plugin: menuBar.plugin

        title: qsTr("Tools")

        Action {
            text: qsTr("Reverse geocode")
            enabled: plugin ? plugin.supportsGeocoding(Plugin.ReverseGeocodingFeature) : false
            onTriggered: selectTool("RevGeocode")
        }
        MenuItem {
            text: qsTr("Geocode")
            enabled: plugin ? plugin.supportsGeocoding() : false
            onTriggered: selectTool("Geocode")
        }
        MenuItem {
            text: qsTr("Route with coordinates")
            enabled: plugin ? plugin.supportsRouting() : false
            onTriggered: selectTool("CoordinateRoute")
        }
        MenuItem {
            text: qsTr("Route with address")
            enabled: plugin ? plugin.supportsRouting() : false
            onTriggered: selectTool("AddressRoute")
        }
        MenuItem {
            text: isMiniMap ? qsTr("Hide minimap") : qsTr("Minimap")
            onTriggered: toggleMapState("MiniMap")
        }
        MenuItem {
            text: isFollowMe ? qsTr("Stop following") : qsTr("Follow me")
            onTriggered: toggleMapState("FollowMe")
        }
        MenuItem {
            text: qsTr("Language")
            onTriggered: selectTool("Language")
        }
        MenuItem {
            text: qsTr("Prefetch Map Data")
            onTriggered: selectTool("Prefetch")
        }
        MenuItem {
            text: qsTr("Clear Map Data")
            onTriggered: selectTool("Clear")
        }
    }
}
