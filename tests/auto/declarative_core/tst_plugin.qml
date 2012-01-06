/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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
    SignalSpy {id: invalidFeaturesSpy; target: invalidPlugin; signalName: "supportedFeaturesChanged"}
    SignalSpy {id: invalidSupportedPlacesFeaturesSpy; target: invalidPlugin; signalName: "supportedPlacesFeaturesChanged"}

    TestCase {
        name: "Plugin properties"
        function test_plugin() {
            verify (invalidPlugin.availableServiceProviders.length > 0)
            verify (invalidPlugin.availableServiceProviders.indexOf('qmlgeo.test.plugin') > -1) // at least test plugin must be present

            // invalid plugins should have no features
            compare(invalidPlugin.supported, Plugin.NoFeatures)

            if (invalidPlugin.availableServiceProviders.indexOf('qmlgeo.test.plugin') > -1) {
                var nokiaFeatures = (Plugin.GeocodingFeature |
                                     Plugin.ReverseGeocodingFeature |
                                     Plugin.RoutingFeature |
                                     Plugin.MappingFeature |
                                     Plugin.AnyPlacesFeature)
                compare(nokiaPlugin.supported & nokiaFeatures, nokiaFeatures)
            }

            var testFeatures =  (Plugin.GeocodingFeature |
                                 Plugin.ReverseGeocodingFeature |
                                 Plugin.RoutingFeature |
                                 Plugin.MappingFeature |
                                 Plugin.AnyPlacesFeature)
            compare(testPlugin.supported & testFeatures, testFeatures)

            // test changing name of plugin
            invalidFeaturesSpy.clear()
            compare(invalidFeaturesSpy.count, 0)
            invalidPlugin.name = 'qmlgeo.test.plugin'
            compare(invalidFeaturesSpy.count, 1)
            compare(invalidPlugin.supported & testFeatures, testFeatures)

            invalidPlugin.name = ''
            compare(invalidFeaturesSpy.count, 2)

            compare(invalidPlugin.supported, Plugin.NoFeatures)
        }

        function test_placesFeatures() {
            verify((testPlugin.supportedPlacesFeatures & Plugin.SavePlaceFeature) === Plugin.SavePlaceFeature);
            verify((testPlugin.supportedPlacesFeatures & Plugin.SaveCategoryFeature) === Plugin.SaveCategoryFeature);
            verify((testPlugin.supportedPlacesFeatures & Plugin.SearchSuggestionsFeature) === Plugin.SearchSuggestionsFeature);
            verify((testPlugin.supportedPlacesFeatures & Plugin.CorrectionsFeature) === 0);
            verify((testPlugin.supportedPlacesFeatures & Plugin.RemovePlaceFeature) === 0);

            verify((nokiaPlugin.supportedPlacesFeatures & Plugin.SavePlaceFeature) === 0);
            verify((nokiaPlugin.supportedPlacesFeatures & Plugin.RemovePlaceFeature) === 0);
            verify((nokiaPlugin.supportedPlacesFeatures & Plugin.SaveCategoryFeature) === 0);
            verify((nokiaPlugin.supportedPlacesFeatures & Plugin.RemoveCategoryFeature) === 0);
            verify((nokiaPlugin.supportedPlacesFeatures & Plugin.RecommendationsFeature) === Plugin.RecommendationsFeature);
            verify((nokiaPlugin.supportedPlacesFeatures & Plugin.SearchSuggestionsFeature) === Plugin.SearchSuggestionsFeature);
            verify((nokiaPlugin.supportedPlacesFeatures & Plugin.CorrectionsFeature) === Plugin.CorrectionsFeature);
            verify((nokiaPlugin.supportedPlacesFeatures & Plugin.LocaleFeature) === Plugin.LocaleFeature);

            invalidSupportedPlacesFeaturesSpy.clear();
            invalidPlugin.name = 'qmlgeo.test.plugin';
            compare(invalidSupportedPlacesFeaturesSpy.count, 1);
            invalidPlugin.name = '';
            compare(invalidSupportedPlacesFeaturesSpy.count, 2);

            invalidFeaturesSpy.clear();
            invalidSupportedPlacesFeaturesSpy.clear();
        }

        function test_locale() {
            compare(nokiaPlugin.locales, [Qt.locale().name]);

            //try assignment of a single locale
            nokiaPlugin.locales = "fr_FR";
            compare(nokiaPlugin.locales, ["fr_FR"]);

            //try assignment of multiple locales
            nokiaPlugin.locales = ["fr_FR","en_US"];
            compare(nokiaPlugin.locales, ["fr_FR","en_US"]);

            //check that assignment of empty locale list defaults to system locale
            nokiaPlugin.locales = [];
            compare(nokiaPlugin.locales, [Qt.locale().name]);
        }
    }
}
