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

TestCase {
    id: testCase

    name: "Supplier"

    property supplier emptySupplier;

    function test_empty() {
        compare(emptySupplier.supplierId, "");
        compare(emptySupplier.name, "");
        compare(emptySupplier.url, "");
        verify(emptySupplier.icon);
    }

    property supplier qmlSupplier
    qmlSupplier {
        supplierId: "test-supplier-id"
        name: "Test Supplier"
        url: "http://example.com/test-supplier-id"

        icon: ({ parameters: { singleUrl: "http://example.com/icons/test-supplier.png" }})
    }

    function test_qmlConstructedSupplier() {
        compare(qmlSupplier.supplierId, "test-supplier-id");
        compare(qmlSupplier.name, "Test Supplier");
        compare(qmlSupplier.url, "http://example.com/test-supplier-id");
        verify(qmlSupplier.icon);
        compare(qmlSupplier.icon.parameters.singleUrl, "http://example.com/icons/test-supplier.png");
    }
}
