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

#include <qplacesearchrequest.h>
#include <qgeoboundingcircle.h>
#include <qgeoboundingbox.h>

QT_USE_NAMESPACE

class tst_QPlaceSearchRequest : public QObject
{
    Q_OBJECT

public:
    tst_QPlaceSearchRequest();

private Q_SLOTS:
    void constructorTest();
    void searchTermTest();
    void categoriesTest();
    void boundingCircleTest();
    void boundingBoxTest();
    void searchAreaTest();
    void maximumCorrectionsTest();
    void visibilityScopeTest();
    void relevanceHintTest();
    void operatorsTest();
    void clearTest();
};

tst_QPlaceSearchRequest::tst_QPlaceSearchRequest()
{
}

void tst_QPlaceSearchRequest::constructorTest()
{
    QPlaceSearchRequest testObj;
    Q_UNUSED(testObj);

    QPlaceSearchRequest *testObjPtr = new QPlaceSearchRequest(testObj);
    QVERIFY2(testObjPtr != NULL, "Copy constructor - null");
    QVERIFY2(*testObjPtr == testObj, "Copy constructor - compare");
    delete testObjPtr;
}

void tst_QPlaceSearchRequest::searchTermTest()
{
    QPlaceSearchRequest testObj;
    QVERIFY2(testObj.searchTerm() == QString(), "Wrong default value");
    testObj.setSearchTerm("testText");
    QVERIFY2(testObj.searchTerm() == "testText", "Wrong value returned");
}

void tst_QPlaceSearchRequest::categoriesTest()
{
    QPlaceSearchRequest testObj;
    QVERIFY2(testObj.categories().count() == 0, "Wrong default value");
    QPlaceCategory cat;
    cat.setCategoryId("45346");
    testObj.setCategory(cat);
    QVERIFY2(testObj.categories().count() == 1, "Wrong categories count returned");
    QVERIFY2(testObj.categories()[0] == cat, "Wrong category returned");

    testObj.setCategory(QPlaceCategory());
    QVERIFY(testObj.categories().isEmpty());
}

void tst_QPlaceSearchRequest::boundingCircleTest()
{
    QPlaceSearchRequest query;
    QVERIFY2(query.searchArea() == NULL, "Wrong default value");
    QGeoBoundingCircle *circle = new QGeoBoundingCircle;
    circle->setCenter(QGeoCoordinate(30,20));
    circle->setRadius(500.0);
    query.setSearchArea(circle);

    QVERIFY(query.searchArea() != NULL);
    QVERIFY(query.searchArea()->type() == QGeoBoundingArea::CircleType);
    QVERIFY(query.searchArea() == circle);

    QGeoBoundingCircle *retrievedCircle =  static_cast<QGeoBoundingCircle *>(query.searchArea());
    QVERIFY2(retrievedCircle->center() == QGeoCoordinate(30,20), "Wrong value returned");
    QVERIFY2(retrievedCircle->radius() == 500.0, "Wrong value returned");
    query.clear();
    QVERIFY2(query.searchArea() == NULL, "Search area not cleared");
}

void tst_QPlaceSearchRequest::boundingBoxTest()
{
    QPlaceSearchRequest query;
    QVERIFY2(query.searchArea() == NULL, "Wrong default value");
    QGeoBoundingBox *box = new QGeoBoundingBox;

    box->setTopLeft(QGeoCoordinate(30,20));
    box->setBottomRight(QGeoCoordinate(10,50));
    query.setSearchArea(box);

    QVERIFY(query.searchArea() != NULL);
    QVERIFY(query.searchArea()->type() == QGeoBoundingArea::BoxType);
    QVERIFY(query.searchArea() == box);

    QGeoBoundingBox *retrievedBox = static_cast<QGeoBoundingBox*>(query.searchArea());
    QVERIFY2(retrievedBox->topLeft() == QGeoCoordinate(30,20), "Wrong value returned");
    QVERIFY2(retrievedBox->bottomRight() == QGeoCoordinate(10,50), "Wrong value returned");

    query.clear();
    QVERIFY2(query.searchArea() == NULL, "Wrong cleared value returned");
}

void tst_QPlaceSearchRequest::searchAreaTest()
{
    //test assignment of new search area over an old search area
    QPlaceSearchRequest *query = new QPlaceSearchRequest;
    QGeoBoundingCircle *circle = new QGeoBoundingCircle;
    circle->setCenter(QGeoCoordinate(30,20));
    circle->setRadius(500.0);
    query->setSearchArea(circle);

    QVERIFY(query->searchArea() == circle);
    QGeoBoundingBox *box = new QGeoBoundingBox;
    box->setTopLeft(QGeoCoordinate(30,20));
    box->setBottomRight(QGeoCoordinate(10,50));
    query->setSearchArea(box);
    QVERIFY2(query->searchArea() == box, "New search area not assigned");
}

void tst_QPlaceSearchRequest::maximumCorrectionsTest()
{
    QPlaceSearchRequest testObj;
    QVERIFY2(testObj.maximumCorrections() == 0, "Wrong default value");
    testObj.setMaximumCorrections(10);
    QVERIFY2(testObj.maximumCorrections() == 10, "Wrong value returned");
    testObj.clear();
    QVERIFY2(testObj.maximumCorrections() == 0, "Wrong cleared value returned");
}

void tst_QPlaceSearchRequest::visibilityScopeTest()
{
    QPlaceSearchRequest query;
    QVERIFY2(query.visibilityScope() == QtLocation::UnspecifiedVisibility, "Wrong default value");

    query.setVisibilityScope(QtLocation::DeviceVisibility);
    QCOMPARE(query.visibilityScope(), QtLocation::DeviceVisibility);

    query.setVisibilityScope(QtLocation::DeviceVisibility | QtLocation::PublicVisibility);
    QVERIFY(query.visibilityScope() & QtLocation::DeviceVisibility);
    QVERIFY(!(query.visibilityScope() & QtLocation::PrivateVisibility));
    QVERIFY(query.visibilityScope() & QtLocation::PublicVisibility);
}

void tst_QPlaceSearchRequest::relevanceHintTest()
{
    QPlaceSearchRequest request;
    QCOMPARE(request.relevanceHint(), QPlaceSearchRequest::UnspecifiedHint);
    request.setRelevanceHint(QPlaceSearchRequest::DistanceHint);
    QCOMPARE(request.relevanceHint(), QPlaceSearchRequest::DistanceHint);
    request.setRelevanceHint(QPlaceSearchRequest::UnspecifiedHint);
    QCOMPARE(request.relevanceHint(), QPlaceSearchRequest::UnspecifiedHint);
}

void tst_QPlaceSearchRequest::operatorsTest()
{
    QPlaceSearchRequest testObj;
    testObj.setSearchTerm("testValue");
    QPlaceSearchRequest testObj2;
    testObj2 = testObj;
    QVERIFY2(testObj == testObj2, "Not copied correctly");
    testObj2.setMaximumCorrections(-5);
    QVERIFY2(testObj != testObj2, "Object should be different");
    testObj2.setMaximumCorrections(0);
    QVERIFY(testObj == testObj2);

    QGeoBoundingBox *b1 = new QGeoBoundingBox(QGeoCoordinate(20,20), QGeoCoordinate(10,30));
    QGeoBoundingBox *b2 = new QGeoBoundingBox(QGeoCoordinate(20,20), QGeoCoordinate(10,30));
    QGeoBoundingBox *b3 = new QGeoBoundingBox(QGeoCoordinate(40,40), QGeoCoordinate(10,40));

    //testing that identical boxes match
    testObj.setSearchArea(b1);
    testObj2.setSearchArea(b2);
    QVERIFY2(testObj == testObj2, "Identical box areas are not identified as matching");

    //test that different boxes do not match
    testObj2.setSearchArea(b3);
    QVERIFY2(testObj != testObj2, "Different box areas identified as matching");

    QGeoBoundingCircle *c1 = new QGeoBoundingCircle(QGeoCoordinate(5,5),500);
    QGeoBoundingCircle *c2 = new QGeoBoundingCircle(QGeoCoordinate(5,5),500);
    QGeoBoundingCircle *c3 = new QGeoBoundingCircle(QGeoCoordinate(9,9),600);

    //test that identical cirlces match
    testObj.setSearchArea(c1);
    testObj2.setSearchArea(c2);
    QVERIFY2(testObj == testObj2, "Identical circle areas are not identified as matching");

    //test that different circle don't match
    testObj2.setSearchArea(c3);
    QVERIFY2(testObj != testObj2, "Different circle areas identified as matching");

    //test that circles and boxes do not match
    QGeoBoundingBox *b4 = new QGeoBoundingBox(QGeoCoordinate(20,20),QGeoCoordinate(10,30));
    QGeoBoundingCircle *c4 = new QGeoBoundingCircle(QGeoCoordinate(20,20),500);
    testObj.setSearchArea(b4);
    testObj2.setSearchArea(c4);
    QVERIFY2(testObj != testObj2, "Circle and box identified as matching");

    //test that identical visibility scopes match
    testObj.clear();
    testObj2.clear();
    testObj.setVisibilityScope(QtLocation::PublicVisibility);
    testObj2.setVisibilityScope(QtLocation::PublicVisibility);
    QVERIFY2(testObj == testObj2, "Identical scopes not identified as matching");

    //test that different scopes do not match
    testObj2.setVisibilityScope(QtLocation::PrivateVisibility);
    QVERIFY2(testObj != testObj2, "Different scopes identified as matching");
}

void tst_QPlaceSearchRequest::clearTest()
{
    QPlaceSearchRequest req;
    req.setSearchTerm("pizza");
    req.setSearchArea(new QGeoBoundingCircle(QGeoCoordinate(1,1), 5000));
    QPlaceCategory category;
    category.setName("Fast Food");
    req.setCategory(category);
    req.setMaximumCorrections(5);
    req.setLimit(100);
    req.setOffset(5);

    req.clear();
    QVERIFY(req.searchTerm().isEmpty());
    QVERIFY(req.searchArea() == 0);
    QVERIFY(req.categories().isEmpty());
    QVERIFY(req.maximumCorrections() == 0);
    QVERIFY(req.limit() == -1);
    QVERIFY(req.offset() == 0);
}

QTEST_APPLESS_MAIN(tst_QPlaceSearchRequest);

#include "tst_qplacesearchrequest.moc"
