import QtQuick 1.1
import Qt.location 5.0

Rectangle {
    property SearchResult result
    id: thisItem
    width: parent.width
    radius: 10
    height: textFields.height
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
        id: placeFields
        Item {
            focus:true
            height: col.height + mediaButton.height + 10
            width:parent.width
            Column {
                id: col
                Text { text: '<b>Name: </b> ' + result.place.name; font.pixelSize: 16 }
                Text { text: '<b>Street: </b> ' + result.place.location.address.street; font.pixelSize: 16 }
                Text { text: '<b>Latitude: </b> ' + result.place.location.coordinate.latitude; font.pixelSize: 16 }
                Text { text: '<b>Longitude: </b> ' + result.place.location.coordinate.longitude; font.pixelSize: 16 }
                Text { text: '<b>Categories: </b> ' + categoryNames(result.place.categories); font.pixelSize: 16 }
                Text { text: '<b>Media count: </b> ' + result.place.mediaModel.totalCount; font.pixelSize: 16 }
                Text { text: '<b>Descriptions count: </b> ' + result.place.descriptions.length; font.pixelSize: 16 }
                Text { text: '<b>Review count: </b> ' + result.place.reviewModel.totalCount; font.pixelSize: 16 }
                Text { text: result.place.primaryPhone.length == 0  ? '':'<b>Phone: </b> ' + result.place.primaryPhone; font.pixelSize: 16 }
                Text { text: result.place.primaryFax.length == 0  ? '':'<b>Fax: </b> ' + result.place.primaryFax; font.pixelSize: 16 }
                Text { text: result.place.primaryEmail.length == 0  ? '':'<b>Email: </b> ' + result.place.primaryEmail; font.pixelSize: 16 }
                Text { text: String(result.place.primaryUrl).length == 0  ? '':'<b>Website: </b> ' + result.place.primaryUrl; font.pixelSize: 16 }
                Text { text: '<b>Tags: </b> ' + result.place.tags; font.pixelSize: 16 }
                //Text { text: '<b>Suppliers: </b> ' + JSON.stringify(place.suppliers); font.pixelSize: 16 }
                Text { id: detailsFetched; text:'<b>Details Fetched: </b> ' + result.place.detailsFetched; font.pixelSize: 16 }
                Text { id: paymentMethods; font.pixelSize: 16 }
            }

            Rectangle {
                id: mediaButton
                anchors.top: col.bottom
                height: (result.place.mediaModel.totalCount > 0) ? showMedia.height : 0

                Text {
                    id: showMedia
                    text: (result.place.mediaModel.totalCount > 0) ? '<a href=\"dummy\">Show Media</a>':''
                    onLinkActivated: {
                        mediaDisplayLoader.sourceComponent = mediaDisplay
                        mediaDisplayLoader.item.model = result.place.mediaModel
                    }
                }
            }

            MouseArea {
                anchors.fill: col
                onClicked: {
                    if (textFields.item.state == 'place-core') {
                        textFields.item.state = 'place-details'
                        if (!result.place.detailsFetched)
                            result.place.getDetails()
                        } else if (textFields.item.state == 'place-details') {
                        textFields.item.state = 'place-core'
                    }
                }

                onPressAndHold: {
                    placesList.model = recommendationModel
                    recommendationModel.placeId = result.place.placeId
                    recommendationModel.executeQuery()
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
                    PropertyChanges { target: paymentMethods; text: '<b>Payment methods: </b> ' + result.place.businessInformation.paymentMethods}
                }
            ]
        }
    }

    Component {
        id: didYouMeanField
        Item {
            height: didYouMeanField.height
            Text { id:didYouMeanField; text:'<b>Did you mean ' + result.didYouMeanSuggestion + '?</b>'; font.pixelSize: 16 }
            state: 'didYouMean'
            states: [
                State {
                    name: 'didYouMean'
                    PropertyChanges { target:gradStop; color: 'palegreen'}
                }
            ]
        }
    }

    Component {
          id: mediaDisplay
        Rectangle {
            property alias model: mediaList.model
            height: fullView.height
            width: fullView.width
            z: 1

            ListView {
                id: mediaList
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: closeMediaButton.top

                delegate: Component {
                    Rectangle {
                        height: mediaUrl.height
                        width: parent.width
                        radius:10
                        border.color: 'black'

                        Text { id: mediaUrl; text: media.url}

                        gradient: Gradient {
                            GradientStop { position: 0.0; color: 'lightblue' }
                            GradientStop { position: 0.5; color: 'white'}
                            GradientStop { position: 1.0; color: 'lightblue'}
                        }
                    }
                }
            }
            Rectangle {
                id: closeMediaButton
                height: mediaClose.height
                width: parent.width
                anchors.bottom: parent.bottom
                gradient: Gradient {
                    GradientStop { position: 0.0; color: 'red' }
                    GradientStop { position: 0.5; color: 'white'}
                    GradientStop { position: 1.0; color: 'red'}
                }

                Text {
                    id: mediaClose;
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: 'Close';
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        mediaDisplayLoader.sourceComponent = undefined
                    }
                }
            }
        }
    }

    Loader {
        id: textFields
        sourceComponent: (result.type == SearchResult.Place) ? placeFields : didYouMeanField
    }
}
