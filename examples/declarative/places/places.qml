/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
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
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
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
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import QtLocation 5.0
import QtLocation.examples 5.0
import "content/places"

Item {
    id: page
    width: (parent && parent.width > 0) ? parent.width : 360
    height: (parent && parent.height > 0) ? parent.height : 640
    property bool mobileUi: true
    property variant map
    property variant searchRegion : startLocation
    property variant searchRegionItem

    onMapChanged: editPlaceDialog.prepareDialog()

    Rectangle {
        id: backgroundRect
        anchors.fill: parent
        color: "lightgrey"
        z:2
    }

    //=====================Menu=====================
    Menu {
        id:mainMenu
        anchors.bottom: parent.bottom
        z: backgroundRect.z + 3

        Component.onCompleted: {
            addItem("Provider");
            addItem("New");
            addItem("Search");
        }

        onClicked: page.state = page.state == "" ? button : "";
    }

    Menu {
        id: providerMenu
        z: backgroundRect.z + 2
        y: page.height
        horizontalOrientation: false
        exclusive: true

        Component.onCompleted: {
            var plugins = getPlacesPlugins()
            for (var i = 0; i<plugins.length; i++) {
                addItem(plugins[i]);

                // default to nokia plugin
                if (plugins[i] === "nokia")
                    exclusiveButton = plugins[i];
            }

            // otherwise default to first
            if (exclusiveButton === "")
                exclusiveButton = plugins[0];
        }

        onClicked: page.state = ""

        onExclusiveButtonChanged: placesPlugin.name = exclusiveButton;
    }

    Menu {
        id: newMenu
        z: backgroundRect.z + 2
        y: page.height
        horizontalOrientation: false

        Component.onCompleted: {
            var item = addItem("Place");
            item.enabled = (function() { return placesPlugin.supportedPlacesFeatures & Plugin.SavePlaceFeature });

            item = addItem("Category");
            item.enabled = (function() { return placesPlugin.supportedPlacesFeatures & Plugin.SaveCategoryFeature });
        }

        onClicked: {
            switch (button) {
            case "Place": {
                editPlaceDialog.place = null;
                page.state = "NewPlace";
                break;
            }
            case "Category": {
                editCategoryDialog.category = null;
                editCategoryDialog.prepareDialog();
                page.state = "NewCategory";
                break;
            }
            }
        }
    }

    Menu {
        id: searchMenu
        z: backgroundRect.z + 2
        y: page.height
        horizontalOrientation: false

        Component.onCompleted: {
            addItem("Search Center");
            addItem("Search Bounding Box");
            addItem("Search Bounding Circle");
            addItem("Search Options");
        }

        onClicked: page.state = button
    }

    //=====================Dialogs=====================
    PlaceDialog {
        id: editPlaceDialog
        z: backgroundRect.z + 3

        onCancelButtonClicked: page.state = ""

        Connections {
            target: editPlaceDialog.place
            onStatusChanged: {
                switch (editPlaceDialog.place.status) {
                case Place.Saving: {
                    console.log("Saving...");
                    break;
                }
                case Place.Ready: {
                    page.state = "";
                    break;
                }
                case Place.Error: {
                    console.log("Error while saving!");
                    break;
                }
                }
            }
        }
    }

    CategoryDialog {
        id: editCategoryDialog
        z: backgroundRect.z + 3

        onCancelButtonClicked: page.state = ""

        Connections {
            target: editCategoryDialog.category
            onStatusChanged: {
                switch (editCategoryDialog.category.status) {
                case Category.Saving: {
                    console.log("Saving...");
                    break;
                }
                case Category.Ready: {
                    page.state = "";
                    break;
                }
                case Category.Error: {
                    console.log("Error while saving!");
                    break;
                }
                }
            }
        }
    }

    Dialog {
        id: searchCenterDialog
        z: backgroundRect.z + 3

        title: "Search center"

        Behavior on opacity { NumberAnimation { duration: 500 } }

        Component.onCompleted: prepareDialog()

        function prepareDialog() {
            setModel([
                ["Latitude", searchRegion.center ? String(searchRegion.center.latitude) : ""],
                ["Longitude", searchRegion.center ? String(searchRegion.center.longitude) : ""]
            ]);
        }

        onCancelButtonClicked: page.state = ""
        onGoButtonClicked: {
            startLocation.center.latitude = dialogModel.get(0).inputText;
            startLocation.center.longitude = dialogModel.get(1).inputText;
            searchRegion = startLocation;

            if (searchRegionItem) {
                map.removeMapItem(searchRegionItem);
                searchRegionItem.destroy();
            }

            page.state = "";
        }
    }

    Dialog {
        id: searchBoxDialog
        z: backgroundRect.z + 3

        title: "Search Bounding Box"

        Behavior on opacity { NumberAnimation { duration: 500 } }

        Component.onCompleted: prepareDialog()

        function prepareDialog() {
            setModel([
                ["Latitude", searchRegion.center ? String(searchRegion.center.latitude) : ""],
                ["Longitude", searchRegion.center ? String(searchRegion.center.longitude) : ""],
                ["Width", searchRegion.width ? String(searchRegion.width) : "" ],
                ["Height", searchRegion.height ? String(searchRegion.height) : "" ]
            ]);
        }

        onCancelButtonClicked: page.state = ""
        onGoButtonClicked: {
            var newRegion = Qt.createQmlObject('import QtLocation 5.0; BoundingBox {}', page, "BoundingCircle");
            newRegion.center.latitude = dialogModel.get(0).inputText;
            newRegion.center.longitude = dialogModel.get(1).inputText;
            newRegion.width = dialogModel.get(2).inputText;
            newRegion.height = dialogModel.get(3).inputText;

            startLocation.center.latitude = dialogModel.get(0).inputText;
            startLocation.center.longitude = dialogModel.get(1).inputText;

            searchRegion = newRegion;

            if (searchRegionItem) {
                map.removeMapItem(searchRegionItem);
                searchRegionItem.destroy();
            }

            searchRegionItem = Qt.createQmlObject('import QtLocation 5.0; MapRectangle { color: "red"; opacity: 0.4 }', page, "MapRectangle");
            searchRegionItem.topLeft = newRegion.topLeft;
            searchRegionItem.bottomRight = newRegion.bottomRight;
            map.addMapItem(searchRegionItem);

            page.state = "";
        }
    }

    Dialog {
        id: searchCircleDialog
        z: backgroundRect.z + 3

        title: "Search Bounding Circle"

        Behavior on opacity { NumberAnimation { duration: 500 } }

        Component.onCompleted: prepareDialog()

        function prepareDialog() {
            setModel([
                ["Latitude", searchRegion.center ? String(searchRegion.center.latitude) : ""],
                ["Longitude", searchRegion.center ? String(searchRegion.center.longitude) : ""],
                ["Radius", searchRegion.radius ? String(searchRegion.radius) : "" ]
            ]);
        }

        onCancelButtonClicked: page.state = ""
        onGoButtonClicked: {
            var newRegion = Qt.createQmlObject('import QtLocation 5.0; BoundingCircle {}', page, "BoundingCircle");
            newRegion.center.latitude = dialogModel.get(0).inputText;
            newRegion.center.longitude = dialogModel.get(1).inputText;
            newRegion.radius = dialogModel.get(2).inputText;

            startLocation.center.latitude = dialogModel.get(0).inputText;
            startLocation.center.longitude = dialogModel.get(1).inputText;

            searchRegion = newRegion;

            if (searchRegionItem) {
                map.removeMapItem(searchRegionItem);
                searchRegionItem.destroy();
            }

            searchRegionItem = Qt.createQmlObject('import QtLocation 5.0; MapCircle { color: "red"; opacity: 0.4 }', page, "MapRectangle");
            searchRegionItem.center = newRegion.center;
            searchRegionItem.radius = newRegion.radius;
            map.addMapItem(searchRegionItem);

            page.state = "";
        }
    }

    OptionsDialog {
        id: optionsDialog
        z: backgroundRect.z + 3

        Behavior on opacity { NumberAnimation { duration: 500 } }

        Component.onCompleted: prepareDialog()

        function prepareDialog() {
            if (placeSearchModel.favoritesPlugin !== null)
                isFavoritesEnabled = true;
            else
                isFavoritesEnabled = false;

            locales = placesPlugin.locales.join(Qt.locale().groupSeparator);
        }

        onCancelButtonClicked: page.state = ""
        onGoButtonClicked: {
            if (isFavoritesEnabled) {
                placeSearchModel.favoritesPlugin = jsonDbPlugin;
                recommendationModel.favoritesPlugin = jsonDbPlugin;
            } else {
                placeSearchModel.favoritesPlugin = null;
                recommendationModel.favoritesPlugin = null;
            }

            placesPlugin.locales = locales.split(Qt.locale().groupSeparator);
            categoryModel.update();
            page.state = "";
        }
    }

    BoundingCircle {
        id: startLocation
    }

    Binding {
        target: startLocation
        property: "center"
        value: map ? map.center : null
    }

    //! [PlaceSearchModel model]
    PlaceSearchModel {
        id: placeSearchModel

        plugin: placesPlugin
        maximumCorrections: 5
        searchArea: searchRegion

        function searchForCategory(category) {
            searchTerm = "";
            categories = category;
            execute();
        }

        function searchForText(text) {
            searchTerm = text;
            categories = null;
            execute();
        }
    }
    //! [PlaceSearchModel model]

    //! [PlaceRecommendationModel model]
    PlaceRecommendationModel {
        id: recommendationModel
        plugin: placesPlugin

        searchArea: searchRegion
    }
    //! [PlaceRecommendationModel model]

    //! [CategoryModel model]
    CategoryModel {
        id: categoryModel
        plugin: placesPlugin
        hierarchical: true
    }
    //! [CategoryModel model]

    SearchBox {
        id: searchBox

        anchors.top: page.top
        anchors.topMargin: page.mobileUi ? 20 : 0
        width: parent.width
        z: map ? map.z + 2 : 0
    }

    Plugin {
        id: placesPlugin

        parameters: pluginParametersFromMap(pluginParameters)
        onNameChanged: createMap(placesPlugin);
    }

    Plugin {
        id: jsonDbPlugin
        name: "nokia_places_jsondb"
    }

    Item {
        id: searchResultTab

        z: map ? map.z + 1 : 0
        height: parent.height - 180
        width: parent.width
        x: -5 - searchResultTabPage.width
        y: 60

        Behavior on x { PropertyAnimation { duration: 300; easing.type: Easing.InOutQuad } }

        Image {
            id: catchImage

            source: "resources/catch.png"
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right

            MouseArea {
                anchors.fill: parent
                onClicked: searchResultTab.state = (searchResultTab.state === "") ? "Open" : ""
            }
        }

        Rectangle {
            id: searchResultTabPage

            width: parent.width - catchImage.width
            height: parent.height
            color: "#ECECEC"
            radius: 5

            SearchResultView {
                id: searchResultView

                anchors.fill: parent
                anchors.margins: 10
            }
        }

        states: [
            State {
                name: "Open"
                PropertyChanges { target: searchResultTab; x: 0 }
            }
        ]
    }

    function createMap(placesPlugin) {
        var mapPlugin;
        if (placesPlugin.supported & Plugin.MappingFeature) {
            mapPlugin = placesPlugin;
        } else {
            mapPlugin = Qt.createQmlObject('import QtLocation 5.0; Plugin { required: Plugin.MappingFeature }', page);
        }

        if (map)
            map.destroy();
        map = Qt.createQmlObject('import QtLocation 5.0; import "content/places"; MapComponent { z : backgroundRect.z + 1; width: page.width; height: page.height; onMapPressed: { page.state = "" }}', page);
        map.plugin = mapPlugin;
    }

    function getPlacesPlugins() {
        var plugin = Qt.createQmlObject('import QtLocation 5.0; Plugin {}', page);
        var myArray = new Array;
        for (var i = 0; i < plugin.availableServiceProviders.length; i++) {
            var tempPlugin = Qt.createQmlObject ('import QtLocation 5.0; Plugin {name: "' + plugin.availableServiceProviders[i]+ '"}', page)
            //note this will allocate all the plugin managers and resources
            if (tempPlugin.supported & Plugin.AnyPlacesFeature)
                myArray.push(tempPlugin.name)
        }

        return myArray;
    }

    function pluginParametersFromMap(pluginParameters) {
        var parameters = new Array()
        for (var prop in pluginParameters){
            var parameter = Qt.createQmlObject('import QtLocation 5.0; PluginParameter{ name: "'+ prop + '"; value: "' + pluginParameters[prop]+'"}',page)
            parameters.push(parameter)
        }
        return parameters
        //createMap(placesPlugin)
    }

    //=====================States of page=====================
    states: [
        State {
            name: ""
            PropertyChanges { target: map; focus: true }
        },
        State {
            name: "Provider"
            PropertyChanges { target: providerMenu; y: page.height - providerMenu.height - mainMenu.height }
        },
        State {
            name: "New"
            PropertyChanges { target: newMenu; y: page.height - newMenu.height - mainMenu.height }
        },
        State {
            name: "Search"
            PropertyChanges { target: searchMenu; y: page.height - searchMenu.height - mainMenu.height }
        },
        State {
            name: "NewPlace"
            PropertyChanges { target: editPlaceDialog; title: "New Place"; opacity: 1 }
        },
        State {
            name: "NewCategory"
            PropertyChanges { target: editCategoryDialog; title: "New Category"; opacity: 1 }
        },
        State {
            name: "EditPlace"
            PropertyChanges { target: editPlaceDialog; opacity: 1 }
        },
        State {
            name: "EditCategory"
            PropertyChanges { target: editCategoryDialog; opacity: 1 }
        },
        State {
            name: "Search Center"
            PropertyChanges { target: searchCenterDialog; opacity: 1 }
            StateChangeScript { script: searchCenterDialog.prepareDialog() }
        },
        State {
            name: "Search Bounding Box"
            PropertyChanges { target: searchBoxDialog; opacity: 1 }
            StateChangeScript { script: searchBoxDialog.prepareDialog() }
        },
        State {
            name: "Search Bounding Circle"
            PropertyChanges { target: searchCircleDialog; opacity: 1 }
            StateChangeScript { script: searchCircleDialog.prepareDialog() }
        },
        State {
            name: "Search Options"
            PropertyChanges { target: optionsDialog; opacity: 1 }
            StateChangeScript { script: optionsDialog.prepareDialog() }
        }
    ]

    //=====================State-transition animations for page=====================
    transitions: [
        Transition {
            to: ""
            NumberAnimation { properties: "opacity,y,x,rotation" ; duration: 500; easing.type: Easing.Linear }
        },
        Transition {
            to: "Provider"
            NumberAnimation { properties: "y" ; duration: 300; easing.type: Easing.Linear }
        },
        Transition {
            to: "New"
            NumberAnimation { properties: "y" ; duration: 300; easing.type: Easing.Linear }
        },
        Transition {
            to: "Search"
            NumberAnimation { properties: "y" ; duration: 300; easing.type: Easing.Linear }
        }
    ]
}
