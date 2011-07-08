import QtQuick 1.0
import Qt.location 5.0

Rectangle {
    property Place place

    height: c.height + 10
    width: parent.width

    radius: 10
    gradient: Gradient {
        GradientStop { position: 0.0; color: "lightgray" }
        GradientStop { position: 1.0; color: "gray" }
    }
    Column {
        id: c
        y: 5
        x: 5
        width: parent.width - 10
        clip: true
        Text { text: '<b>Name: </b> ' + place.name; font.pixelSize: 16 }
        Text { text: '<b>Street: </b> ' + place.location.address.street; font.pixelSize: 16 }
        Text { text: '<b>Latitude: </b> ' + place.location.coordinate.latitude; font.pixelSize: 16 }
        Text { text: '<b>Longitude: </b> ' + place.location.coordinate.longitude; font.pixelSize: 16 }
        Text { text: '<b>Categories: </b> ' + categoryNames(place.categories); font.pixelSize: 16 }
        Text { text: '<b>Media count: </b> ' + place.media.data.length; font.pixelSize: 16 }
        //Text { text: '<b>All media count: </b> ' + place.mediaCount; font.pixelSize: 16 }
        Text { text: '<b>Descriptions count: </b> ' + place.descriptions.length; font.pixelSize: 16 }
        //Text { text: '<b>Reviews count: </b> ' + place.reviews.data.length; font.pixelSize: 16 }
        //Text { text: '<b>All reviews count: </b> ' + place.reviewCount; font.pixelSize: 16 }
        Text { text: '<b>Tags: </b> ' + place.tags; font.pixelSize: 16 }
        //Text { text: '<b>Suppliers: </b> ' + JSON.stringify(place.suppliers); font.pixelSize: 16 }
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
}
