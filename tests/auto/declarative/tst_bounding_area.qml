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
    Coordinate{ id: coordinate1; latitude: 1; longitude: 1}
    Coordinate{ id: coordinate2; latitude: 2; longitude: 2}
    Coordinate{ id: coordinate3; latitude: 80; longitude: 80}

    BoundingCircle{ id: emptyCircle}
    BoundingCircle{ id: circle1; center: coordinate1; radius: 200000}

    SignalSpy {id: radiusSpy; target: emptyCircle; signalName: "radiusChanged"}
    SignalSpy {id: centerSpy; target: emptyCircle; signalName: "centerChanged"}

    TestCase {
        name: "Bounding circle"
        function test_circle_defaults_and_setters() {
            compare (emptyCircle.radius, -1)
            compare (circle1.radius, 200000)

            emptyCircle.radius = 200
            compare (radiusSpy.count, 1)
            emptyCircle.radius = 200
            compare (radiusSpy.count, 1)

            emptyCircle.center = coordinate1
            compare (centerSpy.count, 1)
            emptyCircle.center = coordinate1
            compare (centerSpy.count, 1)
            emptyCircle.center = coordinate2
            compare (centerSpy.count, 2)

            emptyCircle.center = coordinate1
            emptyCircle.radius = 200000
            compare(emptyCircle.contains(coordinate1), true)
            compare(emptyCircle.contains(coordinate2), true)
            compare(emptyCircle.contains(coordinate3), false)
        }
    }

    // coordinate unit square
    Coordinate{ id: bottomLeft; latitude: 0; longitude: 0}
    Coordinate{ id: topLeft; latitude: 1; longitude: 0}
    Coordinate{ id: topRight; latitude: 1; longitude: 1}
    Coordinate{ id: bottomRight; latitude: 0; longitude: 1}
    Coordinate{ id: newTopRight; latitude: 3; longitude: 3}

    Coordinate{ id: inside; latitude: 0.5; longitude: 0.5}
    Coordinate{ id: outside; latitude: 2; longitude: 2}

    BoundingBox {
        id: box;
        bottomLeft: bottomLeft;
        topLeft: topLeft;
        topRight: topRight;
        bottomRight: bottomRight}

    // C++ auto test exists for basics of bounding box, testing here
    // only added functionality
    TestCase {
        name: "Bounding box"
        function test_box_defaults_and_setters() {
            compare (box.bottomRight.longitude, bottomRight.longitude) // sanity
            compare (box.contains(bottomLeft), true)
            compare (box.contains(inside), true)
            compare (box.contains(outside), false)
            box.topRight = newTopRight
            compare (box.contains(outside), true)
        }
    }
}
