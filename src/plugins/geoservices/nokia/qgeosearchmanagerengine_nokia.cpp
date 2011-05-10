/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
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

#include "qgeosearchmanagerengine_nokia.h"
#include "qgeosearchreply_nokia.h"
#include "marclanguagecodes.h"

#include <qgeoaddress.h>
#include <qgeocoordinate.h>
#include <QNetworkProxy>
#include <QUrl>
#include <QMap>

QGeoSearchManagerEngineNokia::QGeoSearchManagerEngineNokia(const QMap<QString, QVariant> &parameters, QGeoServiceProvider::Error *error, QString *errorString)
        : QGeoSearchManagerEngine(parameters),
        m_host("loc.desktop.maps.svc.ovi.com"),
        m_token(QGeoServiceProviderFactoryNokia::defaultToken),
        m_referer(QGeoServiceProviderFactoryNokia::defaultReferer)
{
    m_networkManager = new QNetworkAccessManager(this);

    if (parameters.contains("places.proxy")) {
        QString proxy = parameters.value("places.proxy").toString();
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

    if (parameters.contains("places.host")) {
        QString host = parameters.value("places.host").toString();
        if (!host.isEmpty())
            m_host = host;
    }

    if (parameters.contains("places.referer")) {
        m_referer = parameters.value("places.referer").toString();
    }

    if (parameters.contains("places.token")) {
        m_token = parameters.value("places.token").toString();
    }
    else if (parameters.contains("token")) {
        m_token = parameters.value("token").toString();
    }


    setSupportsGeocoding(true);
    setSupportsReverseGeocoding(true);

    QGeoSearchManager::SearchTypes supportedSearchTypes;
    supportedSearchTypes |= QGeoSearchManager::SearchGeocode;
    setSupportedSearchTypes(supportedSearchTypes);

    if (error)
        *error = QGeoServiceProvider::NoError;

    if (errorString)
        *errorString = "";
}

QGeoSearchManagerEngineNokia::~QGeoSearchManagerEngineNokia() {}

QGeoSearchReply* QGeoSearchManagerEngineNokia::geocode(const QGeoAddress &address,
        QGeoBoundingArea *bounds)
{
    if (!supportsGeocoding()) {
        QGeoSearchReply *reply = new QGeoSearchReply(QGeoSearchReply::UnsupportedOptionError, "Geocoding is not supported by this service provider.", this);
        emit error(reply, reply->error(), reply->errorString());
        return reply;
    }

    QString requestString = "http://";
    requestString += m_host;
    requestString += "/geocoder/gc/1.0?referer=" + m_referer;

    if (!m_token.isNull())
        requestString += "&token=" + m_token;

    requestString += "&lg=";
    requestString += languageToMarc(locale().language());

    requestString += "&country=";
    requestString += address.country();

    if (!address.state().isEmpty()) {
        requestString += "&state=";
        requestString += address.state();
    }

    if (!address.city().isEmpty()) {
        requestString += "&city=";
        requestString += address.city();
    }

    if (!address.postcode().isEmpty()) {
        requestString += "&zip=";
        requestString += address.postcode();
    }

    if (!address.street().isEmpty()) {
        requestString += "&street=";
        requestString += address.street();
    }

    // TODO?
    // street number has been removed from QGeoAddress
    // do we need to try to split it out from QGeoAddress::street
    // in order to geocode properly

    // Old code:
//    if (!address.streetNumber().isEmpty()) {
//        requestString += "&number=";
//        requestString += address.streetNumber();
//    }

    return search(requestString, bounds);
}

QGeoSearchReply* QGeoSearchManagerEngineNokia::reverseGeocode(const QGeoCoordinate &coordinate,
        QGeoBoundingArea *bounds)
{
    if (!supportsReverseGeocoding()) {
        QGeoSearchReply *reply = new QGeoSearchReply(QGeoSearchReply::UnsupportedOptionError, "Reverse geocoding is not supported by this service provider.", this);
        emit error(reply, reply->error(), reply->errorString());
        return reply;
    }

    QString requestString = "http://";
    requestString += m_host;
    requestString += "/geocoder/rgc/1.0?referer=" + m_referer;
    if (!m_token.isNull())
        requestString += "&token=" + m_token;
    requestString += "&long=";
    requestString += trimDouble(coordinate.longitude());
    requestString += "&lat=";
    requestString += trimDouble(coordinate.latitude());

    requestString += "&lg=";
    requestString += languageToMarc(locale().language());

    return search(requestString, bounds);
}

QGeoSearchReply* QGeoSearchManagerEngineNokia::search(const QString &searchString,
        QGeoSearchManager::SearchTypes searchTypes,
        int limit,
        int offset,
        QGeoBoundingArea *bounds)
{
    // NOTE this will eventually replaced by a much improved implementation
    // which will make use of the additionLandmarkManagers()
    if ((searchTypes != QGeoSearchManager::SearchTypes(QGeoSearchManager::SearchAll))
            && ((searchTypes & supportedSearchTypes()) != searchTypes)) {

        QGeoSearchReply *reply = new QGeoSearchReply(QGeoSearchReply::UnsupportedOptionError, "The selected search type is not supported by this service provider.", this);
        emit error(reply, reply->error(), reply->errorString());
        return reply;
    }

    QString requestString = "http://";
    requestString += m_host;
    requestString += "/geocoder/gc/1.0?referer=" + m_referer;

    if (!m_token.isNull())
        requestString += "&token=" + m_token;

    requestString += "&lg=";
    requestString += languageToMarc(locale().language());

    requestString += "&obloc=";
    requestString += searchString;

    if (limit > 0) {
        requestString += "&total=";
        requestString += QString::number(limit);
    }

    if (offset > 0) {
        requestString += "&offset=";
        requestString += QString::number(offset);
    }

    return search(requestString, bounds, limit, offset);
}

QGeoSearchReply* QGeoSearchManagerEngineNokia::search(QString requestString,
        QGeoBoundingArea *bounds,
        int limit,
        int offset)
{
    QNetworkReply *networkReply = m_networkManager->get(QNetworkRequest(QUrl(requestString)));
    QGeoSearchReplyNokia *reply = new QGeoSearchReplyNokia(networkReply, limit, offset, bounds, this);

    connect(reply,
            SIGNAL(finished()),
            this,
            SLOT(placesFinished()));

    connect(reply,
            SIGNAL(error(QGeoSearchReply::Error, QString)),
            this,
            SLOT(placesError(QGeoSearchReply::Error, QString)));

    return reply;
}

QString QGeoSearchManagerEngineNokia::trimDouble(qreal degree, int decimalDigits)
{
    QString sDegree = QString::number(degree, 'g', decimalDigits);

    int index = sDegree.indexOf('.');

    if (index == -1)
        return sDegree;
    else
        return QString::number(degree, 'g', decimalDigits + index);
}

void QGeoSearchManagerEngineNokia::placesFinished()
{
    QGeoSearchReply *reply = qobject_cast<QGeoSearchReply*>(sender());

    if (!reply)
        return;

    if (receivers(SIGNAL(finished(QGeoSearchReply*))) == 0) {
        reply->deleteLater();
        return;
    }

    emit finished(reply);
}

void QGeoSearchManagerEngineNokia::placesError(QGeoSearchReply::Error error, const QString &errorString)
{
    QGeoSearchReply *reply = qobject_cast<QGeoSearchReply*>(sender());

    if (!reply)
        return;

    if (receivers(SIGNAL(error(QGeoSearchReply*, QGeoSearchReply::Error, QString))) == 0) {
        reply->deleteLater();
        return;
    }

    emit this->error(reply, error, errorString);
}

QString QGeoSearchManagerEngineNokia::languageToMarc(QLocale::Language language)
{
    uint offset = 3 * (uint(language));
    if (language == QLocale::C || offset + 3 > sizeof(marc_language_code_list))
        return QLatin1String("eng");

    const unsigned char *c = marc_language_code_list + offset;
    if (c[0] == 0)
        return QLatin1String("eng");

    QString code(3, Qt::Uninitialized);
    code[0] = ushort(c[0]);
    code[1] = ushort(c[1]);
    code[2] = ushort(c[2]);

    return code;
}
