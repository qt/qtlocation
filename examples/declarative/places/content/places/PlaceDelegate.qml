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
import "../components"

Item {
    property Place place
    property real distance

    signal searchForSimilar(variant place)
    signal showReviews(variant place)
    signal showEditorials(variant place)
    signal showImages(variant place)
    signal editPlace(variant place)
    signal deletePlace(variant place)

    Flickable {
        anchors.fill: parent

        contentHeight: c.height
        contentWidth: width

        Column {
            id: c

            width: parent.width
            spacing: 2
            clip: true

            Text {
                text: place ? place.name : ""
                font.pixelSize: 16
                font.bold: true
                color: "white"
            }

            RatingView { rating: (place && place.rating) ? place.rating.value : 0 }

            Group { text: qsTr("Address") }
            Text { text: distance + "m away"; color: "white" }
            Text { text: place ? place.location.address.street : ""; color: "white" }

            Group {
                text: qsTr("Categories")
                visible: place && place.categories.length > 0
            }
            Text {
                function categoryNames(categories) {
                    var result = "";

                    for (var i = 0; i < categories.length; ++i) {
                        if (result == "") {
                            result = categories[i].name;
                        } else {
                            result = result + ", " + categories[i].name;
                        }
                    }

                    return result;
                }

                text: place ? categoryNames(place.categories) : ""
                color: "white"
                width: parent.width
                wrapMode: Text.WordWrap
                visible: place && place.categories.length > 0
            }

            Group {
                text: qsTr("Contact details")
                visible: phone.visible || fax.visible || email.visible || website.visible
            }
            Text {
                id: phone
                text: qsTr("Phone: ") + (place ? place.primaryPhone : "")
                color: "white"
                visible: place && place.primaryPhone.length > 0
            }
            Text {
                id: fax
                text: qsTr("Fax: ") + (place ? place.primaryFax : "")
                color: "white"
                visible: place && place.primaryFax.length > 0
            }
            Text {
                id: email
                text: place ? place.primaryEmail : ""
                color: "white"
                visible: place && place.primaryEmail.length > 0
            }
            Text {
                id: website
                text: place ? '<a href=\"' + place.primaryWebsite + '\">' + place.primaryWebsite + '</a>' : ""
                color: "white"
                visible: place && String(place.primaryWebsite).length > 0
                onLinkActivated: Qt.openUrlExternally(place.primaryWebsite)
            }

            Group {
                text: qsTr("Additional information")
                visible: extendedAttributes.count > 0
            }

            Repeater {
                id: extendedAttributes
                model: place ? place.extendedAttributes.keys() : null
                delegate: Text {
                    text: place.extendedAttributes[modelData].label +
                          place.extendedAttributes[modelData].text
                    color: "white"
                    width: c.width
                    wrapMode: Text.WordWrap
                }
            }

            Column {
                id: buttons

                anchors.horizontalCenter: parent.horizontalCenter

                spacing: 5

                Button {
                    text: qsTr("Editorials")
                    enabled: place && place.editorialModel.totalCount > 0
                    onClicked: showEditorials(place)
                }
                Button {
                    text: qsTr("Reviews")
                    enabled: place && place.reviewModel.totalCount > 0
                    onClicked: showReviews(place)
                }
                Button {
                    text: qsTr("Images")
                    enabled: place && place.imageModel.totalCount > 0
                    onClicked: showImages(place)
                }
                Button {
                    text: qsTr("Find similar")
                    onClicked: searchForSimilar(place)
                }
                Button {
                    text: qsTr("Edit")
                    onClicked: editPlace(place)
                    visible: placesPlugin.supportedPlacesFeatures & Plugin.SavePlaceFeature
                }

                Button {
                    text: qsTr("Delete");
                    onClicked: deletePlace(place)
                    visible: placesPlugin.supportedPlacesFeatures & Plugin.RemovePlaceFeature
                }
            }
        }
    }
}
