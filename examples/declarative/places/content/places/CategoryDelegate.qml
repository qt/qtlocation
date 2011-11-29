/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import QtLocation 5.0
import "../components"

Item {
    id: root

    signal clicked
    signal arrowClicked
    signal crossClicked
    signal editClicked

    width: parent.width
    height: childrenRect.height

    Text {
        anchors.left: parent.left
        anchors.right: arrow.left

        text: category.name
        elide: Text.ElideRight

        MouseArea {
            anchors.fill: parent
            onClicked: root.clicked()
        }
    }

    IconButton {
        id: edit

        anchors.right: cross.left
        visible: placesPlugin.supportedPlacesFeatures & Plugin.SaveCategoryFeature

        source: "../resources/pencil.png"
        hoveredSource: "../resources/pencil_hovered.png"
        pressedSource: "../resources/pencil_pressed.png"

        onClicked: root.editClicked()
    }

    IconButton {
        id: cross

        anchors.right: arrow.left
        visible: placesPlugin.supportedPlacesFeatures & Plugin.RemoveCategoryFeature

        source: "../resources/cross.png"
        hoveredSource: "../resources/cross_hovered.png"
        pressedSource: "../resources/cross_pressed.png"

        onClicked: root.crossClicked()
    }

    IconButton {
        id: arrow

        anchors.right: parent.right
        visible: model.hasModelChildren

        source: "../resources/right.png"
        hoveredSource: "../resources/right_hovered.png"
        pressedSource: "../resources/right_pressed.png"

        onClicked: root.arrowClicked()
    }
}
