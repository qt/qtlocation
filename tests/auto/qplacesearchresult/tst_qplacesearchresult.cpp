/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the test suite module of the Qt Toolkit.
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
#include <QtLocation/QPlaceSearchResult>

QT_USE_NAMESPACE

class tst_QPlaceSearchResult : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void test();
};

void tst_QPlaceSearchResult::test()
{
    QPlaceSearchResult result;

    QCOMPARE(result.type(), QPlaceSearchResult::UnknownSearchResult);
    QVERIFY(qIsNaN(result.distance()));
    QCOMPARE(result.place(), QPlace());
    QVERIFY(result.correction().isEmpty());

    result.setType(QPlaceSearchResult::PlaceResult);
    result.setDistance(2.0);
    result.setPlace(QPlace());
    result.setCorrection(QLatin1String("suggestion"));

    QCOMPARE(result.type(), QPlaceSearchResult::PlaceResult);
    QCOMPARE(result.distance(), 2.0);
    QCOMPARE(result.place(), QPlace());
    QCOMPARE(result.correction(), QLatin1String("suggestion"));

    QPlaceSearchResult result2(result);

    QCOMPARE(result2, result);

    result2.setType(QPlaceSearchResult::CorrectionResult);

    QCOMPARE(result2.type(), QPlaceSearchResult::CorrectionResult);

    QVERIFY(result2 != result);

    QPlaceSearchResult result3;

    QCOMPARE(result3.type(), QPlaceSearchResult::UnknownSearchResult);

    result3 = result;

    QCOMPARE(result3, result);
    QCOMPARE(result3.type(), QPlaceSearchResult::PlaceResult);
}

QTEST_APPLESS_MAIN(tst_QPlaceSearchResult)

#include "tst_qplacesearchresult.moc"
