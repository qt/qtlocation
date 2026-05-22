// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qlocale.h>
#include <QtCore/qurlquery.h>
#include <QtCore/qurl.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtNetwork/qnetworkrequest.h>
#include <QtPositioning/qgeoaddress.h>
#include <QtPositioning/qgeocoordinate.h>
#include <QtPositioning/qgeorectangle.h>
#include <QtPositioning/qgeoshape.h>
#include <qgeocodereplyohosmapkit.h>
#include <qgeocodingmanagerengineohosmapkit.h>
#include <qohosmapkitcommon.h>

QT_BEGIN_NAMESPACE

namespace {

const QString geocodeApiBasePath = QLatin1String("https://siteapi.cloud.huawei.com/mapApi/v1/siteService/");
const QString geocodeApiPath = geocodeApiBasePath + QLatin1String("geocode");
const QString reverseGeocodeApiPath = geocodeApiBasePath + QLatin1String("reverseGeocode");

class QGeoCodingManagerEngineOhosMapKit : public QGeoCodingManagerEngine
{
public:
    QGeoCodingManagerEngineOhosMapKit(
        const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString);

    QGeoCodeReply *geocode(const QGeoAddress &address, const QGeoShape &bounds) override;
    QGeoCodeReply *geocode(const QString &address, int limit, int offset, const QGeoShape &bounds) override;
    QGeoCodeReply *reverseGeocode(const QGeoCoordinate &coordinate, const QGeoShape &bounds) override;

private:
    QGeoCodeReply *createGeoCodeReply(const QUrl &requestUrl, const QByteArray &requestBody);

    void onReplyFinished();
    void onReplyError(QGeoCodeReply::Error errorCode, const QString &errorString);

    QNetworkAccessManager m_networkManager;
    QString m_userAgent;
    QString m_authenticationKey;
};

QGeoCodingManagerEngineOhosMapKit::QGeoCodingManagerEngineOhosMapKit(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString)
    : QGeoCodingManagerEngine(parameters)
    , m_userAgent(OhosMapKit::getUserAgentParameterOrDefault(parameters))
    , m_authenticationKey(OhosMapKit::getAuthenticationKeyParameterOrEmpty(parameters))
{
    if (error != nullptr)
        *error = QGeoServiceProvider::NoError;

    if (errorString != nullptr)
        errorString->clear();
}

QGeoCodeReply *QGeoCodingManagerEngineOhosMapKit::geocode(
    const QGeoAddress &address, const QGeoShape &bounds)
{
    return geocode(address.text().simplified(), 0, 0, bounds);
}

QGeoCodeReply *QGeoCodingManagerEngineOhosMapKit::geocode(
    const QString &address, int limit, int offset, const QGeoShape &bounds)
{
    Q_UNUSED(limit)
    Q_UNUSED(offset)

    if (address.isEmpty()) {
        return new QGeoCodeReply(
            QGeoCodeReply::CombinationError,
            tr("Cannot do geocoding query using empty address"),
            this);
    }

    QJsonObject geocodeRequestBody;

    constexpr int restApiMaxAddressLength = 512;
    geocodeRequestBody.insert(QStringLiteral("address"), address.left(restApiMaxAddressLength));

    auto coordinateBoundsObj =
        OhosMapKit::CoordinateBoundsJson::tryConvertFromQGeoRectangle(bounds.boundingGeoRectangle());
    if (!coordinateBoundsObj.isEmpty())
        geocodeRequestBody.insert(QStringLiteral("bounds"), coordinateBoundsObj);

    geocodeRequestBody.insert(QStringLiteral("language"), OhosMapKit::getLanguageCode());

    return createGeoCodeReply(
        QUrl(geocodeApiPath), QJsonDocument(geocodeRequestBody).toJson(QJsonDocument::Compact));
}

QGeoCodeReply *QGeoCodingManagerEngineOhosMapKit::reverseGeocode(
    const QGeoCoordinate &coordinate, const QGeoShape &bounds)
{
    if (!coordinate.isValid()) {
        return new QGeoCodeReply(
            QGeoCodeReply::CombinationError,
            tr("Cannot do reverse geocoding query using non-valid coordinates"),
            this);
    }

    QJsonObject reverseGeocodeRequestBody;
    reverseGeocodeRequestBody.insert(
        QStringLiteral("location"), OhosMapKit::CoordinateJson::tryConvertFromQGeoCoordinate(coordinate));
    reverseGeocodeRequestBody.insert(QStringLiteral("language"), OhosMapKit::getLanguageCode());

    if (bounds.isValid() && !bounds.isEmpty()) {
        constexpr int restApiMinRadiusInMeters = 0;
        constexpr int restApiMaxRadiusInMeters = 50;
        const int radius = bounds.center().distanceTo(bounds.boundingGeoRectangle().topLeft());
        reverseGeocodeRequestBody.insert(
            QStringLiteral("radius"),
            qBound(restApiMinRadiusInMeters, radius, restApiMaxRadiusInMeters));
    }

    return createGeoCodeReply(
        QUrl(reverseGeocodeApiPath),
        QJsonDocument(reverseGeocodeRequestBody).toJson(QJsonDocument::Compact));
}

QGeoCodeReply *QGeoCodingManagerEngineOhosMapKit::createGeoCodeReply(
    const QUrl &requestUrl, const QByteArray &requestBody)
{
    auto *reply = makeQGeoCodeReplyOhosMapKit(
        m_networkManager.post(
            OhosMapKit::createOhosMapKitNetworkRequestWithJsonBody(
                requestUrl, m_userAgent, m_authenticationKey),
            requestBody),
        this);

    connect(
        reply, &QGeoCodeReply::finished,
        this, &QGeoCodingManagerEngineOhosMapKit::onReplyFinished);
    connect(
        reply, &QGeoCodeReply::errorOccurred,
        this, &QGeoCodingManagerEngineOhosMapKit::onReplyError);

    return reply;
}

void QGeoCodingManagerEngineOhosMapKit::onReplyFinished()
{
    auto *geocodeReply = qobject_cast<QGeoCodeReply *>(sender());
    if (geocodeReply != nullptr)
        Q_EMIT finished(geocodeReply);
}

void QGeoCodingManagerEngineOhosMapKit::onReplyError(
    QGeoCodeReply::Error errorCode, const QString &errorString)
{
    auto *geocodeReply = qobject_cast<QGeoCodeReply *>(sender());
    if (geocodeReply != nullptr)
        Q_EMIT errorOccurred(geocodeReply, errorCode, errorString);
}

}

QGeoCodingManagerEngine *makeQGeoCodingManagerEngineOhosMapKit(
    const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString)
{
    return new QGeoCodingManagerEngineOhosMapKit(parameters, error, errorString);
}

QT_END_NAMESPACE
