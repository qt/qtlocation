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
import Qt.location 5.0

Item {

    Plugin { id: nokiaPlugin; name: "nokia"}
    Plugin { id: invalidPlugin; name: "invalid"}
    Plugin { id: testPlugin;
            name: "qmlgeo.test.plugin"
            parameters: [
                // Parms to guide the test plugin
                PluginParameter { name: "supported"; value: true},
                PluginParameter { name: "finishRequestImmediately"; value: true},
                PluginParameter { name: "validateWellKnownValues"; value: true}
            ]
        }
    SignalSpy {id: invalidGeocodeSpy; target: invalidPlugin; signalName: "supportsGeocodingChanged"}
    SignalSpy {id: invalidReverseGeocodeSpy; target: invalidPlugin; signalName: "supportsReverseGeocodingChanged"}
    SignalSpy {id: invalidRoutingSpy; target: invalidPlugin; signalName: "supportsRoutingChanged"}
    SignalSpy {id: invalidMappingSpy; target: invalidPlugin; signalName: "supportsMappingChanged"}

    TestCase {
        name: "Plugin properties"
        function test_plugin() {
            verify (invalidPlugin.availableServiceProviders.length > 0)
            verify (invalidPlugin.availableServiceProviders.indexOf('qmlgeo.test.plugin') > -1) // at least test plugin must be present
            // invalid plugin
            compare (invalidPlugin.supportsGeocoding, false )
            compare (invalidPlugin.supportsReverseGeocoding, false )
            compare (invalidPlugin.supportsRouting, false )
            compare (invalidPlugin.supportsMapping, false )
            compare (invalidPlugin.supportsPlaces, false )
            // if nokia plugin present
            if (invalidPlugin.availableServiceProviders.indexOf('qmlgeo.test.plugin') > -1) {
                compare (nokiaPlugin.supportsGeocoding, true )
                compare (nokiaPlugin.supportsReverseGeocoding, true )
                compare (nokiaPlugin.supportsRouting, true )
                compare (nokiaPlugin.supportsMapping, true )
                compare (nokiaPlugin.supportsPlaces, true )
            }
            // test plugin does not do mapping
            compare (testPlugin.supportsGeocoding, true )
            compare (testPlugin.supportsReverseGeocoding, true )
            compare (testPlugin.supportsRouting, true )
            compare (testPlugin.supportsMapping, false )
            compare (testPlugin.supportsPlaces, true )
            // test changing name of plugin
            compare (invalidGeocodeSpy.count, 0)
            compare (invalidReverseGeocodeSpy.count, 0)
            compare (invalidRoutingSpy.count, 0)
            compare (invalidMappingSpy.count, 0)
            invalidPlugin.name = 'qmlgeo.test.plugin'
            compare (invalidGeocodeSpy.count, 1)
            compare (invalidReverseGeocodeSpy.count, 1)
            compare (invalidRoutingSpy.count, 1)
            compare (invalidMappingSpy.count, 0)
            compare (invalidPlugin.supportsGeocoding, true )
            compare (invalidPlugin.supportsReverseGeocoding, true )
            compare (invalidPlugin.supportsRouting, true )
            compare (invalidPlugin.supportsMapping, false )
            compare (invalidPlugin.supportsPlaces, true )
            invalidPlugin.name = ''
            compare (invalidGeocodeSpy.count, 2)
            compare (invalidReverseGeocodeSpy.count, 2)
            compare (invalidRoutingSpy.count, 2)
            compare (invalidMappingSpy.count, 0)
            compare (invalidPlugin.supportsGeocoding, false )
            compare (invalidPlugin.supportsReverseGeocoding, false )
            compare (invalidPlugin.supportsRouting, false )
            compare (invalidPlugin.supportsMapping, false )
            compare (invalidPlugin.supportsPlaces, true )
        }
    }
}
