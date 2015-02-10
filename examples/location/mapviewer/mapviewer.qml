/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
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

import QtQuick 2.4
import QtQuick.Controls 1.3
import QtLocation 5.3
import QtPositioning 5.2
import QtLocation.examples 5.0 as OwnControls
import "content/map"

ApplicationWindow {
    id: appWindow
    title: qsTr("Mapviewer")
    height: 640
    width: 360
    visible: true

    property variant map
    property variant minimap
    property variant parameters

    //defaults
    property variant fromCoordinate: QtPositioning.coordinate(-27.575, 153.088)
    property variant toCoordinate: QtPositioning.coordinate(-27.465, 153.023)

    Address {
        id :fromAddress
        street: "53 Brandl St"
        city: "Eight Mile Plains"
        country: "Australia"
        state : ""
        postalCode: ""
    }


    Address {
        id: toAddress
        street: "Heal st"
        city: "New Farm"
        country: "Australia"
    }

    menuBar: MainMenu {
        id: mainMenu

        onSelectProvider: {
            stackView.pop()
            for (var i = 0; i < providerMenu.items.length; i++) {
                providerMenu.items[i].checked = providerMenu.items[i].text === providerName
            }

            if (minimap) {
                minimap.destroy()
                minimap = null
            }

            createMap(providerName)
            if (map.error === Map.NoError) {
                selectMapType(map.activeMapType)
                toolsMenu.createMenu(map);
            } else {
                mapTypeMenu.clear();
                toolsMenu.clear();
            }
        }

        onSelectMapType: {
            stackView.pop(page)
            for (var i = 0; i < mapTypeMenu.items.length; i++) {
                mapTypeMenu.items[i].checked = mapTypeMenu.items[i].text === mapType.name
            }
            map.activeMapType = mapType
        }


        onSelectTool: {
            if (tool === "AddressRoute") {
                stackView.push({ item: Qt.resolvedUrl("RouteAddress.qml") ,
                                   properties: { "plugin": map.plugin,
                                                 "toAddress": toAddress,
                                                 "fromAddress": fromAddress}})
                stackView.currentItem.showRoute.connect(showRoute)
                stackView.currentItem.showMessage.connect(showMessage)
                stackView.currentItem.closeForm.connect(closeForm)
            } else if (tool === "CoordinateRoute") {
                stackView.push({ item: Qt.resolvedUrl("RouteCoordinate.qml") ,
                                   properties: { "toCoordinate": toCoordinate,
                                                 "fromCoordinate": fromCoordinate}})
                stackView.currentItem.showRoute.connect(showRoute)
                stackView.currentItem.closeForm.connect(closeForm)
            } else if (tool === "Geocode") {
                stackView.push({ item: Qt.resolvedUrl("Geocode.qml") ,
                                   properties: { "address": fromAddress}})
                stackView.currentItem.showPlace.connect(showPlace)
                stackView.currentItem.closeForm.connect(closeForm)
            } else if (tool === "RevGeocode") {
                stackView.push({ item: Qt.resolvedUrl("ReverseGeocode.qml") ,
                                   properties: { "coordinate": fromCoordinate}})
                stackView.currentItem.showPlace.connect(showPlace)
                stackView.currentItem.closeForm.connect(closeForm)
            }
        }

        onToggleMapState: {
            stackView.pop(page)
            if (state === "FollowMe") {
                map.followme =! map.followme
                page.state = ""
            } else if (state === "MiniMap") {
                toggleMiniMapState()
                isMiniMap = minimap
            }
        }

        function toggleMiniMapState() {
            if (minimap) {
                minimap.destroy()
                minimap = null
            } else {
                minimap = Qt.createQmlObject ('import "content/map"; MiniMap{ z: map.z + 2 }', map)
            }
            page.state = ""
        }

        //! [routerequest0]
        function showRoute(startCoordinate, endCoordinate) {
            // clear away any old data in the query
            map.routeQuery.clearWaypoints();

            // add the start and end coords as waypoints on the route
            map.routeQuery.addWaypoint(startCoordinate)
            map.routeQuery.addWaypoint(endCoordinate)
            map.routeQuery.travelModes = RouteQuery.CarTravel
            map.routeQuery.routeOptimizations = RouteQuery.FastestRoute
            //! [routerequest0]

            //! [routerequest0 feature weight]
            for (var i=0; i<9; i++) {
                map.routeQuery.setFeatureWeight(i, 0)
            }
            //for (var i=0; i<routeDialog.features.length; i++) {
            //    map.routeQuery.setFeatureWeight(routeDialog.features[i], RouteQuery.AvoidFeatureWeight)
            //}
            //! [routerequest0 feature weight]

            //! [routerequest1]
            map.routeModel.update();
            // center the map on the start coord
            map.center = startCoordinate;
            stackView.pop(page);
            //! [routerequest1]
        }

        function showPlace(geocode) {
            // send the geocode request
            map.geocodeModel.query = geocode
            map.geocodeModel.update()
        }
    }

    function showMessage(title,message,backPage) {
        stackView.push({ item: Qt.resolvedUrl("Message.qml") ,
                           properties: {
                               "title" : title,
                               "message" : message,
                               "backPage" : backPage
                           }})
        stackView.currentItem.closeForm.connect(closeMessage)
    }

    function closeMessage(backPage) {
            stackView.pop(backPage)
    }

    function closeForm() {
        stackView.pop(page)
    }

    function geocodeMessage(){
        var street, district, city, county, state, countryCode, country, postalCode, latitude, longitude, text
        latitude = Math.round(map.geocodeModel.get(0).coordinate.latitude * 10000) / 10000
        longitude =Math.round(map.geocodeModel.get(0).coordinate.longitude * 10000) / 10000
        street = map.geocodeModel.get(0).address.street
        district = map.geocodeModel.get(0).address.district
        city = map.geocodeModel.get(0).address.city
        county = map.geocodeModel.get(0).address.county
        state = map.geocodeModel.get(0).address.state
        countryCode = map.geocodeModel.get(0).address.countryCode
        country = map.geocodeModel.get(0).address.country
        postalCode = map.geocodeModel.get(0).address.postalCode

        text = "<b>Latitude:</b> " + latitude + "<br/>"
        text +="<b>Longitude:</b> " + longitude + "<br/>" + "<br/>"
        if (street) text +="<b>Street: </b>"+ street + " <br/>"
        if (district) text +="<b>District: </b>"+ district +" <br/>"
        if (city) text +="<b>City: </b>"+ city + " <br/>"
        if (county) text +="<b>County: </b>"+ county + " <br/>"
        if (state) text +="<b>State: </b>"+ state + " <br/>"
        if (countryCode) text +="<b>Country code: </b>"+ countryCode + " <br/>"
        if (country) text +="<b>Country: </b>"+ country + " <br/>"
        if (postalCode) text +="<b>PostalCode: </b>"+ postalCode + " <br/>"
        return text
    }

    function createMap(provider){
        var plugin

        if (parameters && parameters.length>0)
            plugin = Qt.createQmlObject ('import QtLocation 5.3; Plugin{ name:"' + provider + '"; parameters: appWindow.parameters}', appWindow)
        else
            plugin = Qt.createQmlObject ('import QtLocation 5.3; Plugin{ name:"' + provider + '"}', appWindow)

        if (map) {
            map.destroy()
            minimap = null
        }

        map = Qt.createQmlObject ('import QtLocation 5.3;\
                                       import "content/map";\
                                       MapComponent{\
                                           z : backgroundRect.z + 1;\
                                           width: page.width;\
                                           height: page.height;\
                                           onFollowmeChanged: {mainMenu.isFollowMe = map.followme}\
                                           onSupportedMapTypesChanged: {mainMenu.mapTypeMenu.createMenu(map)}\
                                           onCoordinatesCaptured: {\
                                               var text = "<b>" + qsTr("Latitude:") + "</b> " + roundNumber(latitude,4) + "<br/><b>" + qsTr("Longitude:") + "</b> " + roundNumber(longitude,4);\
                                               showMessage(qsTr("Coordinates"),text);\
                                           }\
                                           onGeocodeFinished:{\
                                               if (map.geocodeModel.status == GeocodeModel.Ready) {\
                                                   if (map.geocodeModel.count == 0) {\
                                                       showMessage(qsTr("Geocode Error"),qsTr("Unsuccessful geocode"));\
                                                   } else if (map.geocodeModel.count > 1) { \
                                                       showMessage(qsTr("Ambiguous geocode"), map.geocodeModel.count + " " + \
                                                       qsTr("results found for the given address, please specify location"));\
                                                   } else { \
                                                       showMessage(qsTr("Location"), geocodeMessage(),page);\
                                                  ;}\
                                               } else if (map.geocodeModel.status == GeocodeModel.Error) {\
                                                   showMessage(qsTr("Geocode Error"),qsTr("Unsuccessful geocode")); \
                                               }\
                                           }\
                                           onMoveMarker: {\
                                               page.state = "Coordinates";\
                                           }\
                                           onRouteError: {\
                                               showMessage(qsTr("Route Error"),qsTr("Unable to find a route for the given points"),page);\
                                           }\
                                           onRequestLocale:{\
                                               page.state = "Locale";\
                                           }\
                                           onShowGeocodeInfo:{\
                                               showMessage(qsTr("Location"),geocodeMessage(),page);\
                                           }\
                                           onResetState: {\
                                               page.state = "";\
                                           }\
                                           onErrorChanged: {\
                                               if (map.error != Map.NoError) {\
                                               var title = qsTr("ProviderError");\
                                               var message =  map.errorString + "<br/><br/><b>" + qsTr("Try to select other provider") + "</b>";\
                                                   if (map.error == Map.MissingRequiredParameterError) \
                                                       message += "<br/>" + qsTr("or see") + " \'mapviewer --help\' "\
                                                       + qsTr("how to pass plugin parameters.");\
                                               showMessage(title,message);\
                                               }\
                                           }\
                                           onShowMainMenu: {\
                                               mapPopupMenu.show(coordinate);\
                                           }\
                                           onShowMarkerMenu: {\
                                               markerPopupMenu.show(coordinate);\
                                           }\
                                       }',page)
        map.plugin = plugin;
        map.zoomLevel = (map.maximumZoomLevel - map.minimumZoomLevel)/2
    }

    function getPlugins(){
        var plugin = Qt.createQmlObject ('import QtLocation 5.3; Plugin {}', appWindow)
        var tempPlugin
        var myArray = new Array()
        for (var i = 0; i<plugin.availableServiceProviders.length; i++){
            tempPlugin = Qt.createQmlObject ('import QtLocation 5.3; Plugin {name: "' + plugin.availableServiceProviders[i]+ '"}', appWindow)
            if (tempPlugin.supportsMapping())
                myArray.push(tempPlugin.name)
        }
        myArray.sort()
        return myArray
    }

    function initializeProvders(pluginParameters) {
        var parameters = new Array()
        for (var prop in pluginParameters){
            var parameter = Qt.createQmlObject('import QtLocation 5.3; PluginParameter{ name: "'+ prop + '"; value: "' + pluginParameters[prop]+'"}',appWindow)
            parameters.push(parameter)
        }
        appWindow.parameters = parameters
        var plugins = getPlugins()
        mainMenu.providerMenu.createMenu(plugins)
        for (var i = 0; i<plugins.length; i++) {
            if (plugins[i] === "osm")
                mainMenu.selectProvider(plugins[i])
        }
    }

    MapPopupMenu {
        id: mapPopupMenu
        onItemClicked: {
            stackView.pop(page)
            if (item === "addMarker") {
                map.addMarker()
            } else if (item === "getCoordinate") {
                map.coordinatesCaptured(coordinate.latitude, coordinate.longitude)
            } else if (item === "fitViewport") {
                map.fitViewportToMapItems()
            } else if (item === "deleteMarkers") {
                map.deleteMarkers()
            } else if (item === "deleteItems") {
                map.deleteMapItems()
            }
        }

        function show(coordinate) {
            stackView.pop(page)
            mapPopupMenu.coordinate = coordinate
            mapPopupMenu.markersCount = map.markers.length
            mapPopupMenu.mapItemsCount = map.mapItems.length
            mapPopupMenu.update()
            mapPopupMenu.popup()
        }
    }

    MarkerPopupMenu {
        id: markerPopupMenu
        onItemClicked: {
            stackView.pop(page)
            if (item === "deleteMarker") {
                map.deleteMarker(map.currentMarker)
            } else if (item === "getMarkerCoordinate") {
                map.coordinatesCaptured(map.markers[map.currentMarker].coordinate.latitude, map.markers[map.currentMarker].coordinate.longitude)
            } else if (item === "moveMarkerTo") {
                map.moveMarker()
            } else if (item === "showDrawMenu") {
                map.drawItemPopup()
            } else if (item === "routeToNextPoint" || item === "routeToNextPoints") {
                map.calculateRoute()
            } else if (item === "distanceToNextPoint") {
                var coordinate1 = map.markers[currentMarker].coordinate;
                var coordinate2 = map.markers[currentMarker+1].coordinate;
                var distance = map.formatDistance(coordinate1.distanceTo(coordinate2));
                showMessage(qsTr("Distance"),"<b>" + qsTr("Distance:") + "</b> " + distance)
            }
        }

        function show(coordinate) {
            stackView.pop(page)
            markerPopupMenu.markersCount = map.markers.length
            markerPopupMenu.update()
            markerPopupMenu.popup()
        }

    }


    StackView {
        id: stackView
        anchors.fill: parent
        focus: true
        initialItem: Item {
        id: page

        Rectangle {
            id: backgroundRect
            anchors.fill: parent
            color: "lightgrey"
            z:2
        }

        //=====================Dialogs=====================

        //Get new coordinates for marker
        OwnControls.InputDialog {
            id: coordinatesDialog
            title: "New coordinates"
            z: backgroundRect.z + 2

            Component.onCompleted: {
                var obj = [["Latitude", ""],["Longitude", ""]]
                setModel(obj)
            }

            onGoButtonClicked: {
                page.state = ""
                var newLat = parseFloat(dialogModel.get(0).inputText)
                var newLong = parseFloat(dialogModel.get(1).inputText)

                if (newLat !== "NaN" && newLong !== "NaN") {
                    var c = QtPositioning.coordinate(newLat, newLong);
                    if (c.isValid) {
                        map.markers[map.currentMarker].coordinate = c;
                        map.center = c;
                    }
                }
            }

            onCancelButtonClicked: {
                page.state = ""
            }
        }

        //Get new locale
        OwnControls.InputDialog {
            id: localeDialog
            title: "New Locale"
            z: backgroundRect.z + 2

            Component.onCompleted: {
                var obj = [["Language", ""]]
                setModel(obj)
            }

            onGoButtonClicked: {
                page.state = ""
                map.setLanguage(dialogModel.get(0).inputText.split(Qt.locale().groupSeparator));
            }

            onCancelButtonClicked: {
                page.state = ""
            }
        }

        //=====================States of page=====================
        states: [
            State {
                name: "Coordinates"
                PropertyChanges { target: coordinatesDialog; opacity: 1 }
            },
            State {
                name : "Locale"
                PropertyChanges { target: localeDialog;  opacity: 1 }
            }
        ]

        //=====================State-transition animations for page=====================
        transitions: [
            Transition {
                to: "Coordinates"
                NumberAnimation { properties: "opacity" ; duration: 500; easing.type: Easing.Linear }
            },
            Transition {
                to: ""
                NumberAnimation { properties: "opacity" ; duration: 500; easing.type: Easing.Linear }
            }
        ]
    }
    }
}
