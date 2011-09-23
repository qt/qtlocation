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
import Qt.location 5.0

Rectangle {
    property Place place

    height: c.height + 10
    width: parent.width

    radius: 10
    gradient: Gradient {
        GradientStop { position: 0.0; color: "lightgray" }
        GradientStop { position: 1.0; color: "gray" }
    }
    Column {
        id: c
        y: 5
        x: 5
        width: parent.width - 10
        clip: true
        Text { text: '<b>Name: </b> ' + place.name; font.pixelSize: 16 }
        Text { text: '<b>Street: </b> ' + place.location.address.street; font.pixelSize: 16 }
        Text { text: '<b>Latitude: </b> ' + place.location.coordinate.latitude; font.pixelSize: 16 }
        Text { text: '<b>Longitude: </b> ' + place.location.coordinate.longitude; font.pixelSize: 16 }
        Text { text: '<b>Categories: </b> ' + categoryNames(place.categories); font.pixelSize: 16 }
        Text { text: '<b>Media count: </b> ' + place.mediaModel.totalCount; font.pixelSize: 16 }
        Text { text: '<b>Descriptions count: </b> ' + place.descriptions.length; font.pixelSize: 16 }
        Text { text: '<b>All reviews count: </b> ' + place.reviewModel.totalCount; font.pixelSize: 16 }
        Text { text: '<b>Tags: </b> ' + place.tags; font.pixelSize: 16 }
        Text { text: '<b>Suppliers: </b> ' + JSON.stringify(place.suppliers); font.pixelSize: 16 }
        Text { text: '<b>Short Desc: </b> ' + place.shortDescription; font.pixelSize: 16 }
        Text { text: '<b>Tags: </b> ' + place.tags.join(", "); font.pixelSize: 16 }
    }

    function categoryNames(categories) {
        var result = "";

        for (var i = 0; i < categories.length; ++i) {
            if (result == "") {
                result = categories[i].name;
            } else {
                result = result + ", " + categories[i].name;
            }
        }

        return result;
    }
}
