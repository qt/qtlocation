import QtQuick 2.0
import Qt.location 5.0

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
                    text: '<b>Descriptions count: </b> ' + place.descriptions.length +
                          (place.descriptions.length <= 0 ? '' : ', <a href=\"dummy\">show descriptions</a>')
                    font.pixelSize: 16
                    onLinkActivated: descriptionList.model = place.descriptions
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
                Text { id: paymentMethods; font.pixelSize: 16 }
            }

            state: 'place-core'
            states: [
                State {
                    name: "place-core"
                },
                State {
                    name: "place-details"
                    PropertyChanges { target: gradStop; color:"lightskyblue" }
                    PropertyChanges { target: paymentMethods; text: '<b>Payment methods: </b> '
                        + ((place.extendedAttributes.paymentMethods) ? place.extendedAttributes.paymentMethods.text : "No payment methods")}
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
