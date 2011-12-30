/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtLocation module of the Qt Toolkit.
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

#include "searchreply.h"

#include <QtAddOnJsonDb/jsondb-client.h>
#include <QtCore/qnumeric.h>
#include <QtCore/QDebug>
#include <QtLocation/QGeoBoundingCircle>

SearchReply::SearchReply(QPlaceManagerEngineJsonDb *engine)
    : QPlaceSearchReply(engine), m_engine(engine)
{
}

SearchReply::~SearchReply()
{
}

void SearchReply::setResults(const QList<QPlaceSearchResult> &results)
{
    QPlaceSearchReply::setResults(results);
}

void SearchReply::setRequest(const QPlaceSearchRequest &request)
{
    QPlaceSearchReply::setRequest(request);
}

void SearchReply::start()
{
    connect(db(), SIGNAL(response(int,QVariant)), this, SLOT(processResponse(int,QVariant)));
    connect(db(), SIGNAL(error(int,int,QString)), this, SLOT(processError(int,int,QString)));

    if (request().searchArea() != 0) {
        QGeoBoundingArea *searchArea = request().searchArea();
        if (searchArea->type() == QGeoBoundingArea::BoxType) {
            QGeoBoundingBox *box = static_cast<QGeoBoundingBox *>(searchArea);
            if (!box->isValid()) {
                triggerDone(QPlaceReply::BadArgumentError,
                                QString::fromLatin1("Bounding box search area is invalid"));
                return;
            }
        } else if (searchArea->type() == QGeoBoundingArea::CircleType) {
            QGeoBoundingCircle *circle = static_cast<QGeoBoundingCircle *>(searchArea);
            if (!circle->center().isValid() || qIsNaN(circle->center().latitude()) || qIsNaN(circle->center().longitude())) {
                triggerDone(QPlaceReply::BadArgumentError,
                            QString::fromLatin1("The center of the search area is an invalid coordinate"));
                return;
            }
            if (circle->contains(QGeoCoordinate(90,0)) || circle->contains(QGeoCoordinate(-90,0))) {
                triggerDone(QPlaceReply::BadArgumentError,
                            QString::fromLatin1("The search area contains the north or south pole"));
                return;
            }
        }
    }

    QString queryString = JsonConverter::convertToQueryString(request());
    if (!request().categories().isEmpty()) {
        for (int i = 0;  i < request().categories().count(); ++i) {
            if (i == 0) {
                queryString += QString::fromLatin1("[?%1 contains \"%2\" ")
                        .arg(JsonConverter::AllCategoryUuids).arg(request().categories().at(i).categoryId());
            } else {
                queryString += QString::fromLatin1("| ?%1 contains \"%2\" ")
                .arg(JsonConverter::AllCategoryUuids).arg(request().categories().at(i).categoryId());
            }
        }
        queryString += QString::fromLatin1("]");
    }

    m_state = SearchReply::Search;
    m_reqId = db()->query(queryString);
}

void SearchReply::processResponse(int id, const QVariant &data)
{
    if (id != m_reqId)
        return;

    if (m_state == SearchReply::GetCategories) {
        QVariantList categoriesJson = data.toMap().value(QLatin1String("data")).toList();
        QMap<QString, QPlaceCategory> categoriesCollection;
        QPlaceCategory cat;
        foreach (const QVariant categoryJson, categoriesJson) {
            cat = JsonConverter::convertJsonMapToCategory(categoryJson.toMap(), m_engine);
            categoriesCollection.insert(cat.categoryId(), cat);
        }

        QList<QPlaceSearchResult> resultList = results();
        QList<QPlaceCategory> categories;
        QPlace place;
        for (int i=0; i < resultList.count(); ++i) {
            categories.clear();
            place = resultList.at(i).place();
            foreach (const QPlaceCategory &cat, place.categories()) {
                if (!cat.categoryId().isEmpty() && categoriesCollection.contains(cat.categoryId()))
                    categories.append(categoriesCollection.value(cat.categoryId()));
            }
            place.setCategories(categories);
            resultList[i].setPlace(place);
        }
        setResults(resultList);
        triggerDone();
        return;
    } else if (m_state == SearchReply::Search) {
        QList<QPlace> places = JsonConverter::convertJsonResponseToPlaces(data, m_engine);
        QList<QPlaceSearchResult> results;
        QPlaceSearchResult result;
        result.setType(QPlaceSearchResult::PlaceResult);
        bool resultsAlreadySet = false;

        if (request().searchArea() != 0) {
            QGeoBoundingArea *searchArea = request().searchArea();
            if (searchArea->type() == QGeoBoundingArea::CircleType) {
                QGeoBoundingCircle *circle = static_cast<QGeoBoundingCircle*>(searchArea);

                QPlace place;
                for (int i=0; i < places.count(); ++i) {
                    place = places.at(i);
                    qreal dist = circle->center().distanceTo(place.location().coordinate());
                    if (dist < circle->radius() || qFuzzyCompare(dist, circle->radius()) || circle->radius() < 0.0) {
                        result.setDistance(dist);
                        result.setPlace(place);

                        if (request().relevanceHint() == QPlaceSearchRequest::DistanceHint) {
                            //TODO: we can optimize this insertion sort
                            bool added = false;
                            for (int i=0; i < results.count(); ++i) {
                                if (result.distance() < results.at(i).distance() || qFuzzyCompare(result.distance(),results.at(i).distance())) {
                                    results.insert(i, result);
                                    added = true;
                                    break;
                                }
                            }
                            if (!added)
                                results.append(result);

                        } else {
                            results.append(result);
                        }
                    }
                }
                resultsAlreadySet = true;

                //TODO: we can optimize this using a bounding box to cull candidates
                //      and then use distance comparisons for the rest.

            } else if (searchArea->type() == QGeoBoundingArea::BoxType) {
                //There seem to be some issues with using the comparison operators
                //so for now we filter in the plugin code
                QGeoBoundingBox *box = static_cast<QGeoBoundingBox *>(searchArea);
                double tly = box->topLeft().latitude();
                double bry = box->bottomRight().latitude();
                double tlx = box->topLeft().longitude();
                double brx = box->bottomRight().longitude();

                QGeoCoordinate coord;
                foreach (const QPlace &place, places) {
                    coord = place.location().coordinate();
                    if (coord.latitude() > tly)
                        places.removeAll(place);
                    if (coord.latitude() < bry)
                        places.removeAll(place);

                    bool lonWrap = (tlx > brx); //box wraps over the dateline

                    if (!lonWrap) {
                        if (coord.longitude() < tlx || coord.longitude() > brx) {
                            places.removeAll(place);
                        }
                    } else {
                        if (coord.longitude() < tlx && coord.longitude() > brx) {
                            places.removeAll(place);
                        }
                    }
                }
            }
        }

        //if we haven't already filled in the results, fill them in now
        if (!resultsAlreadySet) {
            foreach (const QPlace &place, places) {
                result.setPlace(place);
                results.append(result);
            }
        }

        setResults(results);

        //See if we have to fetch any category data
        QStringList categoryUuids;
        foreach (const QPlaceSearchResult &result, results) {
            if (!result.place().categories().isEmpty()) {
                foreach (const QPlaceCategory &category, result.place().categories())
                    categoryUuids.append(category.categoryId());
            }
            categoryUuids.removeDuplicates();
        }

        if (!categoryUuids.isEmpty()) {
            QString queryString = QString::fromLatin1("[?%1 in [\"%2\"]]")
            .arg(JsonConverter::Uuid).arg(categoryUuids.join(","));
            m_state = SearchReply::GetCategories;
            m_reqId = db()->query(queryString);
            return;
        } else {
            triggerDone();
            return;
        }
    }
}

void SearchReply::processError(int id, int code, const QString &jsonDbErrorString)
{
    QPlaceReply::Error error = QPlaceReply::UnknownError;
    QString errorString = QString::fromLatin1("Unknown error occurred operation: jsondb error code =%1, erroString=%2").
                  arg(code).arg(jsonDbErrorString);
    triggerDone(error, errorString);
}

