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
    name: "Category"

    Category { id: emptyCategory }

    function test_empty() {
        compare(emptyCategory.categoryId, "");
        compare(emptyCategory.name, "");
        compare(emptyCategory.visibility, Category.UnspecifiedVisibility);
    }

    Category {
        id: cat1
        categoryId: "cat-id-1"
        name: "Category 1"
        visibility: Category.DeviceVisibility
    }
    Category {
        id: cat2
        categoryId: "cat-id-2"
        name: "Category 2"
        visibility: Category.PublicVisibility
    }

    function test_setAndGet() {
        compare(cat1.categoryId, "cat-id-1");
        compare(cat1.name, "Category 1");
        compare(cat1.visibility, Category.DeviceVisibility);

        verify(cat1.categoryId !== cat2.categoryId);
        verify(cat1.name !== cat2.name);
        verify(cat1.visibility !== cat2.visibility);

        cat2.categoryId = "cat-id-1";
        cat2.name = "Category 1";
        cat2.visibility = Category.DeviceVisibility;

        compare(cat1.categoryId, cat2.categoryId);
        compare(cat1.name, cat2.name);
        compare(cat1.visibility, cat2.visibility);
    }
}
