import QtQuick 2.0

Rectangle {
    id: root

    property alias text: buttonText.text

    signal clicked()

    width: buttonText.width + 20
    height: buttonText.height + 6

    border.color: "#2b1ae2"

    MouseArea {
        anchors.fill: parent
        onClicked: root.clicked();
    }

    Text {
        id: buttonText
        color: "#fbf8f8"
        anchors.centerIn: parent
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
