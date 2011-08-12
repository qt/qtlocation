import QtQuick 2.0
import Qt.location 5.0

Rectangle {
    id: root

    property Category category
    property bool hasSubCategories: false

    signal clicked()
    signal displaySubCategories()

    width: parent.width
    height: c.height + 10

    radius: 10
    gradient: Gradient {
        GradientStop { position: 0.0; color: "lightgray" }
        GradientStop { position: 1.0; color: "gray" }
    }

    Text {
        id: c

        anchors.left: parent.left
        anchors.leftMargin: 5
        anchors.right: line.left
        anchors.rightMargin: 5

        y: 5
        x: 5
        width: parent.width - 10

        text: category.name
        font.pixelSize: 16
        wrapMode: Text.WordWrap

        MouseArea {
            anchors.fill: parent
            onClicked: root.clicked()
        }
    }

    Rectangle {
        id: line

        color: "black"
        width: 1
        y: 2
        height: parent.height - 4
        anchors.right: sub.left
        anchors.rightMargin: 5

        visible: hasSubCategories
    }

    Text {
        id: sub

        anchors.top: c.top
        anchors.bottom: c.bottom
        anchors.right: parent.right
        anchors.rightMargin: 5

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignRight

        text: ">"
        font.pixelSize: 32

        visible: hasSubCategories

        MouseArea {
            anchors.fill: parent
            onClicked: root.displaySubCategories()
        }
    }
}
