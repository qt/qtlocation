// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/qlist.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtPositioning/qgeolocation.h>
#include <qgeocodereplyohosmapkit.h>
#include <qohosmapkitcommon.h>

QT_BEGIN_NAMESPACE

namespace {

QList<QGeoLocation> parseGeocodeResponse(const QJsonObject &responseBodyJsonObj)
{
    QList<QGeoLocation> results;

    if (responseBodyJsonObj.contains(QStringLiteral("sites"))) {
        const auto sites = responseBodyJsonObj.value(QStringLiteral("sites")).toArray();
        for (const auto &siteValue : sites)
            if (siteValue.isObject())
                results.append(OhosMapKit::SiteJson::tryConvertToQGeoLocation(siteValue.toObject()));
    }

    return results;
}

class QGeoCodeReplyOhosMapKit : public QGeoCodeReply
{
public:
    explicit QGeoCodeReplyOhosMapKit(QNetworkReply *reply, QObject *parent);

private:
    void onNetworkReplyFinished();
    void onNetworkReplyError(QNetworkReply::NetworkError error);
};

QGeoCodeReplyOhosMapKit::QGeoCodeReplyOhosMapKit(QNetworkReply *reply, QObject *parent)
    : QGeoCodeReply(parent)
{
    if (reply == nullptr) {
        setError(QGeoCodeReply::UnknownError, OhosMapKit::msgNullReply());
        return;
    }

    connect(reply, &QNetworkReply::finished, this, &QGeoCodeReplyOhosMapKit::onNetworkReplyFinished);
    connect(reply, &QNetworkReply::errorOccurred, this, &QGeoCodeReplyOhosMapKit::onNetworkReplyError);
    connect(this, &QGeoCodeReply::aborted, reply, &QNetworkReply::abort);
    connect(this, &QObject::destroyed, reply, &QObject::deleteLater);
}

void QGeoCodeReplyOhosMapKit::onNetworkReplyFinished()
{
    auto *reply = static_cast<QNetworkReply *>(sender());
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        setError(
            QGeoCodeReply::CommunicationError,
            OhosMapKit::msgReplyError(reply->errorString()));
        return;
    }

    auto responseBodyJsonDoc = QJsonDocument::fromJson(reply->readAll());
    if (!responseBodyJsonDoc.isObject()) {
        setError(QGeoCodeReply::ParseError, tr("Response parse error"));
        return;
    }

    setLocations(parseGeocodeResponse(responseBodyJsonDoc.object()));

    setFinished(true);
}

void QGeoCodeReplyOhosMapKit::onNetworkReplyError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error)

    auto *reply = static_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    setError(QGeoCodeReply::CommunicationError, reply->errorString());
}

}

QGeoCodeReply *makeQGeoCodeReplyOhosMapKit(QNetworkReply *reply, QObject *parent)
{
    return new QGeoCodeReplyOhosMapKit(reply, parent);
}

QT_END_NAMESPACE
