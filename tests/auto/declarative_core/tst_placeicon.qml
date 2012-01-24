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

    name: "Icon"

    Icon { id: emptyIcon }

    function test_empty() {
        compare(emptyIcon.plugin, null);
        compare(emptyIcon.parameters.keys().length, 0)
    }


    Icon {
        id: qmlIconSingleUrl
    }

    function test_qmlSingleUrlIcon() {
        qmlIconSingleUrl.parameters.singleUrl = "http://example.com/icon.png"

        var u = qmlIconSingleUrl.url(Qt.size(64, 64));
        compare(u, "http://example.com/icon.png");

        u = qmlIconSingleUrl.url(Qt.size(20, 20));
        compare(u, "http://example.com/icon.png");

        qmlIconSingleUrl.parameters.singleUrl = "/home/user/icon.png"
        u = qmlIconSingleUrl.url(Qt.size(20, 20));
        compare(u, "file:///home/user/icon.png");
    }

    Plugin {
        id: testPlugin
        name: "qmlgeo.test.plugin"
    }

    Icon {
        id: qmlIconParams
        plugin: testPlugin
    }

    function test_qmlIconParams() {
        compare(qmlIconParams.plugin, testPlugin);
        qmlIconParams.parameters.s = "http://example.com/icon_small.png"
        qmlIconParams.parameters.m = "http://example.com/icon_medium.png"
        qmlIconParams.parameters.l = "http://example.com/icon_large.png"

        compare(qmlIconParams.url(Qt.size(10, 10)), "http://example.com/icon_small.png");
        compare(qmlIconParams.url(Qt.size(20, 20)), "http://example.com/icon_small.png");
        compare(qmlIconParams.url(Qt.size(24, 24)), "http://example.com/icon_small.png");
        compare(qmlIconParams.url(Qt.size(25, 25)), "http://example.com/icon_medium.png");
        compare(qmlIconParams.url(Qt.size(30, 30)), "http://example.com/icon_medium.png");
        compare(qmlIconParams.url(Qt.size(39, 39)), "http://example.com/icon_medium.png");
        compare(qmlIconParams.url(Qt.size(40, 40)), "http://example.com/icon_large.png");
        compare(qmlIconParams.url(Qt.size(50, 50)), "http://example.com/icon_large.png");
        compare(qmlIconParams.url(Qt.size(60, 60)), "http://example.com/icon_large.png");
    }

    Icon {
        id: testIcon
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
}
