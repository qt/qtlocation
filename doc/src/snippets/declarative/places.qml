/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the documentation of the Qt Toolkit.
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

//! [QtQuick import]
import QtQuick 2.0
//! [QtQuick import]
//! [QtLocation import]
import QtLocation 5.0
//! [QtLocation import]

Item {
    Plugin {
        id: myPlugin
    }

    Place {
        id: place
    }

    //! [Category]
    Category {
        id: category

        plugin: myPlugin
        name: "New Category"
        visibility: Category.PrivateVisibility
    }
    //! [Category]

    function saveCategory() {
        //! [Category save]
        category.save();
        //! [Category save]
    }

    //! [CategoryView]
    ListView {
        model: CategoryModel {
            plugin: myPlugin
            hierarchical: false
        }
        delegate: Text { text: category.name }
    }
    //! [CategoryView]

    //! [ExtendedAttributes]
    ListView {
        model: place.extendedAttributes.keys()
        delegate: Text {
            text: "<b>" + place.extendedAttributes[modelData].label + ": </b>" +
                  place.extendedAttributes[modelData].text
        }
    }
    //! [ExtendedAttributes]

    //! [ExtendedAttributes read]
    function printExtendedAttributes(extendedAttributes) {
        var keys = extendedAttributes.keys();
        for (var i = 0; i < keys.length; ++i) {
            var key = keys[i];
            console.log(extendedAttributes[key].label + ": " + extendedAttributes[key].text);
        }
    }
    //! [ExtendedAttributes read]

    //! [Icon]
    Image {
        source: icon.url(Qt.size(64, 64), Icon.Selected | Icon.List)
    }
    //! [Icon]

    //! [PlaceSearchModel]
    PlaceSearchModel {
        id: searchModel

        searchTerm: "Pizza"
        searchArea: BoundingCircle {
            center: Coordinate {
                // Brisbane
                longitude: 153.02778
                latitude: -27.46778
            }
        }

        Component.onCompleted: execute()
    }

    ListView {
        model: searchModel
        delegate: Text { text: 'Name: ' + place.name }
    }
    //! [PlaceSearchModel]

    //! [RecommendationModel]
    PlaceRecommendationModel {
        id: recommendationModel

        placeId: place.placeId
        searchArea: BoundingCircle {
            center: Coordinate {
                // Brisbane
                longitude: 153.02778
                latitude: -27.46778
            }
        }

        Component.onCompleted: execute()
    }

    ListView {
        model: recommendationModel
        delegate: Text { text: 'Name: ' + place.name }
    }
    //! [RecommendationModel]

    //! [SearchSuggestionModel]
    PlaceSearchSuggestionModel {
        id: suggestionModel
        searchArea: BoundingCircle {
            center: Coordinate {
                // Brisbane
                longitude: 153.02778
                latitude: -27.46778
            }
        }

        onSearchTermChanged: execute()
    }

    ListView {
        model: suggestionModel
        delegate: Text { text: suggestion }
    }
    //! [SearchSuggestionModel]

    //! [Ratings]
    Text {
        text: "This place is rated " + place.ratings.average + " out of " + place.ratings.maximum + " stars."
    }
    //! [Ratings]

    //! [ContactDetails read]
    function printContactDetails(contactDetails) {
        var keys = contactDetails.keys();
        for (var i = 0; i < keys.length; ++i) {
            var contactList = contactDetails[keys[i]];
            for (var j = 0; j < contactList.length; ++j) {
                console.log(contactList[j].label + ": " + contactList[j].value);
            }
        }
    }
    //! [ContactDetails read]

    //! [ContactDetails phoneList]
    ListView {
        model: place.contactDetails.Phone;
        delegate: Text { text: label + ": " + value }
    }
    //! [ContactDetails phoneList]

    //! [Place savePlace def]
    Place {
        id: myPlace
        plugin: myPlugin

        name: "Nokia Brisbane"
        location: Location {
            address: Address {
                street: "53 Brandl Street"
                city: "Eight Mile Plains"
                postalCode: "4113"
                country: "Australia"
            }
            coordinate: Coordinate {
                latitude: -27.579646
                longitude: 153.100308
            }
        }

        visibility: Place.PrivateVisibility
    }
    //! [Place savePlace def]

    function savePlace() {
    //! [Place savePlace]
        myPlace.save();
    //! [Place savePlace]
    }

    function saveToNewPlugin() {
        //! [Place save to different plugin]
        myPlace.plugin = diffPlugin;
        place.placeId = "";
        place.categories = null;
        place.icon = null;
        place.visibility = QtLocation.UnspecifiedVisibility; //let the manager choose an appropriate default visibility
        place.save();
        //! [Place save to different plugin]
    }

    function getPlaceForId() {
    //! [Place placeId]
        place.plugin = myPlugin;
        place.placeId = "known-place-id";
        place.getDetails();
    //! [Place placeId]
    }

    function primaryContacts() {
    //! [Place primaryPhone]
        var primaryPhone;
        if (place.contactDetails["Phone"].length > 0)
            primaryPhone = place.contactDetails["Phone"][0].value;
    //! [Place primaryPhone]
    //! [Place primaryFax]
        var primaryFax;
        if (place.contactDetails["Fax"].length > 0)
            primaryFax = place.contactDetails["Fax"][0].value;
    //! [Place primaryFax]
    //! [Place primaryEmail]
        var primaryEmail;
        if (place.contactDetails["Email"].length > 0)
            primaryEmail = place.contactDetails["Email"][0].value;
    //! [Place primaryEmail]
    //! [Place primaryWebsite]
        var primaryWebsite;
        if (place.contactDetails["Website"].length > 0)
            primaryWebsite = place.contactDetails["Website"][0].value;
    //! [Place primaryWebsite]
    }
}
