// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import Qt 4.7

Item {
    id: container
    property string action: "no-op"
    signal clicked(string action)

    width: 100; height:30

    Rectangle {
        id: rectangle
        border.color: "white"
        border.width: 2
        anchors.fill: parent // Fill the whole container
    }
    Text {
        id: text
        text: container.action
        color: "black"
        anchors.left: parent.left
        anchors.leftMargin: 5
        anchors.verticalCenter: parent.verticalCenter
    }
    MouseArea {
        anchors.fill: parent // Whole container is clickable
        onClicked: container.clicked(container.action)
    }
}
