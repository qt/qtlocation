/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import "../components"

Item {
    id: dialog
    signal goButtonClicked
    signal cancelButtonClicked
    signal clearButtonClicked

    anchors.fill: parent

    property alias title: titleBar.text
    property alias dialogModel: dialogModel
    property alias length: dialogModel.count
    property int gap: 10
    property int listItemHeight: titleBar.font.pixelSize * 1.5
    property alias customLoader: componentLoader
    property bool showButtons: true

    opacity: 0

    function setModel(objects)
    {
        dialogModel.clear()

        for (var i=0; i< objects.length; i++){
            dialogModel.append({"labelText": objects[i][0], "inputText": objects[i][1]})
        }
    }

    Fader {}

    Rectangle {
        id: dialogRectangle

        color: "#ECECEC"
        opacity: parent.opacity
        width: parent.width - gap;
        height: dataRect.height + titleBar.height + buttons.height + gap*1.5

        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: gap/2
        }

        radius: 5

        TitleBar {
            id: titleBar;
            width: parent.width; height: 40;
            anchors.top: parent.top; anchors.left: parent.left;
            opacity: 0.9
            onClicked: { dialog.cancelButtonClicked() }
        }

        ListModel {
            id: dialogModel
        }

        Component{
            id: listDelegate

            TextWithLabel {
                id: textWithLabel
                label: labelText
                text: inputText
                width: dataRect.width - gap
                labelWidth: 95

                onTextChanged:
                {
                    dialogModel.set(index, {"inputText": text})
                }
            }
        }

    Rectangle {
        id: dataRect
        color: "#ECECEC"
        radius: 5
        width:dialogRectangle.width - gap
        height: childrenRect.height + gap
        anchors {
            top: titleBar.bottom
            topMargin: gap/2
            left: parent.left
            leftMargin: gap/2
        }

        Loader {
            id: componentLoader;
            anchors {
                top: dataRect.top
                topMargin: gap/2
                left: parent.left
                leftMargin: gap/2
                right: parent.right
                rightMargin: gap/2
            }

            Component.onCompleted: {
                if (!sourceComponent)
                    sourceComponent = listComponent;
            }

        }

        Component {
            id: listComponent

            ListView {
                id: listview
                model: dialogModel
                delegate: listDelegate
                spacing: gap/2
                clip: true
                snapMode: ListView.SnapToItem
                interactive: height < (listItemHeight + gap/2)*length + gap/2
                width: parent.width
                height: Math.min(dialog.height * 0.7, (listItemHeight + gap/2)*length + gap/2)

                Connections {
                    target: dialog
                    onClearButtonClicked: {
                        for (var i = 0; i<length; i++)
                            dialogModel.set(i, {"inputText": ""})
                    }
                }
            }
        }
    }

        Row {
            id: buttons
            anchors.top: dataRect.bottom
            anchors.topMargin: gap/2
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: gap/3
            height: 32
            visible: showButtons
            Button {
                id: buttonClearAll
                text: "Clear"
                width: 80; height: parent.height
                onClicked: dialog.clearButtonClicked()
            }
            Button {
                id: buttonGo
                text: "Go!"
                width: 80; height: parent.height
                onClicked: {
                    dialog.goButtonClicked ()
                }
            }
        }
    }
}
