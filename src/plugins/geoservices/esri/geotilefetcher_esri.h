/****************************************************************************
**
** Copyright (C) 2013-2018 Esri <contracts@esri.com>
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
**
**
**
****************************************************************************/

#ifndef GEOTILEFETCHERESRI_H
#define GEOTILEFETCHERESRI_H

#include <QtLocation/private/qgeotilefetcher_p.h>

QT_BEGIN_NAMESPACE

class QGeoTiledMappingManagerEngine;
class QNetworkAccessManager;

class GeoTileFetcherEsri : public QGeoTileFetcher
{
    Q_OBJECT

public:
    explicit GeoTileFetcherEsri(QGeoTiledMappingManagerEngine *parent);

    inline const QByteArray &userAgent() const;
    inline void setUserAgent(const QByteArray &userAgent);

    inline const QString &token() const;
    inline void setToken(const QString &token);

private:
    QGeoTiledMapReply *getTileImage(const QGeoTileSpec &spec) override;

    QNetworkAccessManager *m_networkManager;
    QByteArray m_userAgent;
    QString m_token;
};

inline const QByteArray &GeoTileFetcherEsri::userAgent() const
{
    return m_userAgent;
}

inline void GeoTileFetcherEsri::setUserAgent(const QByteArray &userAgent)
{
    m_userAgent = userAgent;
}

inline const QString &GeoTileFetcherEsri::token() const
{
    return m_token;
}

inline void GeoTileFetcherEsri::setToken(const QString &token)
{
    m_token = token;
}

QT_END_NAMESPACE

#endif // GEOTILEFETCHERESRI_H
