// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/qurlquery.h>
#include <QtCore/qurl.h>
#include <QtLocation/private/qgeotilespec_p.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkrequest.h>
#include <qgeomapreplyohosmapkit.h>
#include <qgeotilefetcherohosmapkit.h>
#include <qohosmapkitcommon.h>

QT_BEGIN_NAMESPACE

namespace {

constexpr int minScaleFactor = 1;
constexpr int maxScaleFactor = 2;

const QString tilesApiPath = QLatin1String("https://mapapi.cloud.huawei.com/mapApi/v1/mapService/getTile");

class QGeoTileFetcherOhosMapKit : public QGeoTileFetcher
{
public:
    QGeoTileFetcherOhosMapKit(
        const QString &authenticationKey, const QString &userAgent, int scaleFactor,
        QGeoTiledMappingManagerEngine *parent);

private:
    QGeoTiledMapReply *getTileImage(const QGeoTileSpec &spec) override;

    QNetworkAccessManager m_networkManager;
    QString m_userAgent;
    QString m_authenticationKey;
    int m_scaleFactor;
};

QGeoTileFetcherOhosMapKit::QGeoTileFetcherOhosMapKit(
    const QString &authenticationKey, const QString &userAgent, int scaleFactor,
    QGeoTiledMappingManagerEngine *parent)
    : QGeoTileFetcher(parent)
    , m_userAgent(userAgent)
    , m_authenticationKey(authenticationKey)
    , m_scaleFactor(qBound(minScaleFactor, scaleFactor, maxScaleFactor))
{
}

QGeoTiledMapReply *QGeoTileFetcherOhosMapKit::getTileImage(const QGeoTileSpec &spec)
{
    QUrlQuery query;
    query.addQueryItem(QLatin1String("x"), QString::number(spec.x()));
    query.addQueryItem(QLatin1String("y"), QString::number(spec.y()));
    query.addQueryItem(QLatin1String("z"), QString::number(spec.zoom()));
    query.addQueryItem(QLatin1String("language"), OhosMapKit::getLanguageCode());
    query.addQueryItem(QLatin1String("scale"), QString::number(m_scaleFactor));

    QUrl url(tilesApiPath);
    url.setQuery(query);

    return makeGeoTiledMapReplyOhosMapKit(
        m_networkManager.get(
            OhosMapKit::createOhosMapKitNetworkRequest(url, m_userAgent, m_authenticationKey)),
        spec, this);
}

}

QGeoTileFetcher *makeGeoTileFetcherOhosMapKit(
    const QString &authenticationKey, const QString &userAgent, int scaleFactor,
    QGeoTiledMappingManagerEngine *parent)
{
    return new QGeoTileFetcherOhosMapKit(authenticationKey, userAgent, scaleFactor, parent);
}

QT_END_NAMESPACE
