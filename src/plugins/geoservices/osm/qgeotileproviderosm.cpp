/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeotileproviderosm.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QDebug>

QT_BEGIN_NAMESPACE

static const int maxValidZoom = 30;

QGeoTileProviderOsm::QGeoTileProviderOsm(const QString &urlRedir,
                                   QNetworkAccessManager *nm,
                                   const QGeoMapType &mapType,
                                   const QGeoTileProviderOsm::TileProvider &providerFallback)
    : m_nm(nm), m_urlRedirector(urlRedir),
      m_providerFallback(providerFallback),
      m_mapType(mapType), m_status(Idle)
{
    if (!m_urlRedirector.isValid())
        disableRedirection();
}

QGeoTileProviderOsm::~QGeoTileProviderOsm()
{

}

void QGeoTileProviderOsm::resolveProvider()
{
    switch (m_status) {
        case Resolving:
        case Invalid:
        case Valid:
            return;
        case Idle:
            m_status = Resolving;
            break;
    }

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::UserAgentHeader, QByteArrayLiteral("QGeoTileFetcherOsm"));
    request.setUrl(m_urlRedirector);
    QNetworkReply *reply = m_nm->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(onNetworkReplyFinished()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(onNetworkReplyError(QNetworkReply::NetworkError)));
}

void QGeoTileProviderOsm::disableRedirection()
{
    m_status = Invalid;
    m_provider.m_valid = false;
}

void QGeoTileProviderOsm::handleError(QNetworkReply::NetworkError error)
{
    switch (error) {
        case QNetworkReply::ConnectionRefusedError:
        case QNetworkReply::TooManyRedirectsError:
        case QNetworkReply::InsecureRedirectError:
        case QNetworkReply::ContentAccessDenied:
        case QNetworkReply::ContentOperationNotPermittedError:
        case QNetworkReply::ContentNotFoundError:
        case QNetworkReply::AuthenticationRequiredError:
        case QNetworkReply::ContentGoneError:
        case QNetworkReply::OperationNotImplementedError:
        case QNetworkReply::ServiceUnavailableError:
            // Errors we don't expect to recover from in the near future, which
            // prevent accessing the redirection info but not the actual providers.
            m_status = Invalid;
        default:
            break;
    }
}

QUrl QGeoTileProviderOsm::tileAddress(int x, int y, int z) const
{
    if (m_provider.isValid())
        return m_provider.tileAddress(x,y,z);
    if (m_providerFallback.isValid())
        return m_providerFallback.tileAddress(x,y,z);
    return QUrl();
}

QString QGeoTileProviderOsm::mapCopyRight() const
{
    if (m_provider.isValid())
        return m_provider.mapCopyRight();
    if (m_providerFallback.isValid())
        return m_providerFallback.mapCopyRight();
    return QString();
}

QString QGeoTileProviderOsm::dataCopyRight() const
{
    if (m_provider.isValid())
        return m_provider.dataCopyRight();
    if (m_providerFallback.isValid())
        return m_providerFallback.dataCopyRight();
    return QString();
}

QString QGeoTileProviderOsm::styleCopyRight() const
{
    if (m_provider.isValid())
        return m_provider.styleCopyRight();
    if (m_providerFallback.isValid())
        return m_providerFallback.styleCopyRight();
    return QString();
}

QString QGeoTileProviderOsm::format() const
{
    if (m_provider.isValid())
        return m_provider.format();
    if (m_providerFallback.isValid())
        return m_providerFallback.format();
    return QString();
}

const QGeoMapType &QGeoTileProviderOsm::mapType() const
{
    return m_mapType;
}

bool QGeoTileProviderOsm::isValid() const
{
    return (m_provider.isValid() || m_providerFallback.isValid());
}

bool QGeoTileProviderOsm::isResolved() const
{
    return (m_status == Valid || m_status == Invalid);
}

void QGeoTileProviderOsm::onNetworkReplyFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    reply->deleteLater();

    switch (m_status) {
        case Resolving:
            m_status = Idle;
        case Idle:    // should not happen
        case Invalid: // should not happen
            break;
        case Valid: // should not happen
            return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        handleError(reply->error());
        if (m_status == Invalid)
            emit resolutionError(this, reply->error());
        return;
    }
    m_status = Invalid;

    /*
     * The content of a provider information file must be in JSON format, containing
     * (as of Qt 5.6.2) the following fields:
     *
     * {
     *     "Enabled" : bool,  (optional)
     *     "UrlTemplate" : "<url template>", (mandatory)
     *     "ImageFormat" : "<image format>", (mandatory)
     *     "MapCopyRight" : "<copyright>", (mandatory)
     *     "DataCopyRight" : "<copyright>", (mandatory)
     *     "StyleCopyRight" : "<copyright>", (optional)
     *     "MinimumZoomLevel" : <minimumZoomLevel>, (optional)
     *     "MaximumZoomLevel" : <maximumZoomLevel>,  (optional)
     * }
     *
     * Enabled is optional, and allows to temporarily disable a tile provider if it becomes
     * unavailable, without making the osm plugin fire requests to it. Default is true.
     *
     * MinimumZoomLevel and MaximumZoomLevel are also optional, and allow to prevent invalid tile
     * requests to the providers, if they do not support the specific ZL. Default is 0 and 19,
     * respectively.
     *
     * <server address template> is required, and is the tile url template, with %x, %y and %z as
     * placeholders for the actual parameters.
     * Example:
     * http://localhost:8080/maps/%z/%x/%y.png
     *
     * <image format> is required, and is the format of the tile.
     * Examples:
     * "png", "jpg"
     *
     * <MapCopyRight> is required and is the string that will be displayed in the "Map (c)" part
     * of the on-screen copyright notice. Can be an empty string.
     * Example:
     * "<a href='http://www.mapquest.com/'>MapQuest</a>"
     *
     * <DataCopyRight> is required and is the string that will be displayed in the "Data (c)" part
     * of the on-screen copyright notice. Can be an empty string.
     * Example:
     * "<a href='http://www.openstreetmap.org/copyright'>OpenStreetMap</a> contributors"
     *
     * <StyleCopyRight> is optional and is the string that will be displayed in the optional "Style (c)" part
     * of the on-screen copyright notice.
     */

    QJsonParseError error;
    QJsonDocument d = QJsonDocument::fromJson(reply->readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "QGeoTileProviderOsm: Error parsing redirection data: "<<error.errorString() << "at "<<m_urlRedirector;
        emit resolutionFinished(this);
        return;
    }
    if (!d.isObject()) {
        qWarning() << "QGeoTileProviderOsm: Invalid redirection data" << "at "<<m_urlRedirector;
        emit resolutionFinished(this);
        return;
    }
    const QJsonObject json = d.object();
    const QJsonValue urlTemplate = json.value(QLatin1String("UrlTemplate"));
    const QJsonValue imageFormat = json.value(QLatin1String("ImageFormat"));
    const QJsonValue copyRightMap = json.value(QLatin1String("MapCopyRight"));
    const QJsonValue copyRightData = json.value(QLatin1String("DataCopyRight"));
    if ( urlTemplate == QJsonValue::Undefined
         || imageFormat == QJsonValue::Undefined
         || copyRightMap  == QJsonValue::Undefined
         || copyRightData  == QJsonValue::Undefined
         || !urlTemplate.isString()
         || !imageFormat.isString()
         || !copyRightMap.isString()
         || !copyRightData.isString()) {
        qWarning() << "QGeoTileProviderOsm: Incomplete redirection data" << "at "<<m_urlRedirector;
        emit resolutionFinished(this);
        return;
    }

    const QJsonValue enabled = json.value(QLatin1String("Enabled"));
    if (enabled.isBool() && ! enabled.toBool()) {
        qWarning() << "QGeoTileProviderOsm: Tileserver disabled" << "at "<<m_urlRedirector;
        emit resolutionFinished(this);
        return;
    }

    QString styleCopyRight;
    const QJsonValue copyRightStyle = json.value(QLatin1String("StyleCopyRight"));
    if (copyRightStyle != QJsonValue::Undefined && copyRightStyle.isString())
        styleCopyRight = copyRightStyle.toString();

    int minZL = 0;
    int maxZL = 19;
    const QJsonValue minZoom = json.value(QLatin1String("MinimumZoomLevel"));
    if (minZoom.isDouble())
        minZL = qBound(0, int(minZoom.toDouble()), maxValidZoom);
    const QJsonValue maxZoom = json.value(QLatin1String("MaximumZoomLevel"));
    if (maxZoom.isDouble())
        maxZL = qBound(0, int(maxZoom.toDouble()), maxValidZoom);

    m_provider = TileProvider(urlTemplate.toString(),
                              imageFormat.toString(),
                              copyRightMap.toString(),
                              copyRightData.toString(),
                              minZL,
                              maxZL);
    m_provider.setStyleCopyRight(styleCopyRight);

    if (m_provider.isValid())
        m_status = Valid;

    emit resolutionFinished(this);
}

void QGeoTileProviderOsm::onNetworkReplyError(QNetworkReply::NetworkError error)
{
    if (m_status == Resolving)
        m_status = Idle;

    qWarning() << "QGeoTileProviderOsm::onNetworkReplyError " << error;
    handleError(error);

    static_cast<QNetworkReply *>(sender())->deleteLater();
    if (m_status == Invalid)
        emit resolutionError(this, error);
}

QT_END_NAMESPACE
