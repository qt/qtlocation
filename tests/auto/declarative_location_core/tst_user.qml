// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

import QtTest
import QtLocation

TestCase {
    id: testCase

    name: "User"

    property user emptyUser

    function test_empty() {
        compare(emptyUser.userId, "");
        compare(emptyUser.name, "");
    }

    property user qmlUser
    qmlUser {
        userId: "testuser"
        name: "Test User"
    }

    function test_qmlConstructedUser() {
        compare(qmlUser.userId, "testuser");
        compare(qmlUser.name, "Test User");
    }
}
