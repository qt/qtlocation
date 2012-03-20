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
import QtLocation 5.0
import QtLocation.examples 5.0

Dialog {
    id: root
    property list<Category> __categories
    property Place __place
    property bool __createdPlace: false

    signal completed;

    function prepareDialog(inputPlace) {
        if (!inputPlace) {
            __place = Qt.createQmlObject('import QtLocation 5.0; Place { }', root);
            __createdPlace = true;
        } else {
            __place  = inputPlace;
            __createdPlace = false;
        }

        setDataFields([
            ["Name", __place ? __place.name : ""],
            ["Street", __place ? __place.location.address.street : ""],
            ["District", __place ? __place.location.address.district : ""],
            ["City", __place ? __place.location.address.city : ""],
            ["County", __place ? __place.location.address.county : ""],
            ["State", __place ? __place.location.address.state : ""],
            ["Country code", __place ? __place.location.address.countryCode : ""],
            ["Country", __place ? __place.location.address.country : ""],
            ["Postal code", __place ? __place.location.address.postalCode : ""],
            ["Latitude", __place ? __place.location.latitude : ""],
            ["Longitude", __place ? __place.location.longitude : ""],
            ["Phone", __place ? __place.primaryPhone : ""],
            ["Fax", __place ? __place.primaryFax : ""],
            ["Email", __place ? __place.primaryEmail : ""],
            ["Website", __place ? __place.primaryWebsite.toString() : ""]
        ]);

        __categories = __place ? __place.categories : new Array()
    }

    function setDataFields(objects)
    {
        dataFieldsModel.clear();
        for (var i = 0; i < objects.length; i++)
            dataFieldsModel.append({"labelText": objects[i][0], "inputText": objects[i][1]})
    }

    function processStatus() {
        if (processStatus.prevStatus == Place.Saving) {
            switch (__place.status) {
            case Place.Ready:
                console.log("Save complete");
                if (__createdPlace) {
                    __place.destroy();
                    __createdPlace  = false;
                    processStatus.prevStatus = null;
                }
                completed();
                break;
            case Place.Error:
                console.log("Save failed:" + __place.errorString());
                errorDialog.text = __place.errorString();
                errorDialog.opacity = 1;
                break;
            }
        }

        processStatus.prevStatus = __place.status;
    }

   Behavior on opacity { NumberAnimation { duration: 500 } }

    ErrorDialog {
        id: errorDialog
        title: "Save Place Failed"
    }

    onGoButtonClicked: {
        if (__place.status == Place.Saving)
            return;

        __place.plugin = placesPlugin;

        __place.name = dataFieldsModel.get(0).inputText;
        __place.location.address.street = dataFieldsModel.get(1).inputText;
        __place.location.address.district = dataFieldsModel.get(2).inputText;
        __place.location.address.city = dataFieldsModel.get(3).inputText;
        __place.location.address.county = dataFieldsModel.get(4).inputText;
        __place.location.address.state = dataFieldsModel.get(5).inputText;
        __place.location.address.countryCode = dataFieldsModel.get(6).inputText;
        __place.location.address.country = dataFieldsModel.get(7).inputText;
        __place.location.address.postalCode = dataFieldsModel.get(8).inputText;

        __place.location.coordinate.latitude = parseFloat(dataFieldsModel.get(9).inputText);
        __place.location.coordinate.longitude = parseFloat(dataFieldsModel.get(10).inputText);

        var phone = Qt.createQmlObject('import QtLocation 5.0; ContactDetail { }', __place);
        phone.label = "Phone";
        phone.value = dataFieldsModel.get(11).inputText;
        __place.contactDetails.phone = phone;

        var fax = Qt.createQmlObject('import QtLocation 5.0; ContactDetail { }', __place);
        fax.label = "Fax";
        fax.value = dataFieldsModel.get(12).inputText;
        __place.contactDetails.fax = fax;

        var email = Qt.createQmlObject('import QtLocation 5.0; ContactDetail { }', __place);
        email.label = "Email";
        email.value = dataFieldsModel.get(13).inputText;
        __place.contactDetails.email = email;

        var website = Qt.createQmlObject('import QtLocation 5.0; ContactDetail { }', __place);
        website.label = "Website";
        website.value = dataFieldsModel.get(14).inputText;
        __place.contactDetails.website = website;

        __place.categories = __categories;
        __place.statusChanged.connect(processStatus);
        __place.save();
    }

    onClearButtonClicked: {
        for (var i = 0; i < dataFieldsModel.count; i++)
            dataFieldsModel.set(i, {"inputText": ""});
        __categories = new Array();
    }

    onCancelButtonClicked: {
        if (__place && __createdPlace)
            __place.destroy();
    }

    onOpacityChanged: {
        if (opacity == 0)
            customLoader.item.currentIndex = 0
    }

    ListModel {
        id: dataFieldsModel
    }

    customLoader.sourceComponent: ListView {
        id: view

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 10

        height: Math.min(root.height * 0.6, flickable.contentHeight);
        spacing: gap/2
        orientation: ListView.Horizontal
        interactive: false
        snapMode: ListView.SnapOneItem

        model: VisualItemModel {
            Item {
                id: firstPage
                width: view.width
                height: view.height

                Flickable {
                    id: flickable
                    anchors.top: parent.top
                    anchors.topMargin: gap/2
                    anchors.bottomMargin: gap/2
                    height: parent.height
                    width: parent.width
                    interactive: height < contentHeight
                    contentWidth: parent.width
                    contentHeight:col.height

                    clip: true

                    Column {
                        id: col
                        anchors.top: parent.top
                        anchors.left: parent.left
                        width: parent.width - gap
                        spacing: gap/2

                        Repeater {
                            id: dataFields
                            model: dataFieldsModel

                            TextWithLabel {
                                anchors.leftMargin: gap/2
                                anchors.rightMargin: gap/2

                                width: parent ? parent.width : 0
                                labelWidth: 95
                                label: labelText
                                text: inputText

                                onTextChanged: dataFieldsModel.set(index, {"inputText": text})
                            }
                        }

                        Group {
                            id: categoriesHeading
                            width: parent.width
                            anchors.topMargin: gap

                            text: qsTr("Categories");
                        }

                        Repeater {
                            model: __categories
                            width: parent.width

                            delegate: Item {
                                height: cross.height
                                width: parent.width

                                Text {
                                    id: categoryName

                                    anchors.left: parent.left
                                    anchors.verticalCenter: parent.verticalCenter
                                    verticalAlignment: Text.AlignVCenter
                                    text: model.modelData.name
                                }

                                IconButton {
                                    id: cross

                                    anchors.top: parent.top
                                    anchors.right: parent.right
                                    source: "../../resources/cross.png"
                                    hoveredSource: "../../resources/cross_hovered.png"
                                    pressedSource: "../../resources/cross_pressed.png"

                                    onClicked: {
                                        var cats = new Array();
                                        for (var i =0; i < __categories.length; ++i) {
                                            if (__categories[i].name != model.modelData.name)
                                                cats.push(__categories[i]);
                                        }
                                        __categories = cats
                                    }
                                }
                            }
                        }

                        Button {
                            text: qsTr("Add Category")
                            onClicked: view.currentIndex = 1
                        }
                    }
                }

                Rectangle {
                    id: scrollbar
                    anchors.right: flickable.right
                    y: {
                        var yPosition = flickable.visibleArea.yPosition
                        if (yPosition < 0)
                            yPosition = 0
                        if (yPosition > (1.0 - flickable.visibleArea.heightRatio))
                            yPosition = (1.0 - flickable.visibleArea.heightRatio)
                        yPosition *flickable.height
                    }

                    width: 10
                    height: flickable.visibleArea.heightRatio * flickable.height
                    color: "gray"
                    radius: 5
                }

            }

            Item {
                height:view.height
                width: view.width

                Group {
                    id: chooseCategoryHeading

                    anchors.top: parent.top
                    width: parent.width
                    text: qsTr("Choose Category");
                }

                CategoryView {
                    id: categoryView

                    anchors.top: chooseCategoryHeading.bottom
                    anchors.bottom: categoryCancel.top
                    width: parent.width

                    showSave: false
                    showRemove: false
                    showChildren: false

                    onCategoryClicked: {
                        var categoriesList = new Array();
                        var alreadyExists = false;
                        for (var i = 0; i < __categories.length; ++i) {
                            categoriesList.push(__categories[i]);
                            if (__categories[i].categoryId == category.categoryId)
                                alreadyExists = true;
                        }

                        if (!alreadyExists)
                            categoriesList.push(category);
                        __categories = categoriesList
                        view.currentIndex = 0;
                    }
                }

                Button {
                    id: categoryCancel

                    anchors.bottom: parent.bottom
                    text: qsTr("Cancel ")
                    onClicked: view.currentIndex = 0
                }
            }
        }
    }
}
