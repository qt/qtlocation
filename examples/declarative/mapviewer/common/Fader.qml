import Qt 4.7

Rectangle {
    id: fader
    anchors.fill: parent
    opacity: 0.7
    color:  "darkgrey"
    MouseArea {
        id: mouseArea
        anchors.fill: parent
    }

    Component.onCompleted: {
        var globalXY
        var root = parent

        if (root != null) {
            while (root.parent) root = root.parent
            width = root.width; height = root.height;
            globalXY = mapFromItem(root, root.x, root.y);
        } else {
            globalXY = mapFromItem(null, 0, 0);
        }
        x = globalXY.x;
        y = globalXY.y;
    }
}
