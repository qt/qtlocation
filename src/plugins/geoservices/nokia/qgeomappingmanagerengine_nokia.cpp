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

#include "qgeomappingmanagerengine_nokia.h"
#include "qgeomapreply_nokia.h"

#include <qgeotilespec.h>

#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QNetworkProxy>
#include <QSize>
#include <QDir>
#include <QUrl>
#ifdef USE_CHINA_NETWORK_REGISTRATION
#include <QtSystemInfo/QNetworkInfo>
#endif

#include <map>

#define LARGE_TILE_DIMENSION 256

#define DISK_CACHE_MAX_SIZE 50*1024*1024  //50MB

#if defined(Q_OS_WINCE_WM) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
#undef DISK_CACHE_ENABLED
#else
#define DISK_CACHE_ENABLED 1
#endif

#undef DISK_CACHE_ENABLED

QT_BEGIN_NAMESPACE

const char* MAPTILES_HOST = "1-4.maptile.lbs.ovi.com";
const char* MAPTILES_HOST_CN = "a-k.maptile.maps.svc.nokia.com.cn";

QGeoMappingManagerEngineNokia::QGeoMappingManagerEngineNokia(const QMap<QString, QVariant> &parameters, QGeoServiceProvider::Error *error, QString *errorString)
        : QGeoMappingManagerEngine(parameters),
        m_cache(0),
        m_firstSubdomain(QChar::Null),
        m_maxSubdomains(0)
{
    Q_UNUSED(error)
    Q_UNUSED(errorString)

    setHost(MAPTILES_HOST);
    setMinimumZoomLevel(0.0);
    setMaximumZoomLevel(20.0);

#ifdef USE_CHINA_NETWORK_REGISTRATION
    m_networkInfo = 0;
#endif
}

QGeoMappingManagerEngineNokia::~QGeoMappingManagerEngineNokia() {}

void QGeoMappingManagerEngineNokia::init()
{
    setTileSize(256);

    QList<QGeoMapType> types;
    types << QGeoMapType(QGeoMapType::StreetMap,tr("Street Map"),tr("Nokia Street Map"), false, 1);
    types << QGeoMapType(QGeoMapType::SatelliteMapDay,tr("Satellite Map(day)"),tr("Nokia Satellite Map (day)"), false, 2);
    types << QGeoMapType(QGeoMapType::TerrainMap,tr("Terrain Map"),tr("Nokia Terrain Map"), false, 3);
    types << QGeoMapType(QGeoMapType::HybridMap,tr("Hybrid Map"),tr("Nokia Hybrid Map"), false, 4);
    types << QGeoMapType(QGeoMapType::TransitMap,tr("Transit Map"),tr("Nokia Transit Map"), false, 5);
    types << QGeoMapType(QGeoMapType::GrayStreetMap,tr("Gray Street Map"),tr("Nokia Gray Street Map"), false, 6);
    types << QGeoMapType(QGeoMapType::StreetMap,tr("Mobile Street Map"),tr("Nokia Mobile Street Map"), true, 7);
    types << QGeoMapType(QGeoMapType::TerrainMap,tr("Mobile Terrain Map"),tr("Nokia Mobile Terrain Map"), true, 8);
    types << QGeoMapType(QGeoMapType::HybridMap,tr("Mobile Hybrid Map"),tr("Nokia Mobile Hybrid Map"), true, 9);
    types << QGeoMapType(QGeoMapType::TransitMap,tr("Mobile Transit Map"),tr("Nokia Mobile Transit Map"), true, 10);
    types << QGeoMapType(QGeoMapType::GrayStreetMap,tr("Mobile Gray Street Map"),tr("Nokia Mobile Gray Street Map"), true, 11);
    setSupportedMapTypes(types);

//    QList<QGraphicsGeoMap::ConnectivityMode> modes;
//    modes << QGraphicsGeoMap::OnlineMode;
//    setSupportedConnectivityModes(modes);

    m_networkManager = new QNetworkAccessManager(this);

    QMap<QString, QVariant> parameters = this->parameters();

    if (parameters.contains("mapping.proxy")) {
        QString proxy = parameters.value("mapping.proxy").toString();
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

    if (parameters.contains("mapping.host")) {
        QString host = parameters.value("mapping.host").toString();
        if (!host.isEmpty())
            setHost(host);
    }

    if (parameters.contains("mapping.app_id")) {
        m_applicationId = parameters.value("mapping.app_id").toString();
    }
    else if (parameters.contains("app_id")) {
        m_applicationId = parameters.value("app_id").toString();
    }

    if (parameters.contains("mapping.token")) {
        m_token = parameters.value("mapping.token").toString();
    }
    else if (parameters.contains("token")) {
        m_token = parameters.value("token").toString();
    }
#ifdef DISK_CACHE_ENABLED
    QString cacheDir;
    if (parameters.contains("mapping.cache.directory"))
        cacheDir = parameters.value("mapping.cache.directory").toString();

    if (cacheDir.isEmpty()) {
        cacheDir = QDir::temp().path()+"/maptiles";
    }
    if (!cacheDir.isEmpty()) {
        m_cache = new QNetworkDiskCache(this);
        QDir dir;
        dir.mkpath(cacheDir);
        dir.setPath(cacheDir);

        m_cache->setCacheDirectory(dir.path());

        if (parameters.contains("mapping.cache.size")) {
            bool ok = false;
            qint64 cacheSize = parameters.value("mapping.cache.size").toString().toLongLong(&ok);
            if (ok)
                m_cache->setMaximumCacheSize(cacheSize);
        }

        if (m_cache->maximumCacheSize() > DISK_CACHE_MAX_SIZE)
            m_cache->setMaximumCacheSize(DISK_CACHE_MAX_SIZE);

        m_networkManager->setCache(m_cache);
    }
#endif

#ifdef USE_CHINA_NETWORK_REGISTRATION
    m_networkInfo = new QNetworkInfo(this);
    connect(m_networkInfo, SIGNAL(currentMobileCountryCodeChanged(int, const QString&)),
            SLOT(currentMobileCountryCodeChanged(int, const QString&)));
    currentMobileCountryCodeChanged(0, m_networkInfo->currentMobileCountryCode(0));
#endif

    if (!isValidParameter(m_applicationId) || !isValidParameter(m_token)) {
        qWarning() << "This plug in must have a valid \"app_id\" and \"token\".";
        qWarning() << "These may be obtained from:";
        qWarning() << "https://api.forum.nokia.com/ovi-api/ui/registration";
    }

    // Temporary testing aid for setting China maptile server
    QFile file("/.enable_china_maptile_server");
    if (file.exists()) {
        qDebug() << "CHINA MAPTILE SERVER SET FOR TESTING PURPOSES.";
        setHost(MAPTILES_HOST_CN);
    }

    QGeoMappingManagerEngine::init();
}

QGeoTiledMapReply* QGeoMappingManagerEngineNokia::getTileImage(const QGeoTileSpec &spec)
{
    // TODO add error detection for if request.connectivityMode() != QGraphicsGeoMap::OnlineMode
    QString rawRequest = getRequestString(spec);

    QNetworkRequest netRequest((QUrl(rawRequest))); // The extra pair of parens disambiguates this from a function declaration
    netRequest.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);

#ifdef DISK_CACHE_ENABLED
    netRequest.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
#endif

    QNetworkReply* netReply = m_networkManager->get(netRequest);

    QGeoTiledMapReply* mapReply = new QGeoMapReplyNokia(netReply, spec);

    // TODO goes badly on linux
    //qDebug() << "request: " << QString::number(reinterpret_cast<int>(mapReply), 16) << " " << request.row() << "," << request.column();
    // this one might work better. It follows defined behaviour, unlike reinterpret_cast
    //qDebug("request: %p %i,%i @ %i", mapReply, request.row(), request.column(), request.zoomLevel());
    return mapReply;
}

QString QGeoMappingManagerEngineNokia::getRequestString(const QGeoTileSpec &spec) const
{
    const char subdomain = m_maxSubdomains ? m_firstSubdomain.toAscii() +
                                             (spec.x() + spec.y()) % m_maxSubdomains : 0;
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
    requestString += sizeToStr(tileSize());
    static const QString slashpng("/png8");
    requestString += slashpng;

    if (!m_token.isEmpty() && !m_applicationId.isEmpty()) {
        requestString += "?token=";
        requestString += m_token;
        requestString += "&app_id=";
        requestString += m_applicationId;
    }
    // Remove the code below when app_id and token are fully enforced
    else {
        requestString += "?token=ThisWillCeaseToWork";
    }
    return requestString;
}

QString QGeoMappingManagerEngineNokia::sizeToStr(int size)
{
    static const QString s256("256");
    static const QString s128("128");
    if (size >= LARGE_TILE_DIMENSION)
        return s256;
    else
        return s128;
}

QString QGeoMappingManagerEngineNokia::mapIdToStr(int mapId)
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

const QString & QGeoMappingManagerEngineNokia::token() const
{
    return m_token;
}

const QString & QGeoMappingManagerEngineNokia::host() const
{
    return m_host;
}

const QString & QGeoMappingManagerEngineNokia::applicationId() const
{
    return m_applicationId;
}
QChar QGeoMappingManagerEngineNokia::firstSubdomain() const
{
    return m_firstSubdomain;
}

unsigned char QGeoMappingManagerEngineNokia::maxSubdomains() const
{
    return m_maxSubdomains;
}

void QGeoMappingManagerEngineNokia::setHost(const QString &host)
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
void QGeoMappingManagerEngineNokia::currentMobileCountryCodeChanged(int interface, const QString & mcc)
{
    Q_UNUSED(interface)
    if (mcc == "460" || mcc == "461" || mcc == "454" || mcc == "455") {
        setHost(MAPTILES_HOST_CN);
    }
}
#endif

bool QGeoMappingManagerEngineNokia::isValidParameter(const QString &param)
{
    if (param.isEmpty())
        return false;

    if (param.length() > 512)
        return false;

    foreach (QChar c, param) {
        if (!c.isLetterOrNumber() || c.toAscii() != '%' || c.toAscii() != '-' ||
                c.toAscii() != '+' || c.toAscii() != '_') {
            return false;
        }
    }

    return true;
}

QT_END_NAMESPACE
