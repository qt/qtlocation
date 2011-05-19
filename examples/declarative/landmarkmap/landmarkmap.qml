/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
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

import Qt 4.7
import QtMobility.location 1.2
import "landmarkmapcommon" as Common
import "landmarkmapmobile" as Mobile

Item {
    id: page
    width: 320
    height: 480
    focus: true

    PositionSource {
        id: myPositionSource
        active: true
        updateInterval: 2000
        onPositionChanged: console.log("Position changed in PositionSource")
    }
    //![Map toCoordinate]
    LandmarkBoxFilter {
        id: boxFilter
        topLeft: map.toCoordinate(Qt.point(0,0))
        bottomRight: map.toCoordinate(Qt.point(map.width, map.height))
    }
    //![Map toCoordinate]

     LandmarkProximityFilter {
         id: proximityFilter
         center: myPositionSource.position.coordinate
         radius: 500000
     }

    LandmarkModel {
        id: landmarkModelAll
        autoUpdate: true
        filter: boxFilter
        limit: 20
        onModelChanged: {
            console.log("All landmark model changed, landmark count: " + count)
        }
    }

    LandmarkModel {
        id: landmarkModelNear
        autoUpdate: true
        filter: proximityFilter
        limit: 20
        onModelChanged: {
            console.log("Near landmark model changed, landmark count: " + count)
        }
    }

    Component {
        id: landmarkListDelegate
        Item {
            width: 200; height: 80
            Text {
                color: "white"; font.bold: true; style: Text.Raised; styleColor: "black"
                id: nameField; text: landmark.name
            }
            Text {
                color: "white"; font.bold: true; style: Text.Raised; styleColor: "black"
                anchors.top:  nameField.bottom
                id: latField; text: "    Lat: " + landmark.coordinate.latitude
            }
            Text {
                color: "white"; font.bold: true; style: Text.Raised; styleColor: "black"
                anchors.top:  latField.bottom
                id: lonField; text: "    Lon: " + landmark.coordinate.longitude
            }
        }
    }

    //![Category model]
    LandmarkCategoryModel {
        id: landmarkCategoryModel
        autoUpdate: true
        onModelChanged: console.log("Category model changed, category count: " + count)
    }

    Coordinate {
        id: initCoordinate
        latitude: -27.5
        longitude: 153
    }

    Component {
        id: landmarkCategoryListDelegate
        Item {
            width: 200; height: 50
            Text {
                color: "white"; font.bold: true; style: Text.Raised; styleColor: "black"
                id: nameField; text: category.name
            }
        }
    }
    //![Category model]

    Mobile.TitleBar { id: titleBar; z: 5; width: parent.width - statusBar.width; height: 40; opacity: 0.8 }
    Mobile.StatusBar { id: statusBar; z: 6; width: 80; height: titleBar.height; opacity: titleBar.opacity; anchors.right: parent.right}

    Rectangle {
        id: dataArea
        anchors.top: titleBar.bottom
        anchors.bottom: toolbar2.bottom
        width: parent.width
        color: "#343434"
        //height: toolbar1.y - titleBar.height
        Image { source: "landmarkmapmobile/images/stripes.png"; fillMode: Image.Tile; anchors.fill: parent; opacity: 0.3 }

        ListView {
            id: landmarkListView;
            model: landmarkModelAll;
            delegate: landmarkListDelegate;
            width: parent.width - 20;
            height: parent.height -20;
            x: -(parent.width * 1.5);
            highlightFollowsCurrentItem: false
            focus: true
        }

        //![Category model]
        ListView {
            id: landmarkCategoryListView;
            model: landmarkCategoryModel;
            delegate: landmarkCategoryListDelegate;
            width: parent.width - 20;
            height: parent.height -20;
            x: -(parent.width * 1.5);
            highlightFollowsCurrentItem: false
            focus: true
        }
        //![Category model]
        //![Map Plugin]
        Map {
            id: map
            plugin : Plugin { name : "nokia"}
            //![Map Plugin]
            anchors.fill: parent
            size.width: parent.width
            size.height: parent.height
            zoomLevel:9
            center: initCoordinate

            MapObjectView {
                id: allLandmarks
                model: landmarkModelAll
                delegate: Component {
                    //![MapImage]
                    MapImage {
                        source:  "landmarkmapmobile/images/landmarkstar.png"
                        coordinate: landmark.coordinate
                    }
                    //![MapImage]
                }
            }

            MapObjectView {
                id: landmarksNearMe
                model: landmarkModelNear
                delegate: Component {
                    //![MapGroup]
                    MapGroup {
                        MapCircle {
                            color: "green"
                            radius: 100
                            center: landmark.coordinate
                        }
                        MapCircle {
                            color: "red"
                            radius: 30
                            center: landmark.coordinate
                        }
                    }
                    //![MapGroup]
                }
            }
            onZoomLevelChanged: {
                console.log("Zoom changed")
                updateFilters();
            }
            onCenterChanged: {
                console.log("Center changed")
                updateFilters();
            }
            function updateFilters () {
                // The pixel coordinates used are the map's pixel coordinates, not
                // the absolute pixel coordinates of the application. Hence e.g. (0,0) is always
                // the top left point
                var topLeftCoordinate = map.toCoordinate(Qt.point(0,0))
                var bottomRightCoordinate = map.toCoordinate(Qt.point(map.size.width, map.size.height))
                boxFilter.topLeft = topLeftCoordinate;
                boxFilter.bottomRight = bottomRightCoordinate;
            }
        } // map

        MouseArea {
            anchors.fill: parent

            property bool mouseDown : false
            property int lastX : -1
            property int lastY : -1

            onPressed : {
                mouseDown = true
                // While panning, its better not to actively udpate the model
                // as it results in poor performance.
                landmarkModelAll.autoUpdate = false
                landmarkModelNear.autoUpdate = false
                lastX = mouse.x
                lastY = mouse.y
            }
            onReleased : {
                mouseDown = false
                landmarkModelAll.autoUpdate = true
                landmarkModelNear.autoUpdate = true
                landmarkModelAll.update()
                landmarkModelNear.update()
                lastX = -1
                lastY = -1
            }
            onPositionChanged: {
                if (mouseDown) {
                    var dx = mouse.x - lastX
                    var dy = mouse.y - lastY
                    map.pan(-dx, -dy)
                    page.state = "NoFollowing"
                    myPositionSource.active = false
                    lastX = mouse.x
                    lastY = mouse.y
                }
            }
            onDoubleClicked: {
                page.state = "NoFollowing"
                myPositionSource.active = false
                map.center = map.toCoordinate(Qt.point(mouse.x, mouse.y))
                if (map.zoomLevel < map.maximumZoomLevel)
                    map.zoomLevel += 1
            }
        } // MouseArea



        Mobile.Floater {
            id : dataFloater
            latitude: myPositionSource.position.coordinate.latitude
            longitude:  myPositionSource.position.coordinate.longitude
            landmarks: landmarkModelAll.count
            categories: landmarkCategoryModel.count
        }
    } // dataArea

    Item {
        id: sliderContainer
        anchors {bottom: toolbar1.top;}
        height:  40
        width: parent.width

        Common.Slider {
            id: zoomSlider;
            minimum: 1; maximum: 18;
            anchors {
                fill: parent
                bottomMargin: 5; rightMargin: 5; leftMargin: 5
            }
            onValueChanged: {
                map.zoomLevel = value
            }
        }
    }

    Mobile.ToolBar {
        id: toolbar1
        opacity: titleBar.opacity
        height: 40; width: parent.width
        anchors.bottom: toolbar2.top
        z: 6
        button1Label: "map"; button2Label: "landmarks"; button3Label: "categories"
        onButton1Clicked: {
            if (page.state != "")
                page.state = ""
        }
        onButton2Clicked: {
            if (page.state != "LandmarkView")
                page.state = "LandmarkView"
        }
        onButton3Clicked: {
            if (page.state != "CategoryView")
                page.state = "CategoryView"
        }
    }

    Mobile.ToolBar {
        id: toolbar2
        opacity: titleBar.opacity
        height: 40; width: parent.width
        anchors.bottom: parent.bottom
        z: 6
        button1Label: "nmealog.txt"; button2Label: "mylm.lmx"; button3Label: "follow me"
        button3FontColor: "pink"
        onButton1Clicked: {
            myPositionSource.nmeaSource = "nmealog.txt"
            myPositionSource.start()
        }
        onButton2Clicked: {
            landmarkModelAll.importFile = "mylm.lmx"
            toolbar2.disableButton2()
        }
        onButton3Clicked: {
            page.state = "Following"
            myPositionSource.active = true
        }
    }
    // states of page
    states: [State {
            name: "CategoryView"
            PropertyChanges { target: map; x: -dataArea.width }
            PropertyChanges { target: map; x: -dataArea.width }
            PropertyChanges { target: sliderContainer; x: -dataArea.width}
            PropertyChanges { target: landmarkCategoryListView; x: 0 }
        }, State {
            name: "LandmarkView"
            PropertyChanges { target: map; x: -dataArea.width }
            PropertyChanges { target: sliderContainer; x: -dataArea.width}
            PropertyChanges { target: landmarkListView; x: 0 }
        }, State {
            name:  "Following"
            PropertyChanges { target:  map; center: myPositionSource.position.coordinate}
            PropertyChanges { target: toolbar2; button3FontColor: 'grey'}
            PropertyChanges { target: toolbar2; button3Label: '(following)'}
            PropertyChanges { target: myPositionSource; active: true}
        }, State {
            name : "NoFollowing"
            PropertyChanges { target: toolbar2; button3FontColor: 'pink'}
            PropertyChanges { target: toolbar2; button3Label: 'follow me'}
            PropertyChanges { target: myPositionSource; active: false}
        }]
    // state-transition animations for page
    transitions: Transition {
        NumberAnimation { properties: "x"; duration: 500; easing.type: Easing.InOutQuad }
    }
} // page
