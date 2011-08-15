import QtQuick 2.0

Row {
    property double rating: 0
    property int size: 32

    Repeater {
        model: Math.ceil(rating)
        Image {
            source: "star.png"
            width: size
            height: size
        }
    }
}
