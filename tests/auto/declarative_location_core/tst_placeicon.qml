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
import "utils.js" as Utils

TestCase {
    id: testCase

    name: "Icon"

    property icon emptyIcon

    function test_empty() {
        compare(emptyIcon.parameters, {})
    }


    property icon qmlIconSingleUrl: ({ parameters: { singleUrl: "http://example.com/icon.png" }})

    function test_qmlSingleUrlIcon() {
        var u = qmlIconSingleUrl.url(Qt.size(64, 64));
        compare(u, "http://example.com/icon.png");

        u = qmlIconSingleUrl.url(Qt.size(20, 20));
        compare(u, "http://example.com/icon.png");

        var params = qmlIconSingleUrl.parameters
        params.singleUrl = "/home/user/icon.png"
        qmlIconSingleUrl.parameters = params
        u = qmlIconSingleUrl.url(Qt.size(20, 20));
        compare(u, "file:///home/user/icon.png");
    }
}
