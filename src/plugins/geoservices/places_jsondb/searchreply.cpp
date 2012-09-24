/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtLocation module of the Qt Toolkit.
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

#include "searchreply.h"
#include "traverser.h"

#include <QtCore/qnumeric.h>
#include <QtCore/QDebug>
#include <QtLocation/QGeoCircle>
#include <QtJsonDb/QJsonDbReadRequest>
#include <QtLocation/QPlaceResult>

SearchReply::SearchReply(QPlaceManagerEngineJsonDb *engine)
    : QPlaceSearchReply(engine), m_engine(engine)
{
}

SearchReply::~SearchReply()
{
}

void SearchReply::setResults(const QList<QPlaceResult> &results)
{
    QList<QPlaceSearchResult> searchResults;
    foreach (const QPlaceResult &placeResult, results) {
        searchResults << placeResult;
    }

    QPlaceSearchReply::setResults(searchResults);
}

void SearchReply::setRequest(const QPlaceSearchRequest &request)
{
    QPlaceSearchReply::setRequest(request);
}

void SearchReply::start()
{
    if (request().searchArea().type() == QGeoShape::RectangleType) {
        QGeoRectangle box(request().searchArea());
        if (!box.isValid()) {
            triggerDone(QPlaceReply::BadArgumentError,
                        QString::fromLatin1("Bounding box search area is invalid"));
            return;
        }
    } else if (request().searchArea().type() == QGeoShape::CircleType) {
        QGeoCircle circle(request().searchArea());
        if (!circle.center().isValid() || qIsNaN(circle.center().latitude()) || qIsNaN(circle.center().longitude())) {
            triggerDone(QPlaceReply::BadArgumentError,
                        QString::fromLatin1("The center of the search area is an invalid coordinate"));
            return;
        }
        if (circle.contains(QGeoCoordinate(90,0)) || circle.contains(QGeoCoordinate(-90,0))) {
            triggerDone(QPlaceReply::BadArgumentError,
                        QString::fromLatin1("The search area contains the north or south pole"));
            return;
        }
    }

    if (!request().categories().isEmpty()) {
        foreach (const QPlaceCategory &category, request().categories()) {
            m_catSearchIds.append(categorySearchIds(category.categoryId()));
            db()->searchForPlaces(request(), this, SLOT(searchFinished()), m_catSearchIds);
        }
    } else {
        db()->searchForPlaces(request(), this, SLOT(searchFinished()));
    }
}

QStringList SearchReply::categorySearchIds(const QString &catId)
{
    QStringList ids;
    ids << catId;
    QStringList childIds =  m_engine->childCategoryIds(catId);
    foreach (const QString &childId, childIds)
        ids.append(categorySearchIds(childId));

    return ids;
}

static bool lessThanDistance(const QPlaceSearchResult &p0, const QPlaceSearchResult &p1)
{
    Q_ASSERT(p0.type() == QPlaceSearchResult::PlaceResult);
    Q_ASSERT(p1.type() == QPlaceSearchResult::PlaceResult);
    QPlaceResult r0 = p0;
    QPlaceResult r1 = p1;

    return r0.distance() < r1.distance();
}

static bool lessThanName(const QPlaceSearchResult &p0, const QPlaceSearchResult &p1)
{
    return p0.title() <  p1.title();
}

void SearchReply::searchFinished()
{
    QJsonDbRequest *jsonDbRequest = qobject_cast<QJsonDbRequest *>(sender());
    Q_ASSERT(jsonDbRequest);
    QList<QJsonObject> jsonResults = jsonDbRequest->takeResults();

    QList<QPlace> places = JsonDb::convertJsonObjectsToPlaces(jsonResults, m_engine);
    QList<QPlaceResult> results;
    QPlaceResult result;

    const QGeoShape &area = request().searchArea();
    const QGeoShape::ShapeType &type = area.type();
    const QGeoCoordinate &center = type == QGeoShape::CircleType ? static_cast<QGeoCircle>(area).center() :
                                   type == QGeoShape::RectangleType ? static_cast<QGeoRectangle>(area).center() :
                                                                       QGeoCoordinate();
    const bool noDistanceFilter = type == QGeoShape::UnknownType
                                  || (type == QGeoShape::CircleType
                                      && static_cast<QGeoCircle>(area).radius() < 0.0);

    // First filter the results
    foreach (const QPlace &place, places) {
        const QGeoCoordinate &coord = place.location().coordinate();
        if (noDistanceFilter || area.contains(coord)) {
            // distance is invalid if center is invalid/not set
            qreal distance = center.distanceTo(coord);
            result.setTitle(place.name());
            result.setIcon(place.icon());
            result.setPlace(place);
            result.setDistance(distance);
            results.append(result);
        }
    }

    // Now sort the result set
    switch (request().relevanceHint() ) {
    case QPlaceSearchRequest::DistanceHint:
        qSort(results.begin(), results.end(), lessThanDistance);
        break;
    case QPlaceSearchRequest::LexicalPlaceNameHint:
        qSort(results.begin(), results.end(), lessThanName);
        break;
    default:
        break;
    }

    setResults(results);

    //See if we have to fetch any category data
    QStringList categoryUuids;
    foreach (const QPlaceResult &result, results) {
        if (!result.place().categories().isEmpty()) {
            foreach (const QPlaceCategory &category, result.place().categories())
                categoryUuids.append(category.categoryId());
        }
        categoryUuids.removeDuplicates();
    }

    if (!categoryUuids.isEmpty()) {
        db()->getCategories(categoryUuids,
                            this, SLOT(getCategoriesForPlacesFinished()));
        return;
    } else {
        triggerDone();
        return;
    }
}

void SearchReply::getCategoriesForPlacesFinished()
{
    QJsonDbRequest *request = qobject_cast<QJsonDbRequest *>(sender());
    Q_ASSERT(request);
    QList<QJsonObject> jsonResults = request->takeResults();

    QMap<QString, QPlaceCategory> categoriesCollection;
    foreach (const QJsonObject &categoryJson, jsonResults) {
        if (!categoriesCollection.contains(categoryJson.value(JsonDb::Uuid).toString())) {
            QPlaceCategory category = JsonDb::convertJsonObjectToCategory(categoryJson, m_engine);
            categoriesCollection.insert(category.categoryId(), category);
        }
    }

    QList<QPlaceSearchResult> resultList = results();
    QList<QPlaceCategory> categories;
    QPlace place;
    for (int i=0; i < resultList.count(); ++i) {
        categories.clear();
        place = static_cast<QPlaceResult>(resultList.at(i)).place();
        foreach (const QPlaceCategory &cat, place.categories()) {
            if (!cat.categoryId().isEmpty() && categoriesCollection.contains(cat.categoryId()))
                categories.append(categoriesCollection.value(cat.categoryId()));
        }
        place.setCategories(categories);
        static_cast<QPlaceResult &>(resultList[i]).setPlace(place);
    }
    setResults(resultList);
    triggerDone();
    return;
}

void SearchReply::requestError(QJsonDbRequest::ErrorCode code, const QString &errorString)
{
    QString errorString_ = QString::fromLatin1("Unknown error occurred operation: jsondb error code =%1, erroString=%2").
                  arg(code).arg(errorString);
    triggerDone(QPlaceReply::UnknownError, errorString_);
}
