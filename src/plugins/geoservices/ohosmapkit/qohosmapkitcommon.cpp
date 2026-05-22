// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/qlist.h>
#include <QtCore/qlocale.h>
#include <qohosmapkitcommon.h>

QT_BEGIN_NAMESPACE

namespace OhosMapKit {

namespace {

const QString userAgentParameterName = QStringLiteral("ohosmapkit.useragent");
const QString defaultUserAgent = QStringLiteral("Qt Location based application");

}

QString getLanguageCode()
{
    return QLocale::system().name().section(QLatin1Char('_'), 0, 0);
}

QString getAuthenticationKeyParameterOrEmpty(const QVariantMap &parameters)
{
    return parameters.value(authenticationKeyParameterName).toString();
}

QString getUserAgentParameterOrDefault(const QVariantMap &parameters)
{
    return parameters.contains(userAgentParameterName)
        ? parameters.value(userAgentParameterName).toString()
        : defaultUserAgent;
}

QNetworkRequest createOhosMapKitNetworkRequest(
    const QUrl &requestUrl, const QString &userAgent, const QString &authenticationKey)
{
    QNetworkRequest networkRequest(requestUrl);
    networkRequest.setHeader(QNetworkRequest::UserAgentHeader, userAgent.toLatin1());
    networkRequest.setRawHeader(
        "Authorization", QStringLiteral("Bearer %1").arg(authenticationKey).toLatin1());

    return networkRequest;
}

QNetworkRequest createOhosMapKitNetworkRequestWithJsonBody(
    const QUrl &requestUrl, const QString &userAgent, const QString &authenticationKey)
{
    auto networkRequest = createOhosMapKitNetworkRequest(requestUrl, userAgent, authenticationKey);
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    return networkRequest;
}

namespace CoordinateJson {

const QString latitudeKey = QStringLiteral("lat");
const QString longitudeKey = QStringLiteral("lng");

QJsonObject tryConvertFromQGeoCoordinate(const QGeoCoordinate &coordinate)
{
    return coordinate.isValid()
        ? QJsonObject(
            {
                {latitudeKey, coordinate.latitude()},
                {longitudeKey, coordinate.longitude()},
            })
        : QJsonObject();
}

QGeoCoordinate tryConvertToQGeoCoordinate(const QJsonObject &coordinateObj)
{
    return coordinateObj.contains(latitudeKey) && coordinateObj.contains(longitudeKey)
        ? QGeoCoordinate(
            coordinateObj.value(latitudeKey).toDouble(),
            coordinateObj.value(longitudeKey).toDouble())
        : QGeoCoordinate();
}

}

namespace CoordinateBoundsJson {

const QString northeastKey = QStringLiteral("northeast");
const QString southwestKey = QStringLiteral("southwest");

QJsonObject tryConvertFromQGeoRectangle(const QGeoRectangle &boundingGeoRect)
{
    auto northeastCoordObj = CoordinateJson::tryConvertFromQGeoCoordinate(boundingGeoRect.topRight());
    auto southwestCoordObj = CoordinateJson::tryConvertFromQGeoCoordinate(boundingGeoRect.bottomLeft());

    return !northeastCoordObj.isEmpty() && !southwestCoordObj.isEmpty()
        ? QJsonObject(
            {
                {northeastKey, northeastCoordObj},
                {southwestKey, southwestCoordObj},
            })
        : QJsonObject();
}

QGeoRectangle tryConvertToQGeoRectangle(const QJsonObject &coordinateBounds)
{
    if (!coordinateBounds.contains(northeastKey) || !coordinateBounds.contains(southwestKey))
        return {};

    const auto northeast =
        CoordinateJson::tryConvertToQGeoCoordinate(coordinateBounds.value(northeastKey).toObject());
    const auto southwest =
        CoordinateJson::tryConvertToQGeoCoordinate(coordinateBounds.value(southwestKey).toObject());

    return northeast.isValid() && southwest.isValid()
        ? QGeoRectangle(QList<QGeoCoordinate>({northeast, southwest}))
        : QGeoRectangle();
}

}

namespace SiteJson {

QGeoAddress tryConvertToGeoAddress(const QJsonObject &siteJsonObj)
{
    QGeoAddress address;

    const auto addressJsonObj = siteJsonObj.value(QStringLiteral("address")).toObject();
    if (!addressJsonObj.isEmpty()) {
        address.setCountry(addressJsonObj.value(QStringLiteral("country")).toVariant().toString());
        address.setCountryCode(addressJsonObj.value(QStringLiteral("countryCode")).toVariant().toString());
        address.setState(addressJsonObj.value(QStringLiteral("adminArea")).toVariant().toString());

        auto city = addressJsonObj.value(QStringLiteral("city")).toVariant().toString();
        address.setCity(
            !city.isEmpty()
                ? city
                : addressJsonObj.value(QStringLiteral("locality")).toVariant().toString());
        address.setPostalCode(addressJsonObj.value(QStringLiteral("postalCode")).toVariant().toString());

        auto streetName = addressJsonObj.value(QStringLiteral("thoroughfare")).toVariant().toString();
        auto streetNumber = addressJsonObj.value(QStringLiteral("streetNumber")).toVariant().toString();
        address.setStreet(QString("%1 %2").arg(streetName).arg(streetNumber).trimmed());
    }

    address.setText(siteJsonObj.value(QStringLiteral("formatAddress")).toVariant().toString());

    return address;
}

QGeoLocation tryConvertToQGeoLocation(const QJsonObject &siteJsonObj)
{
    QGeoLocation location;

    location.setAddress(tryConvertToGeoAddress(siteJsonObj));
    location.setCoordinate(
        OhosMapKit::CoordinateJson::tryConvertToQGeoCoordinate(
            siteJsonObj.value(QStringLiteral("location")).toObject()));
    location.setBoundingShape(
        OhosMapKit::CoordinateBoundsJson::tryConvertToQGeoRectangle(
            siteJsonObj.value(QStringLiteral("viewport")).toObject()));

    return location;
}

}

}

QT_END_NAMESPACE
