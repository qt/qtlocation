import QtQuick 1.0

Item {
    id: menu
    property int gap: 0
    property int button: 0
    property alias orientation: menuView.orientation
    property alias count: menuModel.count
    property int itemHeight  //to create menu just set menu item height and width, do not set menu's height and width explicitly
    property int itemWidth
//    height: (menuView.orientation == ListView.Horizontal)? itemHeight : itemHeight * count
    width: (menuView.orientation == ListView.Horizontal)? itemWidth * count : itemWidth

    signal clicked

    function setModel(objects)
    {
        for (var i=0; i< objects.length; i++){
            menuModel.append({"label": objects[i], "enabledItem" : true})
        }
        height = (menuView.orientation == ListView.Horizontal)? itemHeight : itemHeight * count
    }

    function disableItem(index){
        menuModel.set(index, {"enabledItem": false})
    }

    function enableItem(index){
        menuModel.set(index, {"enabledItem": true})
    }

    ListModel {
        id: menuModel
    }

    Component{
        id: menuItemDelegate
        Item {
            height: itemHeight
            width: itemWidth

            BorderImage {
                id: menuItemImage;
                source: "../resources/menuItem.sci";
                height: parent.height + 14;
                width: parent.width
                y: -7
            }

            Text {
                id: menuItemText
                text: label;
                elide: Text.ElideLeft
                font.bold: true;
                color: "white"
                style: Text.Raised;
                styleColor: "dimgrey"
                anchors.verticalCenter: parent.verticalCenter
                Component.onCompleted: {
                    if (menuView.orientation == ListView.Horizontal){
                        anchors.horizontalCenter = parent.horizontalCenter
                    }
                    else {
                        anchors.left = parent.left
                        anchors.leftMargin = 10
                    }
                }
            }
            MouseArea {
                id: mouseRegion
                anchors.fill: parent
                hoverEnabled: true
                enabled: enabledItem
                onClicked: {
                    button = index
                    menu.clicked()
                }
            }

            states: [
                State {
                    name: "Pressed"
                    when: mouseRegion.pressed == true
                    PropertyChanges { target: menuItemImage; source: "../resources/menuItem_pressed.png"}
                    PropertyChanges { target: menuItemText; style: Text.Sunken }
                },
                State {
                    name: "Hovered"
                    when: mouseRegion.containsMouse == true
                    PropertyChanges { target: menuItemImage; source: "../resources/menuItem_hovered.png"}
                },
                State {
                    name: "Disabled"
                    when: mouseRegion.enabled == false
                    PropertyChanges { target: menuItemText; color: "grey"}
                }
            ]
        }
    }

    ListView {
        id: menuView
        anchors.fill: parent
        model: menuModel
        delegate: menuItemDelegate
        spacing: gap
        interactive: false
    }
}
