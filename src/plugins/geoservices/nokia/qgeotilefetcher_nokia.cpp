/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
** This file is part of the Nokia services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file NOKIA_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Nokia services
** plugin source code.
**
****************************************************************************/

#include "qgeotilefetcher_nokia.h"
#include "qgeomapreply_nokia.h"
#include "qgeotiledmapdata_nokia.h"
#include "qgeotiledmappingmanagerengine_nokia.h"
#include "qgeonetworkaccessmanager.h"
#include "qgeouriprovider.h"
#include "uri_constants.h"

#include <QtLocation/private/qgeotilespec_p.h>

#include <QDebug>
#include <QSize>
#include <QDir>
#include <QUrl>
#include <QTime>

#include <map>

#define LARGE_TILE_DIMENSION 256

QT_BEGIN_NAMESPACE

namespace
{
    QString sizeToStr(const QSize &size)
    {
        static const QString s256("256");
        static const QString s128("128");
        if (size.height() >= LARGE_TILE_DIMENSION ||
                size.width() >= LARGE_TILE_DIMENSION)
            return s256;
        else
            return s128;
    }

    QString mapIdToStr(int mapId)
    {
        typedef std::map<int, QString> MapTypeRegistry;
        static MapTypeRegistry registeredTypes;
        if (registeredTypes.empty()) {
            registeredTypes[0] = "normal.day";
            registeredTypes[1] = "normal.day";
            registeredTypes[2] = "satellite.day";
            registeredTypes[3] = "terrain.day";
            registeredTypes[4] = "hybrid.day";
            registeredTypes[5] = "normal.day.transit";
            registeredTypes[6] = "normal.day.grey";
            registeredTypes[7] = "normal.day.mobile";
            registeredTypes[8] = "terrain.day.mobile";
            registeredTypes[9] = "hybrid.day.mobile";
            registeredTypes[10] = "normal.day.transit.mobile";
            registeredTypes[11] = "normal.day.grey.mobile";
        }

        MapTypeRegistry::const_iterator it = registeredTypes.find(mapId);
        if (it != registeredTypes.end()) {
            return it->second;
        }

        qWarning() << "Unknown mapId: " << mapId;
        return "normal.day";
    }
}
QGeoTileFetcherNokia::QGeoTileFetcherNokia(
        const QMap<QString, QVariant> &parameters,
        QGeoNetworkAccessManager *networkManager,
        QGeoTiledMappingManagerEngine *engine,
        const QSize &tileSize)
        : QGeoTileFetcher(engine),
          m_engineNokia(static_cast<QGeoTiledMappingManagerEngineNokia *>(engine)),
          m_networkManager(networkManager),
          m_parameters(parameters),
          m_tileSize(tileSize),
          m_copyrightsReply(0),
          m_uriProvider(new QGeoUriProvider(this, parameters, "mapping.host", MAP_TILES_HOST, MAP_TILES_HOST_CN))
{
    Q_ASSERT(networkManager);
    m_networkManager->setParent(this);
}

QGeoTileFetcherNokia::~QGeoTileFetcherNokia() {}

bool QGeoTileFetcherNokia::init()
{
    qsrand((uint)QTime::currentTime().msec());

    if (m_parameters.contains("app_id")) {
        m_applicationId = m_parameters.value("app_id").toString();
    }

    if (m_parameters.contains("token")) {
        m_token = m_parameters.value("token").toString();
    }
    return true;
}

QGeoTiledMapReply *QGeoTileFetcherNokia::getTileImage(const QGeoTileSpec &spec)
{
    // TODO add error detection for if request.connectivityMode() != QGraphicsGeoMap::OnlineMode
    QString rawRequest = getRequestString(spec);

    QNetworkRequest netRequest((QUrl(rawRequest))); // The extra pair of parens disambiguates this from a function declaration
    netRequest.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);

    QNetworkReply *netReply = m_networkManager->get(netRequest);

    QGeoTiledMapReply *mapReply = new QGeoMapReplyNokia(netReply, spec);

    return mapReply;
}

QString QGeoTileFetcherNokia::getRequestString(const QGeoTileSpec &spec)
{
    static const QString http("http://");
    static const QString path("/maptiler/v2/maptile/newest/");
    static const QChar slash('/');

    QString requestString = http;

    requestString += m_uriProvider->getCurrentHost();
    requestString += path;

    requestString += mapIdToStr(spec.mapId());
    requestString += slash;
    requestString += QString::number(spec.zoom());
    requestString += slash;
    requestString += QString::number(spec.x());
    requestString += slash;
    requestString += QString::number(spec.y());
    requestString += slash;
    requestString += sizeToStr(m_tileSize);
    static const QString slashpng("/png8");
    requestString += slashpng;

    if (!m_token.isEmpty() && !m_applicationId.isEmpty()) {
        requestString += "?token=";
        requestString += m_token;

        requestString += "&app_id=";
        requestString += m_applicationId;
    }

    requestString += "&lg=";
    requestString += getLanguageString();

    return requestString;
}

QString QGeoTileFetcherNokia::getLanguageString() const
{
    const QLocale::Language lang = m_engineNokia.data()->locale().language();
    // English is the default, where no ln is specified. We hardcode the languages
    // here even though the entire list is updated automagically from the server.
    // The current languages are Arabic, Chinese, Simplified Chinese, English
    // French, German, Italian, Polish, Russian and Spanish. The default is English.
    // These are acually available from the same host under the URL: /maptiler/v2/info

    switch (lang) {
    case QLocale::Arabic:
        return "ARA";
    case QLocale::Chinese:
        if (QLocale::TraditionalChineseScript == m_engineNokia.data()->locale().script())
            return "CHI";
        else
            return "CHT";
    case QLocale::French:
        return "FRE";
    case QLocale::German:
        return "GER";
    case QLocale::Italian:
        return "ITA";
    case QLocale::Polish:
        return "POL";
    case QLocale::Russian:
        return "RUS";
    case QLocale::Spanish:
        return "SPA";
    default:
        return "ENG";
    }
    // No "lg" param means that we want English.
}

QString QGeoTileFetcherNokia::token() const
{
    return m_token;
}

QString QGeoTileFetcherNokia::applicationId() const
{
    return m_applicationId;
}

void QGeoTileFetcherNokia::copyrightsFetched()
{
    if (m_engineNokia) {
        QMetaObject::invokeMethod(m_engineNokia.data(),
                                  "loadCopyrightsDescriptorsFromJson",
                                  Qt::QueuedConnection,
                                  Q_ARG(QByteArray, m_copyrightsReply->readAll()));
    }
}

void QGeoTileFetcherNokia::fetchCopyrightsData()
{
    QString copyrightUrl = "http://";

    copyrightUrl += m_uriProvider->getCurrentHost();
    copyrightUrl += "/maptiler/v2/copyright/newest?output=json";

    if (!token().isEmpty()) {
        copyrightUrl += "&token=";
        copyrightUrl += token();
    }

    if (!applicationId().isEmpty()) {
        copyrightUrl += "&app_id=";
        copyrightUrl += applicationId();
    }

    QNetworkRequest netRequest((QUrl(copyrightUrl)));
    m_copyrightsReply = m_networkManager->get(netRequest);
    if (m_copyrightsReply->error() != QNetworkReply::NoError) {
        qWarning() << __FUNCTION__ << m_copyrightsReply->errorString();
        m_copyrightsReply->deleteLater();
        return;
    }

    if (m_copyrightsReply->isFinished()) {
        copyrightsFetched();
    } else {
        connect(m_copyrightsReply, SIGNAL(finished()), this, SLOT(copyrightsFetched()));
    }
}

QT_END_NAMESPACE
