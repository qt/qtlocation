import QtQuick 1.0

Rectangle {
    id: root

    property alias text: buttonText.text

    signal clicked()

    border.color: "#2b1ae2"

    MouseArea {
        anchors.fill: parent
        onClicked: root.clicked();
    }

    Text {
        id: buttonText
        color: "#fbf8f8"
        anchors.fill: parent
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.family: "MS UI Gothic"
        font.pixelSize: 15
    }

    gradient: Gradient {
        GradientStop { position: 0.0; color: "#000000" }
        GradientStop { position: 1.0; color: "#ffffff" }
    }
}
