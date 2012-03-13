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

#include <QtCore/QString>
#include <QtTest/QtTest>

#include <qplaceattribute.h>

QT_USE_NAMESPACE

class tst_QPlaceAttribute : public QObject
{
    Q_OBJECT

public:
    tst_QPlaceAttribute();

private Q_SLOTS:
    void constructorTest();
    void operatorsTest();
    void isEmptyTest();
};

tst_QPlaceAttribute::tst_QPlaceAttribute()
{
}

void tst_QPlaceAttribute::constructorTest()
{
    QPlaceAttribute testObj;

    QPlaceAttribute *testObjPtr = new QPlaceAttribute(testObj);
    QVERIFY2(testObjPtr != NULL, "Copy constructor - null");
    QVERIFY2(testObjPtr->label() == QString(), "Copy constructor - wrong label");
    QVERIFY2(testObjPtr->text() == QString(), "Copy constructor - wrong text");
    QVERIFY2(*testObjPtr == testObj, "Copy constructor - compare");
    delete testObjPtr;
}

void tst_QPlaceAttribute::operatorsTest()
{
    QPlaceAttribute testObj;
    testObj.setLabel(QStringLiteral("label"));
    QPlaceAttribute testObj2;
    testObj2 = testObj;
    QVERIFY2(testObj == testObj2, "Not copied correctly");
    testObj2.setText(QStringLiteral("text"));
    QVERIFY2(testObj != testObj2, "Object should be different");
}

void tst_QPlaceAttribute::isEmptyTest()
{
    QPlaceAttribute attribute;

    QVERIFY(attribute.isEmpty());

    attribute.setLabel(QStringLiteral("label"));
    QVERIFY(!attribute.isEmpty());
    attribute.setLabel(QString());
    QVERIFY(attribute.isEmpty());

    attribute.setText(QStringLiteral("text"));
    QVERIFY(!attribute.isEmpty());
    attribute.setText(QString());
    QVERIFY(attribute.isEmpty());
}

QTEST_APPLESS_MAIN(tst_QPlaceAttribute);

#include "tst_qplaceattribute.moc"
