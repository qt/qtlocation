import QtQuick 2.0
import Qt.location 5.0

Rectangle {
    property Category category_

    width: parent.width
    height: c.height + 10

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
        Text { text: '<b>name:</b> ' + category.name; font.pixelSize:16 }
        Text { text: '<b>id:</b> ' + category.categoryId; font.pixelSize: 16 }
    }
}
