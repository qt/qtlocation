/****************************************************************************
**
** Copyright (C) 2017 Mapbox, Inc.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtFoo module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include "qplacesearchsuggestionreplymapbox.h"
#include "qplacemanagerenginemapbox.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtNetwork/QNetworkReply>
#include <QtPositioning/QGeoCircle>
#include <QtPositioning/QGeoRectangle>
#include <QtLocation/QPlaceResult>
#include <QtLocation/QPlaceSearchRequest>

QT_BEGIN_NAMESPACE

QPlaceSearchSuggestionReplyMapbox::QPlaceSearchSuggestionReplyMapbox(QNetworkReply *reply, QPlaceManagerEngineMapbox *parent)
:   QPlaceSearchSuggestionReply(parent)
{
    Q_ASSERT(parent);
    if (!reply) {
        setError(UnknownError, QStringLiteral("Null reply"));
        return;
    }

    connect(reply, &QNetworkReply::finished, this, &QPlaceSearchSuggestionReplyMapbox::onReplyFinished);
    connect(reply, &QNetworkReply::errorOccurred, this, &QPlaceSearchSuggestionReplyMapbox::onNetworkError);

    connect(this, &QPlaceReply::aborted, reply, &QNetworkReply::abort);
    connect(this, &QObject::destroyed, reply, &QObject::deleteLater);
}

QPlaceSearchSuggestionReplyMapbox::~QPlaceSearchSuggestionReplyMapbox()
{
}

void QPlaceSearchSuggestionReplyMapbox::setError(QPlaceReply::Error errorCode, const QString &errorString)
{
    QPlaceReply::setError(errorCode, errorString);
    emit error(errorCode, errorString);

    setFinished(true);
    emit finished();
}

void QPlaceSearchSuggestionReplyMapbox::onReplyFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError)
        return;

    QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
    if (!document.isObject()) {
        setError(ParseError, tr("Response parse error"));
        return;
    }

    const QJsonArray features = document.object().value(QStringLiteral("features")).toArray();

    QStringList suggestions;
    for (const QJsonValue &feature : features) {
        if (feature.isObject())
            suggestions.append(feature.toObject().value(QStringLiteral("text")).toString());
    }

    setSuggestions(suggestions);

    setFinished(true);
    emit finished();
}

void QPlaceSearchSuggestionReplyMapbox::onNetworkError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error);
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    setError(CommunicationError, reply->errorString());
}

QT_END_NAMESPACE
