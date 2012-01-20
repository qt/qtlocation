/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: http://www.qt-project.org/
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
** This file is part of the Ovi services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file OVI_SERVICES_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Ovi services
** plugin source code.
**
****************************************************************************/

#include "qplacejsonrecommendationparser.h"

#include <QJSEngine>
#include <QJSValue>
#include <QJSValueIterator>

#include "qplacejsondetailsparser.h"
#include <qplace.h>

#if defined(QT_PLACES_LOGGING)
    #include <QDebug>
#endif

static const char *recommendations_element = "recommendations";
static const char *recommendations_nearby_element = "nearby";
static const char *recommendations_distance_element = "distance";
static const char *recommendations_place_element = "place";

QT_USE_NAMESPACE

QPlaceJSonRecommendationParser::QPlaceJSonRecommendationParser(QPlaceManager *manager, QObject *parent) :
    QPlaceJSonParser(parent), m_manager(manager)
{
}

QPlaceJSonRecommendationParser::~QPlaceJSonRecommendationParser()
{
}

QList<QPlaceSearchResult> QPlaceJSonRecommendationParser::results()
{
    return searchResults;
}

void QPlaceJSonRecommendationParser::processJSonData(const QJSValue &sv)
{
    searchResults.clear();

    if (sv.isValid()) {
        QJSValue sv2 = sv.property(recommendations_element);
        if (sv2.isValid()) {
            QJSValueIterator it(sv2.property(recommendations_nearby_element));
            while (it.hasNext()) {
                it.next();
                // array contains count as last element
                if (it.name() != "length") {
                    processResultElement(it.value());
                }
            }
            emit finished(NoError, QString());
            return;
        }
    }
    emit finished(ParsingError, QString("JSON data are invalid"));
}

void QPlaceJSonRecommendationParser::processResultElement(const QJSValue &value)
{
    QPlaceSearchResult result;
    result.setType(QPlaceSearchResult::PlaceResult);

    // Processing properties
    QJSValue distance = value.property(recommendations_distance_element);
    if (distance.isValid() && !distance.toString().isEmpty()) {
        bool isConverted;
        double distanceValue = distance.toString().toDouble(&isConverted);
        if (isConverted) {
            result.setDistance(distanceValue);
        }
    }
    QJSValue place = value.property(recommendations_place_element);
    if (place.isValid()) {
        QPlace newPlace = QPlaceJSonDetailsParser::buildPlace(place, m_manager);
        result.setPlace(newPlace);
        searchResults.append(result);
    }
}
