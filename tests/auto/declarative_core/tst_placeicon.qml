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

TestCase {
    id: testCase

    name: "Icon"

    Icon { id: emptyIcon }

    function test_empty() {
        compare(emptyIcon.baseUrl, "");
        compare(emptyIcon.fullUrl, "");
        compare(emptyIcon.plugin, null);
    }

    Icon {
        id: qmlIconBase

        plugin: testPlugin

        baseUrl: "http://example.com/icon"
    }

    function test_qmlConstructedIconBase() {
        compare(qmlIconBase.baseUrl, "http://example.com/icon");
        compare(qmlIconBase.fullUrl, "");
        compare(qmlIconBase.plugin, testPlugin);

        var u = qmlIconBase.url(Qt.size(64, 64), Icon.Normal | Icon.List);
        verify(u !== "");
        verify(u !== "http://example.com/icon");
    }

    Icon {
        id: qmlIconFull

        plugin: testPlugin

        fullUrl: "http://example.com/icon.png"
    }

    function test_qmlConstructedIconFull() {
        compare(qmlIconFull.baseUrl, "");
        compare(qmlIconFull.fullUrl, "http://example.com/icon.png");
        compare(qmlIconFull.plugin, testPlugin);

        var u = qmlIconFull.url(Qt.size(64, 64), Icon.Normal | Icon.List);
        compare(u, qmlIconFull.fullUrl);
    }

    Icon {
        id: testIcon
    }

    Plugin {
        id: testPlugin
        name: "qmlgeo.test.plugin"
    }

    function test_setAndGet_data() {
        return [
            { tag: "plugin", property: "plugin", signal: "pluginChanged", value: testPlugin },
        ];
    }

    function test_setAndGet(data) {
        var signalSpy = Qt.createQmlObject('import QtTest 1.0; SignalSpy {}', testCase, "SignalSpy");
        signalSpy.target = testIcon;
        signalSpy.signalName = data.signal;

        // set property to something new
        testIcon[data.property] = data.value;
        compare(testIcon[data.property], data.value);
        compare(signalSpy.count, 1);

        // set property to the same value (signal spy should not increase)
        testIcon[data.property] = data.value;
        compare(testIcon[data.property], data.value);
        compare(signalSpy.count, 1);

        // reset property
        if (data.reset === undefined) {
            testIcon[data.property] = null;
            compare(testIcon[data.property], null);
        } else {
            testIcon[data.property] = data.reset;
            compare(testIcon[data.property], data.reset);
        }
        compare(signalSpy.count, 2);

        signalSpy.destroy();
    }

    Icon {
        id: baseIcon
        plugin: testPlugin
    }

    function test_baseUrl() {
        var signalSpy = Qt.createQmlObject('import QtTest 1.0; SignalSpy {}', testCase, "SignalSpy");
        signalSpy.target = baseIcon;
        signalSpy.signalName = "baseUrlChanged";

        // set baseUrl to something new
        var baseUrl = "http://example.com/base-icon"
        baseIcon.baseUrl = baseUrl;
        compare(baseIcon.baseUrl, baseUrl);
        compare(signalSpy.count, 1);
        verify(baseIcon.fullUrl, "");

        // set property to the same value (signals spy should not increase)
        baseIcon.baseUrl = baseUrl;
        compare(baseIcon.baseUrl, baseUrl);
        compare(signalSpy.count, 1);
        verify(baseIcon.fullUrl, "");

        // verify that url() method returns something different
        var url = baseIcon.url(Qt.size(64, 64), Icon.Normal | Icon.List);
        verify(url != "");
        verify(url != baseUrl);

        // reset property
        baseIcon.baseUrl = "";
        compare(baseIcon.baseUrl, "");
        compare(signalSpy.count, 2);
        verify(baseIcon.fullUrl, "");

        signalSpy.destroy();
    }

    Icon {
        id: fullIcon
        plugin: testPlugin
    }

    function test_fullUrl() {
        var signalSpy = Qt.createQmlObject('import QtTest 1.0; SignalSpy {}', testCase, "SignalSpy");
        signalSpy.target = fullIcon;
        signalSpy.signalName = "fullUrlChanged";

        // set fullUrl to something new
        var fullUrl = "http://example.com/full-icon.png"
        fullIcon.fullUrl = fullUrl;
        compare(fullIcon.fullUrl, fullUrl);
        compare(signalSpy.count, 1);
        verify(fullIcon.baseUrl, "");

        // set property to the same value (signals spy should not increase)
        fullIcon.fullUrl = fullUrl;
        compare(fullIcon.fullUrl, fullUrl);
        compare(signalSpy.count, 1);
        verify(fullIcon.baseUrl, "");

        // verify that url() method returns something different
        var url = fullIcon.url(Qt.size(64, 64), Icon.Normal | Icon.List);
        compare(url, fullUrl);

        // reset property
        fullIcon.fullUrl = "";
        compare(fullIcon.fullUrl, "");
        compare(signalSpy.count, 2);
        verify(fullIcon.baseUrl, "");

        signalSpy.destroy();
    }
}
