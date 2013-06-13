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

#include <QtLocation/QPlaceUser>

QT_USE_NAMESPACE

class tst_QPlaceUser : public QObject
{
    Q_OBJECT

public:
    tst_QPlaceUser();

private Q_SLOTS:
    void constructorTest();
    void nameTest();
    void userIdTest();
    void operatorsTest();
    void operatorsTest_data();
};

tst_QPlaceUser::tst_QPlaceUser()
{
}

void tst_QPlaceUser::constructorTest()
{
    QPlaceUser user;
    QVERIFY(user.name().isEmpty());
    QVERIFY(user.userId().isEmpty());

    user.setName(QLatin1String("Thomas Anderson"));
    user.setUserId(QLatin1String("Neo"));

    QPlaceUser user2(user);
    QCOMPARE(user2.name(), QLatin1String("Thomas Anderson"));
    QCOMPARE(user2.userId(), QLatin1String("Neo"));
}

void tst_QPlaceUser::nameTest()
{
    QPlaceUser user;
    user.setName(QLatin1String("Thomas Anderson"));
    QCOMPARE(user.name(), QLatin1String("Thomas Anderson"));
    user.setName(QString());
    QVERIFY(user.name().isEmpty());
}

void tst_QPlaceUser::userIdTest()
{
    QPlaceUser user;
    user.setUserId(QLatin1String("Neo"));
    QCOMPARE(user.userId(), QLatin1String("Neo"));
    user.setUserId(QString());
    QVERIFY(user.userId().isEmpty());
}

void tst_QPlaceUser::operatorsTest()
{
    QPlaceUser user1;
    user1.setName(QLatin1String("Thomas Anderson"));
    user1.setUserId(QLatin1String("Neo"));

    QPlaceUser user2;
    user2.setName(QLatin1String("Thomas Anderson"));
    user2.setUserId(QLatin1String("Neo"));

    QVERIFY(user1 == user2);
    QVERIFY(!(user1 != user2));
    QVERIFY(user2 == user1);
    QVERIFY(!(user2 != user1));

    QPlaceUser user3;
    QVERIFY(!(user1 == user3));
    QVERIFY(user1 != user3);
    QVERIFY(!(user3 == user1));
    QVERIFY(user3 != user1);

    user3 = user1;
    QVERIFY(user1 == user3);
    QVERIFY(!(user1 != user3));
    QVERIFY(user3 == user1);
    QVERIFY(!(user3 != user1));

    QFETCH(QString, field);

    if (field == QLatin1String("name"))
        user3.setName(QLatin1String("bob"));
    else if (field == QLatin1String("userId"))
        user3.setUserId(QLatin1String("Morpheus"));
    else
        qFatal("Unknown data field");

    QVERIFY(!(user1 == user3));
    QVERIFY(user1 != user3);
    QVERIFY(!(user3 == user1));
    QVERIFY(user3 != user1);
}

void tst_QPlaceUser::operatorsTest_data()
{
    QTest::addColumn<QString>("field");

    QTest::newRow("user name") << "name";
    QTest::newRow("user id") << "userId";
}

QTEST_APPLESS_MAIN(tst_QPlaceUser)

#include "tst_qplaceuser.moc"
