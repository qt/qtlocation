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

import QtTest 1.0
import QtLocation 5.0

TestCase {
    id: testCase

    name: "User"

    User { id: emptyUser }

    function test_empty() {
        compare(emptyUser.userId, "");
        compare(emptyUser.name, "");
    }

    User {
        id: qmlUser

        userId: "testuser"
        name: "Test User"
    }

    function test_qmlConstructedUser() {
        compare(qmlUser.userId, "testuser");
        compare(qmlUser.name, "Test User");
    }

    User {
        id: testUser
    }

    function test_setAndGet_data() {
        return [
            { tag: "userId", property: "userId", signal: "userIdChanged", value: "testuser", reset: "" },
            { tag: "name", property: "name", signal: "nameChanged", value: "Test User", reset: "" },
        ];
    }

    function test_setAndGet(data) {
        var signalSpy = Qt.createQmlObject('import QtTest 1.0; SignalSpy {}', testCase, "SignalSpy");
        signalSpy.target = testUser;
        signalSpy.signalName = data.signal;

        // set property to something new
        testUser[data.property] = data.value;
        compare(testUser[data.property], data.value);
        compare(signalSpy.count, 1);

        // set property to the same value (signal spy should not increase)
        testUser[data.property] = data.value;
        compare(testUser[data.property], data.value);
        compare(signalSpy.count, 1);

        // reset property
        testUser[data.property] = data.reset;
        compare(testUser[data.property], data.reset);
        compare(signalSpy.count, 2);

        signalSpy.destroy();
    }
}
