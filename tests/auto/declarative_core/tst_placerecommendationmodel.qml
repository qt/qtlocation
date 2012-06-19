/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import QtTest 1.0
import QtLocation 5.0

TestCase {
    id: testCase

    name: "PlaceRecommendationModel"

    PlaceRecommendationModel {
        id: testModel
    }

    Plugin {
        id: testPlugin
        name: "qmlgeo.test.plugin"
    }

    GeoCircle {
        id: testSearchArea
        center: Coordinate {
            latitude: 10
            longitude: 20
        }
        radius: 5000
    }

    function test_setAndGet_data() {
        var myNull = null;
        return [
            { tag: "plugin", property: "plugin", signal: "pluginChanged", value: testPlugin },
            { tag: "searchArea", property: "searchArea", signal: "searchAreaChanged", value: testSearchArea },
            { tag: "offset", property: "offset", signal: "offsetChanged", value: 10, reset: 0 },
            { tag: "limit", property: "limit", signal: "limitChanged", value: 10, reset: -1 },

            { tag: "placeId", property: "placeId", signal: "placeIdChanged", value: "test-place-id-1", reset: "" },
        ];
    }

    function test_setAndGet(data) {
        var signalSpy = Qt.createQmlObject('import QtTest 1.0; SignalSpy {}', testCase, "SignalSpy");
        signalSpy.target = testModel;
        signalSpy.signalName = data.signal;

        // set property to something new
        testModel[data.property] = data.value;
        compare(testModel[data.property], data.value);
        compare(signalSpy.count, 1);

        // set property to the same value (signal spy should not increase)
        testModel[data.property] = data.value;
        compare(testModel[data.property], data.value);
        compare(signalSpy.count, 1);

        // reset property
        if (data.reset === undefined) {
            testModel[data.property] = null;
            compare(testModel[data.property], null);
        } else {
            testModel[data.property] = data.reset;
            compare(testModel[data.property], data.reset);
        }
        compare(signalSpy.count, 2);

        signalSpy.destroy();
    }
}
