// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

//![0]
import QtQuick 2.0
import QtPositioning 5.2

Rectangle {
        id: page
        width: 350
        height: 350
        PositionSource {
            id: positionSource
            updateInterval: 1000
            active: true
            // nmeaSource: "nmealog.txt"
        }
        Column {
            Text {text: "<==== PositionSource ====>"}
            Text {text: "preferredPositioningMethods: "  + printableMethod(positionSource.preferredPositioningMethods)}
            Text {text: "supportedPositioningMethods: "  + printableMethod(positionSource.supportedPositioningMethods)}
            Text {text: "nmeaSource: "         + positionSource.nmeaSource}
            Text {text: "updateInterval: "     + positionSource.updateInterval}
            Text {text: "active: "     + positionSource.active}
            Text {text: "<==== Position ====>"}
            Text {text: "latitude: "   + positionSource.position.coordinate.latitude}
            Text {text: "longitude: "   + positionSource.position.coordinate.longitude}
            Text {text: "altitude: "   + positionSource.position.coordinate.altitude}
            Text {text: "speed: " + positionSource.position.speed}
            Text {text: "timestamp: "  + positionSource.position.timestamp}
            Text {text: "altitudeValid: "  + positionSource.position.altitudeValid}
            Text {text: "longitudeValid: "  + positionSource.position.longitudeValid}
            Text {text: "latitudeValid: "  + positionSource.position.latitudeValid}
            Text {text: "speedValid: "     + positionSource.position.speedValid}
        }
        function printableMethod(method) {
            if (method == PositionSource.SatellitePositioningMethods)
                return "Satellite";
            else if (method == PositionSource.NoPositioningMethods)
                return "Not available"
            else if (method == PositionSource.NonSatellitePositioningMethods)
                return "Non-satellite"
            else if (method == PositionSource.AllPositioningMethods)
                return "All/multiple"
            return "source error";
        }
    }
//![0]
