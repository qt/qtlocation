import QtQuick 2.0
import Qt.location 5.0

Item {
    id: root
    property variant model

    signal categorySelected(variant category)

    ListView {
        id: categoriesList

        anchors.top: parent.top
        anchors.bottom: toolbar.top
        anchors.bottomMargin: 5
        anchors.left: parent.left
        anchors.right: parent.right

        model: VisualDataModel {
            id: categoryListModel
            model: root.model
            delegate: CategoryDelegate {
                category: model.category
                hasSubCategories: model.hasModelChildren

                onClicked: categorySelected(category)
                onDisplaySubCategories: categoryListModel.rootIndex = categoryListModel.modelIndex(index)
            }
        }
        clip: true
        snapMode: ListView.SnapToItem
    }

    Row {
        id: toolbar

        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        height: root.model.hierarchical ? childrenRect.height : 0
        visible: root.model.hierarchical

        Button {
            text: qsTr("Back")
            onClicked: categoryListModel.rootIndex = categoryListModel.parentModelIndex()
        }
    }
}
