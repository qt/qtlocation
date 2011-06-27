import QtQuick 1.1
import Qt.location 5.0

Rectangle {
    id: fullView
    width: 640
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
                resultSuggestion.executeQuery();
                suggestions.visible = true;
            }
        }
    }

    Button {
        id: searchButton

        width: 70
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
        snapMode: ListView.SnapToItem
        model: resultModel
        delegate: Component {
            Item {
                id: thisItem
                width: parent.width

                Component.onCompleted: {
                    var object;
                    if (searchResult.type == SearchResult.Place) {
                        object = placeDelegate.createObject(thisItem, { "place": searchResult.place });
                    } else {
                        object = didYouMeanDelegate.createObject(thisItem, { "text": searchResult.didYouMeanSuggestion });
                    }
                    height = object.height;
                }

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        if (searchResult.type == SearchResult.Place) {
                            console.log("clicked " + searchResult.place.name)
                            placeManager.getPlaceDetails(searchResult.place)
                            //placeManager.getPlaceReviews(searchResult.place, 10, 50)
                            //placeManager.getPlaceMedia(searchResult.place)
                        } else if (searchResult.type == SearchResult.DidYouMeanSuggestion) {
                            search_term.text = searchResult.didYouMeanSuggestion;
                            searchTerm(searchResult.didYouMeanSuggestion);
                        }
                    }

                    onPressAndHold: {
                        if (searchResult.type == SearchResult.Place) {
                            console.log("longpress " + searchResult.place.name);
                            placesList.model = recommendationModel;
                            recommendationModel.placeId = searchResult.place.placeId;
                            recommendationModel.executeQuery();
                        }
                    }
                }
            }
        }

        Component {
            id: placeDelegate

            PlaceDelegate { }
        }
        Component {
            id: didYouMeanDelegate
            DidYouMeanDelegate { }
        }

    }

    Text {
        id: categoriesText
        text: "Categories tree:"
        anchors.top: queryText.bottom
        anchors.topMargin: 10
        anchors.left: categoriesList.left
        font.pixelSize: 18
    }

    ListView {
        id: categoriesList
        width: 200
        anchors.topMargin: 10
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.top: categoriesText.bottom
        anchors.bottomMargin: 10
        anchors.rightMargin: 10
        model: categoriesModel
        clip: true
        snapMode: ListView.SnapToItem
        delegate: CategoryDelegate {
            category_: category
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    //console.log("clicked = " + category.categoryId)
                    //console.log("clicked = " + JSON.stringify(modelData))
                    searchCategory(category);
                }
            }
        }
    }

    Rectangle {
        id: suggestions
        anchors.left: searchTermRect.left
        anchors.right: searchTermRect.right
        anchors.top: searchTermRect.bottom
        height: Math.min(4, resultSuggestion.predictions.length) * 19
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
                    text: textPrediction
                    font.pixelSize: 16

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            search_term.text = textPrediction;
                            searchTerm(search_term.text);
                        }
                    }
                }
            }
        }
    }

    PlaceManager {
        id: placeManager
    }

    SearchResultModel {
        id: resultModel
        searchCenter:
                Coordinate {
                    latitude: 53
                    longitude: 10
                }
        didYouMean: 5
        //onQueryFinished: console.log("datareceived " + error)
    }

    function searchTerm(term)
    {
        placesList.model = resultModel;
        resultModel.clearCategories();
        resultModel.searchTerm = term;
        resultModel.executeQuery();
        suggestions.visible = false;
    }

    function searchCategory(category)
    {
        placesList.model = resultModel;
        resultModel.clearSearchTerm();
        resultModel.searchCategory = category;
        resultModel.executeQuery();
        suggestions.visible = false;
    }

    TextPredictionModel {
        id: resultSuggestion
        searchCenter:
            Coordinate {
                latitude: 50.05
                longitude: 19.90
            }
        offset: 0
        limit: 15
        searchTerm: search_term.text
    }

    RecommendationModel {
        id: recommendationModel
        searchCenter:
                Coordinate {
                    latitude: 50.05
                    longitude: 19.90
                }
        offset: 0
        limit: 15
        //onQueryFinished: console.log("datareceived " + error)
    }



    SupportedCategoriesModel {
        id: categoriesModel
    }
}
