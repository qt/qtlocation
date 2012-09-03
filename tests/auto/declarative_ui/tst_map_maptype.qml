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

    name: "MapType"

    Plugin {
        id: nokiaPlugin
        name: "nokia"
        parameters: [
                       PluginParameter {
                           name: "app_id"
                           value: "stub"
                       },
                       PluginParameter {
                           name: "token"
                           value: "stub"
                       }
                   ]
    }

    Map {
        id: map;
        plugin: nokiaPlugin
        center {
            latitude: 62.240501
            longitude: 25.757014
        }
        width: 100
        height: 100
    }

    SignalSpy {id: supportedSetSpy; target: map; signalName: "supportedMapTypesChanged"}
    SignalSpy {id: activeMapTypeChangedSpy; target: map; signalName: "activeMapTypeChanged"}

    function initTestCase() {
        if (map.supportedMapTypes.length == 0 && supportedSetSpy.count == 0) {
            wait(1000)
            if (supportedSetSpy.count == 0)
                wait(2000)
            compare(supportedSetSpy.count, 1,
                    "supportedMapTypesChanged signal didn't arrive")
        }
    }

    function test_supported_types() {
        var count = map.supportedMapTypes.length
        console.log('Number of supported map types: ' + count)

        console.log('Supported map types:')
        for (var i = 0; i < count; i++) {
            console.log('style: ' + map.supportedMapTypes[i].style
                        + ', name: ' + map.supportedMapTypes[i].name
                        + ', desc: ' + map.supportedMapTypes[i].description
                        + ', mobile: ' + map.supportedMapTypes[i].mobile)
        }
    }

    function test_setting_types() {
        var count = map.supportedMapTypes.length
        console.log('Number of supported map types: '
                    + map.supportedMapTypes.length)

        activeMapTypeChangedSpy.clear();
        for (var i = 0; i < count; i++) {
            console.log('setting ' + map.supportedMapTypes[i].name)
            map.activeMapType = map.supportedMapTypes[i]
            compare(map.supportedMapTypes[i].name, map.activeMapType.name,
                    "Error setting the active maptype (or getting it after)")
        }
        console.log('change count: ' + activeMapTypeChangedSpy.count)
        compare(activeMapTypeChangedSpy.count, count)
    }
}
