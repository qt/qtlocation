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
import QtLocation 5.0
import QtTest 1.0
import "../configuration.js" as Configuration

Item {
    id: root

    // Properties
    property bool __modelDone: false
    property bool __abort: false
    property TestCase __testCase: null
    property variant __data: null
    property Plugin plugin: null
    property RouteModel model: RouteModel {
        query: RouteQuery { }

        onStatusChanged: {
            var str = errorString ? errorString : "<unknown>";

            switch (status) {
            case RouteModel.Loading:
            case RouteModel.Null:
                break;
            case RouteModel.Error:
                if (!__data.expectError)
                    root.fail("Error: " + str);
                else
                    root.__modelDone = true
                break;
            case RouteModel.Ready:
                root.__modelDone = true
                break;
            }
        }
    }

    // Initialization
    Component.onCompleted: {

        if (Configuration &&
            Configuration.pluginName) {
            var code = "import QtLocation 5.0; Plugin { name: \"" + Configuration.pluginName + "\"; parameters: [ "

            // proxy: string
            if (Configuration.proxy && Configuration.proxy.length > 0) {
                code += "PluginParameter { name: \"proxy\"; value: \"" + Configuration.proxy + "\" }, "
                code += "PluginParameter { name: \"routing.proxy\"; value: \"" + Configuration.proxy + "\" }, "
            }

            // host: string
            if (Configuration.routingHost && Configuration.routingHost.length > 0)
                code += "PluginParameter { name: \"routing.host\"; value: \"" + Configuration.routingHost + "\" }, "

            // app_id: string
            //if (Configuration.appId && Configuration.appId.length > 0)
            //    code += "PluginParameter { name: \"app_id\"; value: \"" + Configuration.appId + "\" }, "

            // token: string
            if (Configuration.token && Configuration.token.length > 0)
                code += "PluginParameter { name: \"token\"; value: \"" + Configuration.token + "\" }, "

            // referer: string
            if (Configuration.routingReferer && Configuration.routingReferer.length > 0)
                code += "PluginParameter { name: \"routing.referer\"; value: \"" + Configuration.routingReferer + "\" }, "


            // pluginParameters: array of key - value pairs
            if (typeof(Configuration.pluginParameters) === "object") {
                for (var i = 1; i < Configuration.pluginParameters.length; i += 2) {
                    code += "PluginParameter { name: \""
                    code += Configuration.pluginParameters[i - 1]
                    code += "\"; value: \""
                    code += Configuration.pluginParameters[i]
                    code += "\" }, "
                }
            }

            // Plugin list termination
            code += "PluginParameter {} ] ";
            code += "}";

            plugin = Qt.createQmlObject(code, root)
            model.plugin = root.plugin
        }
    }

    // TestCase functions, forwarded to fixture
    function fail(_str) {
        __abort = true;
        __testCase.fail(_str);
    }

    function verify(_value) {
        try
        {
            __testCase.verify(_value)
        }
        catch (e)
        {
            console.trace()
            throw e
        }
    }

    function compare(_lhs, _rhs) {
        try
        {
            __testCase.compare(_lhs, _rhs)
        }
        catch (e)
        {
            console.trace()
            throw e
        }
    }

    // Helper functions for routing tests
    function runTest(_test, _data) {

        // In case the plugin hasn't been set up...
        if (plugin === null) {
            _test.skip("Valid Nokia plugin setup required. See the configuration.js file in the parent folder.")
            return
        }

        reset(_test, _data)

        for (var i = 0; i < _data.waypoints.length; ++i)
            model.query.addWaypoint(_data.waypoints[i]);

        if (_data.travelModes !== undefined)
            model.query.travelModes = _data.travelModes

        if (_data.locale !== undefined)
            model.plugin.locales = _data.locale

        if (_data.segmentDetail !== undefined)
            model.query.segmentDetail = _data.segmentDetail

        if (_data.routeOptimizations !== undefined)
            model.query.routeOptimizations = _data.routeOptimizations

        if (_data.numberAlternativeRoutes !== undefined)
            model.query.numberAlternativeRoutes = _data.numberAlternativeRoutes

        if (_data.weights !== undefined) {
            for (var i = 1; i < _data.weights.length; i += 2)
                model.query.setFeatureWeight(_data.weights[i-1], _data.weights[i])
        }

        if (_data.excludedAreas !== undefined) {
            for (var i = 0; i < _data.excludedAreas.length; ++i)
                model.query.addExcludedArea(_data.excludedAreas[i])
        }

        // possible optimization if we expect no route
        if (_data.expectNoRouteFound || _data.expectError)
            model.query.segmentDetail = RouteQuery.NoSegmentData

        computeRoute();

        if (_data.expectError) {
            compare(_data.expectError, model.error)
            return
        }

        if (_data.expectNoRouteFound)
            _data.expectRoutes = 0

        if (_data.expectRoutes === undefined)
            _data.expectRoutes = 1

        //console.debug("Expected " + _data.expectRoutes + ", got " + model.count + " routes")
        compare(model.count, _data.expectRoutes)

        if (model.count == 1) {
            var route = model.get(0)

            verify(route)
            verify(route.path)
            verify(route.path.length >= 2)

            if (_data.expectedMinDistance) {
                //console.debug("Actual distance: " + route.distance + ", expected min distance: " + _data.expectedMinDistance)
                verify(route.distance >= _data.expectedMinDistance)
            }

            if (_data.expectedMaxDistance)
                verify(route.distance <= _data.expectedMaxDistance)

            if (_data.expectedMinTravelTime)
                verify(route.travelTime >= _data.expectedMinTravelTime)

            if (_data.expectedMaxTravelTime)
                verify(route.travelTime <= _data.expectedMaxTravelTime)

            if (_data.expectedMinSegments)
                verify(route.segments.length >= _data.expectedMinSegments)
        }

        for (var i = 0; i < model.count; ++i) {
            var route = model.get(i)
            verify(route)
            verify(route.path)
            verify(route.path.length >= 2)
            verifySegments(route.segments, _data)
        }
    }


    function reset(_testCase, _data) {
        __testCase = _testCase
        __data = _data
        model.plugin.locales = "en"
        model.query.clearWaypoints()
        model.query.segmentDetail = RouteQuery.BasicSegmentData
        model.query.maneuverDetail = RouteQuery.BasicManeuvers
        model.query.resetFeatureWeights()
        model.query.clearExcludedAreas()
        model.query.travelModes = RouteQuery.CarTravel
        model.query.routeOptimizations = RouteQuery.FastestRoute
        model.query.numberAlternativeRoutes = 0
        model.reset()
        __modelDone = false
        __abort = false
    }

    function computeRoute() {
        model.update()
        while (!__modelDone && !__abort) {
            __testCase.wait(100)
        }
    }

    function verifySegments(_segments, _data) {
        verify(_data)
        verify(_segments)
        verify(_segments.length >= 1)

        // Grep for words in instruction text to ensure a particular
        // route is (not) taken.
        var noGrep = []
        var grep = []

        if (_data.grep) {
            grep.length = _data.grep.length
            for (var i = 0; i < grep.length; ++i)
                grep[i] = false
        }

        if (_data.noGrep) {
            noGrep.length = _data.noGrep.length
            for (var i = 0; i < noGrep.length; ++i)
                noGrep[i] = false
        }

        var toleranceStep = 1

        // Do we have a custom tolerance (for ferry routing, public transport routing mostly)
        if (_data.segmentTolerance)
            toleranceStep = _data.segmentTolerance

        var waypointIndex = 0
        var maneuvers = 0;

        for (var i = 0; i < _segments.length; ++i) {
            var segment = _segments[i];
            verify(segment)                     // not null / undefined
            verify(segment.path)                // not null / undefined
            verify(segment.path.length >= 1)    // at least the point of the maneuver
            verify(segment.travelTime >= 0)
            verify(segment.distance >= 0)

            // verify the distances check out
            var computed = 0
            var tolerance = 0

            for (var j = 1; j < segment.path.length; ++j, tolerance += toleranceStep) {
                var prev = segment.path[j - 1];
                var curr = segment.path[j];
                verify(prev)
                verify(curr)
                verify(prev.isValid)
                verify(curr.isValid)
                computed += prev.distanceTo(curr);
            }

            // check the computation (not we may have zero length segments)
            //console.debug("Computed: " + computed + ", REST " + segment.distance + " WPs: " + segment.path.length)
            verify(Math.abs(computed - segment.distance) <= tolerance)

            var maneuver = segment.maneuver
            verify(maneuver) // always there, but possibly invalid

            if (maneuver.valid) {
                ++maneuvers
                verify(maneuver.position) // with position
                verify(maneuver.position.isValid) // ... that is valid
                verify(maneuver.instructionText.length > 0) // some instruction text

                if (_data.printInstructions)
                    console.debug(maneuvers + ". " + maneuver.instructionText)

                // grep instruction text for terms in grep / no-grep lists
                for (var g = 0; g < grep.length; ++g)
                    grep[g] |= maneuver.instructionText.indexOf(_data.grep[g]) >= 0

                for (var g = 0; g < noGrep.length; ++g)
                    noGrep[g] |= maneuver.instructionText.indexOf(_data.noGrep[g]) >= 0

                if (maneuver.waypointValid) {
                    var inputWaypoint = _data.waypoints[waypointIndex++];
                    verify(maneuver.waypoint) // must be there
                    verify(maneuver.waypoint.distanceTo(inputWaypoint) < 0.01) // should be the same
                }
            }
        }

        for (var i = 0; i < grep.length; ++i) {
            if (!grep[i])
                console.debug("Failed to grep '" + _data.grep[i] + "' in instructions")

            verify(grep[i])
        }

        for (var i = 0; i < noGrep.length; ++i) {
            if (noGrep[i])
                console.debug("Instructions contain '" + _data.noGrep[i] + "'")

            verify(!noGrep[i])
        }

        if (_data.expectedManeuvers)
            compare(maneuvers, _data.expectedManeuvers)
    }

    function supportTravelModes() {
        return [RouteQuery.PublicTransitTravel, RouteQuery.CarTravel, RouteQuery.PedestrianTravel]
    }

    function supportTravelModesBitset() {
        var modesList = supportTravelModes()
        var modesInt = 0
        for (var i = 0; i < modesList.length; ++i)
            modesInt |= modesList[i]

        return modesInt
    }
}




