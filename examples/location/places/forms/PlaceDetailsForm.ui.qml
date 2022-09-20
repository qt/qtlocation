/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
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
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
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

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../views"

Item {
    id: root
    property alias placeName: placeName
    property alias placeIcon: placeIcon
    property alias distance: distance
    property alias address: address
    property alias categories: categories
    property alias phone: phone
    property alias fax: fax
    property alias email: email
    property alias website: website
    property alias addInformation: addInformation
    property alias editorialsButton: editorialsButton
    property alias reviewsButton: reviewsButton
    property alias imagesButton: imagesButton
    property alias findSimilarButton: findSimilarButton
    property alias ratingView: ratingView

    ScrollView {
        id:scrollView
        anchors.fill: parent
        anchors.margins: 15

        GridLayout {
            width: scrollView.width - 15
            rows: 7
            columns: 2

            RowLayout {
                Layout.columnSpan: 2
                Layout.fillWidth: true

                Image {
                    id: placeIcon
                    source: Qt.resolvedUrl("../resources/marker.png")
                    anchors.margins: 30
                }

                Label {
                    id: placeName
                    text: qsTr("PlaceName")
                    font.bold: true
                }

                Item {
                    Layout.fillWidth: true
                }
            }

            RatingView {
                id: ratingView
                size: placeName.height * 2
                Layout.columnSpan: 2
            }

            Rectangle {
                Layout.columnSpan: 2
                height: 1
                color: "#46a2da"
                visible: addressBox.visible
            }

            GroupBox {
                id: addressBox
                Layout.fillWidth: true
                Layout.columnSpan: 2
                title: qsTr("Address")

                GridLayout {
                    id: gridLayout3
                    rowSpacing: 10
                    rows: 1
                    columns: 2
                    anchors.fill: parent

                    Label {
                        text: qsTr("Distance:")
                    }

                    Label {
                        id: distance
                        Layout.fillWidth: true
                        text: qsTr("1000 km")
                    }

                    Label {
                        id: address
                        Layout.columnSpan: 2
                        text: qsTr("Street Number<br/>xxxxx City<br/>Country")
                    }
                }
            }

            Rectangle {
                Layout.columnSpan: 2
                height: 1
                color: "#46a2da"
                visible: categoriesBox.visible
            }

            GroupBox {
                id: categoriesBox
                Layout.fillWidth: true
                Layout.columnSpan: 2
                title: qsTr("Categories")

                Label {
                    id: categories
                    anchors.fill: parent
                    text: qsTr("category1, category2 ,category3")
                }
            }

            Rectangle {
                height: 1
                color: "#46a2da"
                visible: contactDetailsBox.visible
            }

            GroupBox {
                id: contactDetailsBox
                Layout.fillWidth: true
                Layout.columnSpan: 2
                title: qsTr("Contact details")
                GridLayout {
                    id: gridLayout4
                    rowSpacing: 10
                    rows: 1
                    columns: 2
                    anchors.fill: parent

                    Label {
                        text: qsTr("Phone: ")
                    }

                    Label {
                        id: phone
                        Layout.fillWidth: true
                        text: qsTr("000-000-000")
                    }

                    Label {
                        text: qsTr("Fax: ")
                    }

                    Label {
                        id: fax
                        Layout.fillWidth: true
                        text: qsTr("000-000-000")
                    }

                    Label {
                        text: qsTr("Email: ")
                    }

                    Label {
                        id: email
                        Layout.fillWidth: true
                        text: qsTr("name@company.com")
                    }

                    Label {
                        text: qsTr("Website: ")
                    }

                    Label {
                        id: website
                        Layout.fillWidth: true
                        text: qsTr("http:://company.com")
                    }
                }
            }

            Rectangle {
                Layout.columnSpan: 2
                height: 1
                color: "#46a2da"
                visible: informationBox.visible
            }

            GroupBox {
                id: informationBox
                Layout.fillWidth: true
                Layout.columnSpan: 2
                title: qsTr("Additional information")
                ColumnLayout {
                    Label {
                        id: addInformation
                        text: qsTr("AdditionalInformation1<br/>AdditionalInformation2<br/>AdditionalInformation3")
                    }
                }
            }

            RowLayout {
                Layout.columnSpan: 2
                Layout.alignment: Qt.AlignHCenter

                Button {
                    id: editorialsButton
                    text: qsTr("Editorials")
                    enabled: false
                }

                Button {
                    id: reviewsButton
                    text: qsTr("Reviews")
                    enabled: false
                }

                Button {
                    id: imagesButton
                    text: qsTr("Images")
                    enabled: false
                }

                Button {
                    id: findSimilarButton
                    text: qsTr("Find similar")
                    enabled: false
                }
            }
        }
    }
}

