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
****************************************************************************/

#include "detailsreply.h"
#include "qplacemanagerengine_jsondb.h"
#include <QDebug>
#include <jsondb-client.h>

DetailsReply::DetailsReply(QPlaceManagerEngineJsonDb *engine)
    : QPlaceDetailsReply(engine), m_engine(engine)
{
}

DetailsReply::~DetailsReply()
{
}

void DetailsReply::setPlaceId(const QString &placeId)
{
    m_placeId = placeId;
}

void DetailsReply::start()
{
    connect(db(), SIGNAL(response(int,QVariant)), this, SLOT(processResponse(int,QVariant)));
    connect(db(), SIGNAL(error(int,int,QString)), this, SLOT(processError(int,int,QString)));

    m_state = DetailsReply::GetPlace;
    m_reqId = db()->query(QString::fromLatin1("[?%1 = \"%2\"][?%3 = \"%4\"]").arg(JsonConverter::Type).arg(JsonConverter::PlaceType)
                          .arg(JsonConverter::Uuid).arg(m_placeId));
}

void DetailsReply::processResponse(int id, const QVariant &data)
{
    if (id != m_reqId)
        return;
    switch (m_state) {
    case (DetailsReply::GetPlace) : {
        if (data.toMap().value(JsonConverter::Length).toInt() <= 0) {
            triggerDone(QPlaceReply::PlaceDoesNotExistError,
                                      QString::fromLatin1("Specified place does not exist"));
            return;
        } else {
            QVariantMap placeMap = data.toMap().value(JsonConverter::Data).toList().first().toMap();
            setPlace(JsonConverter::convertJsonMapToPlace(placeMap, m_engine));
            QVariantList categoryUuids = placeMap.value(JsonConverter::CategoryUuids).toList();
            if (!categoryUuids.isEmpty()) {
                //need to retrieve categories
                QString queryString = QString::fromLatin1("[?%1=\"%2\"]").arg(JsonConverter::Type).arg(JsonConverter::CategoryType)
                                    + QString::fromLatin1("[?%1 in %categoryUuids]").arg(JsonConverter::Uuid);

                QVariantMap bindingsMap;
                bindingsMap.insert(QLatin1String("categoryUuids"), categoryUuids);

                QVariantMap queryObj;
                queryObj.insert(JsonConverter::Query, queryString);
                queryObj.insert(JsonConverter::Bindings, bindingsMap);

                m_state = DetailsReply::GetCategories;
                m_reqId = db()->find(queryObj);
                return;
            } else { //don't need to retrieve categores so return the details
                triggerDone();
                return;
            }
        }
        break;
    } case (DetailsReply::GetCategories): {
        QList<QVariantMap> categoriesJson =JsonConverter::convertToListOfMaps(data);
        QList<QPlaceCategory> categories;
        foreach (const QVariantMap categoryMap, categoriesJson)
            categories.append(JsonConverter::convertJsonMapToCategory(categoryMap, m_engine));

        QPlace p = place();
        p.setCategories(categories);
        setPlace(p);
        triggerDone();
        return;
    } default:
        triggerDone(QPlaceReply::UnknownError, QLatin1String("Unknown state encountered when retrieving place details"));
        return;
    }
}

void DetailsReply::processError(int id, int code, const QString &jsonDbErrorString)
{
    if (id != m_reqId)
        return;
    QPlaceReply::Error error = QPlaceReply::UnknownError;
    QString errorStr = QString::fromLatin1("Unknown error during details fetch operation: jsondb error code =%1, erroString=%2").
                  arg(code).arg(jsonDbErrorString);
    triggerDone(error, errorStr);
}


