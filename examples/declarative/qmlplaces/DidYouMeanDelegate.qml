import QtQuick 2.0

Rectangle {
    property string text

    width: parent.width
    height: t.height + 10

    radius: 10
    gradient: Gradient {
        GradientStop { position: 0.0; color: "lightgray" }
        GradientStop { position: 1.0; color: "gray" }
    }

    Text {
        id: t
        y: 5
        x: 5
        width: parent.width - 10
        text: '<b>Did you mean "' + parent.text + '"?</b>';
        font.pixelSize: 16
    }
}
