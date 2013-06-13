/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/QString>
#include <QtTest/QtTest>

#include <QtLocation/QPlaceContactDetail>

QT_USE_NAMESPACE

class tst_QPlaceContactDetail : public QObject
{
    Q_OBJECT

public:
    tst_QPlaceContactDetail();

private Q_SLOTS:
    void constructorTest();
    void labelTest();
    void valueTest();
    void clearTest();
    void operatorsTest();
    void operatorsTest_data();
};

tst_QPlaceContactDetail::tst_QPlaceContactDetail()
{
}

void tst_QPlaceContactDetail::constructorTest()
{
    QPlaceContactDetail detail;
    QVERIFY(detail.label().isEmpty());
    QVERIFY(detail.value().isEmpty());

    detail.setLabel(QLatin1String("Emergency Services"));
    detail.setValue(QLatin1String("0118 999"));

    QPlaceContactDetail detail2(detail);
    QCOMPARE(detail2.label(), QLatin1String("Emergency Services"));
    QCOMPARE(detail2.value(), QLatin1String("0118 999"));
}

void tst_QPlaceContactDetail::labelTest()
{
    QPlaceContactDetail detail;
    detail.setLabel(QLatin1String("home"));
    QCOMPARE(detail.label(), QLatin1String("home"));
    detail.setLabel(QString());
    QVERIFY(detail.label().isEmpty());
}

void tst_QPlaceContactDetail::valueTest()
{
    QPlaceContactDetail detail;
    detail.setValue(QLatin1String("555-5555"));
    QCOMPARE(detail.value(), QLatin1String("555-5555"));
    detail.setValue(QString());
    QVERIFY(detail.value().isEmpty());
}

void tst_QPlaceContactDetail::clearTest()
{
    QPlaceContactDetail detail;
    detail.setLabel(QLatin1String("Ghostbusters"));
    detail.setValue(QLatin1String("555-2368"));
    detail.clear();
    QVERIFY(detail.label().isEmpty());
    QVERIFY(detail.value().isEmpty());
}

void tst_QPlaceContactDetail::operatorsTest()
{
    QPlaceContactDetail detail1;
    detail1.setLabel(QLatin1String("Kramer"));
    detail1.setValue(QLatin1String("555-filk"));

    QPlaceContactDetail detail2;
    detail2.setLabel(QLatin1String("Kramer"));
    detail2.setValue(QLatin1String("555-filk"));

    QVERIFY(detail1 == detail2);
    QVERIFY(!(detail1 != detail2));
    QVERIFY(detail2 == detail1);
    QVERIFY(!(detail2 != detail1));

    QPlaceContactDetail detail3;
    QVERIFY(!(detail1 == detail3));
    QVERIFY(detail1 != detail3);
    QVERIFY(!(detail1 == detail3));
    QVERIFY(detail1 != detail3);

    detail3 = detail1;
    QVERIFY(detail1 == detail3);
    QVERIFY(!(detail1 != detail3));
    QVERIFY(detail3 == detail1);
    QVERIFY(!(detail3 != detail1));

    QFETCH(QString, field);
    if (field == QLatin1String("label"))
        detail3.setLabel(QLatin1String("Cosmo"));
    else if (field == QLatin1String("value"))
        detail3.setValue(QLatin1String("555-5555"));

    QVERIFY(!(detail1 == detail3));
    QVERIFY(detail1 != detail3);
    QVERIFY(!(detail3 == detail1));
    QVERIFY(detail3 != detail1);
}

void tst_QPlaceContactDetail::operatorsTest_data()
{
    QTest::addColumn<QString>("field");
    QTest::newRow("label") << "label";
    QTest::newRow("value") << "value";
}

QTEST_APPLESS_MAIN(tst_QPlaceContactDetail)

#include "tst_qplacecontactdetail.moc"
