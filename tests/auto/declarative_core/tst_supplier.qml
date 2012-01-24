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

    name: "Supplier"

    Supplier { id: emptySupplier }

    function test_empty() {
        compare(emptySupplier.supplierId, "");
        compare(emptySupplier.name, "");
        compare(emptySupplier.url, "");
        verify(emptySupplier.icon);
    }

    Supplier {
        id: qmlSupplier

        supplierId: "test-supplier-id"
        name: "Test Supplier"
        url: "http://example.com/test-supplier-id"

        icon: Icon {
            Component.onCompleted:  {
                parameters.singleUrl = "http://example.com/icons/test-supplier.png"
            }
        }
    }

    function test_qmlConstructedSupplier() {
        compare(qmlSupplier.supplierId, "test-supplier-id");
        compare(qmlSupplier.name, "Test Supplier");
        compare(qmlSupplier.url, "http://example.com/test-supplier-id");
        verify(qmlSupplier.icon);
        compare(qmlSupplier.icon.parameters.singleUrl, "http://example.com/icons/test-supplier.png");
    }

    Supplier {
        id: testSupplier
    }

    Plugin {
        id: testPlugin
        name: "qmlgeo.test.plugin"
    }

    Plugin {
        id: invalidPlugin
    }

    Icon {
        id: testIcon
    }

    function test_setAndGet_data() {
        return [
            { tag: "name", property: "name", signal: "nameChanged", value: "Test Supplier", reset: "" },
            { tag: "supplierId", property: "supplierId", signal: "supplierIdChanged", value: "test-supplier-id-1", reset: "" },
            { tag: "url", property: "url", signal: "urlChanged", value: "http://example.com/test-supplier-id-1", reset: "" },
            { tag: "icon", property: "icon", signal: "iconChanged", value: testIcon }
        ];
    }

    function test_setAndGet(data) {
        console.log("testing: " + data.tag);
        var signalSpy = Qt.createQmlObject('import QtTest 1.0; SignalSpy {}', testCase, "SignalSpy");
        signalSpy.target = testSupplier;
        signalSpy.signalName = data.signal;

        // set property to something new
        testSupplier[data.property] = data.value;
        compare(testSupplier[data.property], data.value);
        compare(signalSpy.count, 1);

        // set property to the same value (signal spy should not increase)
        testSupplier[data.property] = data.value;
        compare(testSupplier[data.property], data.value);
        compare(signalSpy.count, 1);

        // reset property
        if (data.reset === undefined) {
            testSupplier[data.property] = null;
            compare(testSupplier[data.property], null);
        } else {
            testSupplier[data.property] = data.reset;
            compare(testSupplier[data.property], data.reset);
        }
        compare(signalSpy.count, 2);

        signalSpy.destroy();
    }
}
