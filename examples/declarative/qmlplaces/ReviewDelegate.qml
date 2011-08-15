import QtQuick 2.0
import Qt.location 5.0

Rectangle {
    property Review review

    height: childrenRect.height + 10
    width: parent.width

    radius: 10
    gradient: Gradient {
        GradientStop { position: 0.0; color: "lightgray" }
        GradientStop { position: 1.0; color: "gray" }
    }

    Column {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 5
        y: 5
        spacing: 5

        Text {
            text: review.title
            font.pixelSize: 16
            font.bold: true
            width: parent.width
        }

        RatingView {
            rating: review.rating
            size: 16
        }

        Text {
            text: Qt.formatDate(review.date, "d MMMM yyyy")
        }

        Text {
            text: review.description
            wrapMode: Text.WordWrap
            width: parent.width
        }
    }
}
