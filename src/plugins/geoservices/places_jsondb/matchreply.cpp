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

#include "matchreply.h"
#include "jsondb.h"

#include <QtCore/qnumeric.h>
#include <QtCore/QJsonArray>
#include <QtCore/QDebug>
#include <QtJsonDb/QJsonDbReadRequest>

MatchReply::MatchReply(QPlaceManagerEngineJsonDb *engine)
    : QPlaceMatchReply(engine), m_engine(engine)
{
}

MatchReply::~MatchReply()
{
}

void MatchReply::setPlaces(const QList<QPlace> &places)
{
    QPlaceMatchReply::setPlaces(places);
}

void MatchReply::setRequest(const QPlaceMatchRequest &request)
{
    QPlaceMatchReply::setRequest(request);
}

void MatchReply::start()
{
    if (!request().places().isEmpty()) {
        if (request().parameters().contains(QPlaceMatchRequest::AlternativeId)) {
            QStringList ids;
            foreach (const QPlace &place, request().places())
                ids.append(place.placeId());

            db()->findPlacesByAlternativeId(request().parameters().value(QPlaceMatchRequest::AlternativeId).toString(), ids,
                                                                      this, SLOT(findByAlternativeIdFinished()));
        } else if (request().parameters().contains(QLatin1String("proximityRange"))) {
            //we need to filter in plugin code so get all places and then filter by proximity
            db()->findAllPlaces(this, SLOT(findByProximityFinished()));
        } else {
            for (int i=0; i < request().places().count(); ++i)
                m_outputPlaces.append(QPlace());
            triggerDone();
        }
    } else {
        triggerDone();
    }
}

void MatchReply::findByAlternativeIdFinished()
{
    QJsonDbRequest *jsonDbRequest = qobject_cast<QJsonDbRequest *>(sender());
    Q_ASSERT(jsonDbRequest);
    QList<QJsonObject> results = jsonDbRequest->takeResults();

    QMap<QString, QPlace> placesMap;
    foreach (const QJsonObject &placeJson, results) {
        QPlace place = JsonDb::convertJsonObjectToPlace(placeJson, m_engine);
        if (!place.placeId().isEmpty()) {
            QString alternativeIdKey = request().parameters().value(QPlaceMatchRequest::AlternativeId).toString();
            placesMap.insert(place.extendedAttribute(alternativeIdKey).text(), place);
        }
    }

    foreach (const QPlace &place, request().places())
        m_outputPlaces.append(placesMap.value(place.placeId()));

    QStringList categoryUuids = JsonDb::categoryIds(m_outputPlaces);
    if (!categoryUuids.isEmpty()) {
        db()->getCategories(categoryUuids, this, SLOT(getCategoriesForPlacesFinished()));
    } else {
        setPlaces(m_outputPlaces);
        triggerDone();
    }
}

void MatchReply::findByProximityFinished()
{
    QJsonDbRequest *jsonDbRequest = qobject_cast<QJsonDbRequest *>(sender());
    Q_ASSERT(jsonDbRequest);
    QList<QJsonObject> jsonDbResults = jsonDbRequest->takeResults();

    qreal range = request().parameters().value(QLatin1String("proximityRange")).toReal();

    //TODO: optimize proximity match
    if (!jsonDbResults.isEmpty()) {
        QList<QPlace> candidatePlaces = JsonDb::convertJsonObjectsToPlaces(jsonDbResults, m_engine);
        foreach (const QPlace &m_inputPlace, request().places()) {
            bool isFound = false;
            foreach (const QPlace &candidatePlace, candidatePlaces) {
                if (m_inputPlace.location().coordinate().isValid() && candidatePlace.location().coordinate().isValid()) {
                    qreal dist = candidatePlace.location().coordinate().distanceTo(m_inputPlace.location().coordinate());
                    if ((dist < range) || qFuzzyCompare(dist, range)) {
                        m_outputPlaces.append(candidatePlace);
                        isFound = true;
                        break;
                    }
                }
            }
            if (!isFound)
                m_outputPlaces.append(QPlace());
        }
    } else {
        //no matches, therefore output places are filled with a default constructed place for each input place
        for (int i = 0; i < request().places().count(); ++i)
            m_outputPlaces.append(QPlace());
    }

    QStringList categoryUuids = JsonDb::categoryIds(m_outputPlaces);
    if (!categoryUuids.isEmpty()) {
        db()->getCategories(categoryUuids, this, SLOT(getCategoriesForPlacesFinished()));
    } else {
        setPlaces(m_outputPlaces);
        triggerDone();
    }
}

void MatchReply::getCategoriesForPlacesFinished()
{
    QJsonDbRequest *jsonDbRequest = qobject_cast<QJsonDbRequest *>(sender());
    Q_ASSERT(jsonDbRequest);
    QList<QJsonObject> jsonDbResults = jsonDbRequest->takeResults();

    QMap<QString, QPlaceCategory> categoryMap;
    foreach (const QJsonObject &jsonResult, jsonDbResults) {
        QPlaceCategory category = JsonDb::convertJsonObjectToCategory(jsonResult, m_engine);
        if (!category.categoryId().isEmpty())
            categoryMap.insert(category.categoryId(), category);
    }

    for (int i = 0; i < m_outputPlaces.count(); ++i) {
        QList<QPlaceCategory> categories;
        foreach (const QPlaceCategory &category, m_outputPlaces[i].categories())  {
            if (categoryMap.contains(category.categoryId())) {
                categories.append(categoryMap.value(category.categoryId()));
            }
        }
        m_outputPlaces[i].setCategories(categories);
    }

    setPlaces(m_outputPlaces);
    triggerDone();
}

void MatchReply::requestError(QJsonDbRequest::ErrorCode code, const QString &errorString)
{
    QString errorString_ = QString::fromLatin1("Unknown error occurred operation: jsondb error code =%1, erroString=%2").
                  arg(code).arg(errorString);
    triggerDone(QPlaceReply::UnknownError, errorString_);
}
