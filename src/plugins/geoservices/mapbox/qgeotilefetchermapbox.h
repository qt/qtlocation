// Copyright (C) 2014 Canonical Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#ifndef QGEOTILEFETCHERMAPBOX_H
#define QGEOTILEFETCHERMAPBOX_H

#include <qlist.h>
#include <QtLocation/private/qgeotilefetcher_p.h>

QT_BEGIN_NAMESPACE

class QGeoTiledMappingManagerEngine;
class QNetworkAccessManager;

class QGeoTileFetcherMapbox : public QGeoTileFetcher
{
    Q_OBJECT

public:
    QGeoTileFetcherMapbox(int scaleFactor, QGeoTiledMappingManagerEngine *parent);

    void setUserAgent(const QByteArray &userAgent);
    void setMapIds(const QList<QString> &mapIds);
    void setFormat(const QString &format);
    void setAccessToken(const QString &accessToken);

private:
    QGeoTiledMapReply *getTileImage(const QGeoTileSpec &spec) override;

    QNetworkAccessManager *m_networkManager;
    QByteArray m_userAgent;
    QString m_format;
    QString m_replyFormat;
    QString m_accessToken;
    QList<QString> m_mapIds;
    int m_scaleFactor;
};

QT_END_NAMESPACE

#endif // QGEOTILEFETCHERMAPBOX_H
