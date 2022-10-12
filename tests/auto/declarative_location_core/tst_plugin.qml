/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick
import QtTest
import QtLocation

Item {

    Plugin { id: unattachedPlugin }
    Plugin { id: osmPlugin; name: "osm"}
    Plugin { id: invalidPlugin; name: "invalid"; allowExperimental: true }
    Plugin { id: testPlugin;
            name: "qmlgeo.test.plugin"
            allowExperimental: true
            parameters: [
                // Parms to guide the test plugin
                PluginParameter { name: "supported"; value: true},
                PluginParameter { name: "finishRequestImmediately"; value: true},
                PluginParameter { name: "validateWellKnownValues"; value: true}
            ]
        }
    SignalSpy {id: invalidAttachedSpy; target: invalidPlugin; signalName: "attached"}

    Plugin {
        id: requiredPlugin
        allowExperimental: true
        required {
            mapping: Plugin.OfflineMappingFeature;
            geocoding: Plugin.OfflineGeocodingFeature;
            places: Plugin.AnyPlacesFeatures;
            navigation: Plugin.AnyNavigationFeatures;
        }
    }

    TestCase {
        name: "Plugin properties"
        function test_plugin() {
            verify (invalidPlugin.availableServiceProviders.length > 0)
            verify (invalidPlugin.availableServiceProviders.indexOf('qmlgeo.test.plugin') > -1) // at least test plugin must be present

            // invalid plugins should have no features
            verify(invalidPlugin.isAttached)
            verify(!(invalidPlugin.supportsMapping()))
            verify(!(invalidPlugin.supportsGeocoding()))
            verify(!(invalidPlugin.supportsRouting()))
            verify(!(invalidPlugin.supportsPlaces()))

            if (invalidPlugin.availableServiceProviders.indexOf('qmlgeo.test.plugin') > -1) {
                verify(testPlugin.isAttached)
                verify(testPlugin.supportsMapping())
                verify(testPlugin.supportsGeocoding())
                verify(testPlugin.supportsPlaces())
                verify(testPlugin.supportsRouting())
            }

            if (invalidPlugin.availableServiceProviders.indexOf('osm') > -1) {
                verify(osmPlugin.isAttached)
                verify(osmPlugin.supportsMapping(Plugin.OnlineMappingFeature))
                verify(osmPlugin.supportsGeocoding(Plugin.OnlineGeocodingFeature))
                verify(osmPlugin.supportsRouting(Plugin.OnlineRoutingFeature))
            }

            verify(!unattachedPlugin.isAttached)

            // test changing name of plugin
            invalidAttachedSpy.clear()
            compare(invalidAttachedSpy.count, 0)
            invalidPlugin.name = 'qmlgeo.test.plugin'
            let expectedAttachedSpy = 1
            tryCompare(invalidAttachedSpy, 'count', expectedAttachedSpy)
            verify(invalidPlugin.isAttached)

            verify(invalidPlugin.supportsMapping())
            verify(invalidPlugin.supportsGeocoding())
            verify(invalidPlugin.supportsRouting())
            verify(invalidPlugin.supportsPlaces())

            if (invalidPlugin.availableServiceProviders.indexOf('osm') > -1) {
                invalidPlugin.name = 'osm'
                compare(invalidAttachedSpy.count, ++expectedAttachedSpy)
                verify(invalidPlugin.supportsMapping(Plugin.OnlineMappingFeature))
                verify(invalidPlugin.supportsGeocoding(Plugin.OnlineGeocodingFeature))
                verify(invalidPlugin.supportsRouting(Plugin.OnlineRoutingFeature))
            }

            invalidPlugin.name = ''
            compare(invalidAttachedSpy.count, expectedAttachedSpy)
            verify(!invalidPlugin.supportsMapping())
            verify(!invalidPlugin.supportsGeocoding())
            verify(!invalidPlugin.supportsRouting())
            verify(!invalidPlugin.supportsPlaces())
        }

        function test_required() {
            // the required plugin should either get here or qmlgeo.test.plugin
            // either way the name will be non-empty and it'll meet the spec
            verify(requiredPlugin.name !== "")
            verify(requiredPlugin.supportsMapping(requiredPlugin.required.mapping))
            verify(requiredPlugin.supportsGeocoding(requiredPlugin.required.geocoding))
            verify(requiredPlugin.supportsPlaces(requiredPlugin.required.places))
            verify(requiredPlugin.supportsNavigation(requiredPlugin.required.navigation))
        }

        function test_placesFeatures() {
            verify(testPlugin.supportsPlaces(Plugin.SavePlaceFeature))
            verify(testPlugin.supportsPlaces(Plugin.SaveCategoryFeature))
            verify(testPlugin.supportsPlaces(Plugin.SearchSuggestionsFeature))
            verify(!testPlugin.supportsPlaces(Plugin.RemovePlaceFeature))
        }

        function test_locale() {
            compare(osmPlugin.locales, [Qt.locale().name]);

            //try assignment of a single locale
            osmPlugin.locales = "fr_FR";
            compare(osmPlugin.locales, ["fr_FR"]);

            //try assignment of multiple locales
            osmPlugin.locales = ["fr_FR","en_US"];
            compare(osmPlugin.locales, ["fr_FR","en_US"]);

            //check that assignment of empty locale list defaults to system locale
            osmPlugin.locales = [];
            compare(osmPlugin.locales, [Qt.locale().name]);
        }
    }
}
