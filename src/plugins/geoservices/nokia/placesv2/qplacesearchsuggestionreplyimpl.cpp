/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
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
****************************************************************************/

#include "qplacesearchsuggestionreplyimpl.h"
#include "../qgeoerror_messages.h"

#include <QCoreApplication>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

QT_BEGIN_NAMESPACE

QPlaceSearchSuggestionReplyImpl::QPlaceSearchSuggestionReplyImpl(QNetworkReply *reply,
                                                                 QObject *parent)
:   QPlaceSearchSuggestionReply(parent)
{
    if (!reply) {
        setError(UnknownError, QStringLiteral("Null reply"));
        return;
    }
    connect(reply, SIGNAL(finished()), this, SLOT(replyFinished()));
    connect(reply, SIGNAL(errorOccurred(QNetworkReply::NetworkError)),
            this, SLOT(replyError(QNetworkReply::NetworkError)));
    connect(this, &QPlaceReply::aborted, reply, &QNetworkReply::abort);
    connect(this, &QObject::destroyed, reply, &QObject::deleteLater);
}

QPlaceSearchSuggestionReplyImpl::~QPlaceSearchSuggestionReplyImpl()
{
}

void QPlaceSearchSuggestionReplyImpl::setError(QPlaceReply::Error error_,
                                               const QString &errorString)
{
    QPlaceReply::setError(error_, errorString);
    emit error(error_, errorString);
    setFinished(true);
    emit finished();
}

void QPlaceSearchSuggestionReplyImpl::replyFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError)
        return;

    QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
    if (!document.isObject()) {
        setError(ParseError, QCoreApplication::translate(NOKIA_PLUGIN_CONTEXT_NAME, PARSE_ERROR));
        emit error(error(), errorString());
        return;
    }

    QJsonObject object = document.object();

    QJsonArray suggestions = object.value(QStringLiteral("suggestions")).toArray();

    QStringList s;
    for (int i = 0; i < suggestions.count(); ++i) {
        QJsonValue v = suggestions.at(i);
        if (v.isString())
            s.append(v.toString());
    }

    setSuggestions(s);

    setFinished(true);
    emit finished();
}

void QPlaceSearchSuggestionReplyImpl::replyError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (error == QNetworkReply::OperationCanceledError)
        setError(QPlaceReply::CancelError, QCoreApplication::translate(NOKIA_PLUGIN_CONTEXT_NAME, CANCEL_ERROR));
    else
        setError(QPlaceReply::CommunicationError, QCoreApplication::translate(NOKIA_PLUGIN_CONTEXT_NAME, NETWORK_ERROR));
}

QT_END_NAMESPACE
