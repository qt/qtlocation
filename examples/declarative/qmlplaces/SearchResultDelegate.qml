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

Rectangle {
    id: thisItem
    width: parent.width
    radius: 10
    height: textFields.height + 10
    Behavior on height { PropertyAnimation{} }

    gradient: Gradient {
        GradientStop { position: 0.0; color: "lightgray" }
        GradientStop { id: gradStop; position: 1.0; color: 'gray'
            Behavior on color { ColorAnimation{} }
        }
    }

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

    Component {
        id: placeComponent
        Item {
            focus:true

            anchors.left: parent.left
            anchors.right: parent.right
            height: childrenRect.height

            MouseArea {
                anchors.fill: col
                onClicked: {
                    if (textFields.item.state == 'place-core') {
                        textFields.item.state = 'place-details'
                        if (!place.detailsFetched)
                            place.getDetails()
                    } else if (textFields.item.state == 'place-details') {
                        textFields.item.state = 'place-core'
                    }
                }

                onPressAndHold: {
                    recommendationModel.placeId = place.placeId;
                    recommendationModel.executeQuery();
                    placesList.model = recommendationModel;
                }
            }

            Column {
                id: col

                anchors.left: parent.left
                anchors.right: parent.right
                Text { text: '<b>Name: </b> ' + place.name; font.pixelSize: 16 }

                RatingView { rating: place.rating.value }

                Text { text: '<b>Street: </b> ' + place.location.address.street; font.pixelSize: 16 }
                Text { text: '<b>Latitude: </b> ' + place.location.coordinate.latitude; font.pixelSize: 16 }
                Text { text: '<b>Longitude: </b> ' + place.location.coordinate.longitude; font.pixelSize: 16 }
                Text { text: '<b>Categories: </b> ' + categoryNames(place.categories); font.pixelSize: 16 }

                Text {
                    text: '<b>Editorial count: </b> ' + place.editorialModel.totalCount +
                          (place.editorialModel.totalCount <= 0 ? '' : ', <a href=\"dummy\">show editorials</a>')
                    font.pixelSize: 16
                    onLinkActivated: editorialList.model = place.editorialModel
                }

                Text {
                    text: '<b>Review count: </b> ' + place.reviewModel.totalCount +
                          ((place.reviewModel.totalCount <= 0) ? '' : ', <a href=\"dummy\">show reviews</a>')
                    font.pixelSize: 16
                    onLinkActivated: reviewList.model = place.reviewModel
                }

                Text {
                    text: '<b>Image count: </b> ' + place.imageModel.totalCount +
                          ((place.imageModel.totalCount <= 0) ? '' : ', <a href=\"dummy\">show images</a>')
                    font.pixelSize: 16
                    onLinkActivated: imageGrid.model = place.imageModel
                }

                Text {
                    text: '<b>Phone: </b> ' + place.primaryPhone
                    font.pixelSize: 16
                    visible: place.primaryPhone.length > 0
                }

                Text {
                    text: '<b>Fax: </b> ' + place.primaryFax
                    font.pixelSize: 16
                    visible: place.primaryFax.length > 0
                }

                Text {
                    text: '<b>Email: </b> ' + place.primaryEmail
                    font.pixelSize: 16
                    visible: place.primaryEmail.length > 0
                }

                Text {
                    text: '<b>Website: </b> <a href=\"' + place.primaryUrl + '\">' + place.primaryUrl + '</a>'
                    font.pixelSize: 16
                    visible: String(place.primaryUrl).length > 0
                    onLinkActivated: Qt.openUrlExternally(place.primaryUrl)
                }

                Text { text: '<b>Tags: </b> ' + place.tags; font.pixelSize: 16 }
                //Text { text: '<b>Suppliers: </b> ' + JSON.stringify(place.suppliers); font.pixelSize: 16 }
                Text { id: status;
                       text: {
                            var str='<b>Status: </b> ';
                            switch (place.status) {
                                 case (Place.Ready):
                                        return str + 'Ready';
                                 case (Place.Fetching):
                                   return str + 'Fetching';
                             }
                        }
                       font.pixelSize: 16
                   }

                Repeater {
                    id: extendedAttributes
                    model: place.extendedAttributes.keys()
                    delegate: Text {
                        text: "<b>" + place.extendedAttributes[modelData].label + ": </b>" +
                              place.extendedAttributes[modelData].text
                        width: parent.width
                        font.pixelSize: 16
                        wrapMode: Text.WordWrap
                    }
                }
            }

            state: 'place-core'
            states: [
                State {
                    name: "place-core"
                },
                State {
                    name: "place-details"
                    PropertyChanges { target: gradStop; color:"lightskyblue" }
                }
            ]
        }
    }

    Component {
        id: didYouMeanComponent
        Item {
            height: childrenRect.height
            width: parent.width

            Text {
                text:'<b>Did you mean ' + didYouMean + '?</b>';
                font.pixelSize: 16
            }

            state: 'didYouMean'
            states: [
                State {
                    name: 'didYouMean'
                    PropertyChanges { target:gradStop; color: 'palegreen'}
                }
            ]
        }
    }

    Loader {
        id: textFields

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 5
        y: 5
        clip: true

        sourceComponent: (type == SearchResultModel.Place) ? placeComponent : didYouMeanComponent
    }
}
