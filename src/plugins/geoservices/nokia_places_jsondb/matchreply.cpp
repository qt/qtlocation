/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "matchreply.h"
#include "jsonconverter.h"

#include <QtAddOnJsonDb/jsondb-client.h>
#include <QtCore/qnumeric.h>
#include <QtCore/QDebug>
#include <QtLocation/QGeoBoundingCircle>

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
    connect(db(), SIGNAL(response(int,QVariant)), this, SLOT(processResponse(int,QVariant)));
    connect(db(), SIGNAL(error(int,int,QString)), this, SLOT(processError(int,int,QString)));

    m_inputPlaces = request().places();

    if (m_inputPlaces.count() > 0) {
        if (request().parameters().contains(QPlaceMatchRequest::AlternativeId)) {
            QString alternativeId = m_inputPlaces.takeFirst().placeId();
            findPlace(alternativeId);
        } else if (request().parameters().contains(QLatin1String("proximityRange"))) {
            findPlaceByProximity();
        } else {
            for (int i=0; i < m_inputPlaces.count(); ++i)
                m_outputPlaces.append(QPlace());
            triggerDone();
        }
    } else {
        triggerDone();
    }
}

void MatchReply::processResponse(int id, const QVariant &data)
{
    if (id != m_reqId)
        return;

    if (request().parameters().contains(QPlaceMatchRequest::AlternativeId)) {
        if (data.toMap().value(JsonConverter::Length).toInt() > 0) {
            QList<QPlace> places = JsonConverter::convertJsonResponseToPlaces(data, m_engine);
            m_outputPlaces.append(places.first());
        } else {
            m_outputPlaces.append(QPlace());
        }

        if (!m_inputPlaces.isEmpty()) {
            QString alternativeId = m_inputPlaces.takeFirst().placeId();
            findPlace(alternativeId);
            return;
        } else {
            setPlaces(m_outputPlaces);
            triggerDone();
        }
    } else if (request().parameters().contains(QLatin1String("proximityRange"))) {
        qreal range = request().parameters().value(QLatin1String("proximityRange")).toReal();

        //TODO: optimize proximity match
        if (data.toMap().value(JsonConverter::Length).toInt() > 0) {
            QList<QPlace> candidatePlaces = JsonConverter::convertJsonResponseToPlaces(data, m_engine);

            bool isFound;
            foreach (const QPlace &m_inputPlace, request().places()) {
                isFound = false;
                foreach (const QPlace &candidatePlace, candidatePlaces) {
                    qreal dist = candidatePlace.location().coordinate().distanceTo(m_inputPlace.location().coordinate());
                    if ((dist < range) || qFuzzyCompare(dist, range)) {
                        m_outputPlaces.append(candidatePlace);
                        isFound = true;
                        break;
                    }
                }
                if (!isFound)
                    m_outputPlaces.append(QPlace());
            }
        } else {
            //no matches, therefore output places are filled with a default constructed place for each input place
            for (int i=0; i < m_inputPlaces.count(); ++i)
                m_outputPlaces.append(QPlace());
        }

        setPlaces(m_outputPlaces);
        triggerDone();
    }
}

void MatchReply::processError(int id, int code, const QString &jsonDbErrorString)
{
    Q_UNUSED(id);

    QPlaceReply::Error error = QPlaceReply::UnknownError;
    QString errorString = QString::fromLatin1("Unknown error occurred operation: jsondb error code =%1, erroString=%2").
                  arg(code).arg(jsonDbErrorString);
    triggerDone(error, errorString);
}

void MatchReply::findPlace(const QString &alternativeId)
{
    QString extId = QString(JsonConverter::ExtendedAttributes) + QLatin1String(".")
            + request().parameters().value(QPlaceMatchRequest::AlternativeId).toString() + QLatin1String(".") +
                                   JsonConverter::Text;

    QString queryString = QString::fromLatin1("[?%1 = %placeType]").arg(JsonConverter::Type)
            + QString::fromLatin1("[?%1 = %expectedId]").arg(extId);

    QVariantMap bindingsMap;
    bindingsMap.insert(QLatin1String("placeType"), JsonConverter::PlaceType);
    bindingsMap.insert(QLatin1String("expectedId"), alternativeId);
    QVariantMap queryObj;
    queryObj.insert(JsonConverter::Query, queryString);
    queryObj.insert(JsonConverter::Bindings, bindingsMap);
    m_state = MatchReply::GetPlaces;
    m_reqId = db()->find(queryObj);
}

void MatchReply::findPlaceByProximity()
{
    //TODO: optimize proximity match
    QString queryString = QString::fromLatin1("[?%1= %placeType]").arg(JsonConverter::Type);
    QVariantMap bindingsMap;
    bindingsMap.insert(QLatin1String("placeType"), JsonConverter::PlaceType);

    QVariantMap queryObj;
    queryObj.insert(JsonConverter::Query, queryString);
    queryObj.insert(JsonConverter::Bindings, bindingsMap);
    m_state = MatchReply::GetPlaces;
    m_reqId = db()->find(queryObj);
}

