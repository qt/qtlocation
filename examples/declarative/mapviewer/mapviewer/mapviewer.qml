/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
import "content/components"
import "content/map"
import "content/dialogs"

Item {
    id: page
    width: 360
    height: 640
    property bool mobileUi: true

    Rectangle {
        id: backgroundRect
        anchors.fill: parent
        color: "lightgrey"
        z:2
    }
    TitleBar {
        id: titleBar; z: mainMenu.z; width: parent.width; height: 40; opacity: 0.9; text: "QML mapviewer example"; visible: !page.mobileUi
        onClicked: { Qt.quit() }
    }

//=====================Menu=====================
    Menu {
        id:mainMenu
        anchors.bottom: parent.bottom
        z: backgroundRect.z + 2

        Component.onCompleted: {
            addItem("Options")
            addItem("Map Type")
            addItem("Provider")
            disableItem("Map Type")
        }

        onClicked: {
            switch (button) {
                case "Options": {
                    page.state = "Options"
                    break;
                }
                case "Map Type": {
                    page.state = "MapType"
                    break;
                }
                case "Provider": {
                    page.state = "Provider"
                    break;
                }
            }
        }
    }

    Menu {
        id: optionsMenu
        z: backgroundRect.z + 2
        y: page.height
        horizontalOrientation: false

        Component.onCompleted: {
            addItem("Reverse geocode")
            addItem("Geocode")
            addItem("Route")
        }

        onClicked: {
            switch (button) {
                case "Reverse geocode": {
                    page.state = "RevGeocode"
                    break;
                }
                case "Geocode": {
                    page.state = "Geocode"
                    break;
                }
                case "Route": {
                    page.state = "Route"
                    break;
                }
            }
        }
    }

    Menu {
        id: mapTypeMenu
        z: backgroundRect.z + 2
        y: page.height
        horizontalOrientation: false
        exclusive: true

        Component.onCompleted: {
/*            for (var i = 0; i<map.supportedMapTypes.length; i++)
                addItem(map.supportedMapTypes[i].name)
            if (map.supportedMapTypes.length > 0)
                exclusiveButton = map.supportedMapTypes[0].name*/
        }

        onClicked: {
            page.state = ""
        }

        onExclusiveButtonChanged: {
//             map.mapType = exclusiveButton
        }
    }

    Menu {
        id: providerMenu
        z: backgroundRect.z + 2
        y: page.height
        horizontalOrientation: false
        exclusive: true

        Component.onCompleted: {
            var plugins = map.getPlugins()
            for (var i = 0; i<plugins.length; i++)
                addItem(plugins[i])
            if (plugins.length > 0) exclusiveButton = plugins[0]
        }

        onClicked: {
            page.state = ""
        }

        onExclusiveButtonChanged: {
            for (var i = 0; i<map.maps.length; i++)
                if (map.maps[i].plugin.name == exclusiveButton)
                    map.currentMap = map.maps[i]
        }
    }

//=====================Dialogs=====================
    Message {
        id: messageDialog
        z: backgroundRect.z + 2
        onOkButtonClicked: {
            page.state = ""
        }
        onCancelButtonClicked: {
            page.state = ""
        }

        states: [
            State{
                name: "GeocodeError"
                PropertyChanges { target: messageDialog; title: "Geocode Error" }
                PropertyChanges { target: messageDialog; text: "Unable to find location for the given point" }
            },
            State{
                name: "UnknownGeocodeError"
                PropertyChanges { target: messageDialog; title: "Geocode Error" }
                PropertyChanges { target: messageDialog; text: "Unsuccessful geocode" }
            },
            State{
                name: "AmbiguousGeocode"
                PropertyChanges { target: messageDialog; title: "Ambiguous geocode" }
                PropertyChanges { target: messageDialog; text: map.geocodeModel.count + " results found for the given address, please specify location" }
            },
            State{
                name: "RouteError"
                PropertyChanges { target: messageDialog; title: "Route Error" }
                PropertyChanges { target: messageDialog; text: "Unable to find a route for the given points"}
            },
            State{
                name: "Coordinates"
                PropertyChanges { target: messageDialog; title: "Coordinates" }
            },
            State{
                name: "LocationInfo"
                PropertyChanges { target: messageDialog; title: "Location" }
                PropertyChanges { target: messageDialog; text: geocodeMessage() }
            }
        ]
    }

//Route Dialog
    RouteDialog {
        id: routeDialog
        z: backgroundRect.z + 2

        Coordinate { id: endCoordinate }
        Coordinate { id: startCoordinate }
        Address { id:startAddress }
        Address { id:endAddress }

/*        GeocodeModel {
            id: tempGeocodeModel
            plugin : map.plugin
            property int success: 0
            onStatusChanged:{
                if ((status == GeocodeModel.Ready) && (count == 1)) {
                    success++
                    if (success == 1){
                        startCoordinate.latitude = get(0).coordinate.latitude
                        startCoordinate.longitude = get(0).coordinate.longitude
                        clear()
                        query = endAddress
                        update();
                    }
                    if (success == 2)
                    {
                        endCoordinate.latitude = get(0).coordinate.latitude
                        endCoordinate.longitude = get(0).coordinate.longitude
                        success = 0
                        routeDialog.calculateRoute()
                    }
                }
                else if ((status == GeocodeModel.Ready) || (status == GeocodeModel.Error)){
                    var st = (success == 0 ) ? "start" : "end"
                    messageDialog.state = ""
                    if ((status == GeocodeModel.Ready) && (count == 0 )) messageDialog.state = "UnknownGeocodeError"
                    else if (status == GeocodeModel.Error) {
                        messageDialog.state = "GeocodeError"
                        messageDialog.text = "Unable to find location for the " + st + " point"
                    }
                    else if ((status == GeocodeModel.Ready) && (count > 1 )){
                        messageDialog.state = "AmbiguousGeocode"
                        messageDialog.text = count + " results found for the " + st + " point, please specify location"
                    }
                    success = 0
                    page.state = "Message"
                    map.routeModel.clearAll()
                }
            }
        }
*/
        onGoButtonClicked: {
/*            var status = true

            messageDialog.state = ""
            if (routeDialog.byCoordinates) {
                startCoordinate.latitude = routeDialog.startLatitude
                startCoordinate.longitude = routeDialog.startLongitude
                endCoordinate.latitude = routeDialog.endLatitude
                endCoordinate.longitude = routeDialog.endLongitude

                calculateRoute()
            }
            else {
                startAddress.country = routeDialog.startCountry
                startAddress.street = routeDialog.startStreet
                startAddress.city = routeDialog.startCity

                endAddress.country = routeDialog.endCountry
                endAddress.street = routeDialog.endStreet
                endAddress.city = routeDialog.endCity

                tempGeocodeModel.query = startAddress
                tempGeocodeModel.update();k
            }*/
            page.state = ""
        }

        onCancelButtonClicked: {
            page.state = ""
        }

/*        function calculateRoute(){
            map.routeQuery.clearWaypoints();
            map.center = startCoordinate
            map.routeQuery.addWaypoint(startCoordinate)
            map.routeQuery.addWaypoint(endCoordinate)
            map.routeQuery.travelModes = routeDialog.travelMode
            map.routeQuery.routeOptimizations = routeDialog.routeOptimization

            for (var i=0; i<9; i++) {
                map.routeQuery.setFeatureWeight(i, 0)
            }

            for (var i=0; i<routeDialog.features.length; i++) {
                map.routeQuery.setFeatureWeight(routeDialog.features[i], RouteQuery.AvoidFeatureWeight)
            }

            map.routeModel.update();
        }*/
    }

//Geocode Dialog
    Dialog {
        id: geocodeDialog
        title: "Geocode"
        z: backgroundRect.z + 2

        Component.onCompleted: {
            var obj = [["Street", "Brandl St"],["District",""],["City", "Eight Mile Plains"], ["County", ""],["State", ""],["Country code",""],["Country","Australia"], ["Post code", ""]]
            setModel(obj)
        }

        Address {
            id: geocodeAddress
        }

        onGoButtonClicked: {
            page.state = ""
/*            messageDialog.state = ""
            geocodeAddress.street = dialogModel.get(0).inputText
            geocodeAddress.district = dialogModel.get(1).inputText
            geocodeAddress.city = dialogModel.get(2).inputText
            geocodeAddress.county = dialogModel.get(3).inputText
            geocodeAddress.state = dialogModel.get(4).inputText
            geocodeAddress.countryCode = dialogModel.get(5).inputText
            geocodeAddress.country = dialogModel.get(6).inputText
            geocodeAddress.postcode = dialogModel.get(7).inputText
            map.geocodeModel.clear()
            map.geocodeModel.query = geocodeAddress
            map.geocodeModel.update();*/
        }
        onCancelButtonClicked: {
            page.state = ""
        }
    }

//Reverse Geocode Dialog
    Dialog {
        id: reverseGeocodeDialog
        title: "Reverse Geocode"
        z: backgroundRect.z + 2

        Component.onCompleted: {
            var obj = [["Latitude","-27.575"],["Longitude", "153.088"]]
            setModel(obj)
        }

        Coordinate {
            id: reverseGeocodeCoordinate
        }

        onGoButtonClicked: {
            page.state = ""
            messageDialog.state = ""

/*            reverseGeocodeCoordinate.latitude = dialogModel.get(0).inputText
            reverseGeocodeCoordinate.longitude = dialogModel.get(1).inputText
            map.geocodeModel.clear()
            map.geocodeModel.query = reverseGeocodeCoordinate
            map.geocodeModel.update();*/
        }

        onCancelButtonClicked: {
            page.state = ""
        }
    }

//Get new coordinates for marker
    Dialog {
        id: coordinatesDialog
        title: "New coordinates"
        z: backgroundRect.z + 2

        Component.onCompleted: {
            var obj = [["Latitude", ""],["Longitude", ""]]
            setModel(obj)
        }

        onGoButtonClicked: {
            page.state = ""
            messageDialog.state = ""
/*            map.currentMarker.coordinate.latitude = dialogModel.get(0).inputText
            map.currentMarker.coordinate.longitude = dialogModel.get(1).inputText
            map.center = map.currentMarker.coordinate*/
        }

        onCancelButtonClicked: {
            page.state = ""
       }
    }

/*    GeocodeModel {
        id: geocodeModel
        plugin : Plugin { name : "nokia"}
        onLocationsChanged: {
            if (geocodeModel.count > 0) {
                console.log('setting the coordinate as locations changed in model.')
                map.center = geocodeModel.get(0).coordinate
            }
        }
    }*/


//=====================Map=====================
    MapComponent{
        id: map
        z : backgroundRect.z + 1
        width: parent.width
        height: parent.height
        Connections {
            target: map.currentMap
            onMapPressed: { page.state = "" }
        }
    }

/*        MapObjectView {
            model: geocodeModel
            delegate: Component {
                MapCircle {
                    radius: 10000
                    color: "red"
                    center: location.coordinate
                }
            }
        }

        onSliderUpdated: {
            page.state = ""
        }

        onCoordinatesCaptured: {
            messageDialog.state = "Coordinates"
            messageDialog.text = "<b>Latitude:</b> " + roundNumber(latitude,4) + "<br/><b>Longitude:</b> " + roundNumber(longitude,4);
            page.state = "Message"
        }

        onGeocodeFinished:{
            var street, district, city, county, state, countryCode, country, latitude, longitude, text

            if (map.geocodeModel.status == GeocodeModel.Ready){
                if (map.geocodeModel.count == 0) messageDialog.state = "UnknownGeocodeError"
                else if (map.geocodeModel.count > 1) messageDialog.state = "AmbiguousGeocode"
                else messageDialog.state = "LocationInfo";
            }
            else if (map.geocodeModel.status == GeocodeModel.Error) messageDialog.state = "GeocodeError"
            page.state = "Message"
        }

        onShowGeocodeInfo:{
            messageDialog.state = "LocationInfo"
            page.state = "Message"
        }

        onMoveMarker: {
            page.state = "Coordinates"
        }

        onRouteError: {
            messageDialog.state = "RouteError"
            page.state = "Message"
        }
    }
*/
    function geocodeMessage(){
        var street, district, city, county, state, countryCode, country, postcode, latitude, longitude, text
/*        latitude = map.geocodeModel.get(0).coordinate.latitude
        longitude = map.geocodeModel.get(0).coordinate.longitude
        street = map.geocodeModel.get(0).address.street
        district = map.geocodeModel.get(0).address.district
        city = map.geocodeModel.get(0).address.city
        county = map.geocodeModel.get(0).address.county
        state = map.geocodeModel.get(0).address.state
        countryCode = map.geocodeModel.get(0).address.countryCode
        country = map.geocodeModel.get(0).address.country
        postcode = map.geocodeModel.get(0).address.postcode

        text = "<b>Latitude:</b> " + latitude + "<br/>"
        text +="<b>Longitude:</b> " + longitude + "<br/>" + "<br/>"
        if (street) text +="<b>Street: </b>"+ street + " <br/>"
        if (district) text +="<b>District: </b>"+ district +" <br/>"
        if (city) text +="<b>City: </b>"+ city + " <br/>"
        if (county) text +="<b>County: </b>"+ county + " <br/>"
        if (state) text +="<b>State: </b>"+ state + " <br/>"
        if (countryCode) text +="<b>Country code: </b>"+ countryCode + " <br/>"
        if (country) text +="<b>Country: </b>"+ country + " <br/>"
        if (postcode) text +="<b>Postcode: </b>"+ postcode + " <br/>"*/
        return text
    }
//=====================States of page=====================
    states: [
       State {
            name: ""
            PropertyChanges { target: map; focus: true }
        },
        State {
            name: "RevGeocode"
            PropertyChanges { target: reverseGeocodeDialog; opacity: 1 }
        },
        State {
            name: "Route"
            PropertyChanges { target: routeDialog; opacity: 1 }
        },
        State {
            name: "Geocode"
            PropertyChanges { target: geocodeDialog; opacity: 1 }
        },
        State {
            name: "Coordinates"
            PropertyChanges { target: coordinatesDialog; opacity: 1 }
        },
        State {
            name: "Message"
            PropertyChanges { target: messageDialog; opacity: 1 }
        },
        State {
            name : "Options"
            PropertyChanges { target: optionsMenu; y: page.height - optionsMenu.height - mainMenu.height }
        },
        State {
            name : "Provider"
            PropertyChanges { target: providerMenu; y: page.height - providerMenu.height - mainMenu.height }
        },
        State {
            name : "MapType"
            PropertyChanges { target: mapTypeMenu; y: page.height - mapTypeMenu.height - mainMenu.height }
        }
    ]

//=====================State-transition animations for page=====================
    transitions: [
        Transition {
            to: "RevGeocode"
            NumberAnimation { properties: "opacity" ; duration: 500; easing.type: Easing.Linear }
        },
        Transition {
            to: "Route"
            NumberAnimation { properties: "opacity" ; duration: 500; easing.type: Easing.Linear }
        },
        Transition {
            to: "Geocode"
            NumberAnimation { properties: "opacity" ; duration: 500; easing.type: Easing.Linear }
        },
        Transition {
            to: "Coordinates"
            NumberAnimation { properties: "opacity" ; duration: 500; easing.type: Easing.Linear }
        },
        Transition {
            to: "Message"
            NumberAnimation { properties: "opacity" ; duration: 500; easing.type: Easing.Linear }
        },
        Transition {
            to: ""
            NumberAnimation { properties: "opacity" ; duration: 500; easing.type: Easing.Linear }
        },
        Transition {
            to: "Provider"
            NumberAnimation { properties: "y" ; duration: 300; easing.type: Easing.Linear }
        },
        Transition {
            to: "MapType"
            NumberAnimation { properties: "y" ; duration: 300; easing.type: Easing.Linear }
        },
        Transition {
            to: "Options"
            NumberAnimation { properties: "y" ; duration: 300; easing.type: Easing.Linear }
        }
    ]



/*    function getPlugins(){
        var plugin = Qt.createQmlObject ('import QtLocation 5.0; Plugin {}', page)
        var tempPlugin
        var myArray = new Array()

        console.log("getting plugins")
        for (var i = 0; i<plugin.availableServiceProviders.length; i++){
            tempPlugin = Qt.createQmlObject ('import QtLocation 5.0; Plugin {name: "' + plugin.availableServiceProviders[i]+ '"}', page)
            if (tempPlugin.supportsMapping && tempPlugin.supportsGeocoding && tempPlugin.supportsReverseGeocoding && tempPlugin.supportsRouting )
                myArray.push(tempPlugin.name)
        }

        return myArray
    }*/
}
