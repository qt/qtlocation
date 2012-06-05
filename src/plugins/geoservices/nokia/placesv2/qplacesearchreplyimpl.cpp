/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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
** This file is part of the Nokia services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file NOKIA_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Nokia services
** plugin source code.
**
****************************************************************************/

#include "qplacesearchreplyimpl.h"
#include "jsonparserhelpers.h"
#include "../qplacemanagerengine_nokiav2.h"
#include "../qgeoerror_messages.h"

#include <QCoreApplication>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtLocation/QPlaceIcon>

#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

QPlaceSearchReplyImpl::QPlaceSearchReplyImpl(QNetworkReply *reply,
                                             QPlaceManagerEngineNokiaV2 *parent)
    :   QPlaceSearchReply(parent), m_reply(reply), m_engine(parent)
{
    Q_ASSERT(parent);

    if (!m_reply)
        return;

    m_reply->setParent(this);
    connect(m_reply, SIGNAL(finished()), this, SLOT(replyFinished()));
    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(replyError(QNetworkReply::NetworkError)));
}

QPlaceSearchReplyImpl::~QPlaceSearchReplyImpl()
{
}

void QPlaceSearchReplyImpl::abort()
{
    if (m_reply)
        m_reply->abort();
}

void QPlaceSearchReplyImpl::setError(QPlaceReply::Error error_, const QString &errorString)
{
    QPlaceReply::setError(error_, errorString);
    emit error(error_, errorString);
    setFinished(true);
}

void QPlaceSearchReplyImpl::replyFinished()
{
    QJsonDocument document = QJsonDocument::fromJson(m_reply->readAll());
    if (!document.isObject()) {
        setError(ParseError, QCoreApplication::translate(NOKIA_PLUGIN_CONTEXT_NAME, PARSE_ERROR));
        emit error(error(), errorString());
        return;
    }

    QJsonObject object = document.object();

    //QJsonObject searchObject = object.value(QLatin1String("search")).toObject();

    QJsonObject resultsObject = object.value(QLatin1String("results")).toObject();
    QJsonArray items = resultsObject.value(QLatin1String("items")).toArray();

    QList<QPlaceSearchResult> results;
    for (int i = 0; i < items.count(); ++i) {
        QJsonObject item = items.at(i).toObject();

        QPlaceSearchResult result;

        result.setType(QPlaceSearchResult::PlaceResult);

        if (item.contains(QLatin1String("distance")))
            result.setDistance(item.value(QLatin1String("distance")).toDouble());

        QPlace place;

        QGeoLocation location;

        location.setCoordinate(parseCoordinate(item.value(QLatin1String("position")).toArray()));

        const QString vicinity = item.value(QLatin1String("vicinity")).toString();
        QGeoAddress address;
        address.setText(vicinity);
        location.setAddress(address);

        if (item.contains(QLatin1String("bbox"))) {
            QJsonArray bbox = item.value(QLatin1String("bbox")).toArray();
            QGeoBoundingBox box(QGeoCoordinate(bbox.at(3).toDouble(), bbox.at(0).toDouble()),
                                QGeoCoordinate(bbox.at(1).toDouble(), bbox.at(2).toDouble()));
            location.setBoundingBox(box);
        }

        place.setLocation(location);

        QPlaceRatings ratings;
        ratings.setAverage(item.value(QLatin1String("averageRating")).toDouble());
        ratings.setMaximum(5.0);
        place.setRatings(ratings);

        place.setName(item.value(QLatin1String("title")).toString());

        place.setIcon(m_engine->icon(item.value(QLatin1String("icon")).toString()));

        place.setCategory(parseCategory(item.value(QLatin1String("category")).toObject(),
                                        m_engine));

        //QJsonArray having = item.value(QLatin1String("having")).toArray();

        result.setSponsored(item.value(QLatin1String("sponsored")).toBool());

        QUrl href = item.value(QLatin1String("href")).toString();
        //QUrl type = item.value(QLatin1String("type")).toString();

        place.setPlaceId(href.path().mid(18, 41));

        QPlaceAttribute provider;
        provider.setText(QLatin1String("nokia"));
        place.setExtendedAttribute(QPlaceAttribute::Provider, provider);

        result.setPlace(place);

        results.append(result);
    }

    setResults(results);

    m_reply->deleteLater();
    m_reply = 0;

    setFinished(true);
    emit finished();
}

void QPlaceSearchReplyImpl::replyError(QNetworkReply::NetworkError error)
{
    switch (error) {
    case QNetworkReply::OperationCanceledError:
        setError(CancelError, "Request canceled.");
        break;
    default:
        setError(CommunicationError, "Network error.");
    }
}

QT_END_NAMESPACE
