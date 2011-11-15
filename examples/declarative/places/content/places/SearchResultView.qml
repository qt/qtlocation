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

    property variant model

    clip: true

    ListView {
        id: page

        orientation: ListView.Horizontal
        interactive: false
        anchors.fill: parent
        snapMode: ListView.SnapOneItem

        model: VisualItemModel {
            ListView {
                // search results (page 0)
                width: root.width
                height: root.height

                spacing: 5

                model: root.model
                delegate: SearchResultDelegate {
                    onDisplayPlaceDetails: {
                        placeDetails.place = data.place;
                        placeDetails.distance = data.distance;
                        page.currentIndex = 1;
                    }
                }
            }
            Item {
                // place details (page 1)
                width: root.width
                height: root.height

                IconButton {
                    id: placeBackButton

                    source: "../resources/left.png"
                    hoveredSource: "../resources/left_hovered.png"
                    pressedSource: "../resources/left_pressed.png"

                    onClicked: page.currentIndex = 0
                }

                PlaceDelegate {
                    id: placeDetails

                    anchors.top: placeBackButton.bottom
                    anchors.bottom: parent.bottom
                    width: parent.width
                    anchors.margins: 10

                    onShowEditorials: {
                        placeContentList.source = "";
                        placeContentList.place = place;
                        placeContentList.source = "PlaceEditorials.qml";
                        page.currentIndex = 2;
                    }

                    onShowReviews: {
                        placeContentList.source = "";
                        placeContentList.place = place;
                        placeContentList.source = "PlaceReviews.qml";
                        page.currentIndex = 2;
                    }

                    onShowImages: {
                        placeContentList.source = "";
                        placeContentList.place = place;
                        placeContentList.source = "PlaceImages.qml";
                        page.currentIndex = 2;
                    }

                    onSearchForSimilar: {
                        recommendationModel.placeId = place.placeId;
                        recommendationModel.execute();
                        //page.currentIndex = 0;  // this causes a crash?
                    }

                    onEditPlace: {
                        editPlaceDialog.place = place;
                        root.parent.state = "EditPlace"
                    }

                    onDeletePlace: place.remove();
                }
            }
            Item {
                // content list (page 2)
                width: root.width
                height: root.height

                IconButton {
                    id: contentListBackButton

                    source: "../resources/left.png"
                    hoveredSource: "../resources/left_hovered.png"
                    pressedSource: "../resources/left_pressed.png"

                    onClicked: page.currentIndex = 1
                }

                Loader {
                    id: placeContentList

                    anchors.top: contentListBackButton.bottom
                    anchors.bottom: parent.bottom
                    width: parent.width
                    anchors.margins: 10

                    property Place place
                }
            }
            Item {
                // content (page 3)
                width: root.width
                height: root.height

                IconButton {
                    id: contentBackButton

                    source: "../resources/left.png"
                    hoveredSource: "../resources/left_hovered.png"
                    pressedSource: "../resources/left_pressed.png"

                    onClicked: {
                        page.currentIndex = 2;
                        placeContent.source = "";
                        placeContent.data = null;
                    }
                }

                Loader {
                    id: placeContent

                    anchors.top: contentBackButton.bottom
                    anchors.bottom: parent.bottom
                    width: parent.width
                    anchors.margins: 10

                    onLoaded: page.currentIndex = 3

                    property variant data
                }
            }
        }
    }
}
