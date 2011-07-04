/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#ifndef QLOCATIONTESTUTILS_P_H
#define QLOCATIONTESTUTILS_P_H

#include <QString>
#include <QTest>

#define QTRY_COMPARE(a,e)                       \
    for (int _i = 0; _i < 5000; _i += 100) {    \
        if ((a) == (e)) break;                  \
        QTest::qWait(100);                      \
    }                                           \
    QCOMPARE(a, e)

#define QTRY_COMPARE_WITH_TIMEOUT(a,e,t)                       \
    for (int _i = 0; _i < t; _i += 100) {    \
        if ((a) >= (e)) break;                  \
        QTest::qWait(100);                      \
    }                                           \
    QCOMPARE(a, e)

#define QTRY_VERIFY_WITH_TIMEOUT(a,t)                       \
    for (int _i = 0; _i < t; _i += 100) {    \
        if (a) break;                  \
        QTest::qWait(100);                      \
    }                                           \
    QVERIFY(a)

#define QTRY_COMPARE_WITH_TIMEOUT_RANGE(a,e,t1,t2) {                      \
    int _i;         \
    for (_i = 0; _i < t1; _i += 100) {    \
        if ((a) >= (e)) {char str[100]; sprintf(str, "Got signal earlier than expected, i = %d, n = %d", _i, e);QFAIL((const char *)str); break;}                  \
        QTest::qWait(100);                      \
    }                                           \
    for (; _i < t2; _i += 100) {    \
        if ((a) >= (e))  break;                  \
        QTest::qWait(100);                      \
    }      }                                     \
    QCOMPARE(a, e)

#if defined(Q_OS_WINCE)
    #define EXPECT_FAIL_WINCE_SEE_MOBILITY_337 { QEXPECT_FAIL("", "MOBILITY-337: This test will fail on the test farm for WinCE", Continue); }
#else
    #define EXPECT_FAIL_WINCE_SEE_MOBILITY_337
#endif

#if defined(Q_OS_WINCE)
    #define EXPECT_FAIL_WINCE_SEE_MOBILITY_337_ABORT { QEXPECT_FAIL("", "MOBILITY-337: This test will fail on the test farm for WinCE", Abort); }
#else
    #define EXPECT_FAIL_WINCE_SEE_MOBILITY_337_ABORT
#endif


class QLocationTestUtils
{
public:
    static void uheap_mark();
    static void uheap_mark_end();

    static bool hasDefaultSource();
    static bool hasDefaultMonitor();

    static QString addNmeaChecksumAndBreaks(const QString &sentence);

    static QString createRmcSentence(const QDateTime &dt);
    static QString createGgaSentence(const QTime &time);
    static QString createGgaSentence(int lat, int lng, const QTime &time);
    static QString createZdaSentence(const QDateTime &dt);
};

#endif
