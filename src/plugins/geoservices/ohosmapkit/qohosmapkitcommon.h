// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOHOSMAPKITCOMMON_H
#define QOHOSMAPKITCOMMON_H

#include <QtCore/qjsonobject.h>
#include <QtCore/qstring.h>
#include <QtCore/qurl.h>
#include <QtCore/qvariant.h>
#include <QtNetwork/qnetworkrequest.h>
#include <QtPositioning/qgeoaddress.h>
#include <QtPositioning/qgeocoordinate.h>
#include <QtPositioning/qgeolocation.h>
#include <QtPositioning/qgeorectangle.h>

QT_BEGIN_NAMESPACE

namespace OhosMapKit {

const QString authenticationKeyParameterName = QStringLiteral("ohosmapkit.authenticationKey");

QString getLanguageCode();
QString getAuthenticationKeyParameterOrEmpty(const QVariantMap &parameters);
QString getUserAgentParameterOrDefault(const QVariantMap &parameters);
QNetworkRequest createOhosMapKitNetworkRequest(
    const QUrl &requestUrl, const QString &userAgent, const QString &authenticationKey);
QNetworkRequest createOhosMapKitNetworkRequestWithJsonBody(
    const QUrl &requestUrl, const QString &userAgent, const QString &authenticationKey);

namespace CoordinateJson {

QJsonObject tryConvertFromQGeoCoordinate(const QGeoCoordinate &coordinate);
QGeoCoordinate tryConvertToQGeoCoordinate(const QJsonObject &coordinateObj);

}

namespace CoordinateBoundsJson {

QJsonObject tryConvertFromQGeoRectangle(const QGeoRectangle &boundingGeoRect);
QGeoRectangle tryConvertToQGeoRectangle(const QJsonObject &coordinateBounds);

}

namespace SiteJson {

QGeoLocation tryConvertToQGeoLocation(const QJsonObject &siteJsonObj);

}

}

QT_END_NAMESPACE

#endif // QOHOSMAPKITCOMMON_H
