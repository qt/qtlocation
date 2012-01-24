/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include <qplacematchrequest.h>


QT_USE_NAMESPACE

class tst_QPlaceMatchRequest : public QObject
{
    Q_OBJECT

public:
    tst_QPlaceMatchRequest();

private Q_SLOTS:
    void constructorTest();
    void placesTest();
    void resultsTest();
    void parametersTest();
    void clearTest();
};

tst_QPlaceMatchRequest::tst_QPlaceMatchRequest()
{
}

void tst_QPlaceMatchRequest::constructorTest()
{
    QPlaceMatchRequest request;
    QVariantMap params;
    params.insert(QLatin1String("key"), QLatin1String("val"));

    QPlace place1;
    place1.setName(QLatin1String("place1"));

    QPlace place2;
    place2.setName(QLatin1String("place2"));

    QList<QPlace> places;
    places << place1 << place2;

    request.setPlaces(places);
    request.setParameters(params);

    QPlaceMatchRequest copy(request);
    QCOMPARE(copy, request);
    QCOMPARE(copy.places(), places);
    QCOMPARE(copy.parameters(), params);
}

void tst_QPlaceMatchRequest::placesTest()
{
    QPlaceMatchRequest request;
    QCOMPARE(request.places().count(), 0);

    QPlace place1;
    place1.setName(QLatin1String("place1"));

    QPlace place2;
    place2.setName(QLatin1String("place2"));

    QList<QPlace> places;
    places << place1 << place2;

    request.setPlaces(places);
    QCOMPARE(request.places(), places);

    request.setPlaces(QList<QPlace>());
    QCOMPARE(request.places().count(), 0);
}

void tst_QPlaceMatchRequest::resultsTest()
{
    QPlaceMatchRequest request;
    QCOMPARE(request.places().count(), 0);

    QPlace place1;
    place1.setName(QLatin1String("place1"));
    QPlaceSearchResult result1;
    result1.setPlace(place1);

    QPlace place2;
    place2.setName(QLatin1String("place2"));
    QPlaceSearchResult result2;
    result2.setPlace(place2);

    QList<QPlaceSearchResult> results;
    results << result1 << result2;

    request.setResults(results);

    QCOMPARE(request.places().count(), 2);
    QCOMPARE(request.places().at(0), place1);
    QCOMPARE(request.places().at(1), place2);

    request.setResults(QList<QPlaceSearchResult>());
    QCOMPARE(request.places().count(), 0);
}

void tst_QPlaceMatchRequest::parametersTest()
{
    QPlaceMatchRequest request;
    QVERIFY(request.parameters().isEmpty());

    QVariantMap params;
    params.insert(QLatin1String("key"), QLatin1String("value"));

    request.setParameters(params);
    QCOMPARE(request.parameters(), params);
}

void tst_QPlaceMatchRequest::clearTest()
{
    QPlaceMatchRequest request;
    QVariantMap params;
    params.insert(QLatin1String("key"), QLatin1String("value"));

    QPlace place1;
    place1.setName(QLatin1String("place1"));

    QPlace place2;
    place2.setName(QLatin1String("place2"));

    QList<QPlace> places;
    places << place1 << place2;

    request.setPlaces(places);
    request.setParameters(params);

    request.clear();
    QVERIFY(request.places().isEmpty());
    QVERIFY(request.parameters().isEmpty());
}

QTEST_APPLESS_MAIN(tst_QPlaceMatchRequest)

#include "tst_qplacematchrequest.moc"
