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

Rectangle {
    id: fullView
    width: 800
    height: 360

    Text {
        id: queryText
        text: "Query:"
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.top: parent.top
        anchors.topMargin: 10
        font.pixelSize: 18
    }
    Rectangle {
        id: searchTermRect
        color: "#ffffff"
        anchors.right: searchButton.left
        anchors.rightMargin: 5
        anchors.left: queryText.right
        anchors.leftMargin: 5
        anchors.top: parent.top
        anchors.topMargin: 10
        height: search_term.height
        border.color: "#23b98a"

        TextInput {
            id: search_term
            text: "p"
            anchors.fill: parent
            clip: false
            opacity: 1
            font.pixelSize: 18

            onAccepted: searchTerm(text)
            onTextChanged: {
                resultSuggestion.searchTerm = text;
                resultSuggestion.execute();
                suggestions.visible = true;
            }
        }
    }

    Button {
        id: searchButton

        text: "Search"

        anchors.bottom: searchTermRect.bottom
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.top: searchTermRect.top

        onClicked: searchTerm(search_term.text)
    }

    ListView {
        id: placesList
        anchors.rightMargin: 10
        anchors.leftMargin: 10
        anchors.bottomMargin: 10
        anchors.top: queryText.bottom
        anchors.right: categoriesList.left
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.topMargin: 10
        clip: true
        visible: !reviewList.visible

        snapMode: ListView.SnapToItem
        model: searchModel
        delegate: SearchResultDelegate { }
    }

    ListView {
        id: reviewList
        anchors.fill: placesList
        clip: true
        snapMode: ListView.SnapToItem

        visible: model !== undefined
        delegate: ReviewDelegate { }
    }

    ListView {
        id: editorialList
        anchors.fill: placesList
        clip: true
        snapMode: ListView.SnapToItem

        visible: model !== undefined
        delegate: EditorialDelegate { }
    }

    Text {
        id: categoriesText
        text: "Categories tree:"
        anchors.top: queryText.bottom
        anchors.topMargin: 10
        anchors.left: categoriesList.left
        font.pixelSize: 18
    }

    CategoryView {
        id: categoriesList

        width: 300
        anchors.topMargin: 10
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.top: categoriesText.bottom
        anchors.bottomMargin: 10
        anchors.rightMargin: 10
        model: categoryModel

        onCategorySelected: searchCategory(category);
    }

    Rectangle {
        id: suggestions
        anchors.left: searchTermRect.left
        anchors.right: searchTermRect.right
        anchors.top: searchTermRect.bottom
        height: Math.min(4, resultSuggestion.suggestions.length) * 19
        visible: false
        color: "#f0f0f0"
        clip: true
        ListView {
            id: suggestionsList
            anchors.fill: parent
            model: resultSuggestion
            delegate: Component {
                Text {
                    width: parent.width
                    text: suggestion
                    font.pixelSize: 16

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            search_term.text = suggestion;
                            searchTerm(search_term.text);
                        }
                    }
                }
            }
        }
    }

    Plugin {
        id: geoServices
        name: "nokia"
    }

    PlaceSearchModel {
        id: searchModel
        plugin: geoServices

        searchArea: BoundingCircle {
            id: proximity
            center: Coordinate {
                id: searchCoordinate
                // Paris
                longitude: 2.296
                latitude: 48.87
            }
            radius:5000
        }
        maximumCorrections: 5
    }

    function searchTerm(term)
    {
        searchModel.categories = null;
        searchModel.searchTerm = term;
        searchModel.execute();
        placesList.model = searchModel;
        suggestions.visible = false;
    }

    function searchCategory(category)
    {
        searchModel.searchTerm = "";

        var categories = new Array();
        categories.push(category);
        searchModel.categories = categories;

        searchModel.execute();
        placesList.model = searchModel;
        suggestions.visible = false;
    }

    PlaceSearchSuggestionModel {
        id: resultSuggestion
        plugin: geoServices

        searchArea: proximity
        offset: 0
        limit: 15
        searchTerm: search_term.text
    }

    PlaceRecommendationModel {
        id: recommendationModel
        plugin: geoServices

        searchArea: proximity
        offset: 0
        limit: 15
        //onQueryFinished: console.log("datareceived " + error)
    }

    CategoryModel {
        id: categoryModel
        plugin: geoServices
        hierarchical: true
    }

    ImageGrid {
        id: imageGrid

        anchors.fill: parent
        visible: imageGrid.model !== undefined
    }
}
