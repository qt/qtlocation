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
import QtLocation.examples 5.0

Item {
    id: dialog

    property int gap: 10
    property int listItemHeight: titleBar.font.pixelSize * 1.5
    property alias isFavoritesEnabled: enableFavoritesButton.selected
    property alias locales: localesInput.text

    signal goButtonClicked
    signal cancelButtonClicked

    opacity: 0
    anchors.fill: parent

    Fader{}

    Rectangle {
        id: dialogRectangle
        color: "#ECECEC"
        opacity:  1
        width: parent.width - gap
        height: titleBar.height + options.height + gap * 1.5

        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: gap/2
        }

        border.width: 1
        border.color: "dodgerblue"
        radius: 5

        TitleBar {
            id: titleBar;
            width: parent.width; height: 40;
            anchors.top: parent.top; anchors.left: parent.left;
            opacity: 0.9
            text: "Options"

            onClicked: { dialog.cancelButtonClicked() }
        }

        Column {
            id: options
            height: childrenRect.height
            width: parent.width
            spacing: gap

            anchors {
                top: titleBar.bottom
                topMargin: gap
                left: dialogRectangle.left
                right: dialogRectangle.right
                leftMargin: gap
                rightMargin: gap
            }

            TextWithLabel {
                id: localesInput

                function resetVisibility() {
                    visible = placesPlugin.supportedPlacesFeatures & Plugin.LocaleFeature
                }

                width: parent.width - gap
                height: listItemHeight
                label: "Locale(s)"
                enabled: true

                Component.onCompleted: {
                    resetVisibility();
                    placesPlugin.nameChanged.connect(resetVisibility);
                }
            }

            Optionbutton {
                id: enableFavoritesButton

                function resetVisibility() {
                    if (placesPlugin.name !== "nokia_places_jsondb") {
                        var pluginNames = placesPlugin.availableServiceProviders;
                        for (var i= 0; i < pluginNames.length; ++i) {
                            if (pluginNames[i] === "nokia_places_jsondb") {
                                enableFavoritesButton.visible = true;
                                return;
                            }
                        }
                    }
                    enableFavoritesButton.visible = false;
                }

                width:  parent.width
                text: "Enable favorites"
                toggle:  true
                visible: false

                Component.onCompleted: {
                    resetVisibility();
                    placesPlugin.nameChanged.connect(resetVisibility);
                }
            }

            Row {
                id: buttons
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: gap/3
                height: 32

                Button {
                    id: cancelButton
                    text: "Cancel"
                    width: 80; height: parent.height

                    onClicked: dialog.cancelButtonClicked()
                }

                Button {
                    id: okButton
                    text: "Ok"
                    width:80; height: parent.height

                    onClicked: dialog.goButtonClicked()
                }
            }
        }
    }
}
