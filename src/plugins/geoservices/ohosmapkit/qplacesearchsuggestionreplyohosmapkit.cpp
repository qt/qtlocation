// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/qjsonarray.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qmetaobject.h>
#include <qplacesearchsuggestionreplyohosmapkit.h>

QT_BEGIN_NAMESPACE

namespace {

QStringList parsePlaceSearchSuggestionReply(const QJsonDocument &responseBodyJsonDoc)
{
    QStringList suggestions;

    const QJsonObject responseBodyJsonObj = responseBodyJsonDoc.object();
    if (responseBodyJsonObj.contains(QStringLiteral("sites"))) {
        const auto sites = responseBodyJsonObj.value(QStringLiteral("sites")).toArray();
        for (const auto &value : sites) {
            if (value.isObject()) {
                auto siteObj = value.toObject();
                auto siteName = siteObj.value(QStringLiteral("name")).toString();
                auto formatAddress = siteObj.value(QStringLiteral("formatAddress")).toString();
                suggestions.append(QString("%1, %2").arg(siteName, formatAddress));
            }
        }
    }

    return suggestions;
}

class QPlaceSearchSuggestionReplyOhosMapKit : public QPlaceSearchSuggestionReply
{
public:
    QPlaceSearchSuggestionReplyOhosMapKit(QNetworkReply *reply, QPlaceManagerEngine *parent);

private:
    void onReplyFinished();
    void onNetworkError(QNetworkReply::NetworkError error);
};

QPlaceSearchSuggestionReplyOhosMapKit::QPlaceSearchSuggestionReplyOhosMapKit(
    QNetworkReply *reply, QPlaceManagerEngine *parent)
    : QPlaceSearchSuggestionReply(parent)
{
    Q_ASSERT(parent);
    if (!reply) {
        setError(QPlaceReply::UnknownError, QStringLiteral("Null reply"));
        return;
    }

    connect(reply, &QNetworkReply::finished, this, &QPlaceSearchSuggestionReplyOhosMapKit::onReplyFinished);
    connect(reply, &QNetworkReply::errorOccurred, this, &QPlaceSearchSuggestionReplyOhosMapKit::onNetworkError);

    connect(this, &QPlaceReply::aborted, reply, &QNetworkReply::abort);
    connect(this, &QObject::destroyed, reply, &QObject::deleteLater);
}

void QPlaceSearchSuggestionReplyOhosMapKit::onReplyFinished()
{
    auto *reply = static_cast<QNetworkReply *>(sender());
    reply->deleteLater();

    auto setErrorEmitAndFinish = [&](QPlaceReply::Error errorCode, const QString &errorString) {
        setError(errorCode, errorString);
        Q_EMIT errorOccurred(errorCode, errorString);

        setFinished(true);
        Q_EMIT finished();
    };

    if (reply->error() != QNetworkReply::NoError) {
        setErrorEmitAndFinish(
            QPlaceReply::CommunicationError,
            tr("Network reply error: %1")
                .arg(QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(reply->error())));
        return;
    }

    const auto responseBodyJsonDoc = QJsonDocument::fromJson(reply->readAll());
    if (!responseBodyJsonDoc.isObject()) {
        setErrorEmitAndFinish(QPlaceReply::ParseError, tr("Response parse error"));
        return;
    }

    setSuggestions(parsePlaceSearchSuggestionReply(responseBodyJsonDoc));

    setFinished(true);
    Q_EMIT finished();
}

void QPlaceSearchSuggestionReplyOhosMapKit::onNetworkError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error)
    auto *reply = static_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    setError(QPlaceReply::CommunicationError, reply->errorString());
}

}

QPlaceSearchSuggestionReply *makeQPlaceSearchSuggestionReplyOhosMapKit(
    QNetworkReply *reply, QPlaceManagerEngine *parent)
{
    return new QPlaceSearchSuggestionReplyOhosMapKit(reply, parent);
}

QT_END_NAMESPACE
