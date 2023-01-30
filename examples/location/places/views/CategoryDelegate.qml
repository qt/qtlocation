// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Controls
import QtLocation

Item {
    id: root

    property alias text: labelItem.text
    property bool checked: false
    signal searchCategory()
    signal showSubcategories()

    height: Math.max(icon.height, labelItem.height * 2)

    Image {
        id: icon
        anchors.left: parent.left
        anchors.leftMargin: 30
        anchors.verticalCenter: parent.verticalCenter
        source: category.icon.url()
    }

    Rectangle {
        anchors.fill: parent
        color: "#44ffffff"
        visible: mouse.pressed
    }

    //! [CategoryModel delegate text]
    Label {
        id: labelItem
        text: category.name
        anchors.left: icon.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: arrow.left
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        onClicked: root.searchCategory()
    }
    //! [CategoryModel delegate text]

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 15
        height: 1
        color: "#46a2da"
    }
    //! [CategoryModel delegate arrow]
    ToolButton {
        id: arrow
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.rightMargin: 15
        visible: model.hasModelChildren
        icon.source: Qt.resolvedUrl("../resources/right.png")
        onClicked: root.showSubcategories()
    }
    //! [CategoryModel delegate arrow]
}
