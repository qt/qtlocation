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

#ifndef QGEOTILEFETCHER_NOKIA_H
#define QGEOTILEFETCHER_NOKIA_H

#include "qgeoserviceproviderplugin_nokia.h"

#include <QtLocation/private/qgeotilefetcher_p.h>

QT_BEGIN_NAMESPACE

class QGeoTiledMapReply;
class QGeoTileSpec;
class QGeoTiledMappingManagerEngine;
class QGeoTiledMappingManagerEngineNokia;
class QNetworkReply;
class QGeoNetworkAccessManager;
class QGeoUriProvider;

class QGeoTileFetcherNokia : public QGeoTileFetcher
{
    Q_OBJECT

public:
    QGeoTileFetcherNokia(const QVariantMap &parameters, QGeoNetworkAccessManager *networkManager,
                         QGeoTiledMappingManagerEngineNokia *engine, const QSize &tileSize, int ppi);
    ~QGeoTileFetcherNokia();

    QGeoTiledMapReply *getTileImage(const QGeoTileSpec &spec);

    QString token() const;
    QString applicationId() const;

public Q_SLOTS:
    void copyrightsFetched();
    void fetchCopyrightsData();
    void versionFetched();
    void fetchVersionData();

private:
    Q_DISABLE_COPY(QGeoTileFetcherNokia)

    QString getRequestString(const QGeoTileSpec &spec, int ppi=72);

    QString getLanguageString() const;

    QPointer<QGeoTiledMappingManagerEngineNokia> m_engineNokia;
    QGeoNetworkAccessManager *m_networkManager;
    int m_tileSize;
    int m_ppi;
    QString m_token;
    QNetworkReply *m_copyrightsReply;
    QNetworkReply *m_versionReply;

    QString m_applicationId;
    QGeoUriProvider *m_baseUriProvider;
    QGeoUriProvider *m_aerialUriProvider;
};

QT_END_NAMESPACE

#endif
