/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import QtTest 1.0
import QtLocation 5.0

Item {
    Coordinate { id: empty }
    Coordinate { id: base; longitude: 1.0; latitude: 1.0; altitude: 5.0 }
    Coordinate { id: zero; longitude: 0; latitude: 0 }
    Coordinate { id: plusone; longitude: 1; latitude: 0 }
    Coordinate { id: minusone; longitude: -1; latitude: 0 }
    Coordinate { id: north; longitude: 0; latitude: 3 }

    SignalSpy { id: validitySpy; target: empty; signalName: "validityChanged" }
    SignalSpy { id: longSpy; target: base; signalName: "longitudeChanged" }
    SignalSpy { id: latSpy; target: base; signalName: "latitudeChanged" }
    SignalSpy { id: coordSpy; target: base; signalName: "coordinateChanged" }
    SignalSpy { id: altSpy; target: base; signalName: "altitudeChanged" }

    TestCase {
        name: "Coordinate"

        function test_validity() {
            compare(empty.isValid, false)
            empty.longitude = 0.0
            empty.latitude = 0.0
            compare(empty.isValid, true)
            compare(validitySpy.count, 1)
        }

        function test_accessors() {
            compare(base.longitude, 1.0)
            compare(base.latitude, 1.0)
            compare(base.altitude, 5.0)
            base.longitude = 2.0
            base.latitude = 3.0
            base.altitude = 5.0
            compare(base.longitude, 2.0)
            compare(base.latitude, 3.0)
            compare(base.altitude, 5.0)
            compare(longSpy.count, 1)
            compare(latSpy.count, 1)
            compare(altSpy.count, 0)
            compare(coordSpy.count, 2)
        }

        function test_distance() {
            compare(zero.distanceTo(plusone), zero.distanceTo(minusone))
            compare(2*plusone.distanceTo(zero), plusone.distanceTo(minusone))
            compare(zero.distanceTo(plusone) > 0, true)
        }

        function test_azimuth() {
            compare(zero.azimuthTo(north), 0)
            compare(zero.azimuthTo(plusone), 90)
            compare(zero.azimuthTo(minusone), 270)
            compare(minusone.azimuthTo(plusone), 360 - plusone.azimuthTo(minusone))
        }

        function test_atDistanceAndAzimuth() {
            var coord = zero.atDistanceAndAzimuth(20000, 30)
            compare(coord.longitude > 0, true)
            compare(coord.latitude > 0, true)
            compare(zero.distanceTo(coord), 20000)
            compare(zero.azimuthTo(coord), 30)

            var coord2 = coord.atDistanceAndAzimuth(200, 30)
            compare(zero.distanceTo(coord2), 20200)
        }

    }
}
