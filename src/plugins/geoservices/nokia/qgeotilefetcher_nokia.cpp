/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
** This file is part of the Ovi services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file OVI_SERVICES_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Ovi services
** plugin source code.
**
****************************************************************************/

#include "qgeotilefetcher_nokia.h"
#include "qgeomapreply_nokia.h"
#include "qgeotiledmapdata_nokia.h"

#include <qgeotilespec.h>

#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QSize>
#include <QDir>
#include <QUrl>
#include <QTime>

#include <map>

#define LARGE_TILE_DIMENSION 256

QT_BEGIN_NAMESPACE

const char* MAPTILES_HOST = "1-4.maptile.lbs.ovi.com";
const char* MAPTILES_HOST_CN = "a-k.maptile.maps.svc.nokia.com.cn";

QGeoTileFetcherNokia::QGeoTileFetcherNokia(QGeoTiledMappingManagerEngine *engine)
        : QGeoTileFetcher(engine),
        m_networkManager(0),
        m_firstSubdomain(QChar::Null),
        m_maxSubdomains(0)
{
}

QGeoTileFetcherNokia::~QGeoTileFetcherNokia() {}

bool QGeoTileFetcherNokia::init()
{
    setHost(MAPTILES_HOST);

    qsrand((uint)QTime::currentTime().msec());

    m_networkManager = new QNetworkAccessManager(this);

    if (m_parameters.contains("mapping.proxy")) {
        QString proxy = m_parameters.value("mapping.proxy").toString();
        if (!proxy.isEmpty()) {
            QUrl proxyUrl(proxy);
            if (proxyUrl.isValid()) {
                m_networkManager->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy,
                    proxyUrl.host(),
                    proxyUrl.port(8080),
                    proxyUrl.userName(),
                    proxyUrl.password()));
            }
        }
    }

    if (m_parameters.contains("mapping.host")) {
        QString host = m_parameters.value("mapping.host").toString();
        if (!host.isEmpty())
            setHost(host);
    }

    if (m_parameters.contains("mapping.app_id")) {
        m_applicationId = m_parameters.value("mapping.app_id").toString();
    }
    else if (m_parameters.contains("app_id")) {
        m_applicationId = m_parameters.value("app_id").toString();
    }

    if (m_parameters.contains("mapping.token")) {
        m_token = m_parameters.value("mapping.token").toString();
    }
    else if (m_parameters.contains("token")) {
        m_token = m_parameters.value("token").toString();
    }

#ifdef USE_CHINA_NETWORK_REGISTRATION
    connect(&m_networkInfo, SIGNAL(currentMobileCountryCodeChanged(int, const QString&)),
            SLOT(currentMobileCountryCodeChanged(int, const QString&)));
    currentMobileCountryCodeChanged(0, m_networkInfo.currentMobileCountryCode(0));
#endif

    if (!isValidParameter(m_applicationId) || !isValidParameter(m_token)) {
        qWarning() << "Qt Location requires usage of app_id and token parameters obtained from:";
        qWarning() << "https://api.forum.nokia.com/ovi-api/ui/registration";
    }

    // Temporary testing aid for setting China maptile server
    QFile file("/.enable_china_maptile_server");
    if (file.exists()) {
        qDebug() << "CHINA MAPTILE SERVER SET FOR TESTING PURPOSES.";
        setHost(MAPTILES_HOST_CN);
    }

    return true;
}

QGeoTiledMapReply* QGeoTileFetcherNokia::getTileImage(const QGeoTileSpec &spec)
{
    // TODO add error detection for if request.connectivityMode() != QGraphicsGeoMap::OnlineMode
    QString rawRequest = getRequestString(spec);

    QNetworkRequest netRequest((QUrl(rawRequest))); // The extra pair of parens disambiguates this from a function declaration
    netRequest.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);

    QNetworkReply* netReply = m_networkManager->get(netRequest);

    QGeoTiledMapReply* mapReply = new QGeoMapReplyNokia(netReply, spec);

    return mapReply;
}

QString QGeoTileFetcherNokia::getRequestString(const QGeoTileSpec &spec)
{
    const char subdomain = m_maxSubdomains ? m_firstSubdomain.toAscii() +
                                             qrand() % m_maxSubdomains : 0;
    static const QString http("http://");
    static const QString path("/maptiler/v2/maptile/newest/");
    static const QChar dot('.');
    static const QChar slash('/');

    QString requestString = http;
    if (subdomain != 0) {
        requestString += subdomain;
        requestString += dot;
    }
    requestString += m_host;
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
    } else {
        requestString += "?token=";
        requestString += QGeoServiceProviderFactoryNokia::defaultToken;

        requestString += "&referer=";
        requestString += QGeoServiceProviderFactoryNokia::defaultReferer;
    }
    return requestString;
}

QString QGeoTileFetcherNokia::sizeToStr(const QSize &size)
{
    static const QString s256("256");
    static const QString s128("128");
    if (size.height() >= LARGE_TILE_DIMENSION ||
            size.width() >= LARGE_TILE_DIMENSION)
        return s256;
    else
        return s128;
}

QString QGeoTileFetcherNokia::mapIdToStr(int mapId)
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

void QGeoTileFetcherNokia::setParams(const QMap<QString, QVariant> &parameters)
{
    m_parameters = parameters;
}

void QGeoTileFetcherNokia::setTileSize(QSize tileSize)
{
    m_tileSize = tileSize;
}

const QString & QGeoTileFetcherNokia::token() const
{
    return m_token;
}

const QString & QGeoTileFetcherNokia::host() const
{
    return m_host;
}

const QString & QGeoTileFetcherNokia::applicationId() const
{
    return m_applicationId;
}
QChar QGeoTileFetcherNokia::firstSubdomain() const
{
    return m_firstSubdomain;
}

unsigned char QGeoTileFetcherNokia::maxSubdomains() const
{
    return m_maxSubdomains;
}

void QGeoTileFetcherNokia::setHost(const QString &host)
{
    if (host.length() > 4 && host.at(1) == QChar('-') && host.at(3) == QChar('.')) {
        QString realHost = host.right(host.length() - 4);
        m_firstSubdomain = host.at(0);
        m_maxSubdomains = host.at(2).toAscii() - host.at(0).toAscii() + 1;
        m_host = realHost;
    } else {
        m_host = host;
        m_firstSubdomain = QChar::Null;
        m_maxSubdomains = 0;
    }
}

#ifdef USE_CHINA_NETWORK_REGISTRATION
void QGeoTileFetcherNokia::currentMobileCountryCodeChanged(int interface, const QString & mcc)
{
    Q_UNUSED(interface)
    if (mcc == "460" || mcc == "461" || mcc == "454" || mcc == "455") {
        setHost(MAPTILES_HOST_CN);
    }
}
#endif

bool QGeoTileFetcherNokia::isValidParameter(const QString &param)
{
    if (param.isEmpty())
        return false;

    if (param.length() > 512)
        return false;

    foreach (QChar c, param) {
        if (!c.isLetterOrNumber() && c.toAscii() != '%' && c.toAscii() != '-' &&
            c.toAscii() != '+' && c.toAscii() != '_') {
            return false;
        }
    }

    return true;
}

QT_END_NAMESPACE
