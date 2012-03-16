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

#ifndef QGEOTILEFETCHER_NOKIA_H
#define QGEOTILEFETCHER_NOKIA_H

#include "qgeoserviceproviderplugin_nokia.h"

#include <QGeoServiceProvider>
#include "qgeotilefetcher.h"

#include <QMap>

#ifdef USE_CHINA_NETWORK_REGISTRATION
#include <qnetworkinfo.h>
#endif

QT_BEGIN_NAMESPACE

class QChar;
class QSize;
class QVariant;

class QNetworkAccessManager;
class QNetworkDiskCache;

class QGeoTiledMapReply;
class QGeoTileSpec;
class QGeoTileCache;
class QGeoTiledMappingManagerEngine;

class QGeoTileFetcherNokia : public QGeoTileFetcher
{
    Q_OBJECT
public:
    QGeoTileFetcherNokia(QGeoTiledMappingManagerEngine *engine);
    ~QGeoTileFetcherNokia();

    bool init();

    QGeoTiledMapReply* getTileImage(const QGeoTileSpec &spec);

    const QString& host() const;
    QChar firstSubdomain() const;
    unsigned char maxSubdomains() const;
    const QString& token() const;
    const QString& applicationId() const;
    const QString& referer() const;

    void setParams(const QMap<QString, QVariant> &parameters);
    void setTileSize(QSize tileSize);

#ifdef USE_CHINA_NETWORK_REGISTRATION
private Q_SLOTS:
    void currentMobileCountryCodeChanged(int interface, const QString& mcc);
#endif

private:
    Q_DISABLE_COPY(QGeoTileFetcherNokia)

    QString getRequestString(const QGeoTileSpec &spec);

    static QString sizeToStr(const QSize &size);
    static QString mapIdToStr(int mapId);

    void setHost(const QString& host);
    bool isValidParameter(const QString& param);

    QNetworkAccessManager *m_networkManager;
    QNetworkDiskCache *m_cache;
    QMap<QString, QVariant> m_parameters;
    QSize m_tileSize;
    QString m_host;
    QString m_token;
    QString m_referer;
    QChar m_firstSubdomain;

    unsigned char m_maxSubdomains;
    QString m_applicationId;

#ifdef USE_CHINA_NETWORK_REGISTRATION
    QNetworkInfo m_networkInfo;
#endif
};

QT_END_NAMESPACE

#endif
