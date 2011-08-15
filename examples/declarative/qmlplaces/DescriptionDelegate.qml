import QtQuick 2.0
import Qt.location 5.0

Rectangle {
    property Description description

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

        Text {
            text: '<b>' + description.contentTitle + '</b>'
            font.pixelSize: 16
            width: parent.width
        }
        Text {
            text: description.content
            font.pixelSize: 16
            wrapMode: Text.WordWrap
            width: parent.width
        }
    }
}
